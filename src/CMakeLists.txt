set(SOURCES get_self_mac.c main.c arp_fill_hdr.c arp_fill_eth.c
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
        get_mac_address.c
        get_gateway_mac_address.c
        tcp_checksum.c
        build_tcp_init_hdr.c
)

add_executable(src ${SOURCES})
target_include_directories(src PRIVATE ${PROJECT_SOURCE_DIR}/include)
