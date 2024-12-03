set(SOURCES main.c arp_fill_hdr.c arp_fill_eth.c
        ../include/sctp.h
        ../include/ipv4.h
        fill_ipv4.c
        sctp_fill_common_hdr.c
        sctp_fill_init.c
        sctp_fill_chunk_hdr.c
        eth_open.c
        eth_close.c
        eth_send.c
        ../include/udp.h
        fill_udp.c
        ../include/ipv6.h
        ../include/tcp.h
        tcp_fill_init_hdr.c
        tcp_fill_opt_mss.c
        tcp_checksum.c
        build_tcp_init_hdr.c
        build_tcp_raw.c
        ../include/mac.h
        build_mac_client_raw.c
        ../include/if_packet.h
        if_ipv4.c
        recv_filtered.c
        if_tcp.c
        if_ipv4_tcp.c
        build_tcp_ack_hdr.c
        tcp_make_handshake.c
        tcp_get_html.c
        build_tcp_payload_hdr.c
)

add_executable(src ${SOURCES})
target_include_directories(src PRIVATE ${PROJECT_SOURCE_DIR}/include)
