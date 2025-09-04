//
// Created by KiberPerdun on 9/4/25.
//

#ifndef LIBKPNET_PREFILLED_H
#define LIBKPNET_PREFILLED_H

#include "if_packet.h"

frame_data_t *prefill_mac_ip_sctp (frame_data_t *frame);
frame_data_t *build_prefilled_mac_ip_sctp_init_hdr (frame_data_t *frame);

#endif // LIBKPNET_PREFILLED_H
