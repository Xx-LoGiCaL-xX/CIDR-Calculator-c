# CIDR Calculator

> Given an IPv4 address and prefix length, print the network and broadcast addresses, the usable host range, the host count, and the subnet mask.

## 
It should:
- Accept prompt input like `192.168.1.10/24`
- Print: network address, broadcast address, subnet mask (dotted decimal), first and last usable host, and total/usable host count.
- Handle prefix lengths 0-32.
- Reject malformed input with a clear message.

<br />

##
Example Output:

```
.\CIDR_Calculator.exe
Enter the IP adress with prefix length (e.g., 192.168.10.0/24): 172.16.55.77/22


Network Address: 172.16.52.0
Broadcast Address: 172.16.55.255
Subnet mast: 255.255.252.0
First usable host: 172.16.52.1
Last usable host: 172.16.55.254
Total usable addresses: 1022
```
