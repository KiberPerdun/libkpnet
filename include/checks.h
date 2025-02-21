//
// Created by KiberPerdun on 2/14/25.
//

#ifndef CHECKS_H
#define CHECKS_H

#include "if_packet.h"

frame_data_t *fix_check (frame_data_t *frame);
frame_data_t *fix_check_ip_tcp (frame_data_t *frame, u16 frm_max_len); /* linux_mac -> ip -> tcp */
frame_data_t *fix_check_ip (frame_data_t *frame, u16 opt_len);

#endif //CHECKS_H
