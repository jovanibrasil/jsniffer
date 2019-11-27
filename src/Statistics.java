public class Statistics {

    // Packages counters by type
    public int totalPackets;
    public int countIpv4;
    public int countIpv6;
    public int countUdp;
    public int countTcp;
    public int countIcmp4;
    public int countIcmp6;
    public int countArp;
    public int countEthernet;

    @Override
    public String toString() {
        return "Statistics{" +
                "total_packets=" + totalPackets +
                ", \n   count_ipv4=" + countIpv4 +
                ", \n   count_ipv6=" + countIpv6 +
                ", \n   count_udp=" + countUdp +
                ", \n   count_tcp=" + countTcp +
                ", \n   count_icmp4=" + countIcmp4 +
                ", \n   count_icmp6=" + countIcmp6 +
                ", \n   count_arp=" + countArp +
                ", \n   count_ethernet=" + countEthernet +
                "\n}";
    }
}