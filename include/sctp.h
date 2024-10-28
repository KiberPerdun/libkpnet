//
// Created by KiberPerdun on 9/25/24.
//

#ifndef LIBKPNET_SCTP_H
#define LIBKPNET_SCTP_H

#include "malloc.h"
#include "types.h"
#include <netinet/in.h>

typedef struct sctp_common_hdr
{
  u16 srcp;
  u16 dstp;
  u32 tag;
  u32 checksum;
} sctp_common_hdr_t;

typedef struct sctp_chunk_hdr
{
  u8 type;
  u8 flags;
  u16 len;
} sctp_chunk_hdr_t;

typedef struct sctp_data_payload
{
  u32 tsn;
  u16 stream_id;
  u16 stream_seq;
  u32 proto;
} sctp_data_payload_t;

typedef struct sctp_init
{
  u32 init_tag;
  u32 a_rwnd;
  u16 sout;
  u16 iout;
  u32 tsn;
} sctp_init_t;

sctp_common_hdr_t *sctp_fill_common_hdr (u16 src, u16 dst);
sctp_init_t *sctp_fill_init (u16 inbound_streams, u16 outbound_streams,
                             u32 init, u32 tsn, u32 a_rwnd);
sctp_chunk_hdr_t *sctp_fill_chunk_hdr (u8 type, u8 flags, u16 plen);

#endif // LIBKPNET_SCTP_H
