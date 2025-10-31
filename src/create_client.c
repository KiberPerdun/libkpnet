//
// Created by KiberPerdun on 11.12.2024.
//

#include "checks.h"
#include "eth.h"
#include "hwinfo.h"
#include "if_packet.h"
#include "ipv4.h"
#include "netlink.h"
#include "prefilled.h"
#include "random.h"
#include "ring_buffer.h"
#include "tcp.h"
#include "types.h"

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <thread_db.h>
#include <threads.h>
#include <time.h>

#define NS_PER_SEC 1000000000LL

#define BENCH_START() \
struct timespec _bench_start, _bench_end; \
clock_gettime(CLOCK_MONOTONIC, &_bench_start);

#define BENCH_END(name, ops) \
clock_gettime(CLOCK_MONOTONIC, &_bench_end); \
int64_t _ns = (_bench_end.tv_sec - _bench_start.tv_sec) * NS_PER_SEC + \
(_bench_end.tv_nsec - _bench_start.tv_nsec); \
printf("%s: %ld ns total, %.2f ns/op, %.2f Mops/sec\n", \
name, _ns, (double)_ns/(ops), ((double)ops*1e3)/_ns);

#define MAX_PACKET_LEN 1516
u0 *
create_client ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL, *prefill = NULL;
  ringbuf_t *rb_tx, *rb_rx, *rb_prefill;
  eth_t *eth;
  u16 src_port, dst_port;
  frame_data_t *frame;
  thread_t cons, prod;
  sctp_thread_t *thread;

  if (!((frame = aligned_alloc (CACHELINE_SIZE, sizeof (frame_data_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  if (!((packet = aligned_alloc (CACHELINE_SIZE, MAX_PACKET_LEN + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  if (!((state = aligned_alloc (CACHELINE_SIZE, sizeof (connection_sctp_state_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  if (!((prefill = aligned_alloc (CACHELINE_SIZE, MAX_PACKET_LEN + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  memset (prefill, 0, MAX_PACKET_LEN + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1));
  memset (packet, 0, MAX_PACKET_LEN + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1));

  frame->packet = packet;
  frame->prefill = prefill;
  frame->plen = MAX_PACKET_LEN;
  frame->state = state;

  eth = eth_open (CLIENT_INAME);

  rb_arg_t arg_tx;
  arg_tx.eth = eth;
  rb_tx = create_ringbuf (64);
  arg_tx.rb = rb_tx;

  rb_arg_t arg_rx;
  arg_rx.eth = eth;
  rb_rx = create_ringbuf (1024);
  arg_rx.rb = rb_rx;
  if (pthread_create (&prod, NULL, recv_packet, &arg_rx) != 0)
    return 0;

  dst_port = 80;
  src_port = get_random_u16 ();

  u32 src_ip = inet_addr ("192.168.1.2");
  u32 dst_ip = inet_addr ("192.168.1.3");

  if_ip_sctp_meta_t *meta = calloc (sizeof (if_ip_sctp_meta_t), 1);
  frame->state = meta;
  meta->state = SCTP_INIT_SENT;
  meta->src_ip = src_ip;
  meta->dst_ip = dst_ip;
  meta->src_port = src_port;
  meta->dst_port = dst_port;
  meta->src_arwnd = ~0;
  meta->src_os = 32;
  meta->src_mis = 32;

  if (get_ifmac (SERVER_INAME, meta->gateway))
    {
      fputs ("Failed to get gateway interface mac\n", stderr);
      goto cleanup;
    }

  if (get_ifmac (CLIENT_INAME, meta->dev))
    {
      fputs ("Failed to get dev interface mac\n", stderr);
      goto cleanup;
    }

  frame->sync = NULL;
  frame->plen = 0;

  generate_random_buffer ();

  sctp_ulp_config_t *ulp;
  if (!((ulp = aligned_alloc (CACHELINE_SIZE, sizeof (sctp_ulp_config_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  meta->state = SCTP_INIT_SENT;
  meta->src_ip = src_ip;
  meta->dst_ip = dst_ip;
  meta->src_port = src_port;
  meta->dst_port = dst_port;
  meta->src_arwnd = ~0;
  meta->src_os = 32;
  meta->src_mis = 32;

  ulp->src_ip = src_ip;
  ulp->dst_ip = dst_ip;
  ulp->src_port = src_port;
  ulp->dst_port = dst_port;
  ulp->src_arwnd = ~0;
  ulp->src_os = 32;
  ulp->src_mis = 32;

  sctp_association_t *assoc;
  if (!((assoc = aligned_alloc (CACHELINE_SIZE, sizeof (sctp_association_t) + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1)))))
    goto cleanup;

  assoc->id = get_random_u16 ();
  pthread_spin_init (&assoc->lock, PTHREAD_PROCESS_PRIVATE);
  assoc->tx_ring = rb_tx;
  assoc->rx_ring = rb_rx;
  assoc->retry_ring = NULL;
  assoc->ulp = ulp;
  assoc->base = frame;
  assoc->os = htons (16);
  assoc->mis = htons (16);
  assoc->base->state = meta;
  /*
  pthread_spin_lock(&lock);
  pthread_spin_unlock(&lock);
  pthread_spin_destroy(&lock);
   */
  ringbuf_cell_t *cell;

  rb_prefill = create_ringbuf (rb_tx->size);
  for (i32 i = 0; i < rb_tx->size; ++ i)
    {
      frame->prefill = aligned_alloc (CACHELINE_SIZE, MAX_PACKET_LEN + (CACHELINE_SIZE - 1) & ~(CACHELINE_SIZE - 1));
      if (!frame->prefill)
        {
          cell = pop_ringbuf (rb_prefill);
          for (; cell; cell = pop_ringbuf (rb_prefill))
            {
              free (cell->packet);
              free (cell);
            }
          free (frame->prefill);
          goto cleanup;
        }

      frame = prefill_mac_ip_sctp (frame);
      push_ringbuf (rb_prefill, frame->prefill, frame->plen);
      frame->plen = 0;
    }
  assoc->prefilled_ring = rb_prefill;

  arg_tx.rb_prefill = rb_prefill;

  if (pthread_create (&cons, NULL, eth_send_rb, &arg_tx) != 0)
    return 0;

  sctp_init ();

  BENCH_START ()
  build_prefilled_mac_ip_sctp_init_hdr (assoc);
  BENCH_END ("build_prefilled_mac_ip_sctp_init_hdr", 1)

  do
    for (; (cell = pop_ringbuf (rb_rx)) == 0;)
      ;

  while (if_ip_sctp (cell->packet, cell->plen, assoc) == NULL);

  do
    for (; (cell = pop_ringbuf (rb_rx)) == 0;)
      ;

  while (if_ip_sctp (cell->packet, cell->plen, assoc) == NULL);

  char buffer[] = "Америка. Пустынная прерия. Скоро полночь. Около пустого шоссе среди песков и кактусов стоит маленькая гостиница — единственное строение на несколько миль вокруг. Все места заняты. К гостинице подъезжают три студента и начинают стучать в дверь. Выходит хозяин. Студенты:\n"
                  "— Слушай, нам бы переночевать…\n"
                  "— У меня все номера заняты; есть только одна комнатушка: шесть метров, без окон, без мебели, стены не покрашены, холодно, сыро, крысы.\n"
                  "— Ну, мы же студенты — люди привычные. Для одной ночи сойдёт.\n"
                  "Хозяин их впускает, они заходят в комнату, дверь закрывается.\n"
                  "Полночь. К гостинице на лошадях прискакивают три ковбоя. Стучатся в дверь, хозяин открывает.\n"
                  "— Впусти, друг, переночевать.\n"
                  "— Я бы вас впустил, но у меня все номера заняты; есть только одна комнатушка: шесть метров, без окон, без мебели, стены не покрашены, холодно, сыро, крысы, да и то там уже три студента.\n"
                  "— Да ладно тебе — мы же ковбои, договоримся.\n"
                  "Хозяин впускает ковбоев, они заходят в комнату, дверь закрывается.\n"
                  "Час ночи. К гостинице на лимузине подъезжают три мафиози. Стучатся в дверь, хозяин открывает.\n"
                  "— Нам бы, браток, ночку перекантоваться у тебя…\n"
                  "— Я бы вас впустил, но у меня все номера заняты; есть только одна комнатушка: шесть метров, без окон, без мебели, стены не покрашены, холодно, сыро, крысы, да и то там уже три студента и три ковбоя.\n"
                  "— Братан, мы же мафия. Разберёмся.\n"
                  "Хозяин их впускает, они заходят в комнату, дверь закрывается.\n"
                  "Пол второго ночи. К гостинице на дорогущем автомобиле подъезжает сногсшибательная блондинка. Стучится в дверь, хозяин открывает.\n"
                  "— Мне бы переночевать…\n"
                  "— Я бы вас впустил, но у меня все номера заняты; есть только одна комнатушка: шесть метров, без окон, без мебели, стены не покрашены, холодно, сыро, крысы и ещё там уже спят три студента, три ковбоя и три мафиози…\n"
                  "— Я разберусь с ними, как истинная женщина.\n"
                  "Хозяин впускает. После чего не может уснуть всю ночь: 6 метров, без окон, без мебели, стены не покрашены, холодно, сыро, крысы, три студента, три ковбоя, три мафиози, блондинка… Что же завтра будет?!\n"
                  "Наутро из комнаты выходят три студента, довольные жизнью, покупают три Кока-Колы и уезжают. Затем выходят три ковбоя, довольные жизнью, покупают три Кока-Колы и уезжают. После выходят три мафиози, довольные жизнью, покупают три Кока-Колы и уезжают. Последней выходит блондинка, еще краше, чем ночью, покупает виски, выпивает и уезжает.\n"
                  "Мораль: девять из десяти человек в Америке пьют Кока-Колу.";
  thread = assoc->os_threads[0];
  thread->buffer = buffer;
  thread->pos_high = sizeof (buffer);
  thread->pos_low = 0;
  thread->pos_current = 0;
  for (; thread->pos_current != thread->pos_high;)
    build_prefilled_mac_ip_sctp_data_hdr (assoc, 0);


  if (pthread_join (cons, NULL) != 0)
    return 0;

  if (pthread_join (prod, NULL) != 0)
    return 0;

cleanup:
  eth_close (eth);
  free (ulp);
  free (assoc);
  free (prefill);
  free (meta);
  free (state);
  free (packet);
  free (frame);
  return NULL;
}
#undef MAX_PACKET_LEN
