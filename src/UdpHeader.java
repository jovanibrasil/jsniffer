public class UdpHeader extends Header {

    int sourcePort;
    int destinationPort;
    int checkSum;
    int length;

    public UdpHeader(int sourcePort, int destinationPort,
                     int checkSum, int length) {
        this.sourcePort = sourcePort;
        this.destinationPort = destinationPort;
        this.checkSum = checkSum;
        this.length = length;
    }

    @Override
    public String toString() {
        return "UdpHeader{" +
                "sourcePort=" + sourcePort +
                ", destinationPort=" + destinationPort +
                ", checkSum=" + checkSum +
                ", length=" + length +
                "}\n";
    }
}
