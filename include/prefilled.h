//
// Created by KiberPerdun on 9/4/25.
//

#ifndef LIBKPNET_PREFILLED_H
#define LIBKPNET_PREFILLED_H

#include "if_packet.h"

frame_data_t *prefill_mac_ip_sctp (frame_data_t *frame);
i32 build_prefilled_mac_ip_sctp_init_hdr (sctp_association_t *assoc);
i32 build_prefilled_mac_ip_sctp_init_ack_hdr (sctp_association_t *assoc);
i32 build_prefilled_mac_ip_sctp_cookie_echo_hdr (sctp_association_t *assoc);
i32 build_prefilled_mac_ip_sctp_cookie_ack_hdr (sctp_association_t *assoc);

#endif // LIBKPNET_PREFILLED_H
