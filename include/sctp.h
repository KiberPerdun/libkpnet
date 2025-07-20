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

typedef struct sctp_opt
{
  u16 type;
  u16 len;
} sctp_opt_t;

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
  sctp_opt_t cookie;
} sctp_init_ack_hdr_t;

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

typedef struct sctp_hdr
{
  sctp_cmn_hdr_t cmn; /* 12 */
  sctp_fld_hdr_t fld;  /* 4  */
  union
  {
    sctp_init_hdr_t init;
    sctp_init_ack_hdr_t init_ack;
    sctp_data_hdr_t data;
    sctp_sack_hdr_t sack;
    sctp_shut_hdr_t shut;
  } type;
} sctp_t;

u32 generate_crc32c (const u8 *buffer, u32 length);
bool build_sctp_hdr_raw (u16 srcp, u16 dstp, u32 tag, SCTP_HDR_TYPE_T type, u16 os, u16 mis, u32 a_rwnd, u8 flags, u0 *_args);
frame_data_t *build_sctp_cmn_hdr_raw (frame_data_t *frame, u16 srcport, u16 dstport, u32 tag);
frame_data_t *build_sctp_fld_hdr_raw (frame_data_t *frame, u8 type, u8 flags, u16 len);
frame_data_t *build_sctp_init_hdr (frame_data_t *frame, u32 tag, u32 a_rwnd, u16 os, u16 mis, u32 tsn);
frame_data_t *build_sctp_init_ack_hdr (frame_data_t *frame, u32 a_rwnd, u16 os, u16 mis, u0 *meta);
frame_data_t *build_sctp_cookie_echo_hdr (frame_data_t *frame, u0 *meta);
#endif // LIBKPNET_SCTP_H
