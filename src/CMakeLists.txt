set(SOURCES get_self_mac.c main.c get_gateway_mac.c arp_fill_hdr.c arp_fill_eth.c
        ../include/sctp.h
        ../include/ipv4.h
        fill_ipv4.c)

add_executable(src ${SOURCES})
target_include_directories(src PRIVATE ${PROJECT_SOURCE_DIR}/include)
