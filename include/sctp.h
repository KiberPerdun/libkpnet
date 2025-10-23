//
// Created by KiberPerdun on 9/25/24.
//

#ifndef LIBKPNET_SCTP_H
#define LIBKPNET_SCTP_H

#include "malloc.h"
#include "types.h"
#include <netinet/in.h>
#include "stdbool.h"
#include "if_packet.h"

typedef enum SCTP_STATUS
{
  SCTP_LISTEN,
  SCTP_INIT_SENT,
  SCTP_INIT_RECEIVED,
  SCTP_INIT_ACK_SENT,
  SCTP_INIT_ACK_RECEIVED,
  SCTP_COOKIE_RECEIVED,
  SCTP_COOKIE_ECHO_SENT,
  SCTP_COOKIE_ECHO_RECEIVED,
  SCTP_ESTABLISHED,
} SCTP_STATUS_T;

typedef enum SCTP_HDR_TYPE
{
  SCTP_DATA              = 0,
  SCTP_INIT              = 1,
  SCTP_INIT_ACK          = 2,
  SCTP_SACK              = 3,
  SCTP_HEARTBEAT         = 4,
  SCTP_HEARTBEAT_ACK     = 5,
  SCTP_ABORT             = 6,
  SCTP_SHUTDOWN          = 7,
  SCTP_SHUTDOWN_ACK      = 8,
  SCTP_ERROR             = 9,
  SCTP_COOKIE_ECHO       = 10,
  SCTP_COOKIE_ACK        = 11,
  SCTP_SHUTDOWN_COMPLETE = 14,
} SCTP_HDR_TYPE_T;

typedef enum SCTP_ERROR_KIND
{
  SCTP_ERROR_INVALID_STREAM_IDENT                         = 1,
  SCTP_ERROR_MISSING_MANDATORY_PARAMETER                  = 2,
  SCTP_ERROR_STALE_COOKIE                                 = 3,
  SCTP_ERROR_OUT_OF_RESOURCE                              = 4,
  SCTP_ERROR_UNRESOLVABLE_ADDRESS                         = 5,
  SCTP_ERROR_UNRECOGNIZED_CHUNK_TYPE                      = 6,
  SCTP_ERROR_INVALID_MANDATORY_PARAMETER                  = 7,
  SCTP_ERROR_UNRECOGNIZED_PARAMETERS                      = 8,
  SCTP_ERROR_NO_USER_DATA                                 = 9,
  SCTP_ERROR_COOKIE_RECEIVED_WHILE_SHUTTING_DOWN          = 10,
  SCTP_ERROR_RESTART_OF_AN_ASSOCIATION_WITH_NEW_ADDRESSES = 11,
  SCTP_ERROR_USER_INITIATED_ABORT                         = 12,
  SCTP_ERROR_PROTOCOL_VIOLATION                           = 13,
} SCTP_ERROR_KIND_T;

#define SCTP_EBIT 0x01
#define SCTP_BBIT 0x02
#define SCTP_UBIT 0x04
#define SCTP_IBIT 0x08

#define SCTP_UNFRAGMENTED (SCTP_BBIT | SCTP_EBIT)

typedef struct sctp_opt
{
  u16 type;
  u16 len;
} __attribute__ ((aligned (4))) sctp_opt_t;

typedef struct sctp_common_hdr
{
  u16 srcp;
  u16 dstp;
  u32 tag;
  u32 check;
} sctp_cmn_hdr_t;

typedef struct sctp_fld_hdr
{
  u8 type;
  u8 flags;
  u16 len;
} sctp_fld_hdr_t;

typedef struct sctp_init_hdr
{
  u32 init_tag;
  u32 a_rwnd;
  u16 os;
  u16 mis;
  u32 init_tsn;
} sctp_init_hdr_t;

typedef struct sctp_data_hdr
{
  u32 tsn;
  u16 so;
  u16 sn;
  u32 proto_def;
} sctp_data_hdr_t;

