//
// Created by KiberPerdun on 11.12.2024.
//

#include "arp.h"
#include "checks.h"
#include "eth.h"
#include "hwinfo.h"
#include "if_packet.h"
#include "ipv4.h"
#include "netlink.h"
#include "prefilled.h"
#include "random.h"
#include "ring_buffer.h"
#include "sctp.h"
#include "tcp.h"
#include "types.h"

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/param.h>
#include <thread_db.h>
#include <threads.h>
#include <time.h>

#define NS_PER_SEC 1000000000LL

#define BENCH_START() \
struct timespec _bench_start, _bench_end; \
clock_gettime(CLOCK_MONOTONIC, &_bench_start);

#define BENCH_END(name, ops)                                                  \
  clock_gettime (CLOCK_MONOTONIC, &_bench_end);                               \
  int64_t _ns = (_bench_end.tv_sec - _bench_start.tv_sec) * NS_PER_SEC        \
                + (_bench_end.tv_nsec - _bench_start.tv_nsec);                \
  printf ("%s: %ld ns total, %.2f ns/op, %.2f Mops/sec\n", name, _ns,         \
          (double)_ns / (ops), ((double)ops * 1e3) / _ns);

#define MAX_PACKET_LEN 1516
u0 *
create_client ()
{
  /* init */
  connection_sctp_state_t *state = NULL;
  u0 *packet = NULL, *prefill = NULL;
  ringbuf_t *rb_tx, *rb_rx, *rb_prefill;
  eth_t *eth;
  frame_data_t *frame;
  thread_t cons, prod;
  sctp_thread_t *thread;
  static u64 g_global_time;
  static u64 g_current_time;
  g_global_time = update_time ();

  kpnet_env_t *env = kpnet_env_init (CLIENT_INAME);
  if (!env) return NULL;

  u32 src_ip = inet_addr ("192.168.1.2");
  u32 dst_ip = inet_addr ("192.168.1.3");
  u16 src_port = get_random_u16 ();
  u16 dst_port = 80;

  sctp_association_t *assoc = sctp_assoc_create (
      env, src_ip, src_port, dst_ip, dst_port,
      SCTP_INIT_SENT, CLIENT_INAME, SERVER_INAME);

  if (!assoc)
    goto cleanup;

  sctp_init ();
  sctp_prepare_packet (assoc);
  assoc->umem_hdrs_cursor = slab_alloc_64b (&env->state_alloc, env->xdp->umem);

  assoc->status = SCTP_COOKIE_WAIT;
  ringbuf_t *events_allocator = create_allocator (CACHELINE_SIZE, 256);
  assoc->timer = create_ringtimer (5 * 60, events_allocator);
  g_global_time = update_time ();
  u64 handshake_retrans = 0;
  u64 t1_timer = SCTP_RTO_MIN / SCTP_TIMER_STEP;
  build_prefilled_mac_ip_sctp_init_hdr (assoc);
  insert_ringtimer (SCTP_T1_INIT_EXPIRE, t1_timer, assoc->timer);
  timer_tick_result_t timer_results;

  for (;;)
    {
      if (handshake_retrans > SCTP_ASSOC_MAX_RETRANS)
        {
          puts ("INIT ERROR");
          break;
        }
      g_current_time = update_time ();
      for (; (g_current_time - g_global_time) >= SCTP_TIMER_STEP;)
        {
          timer_results = tick_ringtimer (assoc->timer);
          for (; timer_results.count > 0;)
            {
              switch (timer_results.signals[--timer_results.count])
                {
                case (SCTP_T1_INIT_EXPIRE):
                  {
                    puts ("T1-INIT EXPIRE");
                    t1_timer
                        = MIN (t1_timer * 2, SCTP_RTO_MAX / SCTP_TIMER_STEP);
                    insert_ringtimer (SCTP_T1_INIT_EXPIRE, t1_timer,
                                      assoc->timer);
                    eth_send_sctp (assoc);
                    ++handshake_retrans;
                    break;
                  }
                default:
                  break;

                }
            }
          g_global_time += SCTP_TIMER_STEP;
        }
    }

  /*
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
  char buffer1[] = "Пять людоедов устроились на работу. Генеральный директор показал им\n"
                   "кабинет и говорит:\n"
                   "— Ребята работайте, только сотрудников не трогайте, к вашим услугам\n"
                   "наша столовая, все за счет фирмы.\n"
                   "Проходит два месяца. Генеральный приходит, говорит:\n"
                   "— Ребята, молодцы! Я вами очень доволен. Вот только вы нашу уборщицу\n"
                   "не видели? В офисе грязно, а ее второй день нет.\n"
                   "Людоеды божатся, что ни словом, ни духом. Директор уходит. Самый главный\n"
                   "людоед говорит:\n"
                   "— Ну, кто сожрал уборщицу?\n"
                   "Один сознается.\n"
                   "— Слушай, два месяца мы ели менеджеров, пиарщиков, рекламщиков, маркетологов,\n"
                   "бухгалтеров — никто ничего не замечал. Зачем ты сожрал уборщицу?";
  char buffer2[] = "На советский завод приезжает американская делегация. \n"
                   "Идут по цеху и видят: стоят возле станка мастер и рабочий. \n"
                   "Переводчица - американская, попросили перевести. \n"
                   "Та, слегка смущаясь, переводит: \n"
                   "— Мастер говорит рабочему: \n"
                   "«Кто-то вступил в интимные отношения с твоей матерью, ты, гулящая женщина, даже эту изнасилованную шестеренку, гулящая женщина, не можешь, гулящая женщина, правильно сделать, гулящая женщина. Директор, гулящая женщина, даст тебе хороший женский половой орган, гулящая женщина, и вступит с тобой в интимные отношения посредством заднепроходного отверстия, гулящая женщина, если из-за тебя, пассивного гомосексуалиста, опять сорвется на половой член план, гулящая женщина, который должен выполнить завод!\" \n"
                   "В ответ на это рабочий отвечает, что рабочий уже вступил в интимные отношения с речевыми органами директора, что рабочий вступил в интимные отношения со всеми на заводе шестеренками и, что самое невероятное: он вращал на половом члене завод со всеми его планами!»\n"
                   "\n";

  thread = assoc->os_threads[0];
  thread->buffer = buffer;
  thread->pos_high = sizeof (buffer);
  thread->pos_low = 0;
  thread->pos_current = 0;

  thread = assoc->os_threads[1];
  thread->buffer = buffer1;
  thread->pos_high = sizeof (buffer1);
  thread->pos_low = 0;
  thread->pos_current = 0;

  thread = assoc->os_threads[2];
  thread->buffer = buffer2;
  thread->pos_high = sizeof (buffer2);
  thread->pos_low = 0;
  thread->pos_current = 0;

  // ringbuf_t *allocator_64 = create_allocator (CACHELINE_SIZE, 256);
  assoc->events = events_allocator;


  cell = pop_ringbuf (assoc->prefilled_ring);
  assoc->current_packet = cell->packet;
  assoc->remain_plen = assoc->mtu;
  assoc->cursor = cell->packet + sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t);

  sctp_prepare_packet (assoc);
   for (;;)
    {
      sctp_build_data_hdr (assoc, 0);
      sctp_build_data_hdr (assoc, 0);
      sctp_build_data_hdr (assoc, 0);
      sctp_build_data_hdr (assoc, 0);
      sctp_build_data_hdr (assoc, 1);
      sctp_build_data_hdr (assoc, 2);
      sctp_build_data_hdr (assoc, 2);
      sctp_build_data_hdr (assoc, 2);
      push_ringbuf (assoc->tx_ring, assoc->current_packet, assoc->mtu - assoc->remain_plen + (sizeof (mac_t) + sizeof (ipv4_t) + sizeof (sctp_cmn_hdr_t)));
      break;
    }

  if (pthread_join (cons, NULL) != 0)
    return 0;

  if (pthread_join (prod, NULL) != 0)
    return 0;
  */


cleanup:
  eth_close (eth);
  free (assoc);
  free (prefill);
  free (state);
  free (packet);
  free (frame);
  return NULL;
}
#undef MAX_PACKET_LEN
