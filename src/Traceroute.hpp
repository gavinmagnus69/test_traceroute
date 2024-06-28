#ifndef TRACEROUTE_HPP
#define TRACEROUTE_HPP

#include "ICMP.hpp"

#define MAX_HOPS_DEFAULT 30
#define RESPONSE_TIMEOUT_DEFAULT 1
#define DNS_RESOLVING_DEFAULT true
#define QUERIES_DEFAULT 1

struct Params{
    uint32_t max_hops = MAX_HOPS_DEFAULT;
    uint32_t response_timeout = RESPONSE_TIMEOUT_DEFAULT;
    uint32_t queries = QUERIES_DEFAULT;
    std::string address = "";
    bool dns_resolving = DNS_RESOLVING_DEFAULT;
};

char* dns_resolver(const std::string& url){

    hostent* he = gethostbyname(url.c_str());

    if (he == nullptr) {
        return "";
    }

    return inet_ntoa(*reinterpret_cast<in_addr*>(he->h_addr));
}

std::string to_dns(const std::string& ipv4, std::string& buf){

    sockaddr_in sa;

    sa.sin_family = AF_INET;

    inet_pton(AF_INET, ipv4.c_str(), &(sa.sin_addr.s_addr));

    int result = getnameinfo(reinterpret_cast<sockaddr*>(&sa), sizeof(sa), buf.data(), NI_MAXHOST, nullptr, 0, 0);

    if (result == 0) {
        return buf;
    } else {
        return ipv4;
    }
}


uint16_t checksum(uint16_t *buffer, size_t size) {
    unsigned long cksum = 0;

	while (size > 1){

		cksum += *buffer++;
		size -= sizeof(uint16_t);

	}

	if (size){
	    cksum += *reinterpret_cast<unsigned char*>(buffer);
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return static_cast<uint16_t>(~cksum);
}

void close_socket(int socketFD){
    close(socketFD);
}

void traceroute(const std::string& ip, const Params& params) {

    std::string dest = ip;
    
    sockaddr_in in_addr;

    in_addr.sin_family = AF_INET; //ipv4
    in_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    in_addr.sin_port = htons(0);

    int socketFD = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    if (socketFD < 0) {
        std::cout << "socket error\n";
        return;
    }

    pid_t ppid = getppid();
    
    icmpv4_header icmpv4_packet;
    
    sockaddr_in src_addr;

    iphdr ip_response_header;

    std::string host(NI_MAXHOST, '\0');
    
    timeval tv;

    tv.tv_sec = params.response_timeout;
    tv.tv_usec = 0;

    setsockopt(socketFD, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    
    
    for (int i = 0; i < params.max_hops; ++i) {
        for(int q = 0; q < params.queries; ++q){

            icmpv4_packet.type = 8;
            icmpv4_packet.code = 0;
            icmpv4_packet.checksum = 0;
            icmpv4_packet.identifier = ppid;
            icmpv4_packet.payload = 228133769;
            icmpv4_packet.sequence = i;
            icmpv4_packet.checksum = checksum(reinterpret_cast<uint16_t*>(&icmpv4_packet), sizeof(icmpv4_packet));

            int ttl = i + 1;

            setsockopt(socketFD, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

            auto start = std::chrono::high_resolution_clock::now();

            auto send_flag = sendto(socketFD, &icmpv4_packet, sizeof(icmpv4_packet), 0, reinterpret_cast<sockaddr*>(&in_addr),
                                    socklen_t(sizeof(in_addr)));

            if (send_flag < 0) {
                std::cout << "sending error\n";
                close_socket(socketFD);
                return;
            }

            auto data_length_byte = recv(socketFD, &ip_response_header, sizeof(ip_response_header), 0);

            auto end = std::chrono::high_resolution_clock::now();



            if (data_length_byte == -1) {

                if(q == 0){
                    std::cout << ttl;
                }

                std::cout << " *";
                continue;
            }

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            src_addr.sin_addr.s_addr = ip_response_header.saddr;


            if(q == 0){
                std::cout << ttl << ' ';

                if(params.dns_resolving){
                    std::cout << to_dns(inet_ntoa(src_addr.sin_addr), host);
                }

                std::cout << " (" <<  inet_ntoa(src_addr.sin_addr) << ')';
            }
                        
                        
            std::cout << ' ' << duration << " ms";
            
            host.assign(host.length(), '\0');

            

        }

        if (strcmp(inet_ntoa(src_addr.sin_addr), dest.c_str()) == 0) {
            break;    
        }

        std::cout << '\n';    
    }

    close_socket(socketFD);
}

void help(const char* prog_name){
    std::cout << "Usage: "
              << "sudo " << prog_name
              << " host "
              << "[options]\n"
              << "Options:\n"
              << "-m max_ttl --max-hops max_ttl      Set the max number of hops (max TTL to be reached). Default is 30\n"
              << "-r rt --response-timeout rt    Set the max response timeout time (in seconds). Default is 1 second\n"
              << "-n --no-dns    Do not resolve IP addresses to their domain names\n"
              << "-h --help   Read this help and exit\n"
              << "-q nqueries --queries nqueries    Set the number of probes per each hop. Default is 1\n"
              << "Arguments:\n"
              << "  host    The host to traceroute to\n";
}

void arg_error(const std::string& arg){
    std::cout << "Cannot resolve argument for " << arg << '\n';
}

bool parse_args(int argc, char** argv, Params& params){

    if(argc == 1){
        help(argv[0]);
        return false;
    }

    static struct option longOptions[] =
    {
        {"max-hops", required_argument, nullptr, 'm'},
        {"response-timeout", required_argument, nullptr, 'r'},
        {"queries", required_argument, nullptr, 'q'},
        {"help", no_argument, nullptr, 'h'},
        {"no-dns", no_argument, nullptr, 'n'},
        {nullptr, 0, nullptr, 0}
    };

    const char* shortOptions = "m:r:q:h:n";

    char ch;

    int hops;
    int rt;
    int qe;

    while ((ch = getopt_long(argc, argv, shortOptions, longOptions, nullptr)) != -1) {
        switch (ch) {
            
            case 'h':
                help(argv[0]);
                return false;
            
            case 'm':
                hops = atoi(optarg);
                if(hops <= 0){
                    arg_error("-m");
                    return false;
                }
                params.max_hops = hops;
                break;

            case 'r':
                rt = atoi(optarg);
                if(rt <= 0){
                    arg_error("-r");
                    return false;
                }
                params.response_timeout = rt;
                break;

            case 'n':
                params.dns_resolving = false;
                break;

            case 'q':
                qe = atoi(optarg);

                if(qe <= 0){
                    arg_error("-q");
                    return false;
                }
                params.queries = qe;
                break;

            case '?':
                help(argv[0]);
                return false;

            default:
                help(argv[0]);
                return false;
                
        }
    }

    if(optind >= argc){
        help(argv[0]);
        return false;
    }
    
    params.address = argv[optind];
    
    return true;
}

#endif