typedef struct sctp_init_ack_hdr
{
  u32 init_tag;
  u32 a_rwnd;
  u16 os;
  u16 mis;
  u32 init_tsn;
} sctp_init_ack_hdr_t;

typedef struct sctp_cookie_echo_hdr
{
  u8 type;
  u8 flags;
  u16 len;
} sctp_cookie_echo_hdr_t;

typedef struct sctp_cookie_ack_hdr
{
  u8 type;
  u8 flags;
  u16 len;
} sctp_cookie_ack_hdr_t;

typedef struct sctp_sack_hdr
{
  u32 cum_tsn;
  u32 a_rwnd;
  u16 gap_acks;
  u16 dup_tsns;
} sctp_sack_hdr_t;

typedef struct sctp_shut_hdr
{
  u32 cum_tsn;
} sctp_shut_hdr_t;

typedef struct sctp_error_kind
{
  u16 cause;
  u16 len;
  union
  {
    struct
    {
      u16 stdint;
      u16 zeros;
    } sctp_error_invalid_stream_ident;

    struct
    {
      u32 num;
    } sctp_error_missing_mandatory_parameter;

    struct
    {
      u32 measure;
    } sctp_error_stale_cookie;

    struct {} sctp_error_out_of_resource;

    struct {} sctp_error_unresolvable_address;

    struct {} sctp_error_unrecognized_chunk_type;

    struct {} sctp_error_invalid_mandatory_parameter;

    struct {} sctp_error_unrecognized_parameters;

    struct
    {
      u32 tsn;
    } sctp_error_no_user_data;

    struct {} sctp_error_cookie_received_while_shutting_down;

    struct {} sctp_error_restart_of_an_association_with_new_addresses;

    struct {} sctp_error_user_initiated_abort;

    struct {} sctp_error_protocol_violation;
  } kind;
} sctp_error_kind_t;

typedef struct sctp_error
{
  u16 cause;
  u16 len;
} sctp_error_t;

typedef struct sctp_parameter
{
  u16 type;
  u16 len;
} sctp_parameter_t;

typedef struct sctp_parameter sctp_cookie_t;

typedef struct cookie
{
  u32 ver_tag;
  u32 init_tag;
  u32 a_rwnd;
  u16 os;
  u16 mis;
  u32 tsn;
  u32 src_ip;
  u32 dst_ip;
  u32 src_port;
  u32 dst_port;
  /* params */

  /* MAC */
} cookie_t;

typedef struct if_ip_sctp_meta
{
  /* cookie_t */
  u32 ver_tag;
  u32 init_tag;
  u32 a_rwnd;
  u16 os;
  u16 mis;
  u32 tsn;
  u32 src_ip;
  u32 dst_ip;
  u32 src_port;
  u32 dst_port;

  /* others */
  i32 state;
  u32 src_arwnd;
  u16 src_os;
  u16 src_mis;
  u8 dev[6];
  u8 gateway[6];

  u0 *add;
  u16 add_len;
} if_ip_sctp_meta_t;

typedef struct sctp_hdr
{
  sctp_cmn_hdr_t cmn; /* 12 */
  sctp_fld_hdr_t fld;  /* 4  */
  union
  {
    sctp_init_hdr_t init;
    struct
    {
      sctp_init_ack_hdr_t init_ack;
      sctp_cookie_t cookie;
    };
    sctp_data_hdr_t data;
    sctp_sack_hdr_t sack;
    sctp_shut_hdr_t shut;
  } type;
} sctp_t;

typedef struct sctp_ulp_config
{
  u32 max_retrans;
  u32 rto_initial;
  u32 rto_min;
  u32 rto_max;
  u32 max_burst;
  u32 cookie_life;

  u32 src_ip;
  u32 dst_ip;
  u32 src_port;
  u32 dst_port;

  u32 src_arwnd;
  u32 src_tsn;
  u16 src_os;
  u16 src_mis;
  u8 dev[6];
  u8 gateway[6];
} sctp_ulp_config_t;

