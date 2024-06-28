#include "Traceroute.hpp"

int main(int argc, char* argv[]){

    Params params;

    if(!parse_args(argc, argv, params)){
        return 0;
    }

    std::string ipv4 = dns_resolver(params.address);

    if(ipv4 == ""){
        std::cout << params.address << ": Name or service not known\n";
        return 0;
    }

    std::cout <<"traceroute to " 
              << params.address
              << " (" << ipv4 << "), "
              << params.max_hops << " hops max, "
              << params.response_timeout << "s response timeout" << '\n';

    traceroute(ipv4, params);

    return 0;
}