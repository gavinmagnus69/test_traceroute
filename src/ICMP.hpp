#ifndef ICMP_HPP
#define ICMP_HPP

#include <iostream>
#include <bits/stdc++.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <netinet/ip.h>
#include <netdb.h>
#include "getopt.h"


struct icmpv4_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;
    uint64_t payload;    
};


#endif