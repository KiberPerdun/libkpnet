//
// Created by KiberPerdun on 9/25/25.
//

#include "if_packet.h"

sctp_process *sctp_methods[16] = { 0 };

u0 sctp_init ()
{
  for (i32 i = 0; i < 16; ++i)
    sctp_methods[i] = NULL;

  sctp_methods[SCTP_DATA]          = NULL; /* 0 */
  //sctp_methods[SCTP_INIT]          = sctp_process_sctp_init; /* 1 */
  //sctp_methods[SCTP_INIT_ACK]      = sctp_process_sctp_cookie_ack; /* 2 */
  sctp_methods[SCTP_SACK]          = NULL; /* 3 */
  sctp_methods[SCTP_HEARTBEAT]     = NULL; /* 4 */
  sctp_methods[SCTP_HEARTBEAT_ACK] = NULL; /* 5 */
  sctp_methods[SCTP_ABORT]         = NULL; /* 6 */
  sctp_methods[SCTP_SHUTDOWN]      = NULL; /* 7 */
  sctp_methods[SCTP_SHUTDOWN_ACK]  = NULL; /* 8 */
  sctp_methods[SCTP_ERROR]         = NULL; /* 9 */
  //sctp_methods[SCTP_COOKIE_ECHO]   = sctp_process_sctp_cookie_echo; /* 10 */
  //sctp_methods[SCTP_COOKIE_ACK]    = sctp_process_sctp_cookie_ack; /* 11 */
}