typedef struct sctp_thread
{
  u16 id;
  u0 *buffer;
  u32 pos_low;
  u32 pos_current;
  u32 pos_high;
  u16 *misses;
  u16 num_misses;
  u16 ssn;
  u16 sin;
  u32 proto;
  i8 flags;
  u32 buffer_remain;
  bool active;
} sctp_thread_t;

typedef struct sctp_association
{
  u16 id;
  SCTP_STATUS_T status;

  u32 ver_tag;
  u32 init_tag;
  u32 a_rwnd;
  u32 dst_port;
  u32 dst_ip;
  u16 os;
  u16 mis;
  u32 tsn;
  u32 rtt;
  u32 rto;
  pthread_spinlock_t lock;
  sctp_thread_t **os_threads;
  sctp_thread_t **mis_threads;
  ringbuf_t *tx_ring;
  ringbuf_t *rx_ring;
  ringbuf_t *retry_ring;
  ringbuf_t *prefilled_ring;
  frame_data_t *base; /* will be replaced with ulp config */
  sctp_ulp_config_t *ulp;
} sctp_association_t;

typedef i64 (*sctp_process) (sctp_association_t *, u0 *, u32);
extern sctp_process *sctp_methods[16];

u0  sctp_init ();
u32 generate_crc32c (const u8 *buffer, u32 length);
u64 generate_crc32c_on_crc32c (const u8 *buffer, u32 length, u64 crc);
bool build_sctp_hdr_raw (u16 srcp, u16 dstp, u32 tag, SCTP_HDR_TYPE_T type, u16 os, u16 mis, u32 a_rwnd, u8 flags, u0 *_args);
/* frame_data_t *build_sctp_cmn_hdr_raw (frame_data_t *frame, u16 srcport, u16 dstport, u32 tag); */
/* frame_data_t *build_sctp_fld_hdr_raw (frame_data_t *frame, u8 type, u8 flags, u16 len); */
/* frame_data_t *build_sctp_init_hdr (frame_data_t *frame, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn); */
/* frame_data_t *build_sctp_init_ack_hdr (frame_data_t *frame, u32 a_rwnd, u16 os, u16 mis, u0 *meta); */
/* frame_data_t *build_sctp_cookie_echo_hdr (frame_data_t *frame, u0 *meta); */
u0 *build_sctp_cmn_hdr_raw (u0 *packet, u16 *plen, u16 srcport, u16 dstport, u32 tag);
u0 *build_sctp_fld_hdr_raw (u0 *packet, u16 *plen, u8 type, u8 flags, u16 len);
u0 *build_sctp_init_hdr_raw (u0 *packet, u16 *plen, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn);
u0 *build_sctp_init_ack_hdr_raw (u0 *packet, u16 *plen, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn);
u0 *build_sctp_cookie_echo_hdr_raw (u0 *packet, u16 *plen, u0 *cookie, u16 cookie_len);

u0 *build_sctp_cookie_param_raw (u0 *packet, u16 *plen, cookie_t *cookie);

frame_data_t *build_sctp_init_hdr (frame_data_t *frame);
frame_data_t *build_sctp_init_ack_hdr (frame_data_t *frame);
frame_data_t *build_sctp_cookie_echo_hdr (frame_data_t *frame);
frame_data_t *build_sctp_cookie_ack_hdr (frame_data_t *frame);

i64 prepare_sctp_threads (sctp_association_t *assoc);

i64 sctp_process_sctp_init (sctp_association_t *assoc, u0 *packet, u32 plen);
i64 sctp_process_sctp_init_ack (sctp_association_t *assoc, u0 *packet, u32 plen);
i64 sctp_process_sctp_cookie_echo (sctp_association_t *assoc, u0 *packet, u32 plen);
i64 sctp_process_sctp_cookie_ack (sctp_association_t *assoc, u0 *packet, u32 plen);

#define HMAC_MD5_KEY_LEN 16

u0 hmac_md5 (u8 *text, i32 text_len, u8 *key, i32 key_len, caddr_t digest);

#endif // LIBKPNET_SCTP_H
