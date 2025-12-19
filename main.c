#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <net/if.h> 

#define MY_PORT_NUM 80
#define TARGET_INTERFACE "libkpnet_s"  // Вернул твой интерфейс
#define TARGET_IP "192.168.1.3"        // Вернул твой IP
#define RECV_BUF_SIZE (1024 * 1024)

int main() {
  int listenFd, connFd, flags, ret;
  struct sockaddr_in servaddr;
  struct sctp_initmsg initmsg;
  struct sctp_sndrcvinfo sndrcvinfo;
  struct sctp_event_subscribe events;

  char *buffer = malloc(RECV_BUF_SIZE);
  if (!buffer) { perror("malloc"); return 1; }

  listenFd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
  if (listenFd < 0) { perror("socket"); free(buffer); return 1; }

  // Важно: SO_BINDTODEVICE
  if (setsockopt(listenFd, SOL_SOCKET, SO_BINDTODEVICE, TARGET_INTERFACE, strlen(TARGET_INTERFACE)) < 0) {
      perror("setsockopt SO_BINDTODEVICE");
      // Не выходим, пробуем дальше, вдруг прав хватит на bind
    }

  int optval = 1;
  setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(TARGET_IP);
  servaddr.sin_port = htons(MY_PORT_NUM);

  ret = bind(listenFd, (struct sockaddr *)&servaddr, sizeof(servaddr));
  if (ret < 0) { perror("bind"); close(listenFd); free(buffer); return 1; }

  printf("[*] Server listening on %s:%d dev %s\n", TARGET_IP, MY_PORT_NUM, TARGET_INTERFACE);

  memset(&initmsg, 0, sizeof(initmsg));
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 5;
  initmsg.sinit_max_attempts = 4;
  setsockopt(listenFd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));

  memset(&events, 0, sizeof(events));
  events.sctp_data_io_event = 1;
  events.sctp_association_event = 1;
  events.sctp_shutdown_event = 1;
  events.sctp_send_failure_event = 1;
  events.sctp_peer_error_event = 1;
  setsockopt(listenFd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));

  ret = listen(listenFd, 5);
  if (ret < 0) { perror("listen"); close(listenFd); free(buffer); return 1; }

  while (1) {
      printf("[*] Waiting...\n");
      connFd = accept(listenFd, (struct sockaddr *)NULL, NULL);
      if (connFd < 0) { perror("accept"); continue; }

      printf("[+] Connected!\n");

      flags = 0;
      memset(&sndrcvinfo, 0, sizeof(sndrcvinfo));
      int in;
      while ((in = sctp_recvmsg(connFd, buffer, RECV_BUF_SIZE, (struct sockaddr *)NULL, 0, &sndrcvinfo, &flags)) > 0) {
          if (flags & MSG_NOTIFICATION) {
              printf("    [Notify] Type: 0x%x\n", flags);
            } else {
              printf("    [Data] %d bytes. TSN: %u\n", in, sndrcvinfo.sinfo_tsn);
              printf("    [Hex]: ");
              for(int i=0; i<(in<16?in:16); i++) printf("%02X ", (unsigned char)buffer[i]);
              printf("\n");
            }
          flags = 0;
          memset(&sndrcvinfo, 0, sizeof(sndrcvinfo));
        }
      close(connFd);
    }
  return 0;
}
