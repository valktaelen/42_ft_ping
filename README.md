# 42_ft_ping
This project is about recoding the ping command

## Usage

### Compilation

```bash
make
```

### Execute

*Warning*, to execute we need to priviledge user (for open socket).

```bash
sudo ./ft_ping [args ...]
```

```
Usage
ft_ping [options] <destination>

Options:
        <destination>   dns name or ip address
        -v              verbose output
        -h              help
        -c              count <integer>
        -t              time to live <integer>
        -W              timeout <integer in second>
        -w              deadline <integer in second>
        -q              quiet
        -4              ipv4 only
```

## How it works

To do a ping, we need ICMP ([Internet Control Message Protocol](https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol)), which is a network layer protocol that is an integral part of the Internet Protocol (IP) suite.
The main purpose of using ICMP in the ping command is to check the reachability of a destination host. By sending ICMP Echo Request messages, the source host can determine if the destination host is responsive.


Information Obtained from ICMP Packet:
- ICMP packets, particularly Echo Request and Echo Reply messages, provide valuable information about the host:
	- Host Reachability: An unanswered Echo Request indicates that the host might be unreachable or there might be network issues.
	- Round-Trip Time (RTT): By measuring the time it takes for an Echo Request to travel to the destination and back in the Echo Reply, ping can calculate the Round-Trip Time. RTT is a key metric for network responsiveness.
