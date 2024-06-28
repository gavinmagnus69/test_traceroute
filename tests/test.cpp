#include <gtest/gtest.h>
#include "../src/Traceroute.hpp"

TEST(DnsResolverTest, ValidUrl) {
    std::string url = "www.google.com";
    char* result = dns_resolver(url);
    EXPECT_STREQ("172.217.16.36", result);
}

TEST(DnsResolverTest, InvalidUrl) {
    std::string url = "invalid-url";
    char* result = dns_resolver(url);
    EXPECT_STREQ("", result);
}

TEST(DnsResolverTest, EmptyUrl) {
    std::string url = "";
    char* result = dns_resolver(url);
    EXPECT_STREQ("", result);
}

TEST(ToDnsTest, ValidIPv4) {
    std::string ipv4 = "172.217.168.196";
    std::string buf(1024, '\0');
    std::string result = to_dns(ipv4, buf);
    std::string ans;
    int i = 0;
    while(result[i] != '\0'){
        ans += result[i];
        ++i;
    }
    EXPECT_EQ("ams16s32-in-f4.1e100.net", ans);
}

TEST(ToDnsTest, InvalidIPv4) {
    std::string ipv4 = "256.0.0.1"; // Invalid IPv4
    std::string buf(1024, '\0');
    std::string result = to_dns(ipv4, buf);
    std::string ans;
    int i = 0;
    while(result[i] != '\0'){
        ans += result[i];
        ++i;
    }
    EXPECT_EQ("0.0.0.0", ans);
}

TEST(ToDnsTest, EmptyIPv4) {
    std::string ipv4 = ""; // Empty IPv4
    std::string buf(1024, '\0');
    std::string result = to_dns(ipv4, buf);
    std::string ans;
    int i = 0;
    while(result[i] != '\0'){
        ans += result[i];
        ++i;
    }
    EXPECT_EQ("0.0.0.0", ans);
}

TEST(ChecksumTest, EvenSizeBuffer) {
    uint16_t buffer[] = {0x1234, 0x5678, 0x9abc, 0xdef0};
    size_t size = sizeof(buffer) / sizeof(buffer[0]);
    uint16_t result = checksum(buffer, size);
    EXPECT_EQ(38739, result);
}

TEST(ChecksumTest, OddSizeBuffer) {
    uint16_t buffer[] = {0x1234, 0x5678, 0x9abc, 0xdef0, 0x1111};
    size_t size = sizeof(buffer) / sizeof(buffer[0]);
    uint16_t result = checksum(buffer, size);
    EXPECT_EQ(38551, result);
}

TEST(ChecksumTest, SingleElementBuffer) {
    uint16_t buffer[] = {0x1234};
    size_t size = sizeof(buffer) / sizeof(buffer[0]);
    uint16_t result = checksum(buffer, size);
    EXPECT_EQ(65483, result);
}

TEST(Traceroute, traceroute_hoster) {
    
    std::string ip = "178.172.160.100";
    
    Params params;
    params.dns_resolving = false;

    EXPECT_NO_THROW(traceroute(ip, params));
}

TEST(Traceroute, traceroute_google) {
    
    std::string ip = "142.250.186.206";
    
    Params params;
    params.queries = 3;

    EXPECT_NO_THROW(traceroute(ip, params));
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

}