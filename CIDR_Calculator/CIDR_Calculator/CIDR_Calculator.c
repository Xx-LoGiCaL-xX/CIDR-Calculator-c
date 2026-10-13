#include <stdio.h>
#include <stdlib.h>
#include <math.h>


// Function Prototypes
long long int ip2decimal(int* ip);
void decimal2ip(long long int decimal_ip, int* ip);
void set_network_mask(int* net_mask, int suffix);
void obtain_network_ip(int* net_mask, int* network_ip, int* original);
void input_format_check(int* ipaddress, int prefix);
void init_range_data(struct cidr_range* cidr);
void deallocate_space(struct cidr_range* cidr);

struct cidr_range {
    int* range_octets;
    int* broadcast_address;
    int* network_ip_address;
    int* first_usable_ip_address;
    int* last_usable_ip_address;
    int* network_mask;
    int prefix;
    unsigned long long int network_decimal;
    unsigned long long int broadcast_decimal;
    unsigned long long int first_available_decimal;
    unsigned long long int last_available_decimal;
    unsigned long long int total_ips;    
};

int main(void)
{
    struct cidr_range c_range;
    init_range_data(&c_range);

    //prompt for initial cidr range octets and parse values
    printf_s("Enter the IP adress with prefix length (e.g., 192.168.10.0/24): ");
    scanf_s("%d.%d.%d.%d/%d",
        &c_range.range_octets[0],
        &c_range.range_octets[1],
        &c_range.range_octets[2],
        &c_range.range_octets[3],
        &c_range.prefix
    );

    //format check, exit if fail
    input_format_check(c_range.range_octets, c_range.prefix);

    //obtain subnetmask and network address
    set_network_mask(c_range.network_mask, c_range.prefix);
    obtain_network_ip(c_range.network_mask, c_range.network_ip_address, c_range.range_octets);

    //obtain total ips, network decimal, broadcast decimal,
    //first available decimal, and last available decimal
    c_range.total_ips = pow(2, 32 - c_range.prefix);    // form obtained from https://subnetplus.com/cidr-cheat-sheet.html
    c_range.network_decimal = ip2decimal(c_range.network_ip_address);
    c_range.broadcast_decimal = (c_range.network_decimal + c_range.total_ips) - 1;  

    c_range.first_available_decimal = (c_range.network_decimal + 1 < c_range.broadcast_decimal)  // test for Point-to-Point Link Exception (RFC 3021)
        ? c_range.network_decimal + 1
        : c_range.network_decimal;

    c_range.last_available_decimal = (c_range.broadcast_decimal - 1 > c_range.network_decimal)   // test for Point-to-Point Link Exception (RFC 3021)
        ? c_range.broadcast_decimal - 1
        : c_range.broadcast_decimal;

    // obtain broadcast, first available, last available IP addressess
    decimal2ip(c_range.broadcast_decimal, c_range.broadcast_address);
    decimal2ip(c_range.first_available_decimal, c_range.first_usable_ip_address);
    decimal2ip(c_range.last_available_decimal, c_range.last_usable_ip_address);

    // print out calculated results       
    printf_s("\n\nNetwork Address: %d.%d.%d.%d\n",
        c_range.network_ip_address[0],
        c_range.network_ip_address[1],
        c_range.network_ip_address[2],
        c_range.network_ip_address[3]
    );
   
    printf_s("Broadcast Address: %d.%d.%d.%d\n",
        c_range.broadcast_address[0],
        c_range.broadcast_address[1],
        c_range.broadcast_address[2],
        c_range.broadcast_address[3]
    );
    
    printf_s("Subnet mast: %d.%d.%d.%d\n",
        c_range.network_mask[0],
        c_range.network_mask[1],
        c_range.network_mask[2],
        c_range.network_mask[3]
    );
    
    printf_s("First usable host: %d.%d.%d.%d\n",
        c_range.first_usable_ip_address[0],
        c_range.first_usable_ip_address[1],
        c_range.first_usable_ip_address[2],
        c_range.first_usable_ip_address[3]
    );
    
    printf_s("Last usable host: %d.%d.%d.%d\n",
        c_range.last_usable_ip_address[0],
        c_range.last_usable_ip_address[1],
        c_range.last_usable_ip_address[2],
        c_range.last_usable_ip_address[3]
    );

    printf_s("Total usable addresses: %llu\n", (c_range.total_ips > 2)
        ? c_range.total_ips - 2  // subtract network and broadcast and test for Point-to-Point Link Exception (RFC 3021)
        : c_range.total_ips);

    deallocate_space(&c_range);


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

void init_range_data(struct cidr_range* cidr)
{
    // allocate memory for ip arrays
    cidr->range_octets = malloc(4 * sizeof(int));
    cidr->broadcast_address = malloc(4 * sizeof(int));
    cidr->network_ip_address = malloc(4 * sizeof(int));
    cidr->first_usable_ip_address = malloc(4 * sizeof(int));
    cidr->last_usable_ip_address = malloc(4 * sizeof(int));
    cidr->network_mask = malloc(4 * sizeof(int));

    //  set values or arrays to 0
    memset(cidr->range_octets, 0, 4 * sizeof *cidr->range_octets);    
    memset(cidr->broadcast_address, 0, 4 * sizeof *cidr->broadcast_address);
    memset(cidr->network_ip_address, 0, 4 * sizeof *cidr->network_ip_address);
    memset(cidr->first_usable_ip_address, 0, 4 * sizeof *cidr->first_usable_ip_address);
    memset(cidr->last_usable_ip_address, 0, 4 * sizeof *cidr->last_usable_ip_address);
    memset(cidr->network_mask, 0, 4 * sizeof * cidr->network_mask);

}

void deallocate_space(struct cidr_range* cidr)
{
    free(cidr->range_octets);
    free(cidr->broadcast_address);
    free(cidr->network_ip_address);
    free(cidr->first_usable_ip_address);
    free(cidr->last_usable_ip_address);
    free(cidr->network_mask);
}
