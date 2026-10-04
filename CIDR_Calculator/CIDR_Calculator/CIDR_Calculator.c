#include <stdio.h>
#include <math.h>


// Function Prototypes
long long int ip2decimal(int* ip);
void decimal2ip(long long int decimal_ip, int* ip);
void set_network_mask(int* net_mask, int suffix);
void obtain_network_ip(int* net_mask, int* network_ip, int* original);
void input_format_check(int* ipaddress, int prefix);



int main(void)
{
    int ip_octets[4] = { 0 }, prefix = 0, broadcast_address[4] = { 0 }, network_ip_address[4] = { 0 }, first_usable_ip_address[4] = {0}, last_usable_ip_address[4] = {0};
    unsigned long long int network_decimal, broadcast_decimal, first_available_decimal, last_available_decimal;
    unsigned long long int total_ips;
    int network_mask[4] = { 0 };
        

    printf_s("Enter the IP adress with prefix length (e.g., 192.168.10.0/24): ");
    scanf_s("%hu.%hu.%hu.%hu/%hu", &ip_octets[0], &ip_octets[1], &ip_octets[2], &ip_octets[3], &prefix);  //format specifier for unsigned short: h = short & u = unsigned

    //format check, exit if fail
    input_format_check(ip_octets, prefix);

    //obtain subnetmask and network address
    set_network_mask(network_mask, prefix);
    obtain_network_ip(network_mask, network_ip_address, ip_octets);

    //obtain total ips, network decimal, broadcast decimal,
    //first available decimal, and last available decimal
    total_ips = pow(2, 32 - prefix);    // form obtained from https://subnetplus.com/cidr-cheat-sheet.html
    network_decimal = ip2decimal(network_ip_address);
    broadcast_decimal = (network_decimal + total_ips) - 1;
    first_available_decimal = (network_decimal + 1 < broadcast_decimal) ? network_decimal + 1: network_decimal;    // test for Point-to-Point Link Exception (RFC 3021)
    last_available_decimal = (broadcast_decimal - 1 > network_decimal) ? broadcast_decimal - 1: broadcast_decimal; // test for Point-to-Point Link Exception (RFC 3021)

    // obtain broadcast, first available, last available IP addressess
    decimal2ip(broadcast_decimal, broadcast_address);
    decimal2ip(first_available_decimal, first_usable_ip_address);
    decimal2ip(last_available_decimal, last_usable_ip_address);

    // print out calculated results   
    printf_s("\n\nNetwork Address: %d.%d.%d.%d\n", network_ip_address[0], network_ip_address[1], network_ip_address[2], network_ip_address[3]);
    printf_s("Broadcast Address: %d.%d.%d.%d\n", broadcast_address[0], broadcast_address[1], broadcast_address[2], broadcast_address[3]);
    printf_s("Subnet mast: %d.%d.%d.%d\n", network_mask[0], network_mask[1], network_mask[2], network_mask[3]);
    printf_s("First usable host: %d.%d.%d.%d\n", first_usable_ip_address[0], first_usable_ip_address[1], first_usable_ip_address[2], first_usable_ip_address[3]);
    printf_s("Last usable host: %d.%d.%d.%d\n", last_usable_ip_address[0], last_usable_ip_address[1], last_usable_ip_address[2], last_usable_ip_address[3]);
    printf_s("Total usable addresses: %llu\n", (total_ips > 2) ? total_ips - 2 : total_ips);  // subtract network and broadcast and test for Point-to-Point Link Exception (RFC 3021)


    return 0;

}



long long int ip2decimal(int* ip)
{
    long long decimal_ip = 0;

    // ip to decimal formula:  (A x 256^3) + (B x 256^2) + (C x 256^1) + (D x 256^0)
    // source: https://interlir.com/2024/02/19/converting-ipv4-addresses-to-decimal-a-step-by-step-guide/
    decimal_ip = (ip[0] * pow(256, 3)) + (ip[1] * pow(256, 2)) + (ip[2] * pow(256, 1)) + (ip[3] * pow(256, 0));

    return decimal_ip;

}

void decimal2ip(long long int decimal_ip, int* ip)
{
    // decimal to ip formula source: https://freeipsubnetanalyzer.com/decimal-to-ip.html
    ip[0] = decimal_ip >> 24;
    ip[1] = (decimal_ip >> 16) & 255;
    ip[2] = (decimal_ip >> 8) & 255;
    ip[3] = decimal_ip & 255;

}

void set_network_mask(int* net_mask, int prefix)
{
    const int base_network_mask[4] = { 255, 255, 255, 255 };
    const long long int base_mask_decimal = 4294967295;
    long long int network_mask_decimal = (prefix == 0) ? 0 : (0xFFFFFFFF << (32 - prefix)) & 0xFFFFFFFF;

    decimal2ip(network_mask_decimal, net_mask);

}

void obtain_network_ip(int* net_mask, int* network_ip, int* original)
{
    // perform bitwise AND operation on corresponding original octets and
    // net mask octets to obtain network ip octets
    for (int octet = 0; octet < 4; octet++) {
        network_ip[octet] = original[octet] & net_mask[octet];
    }

}

void input_format_check(int* ipaddress, int prefix)
{
    if (prefix < 0 || prefix > 32)
    {
        printf_s("\nIncorrect CIDR prefix range format.\nPlease follow this format example: 192.168.100.0/24\n");
        exit(0);
    }

    for (int i = 0; i < 4; i++) {
        if (ipaddress[i] < 0 || ipaddress[i] > 255)
        {
            printf_s("\nIncorrect format for CIDR Address.\nPlease follow this format example: 192.168.100.0/24\n");
            exit(0);
        }
    }

}
