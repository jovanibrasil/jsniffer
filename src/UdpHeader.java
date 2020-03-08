public class UdpHeader extends Header {

    public int sourcePort;
    public int destinationPort;
    public int checkSum;
    public int length;

    public UdpHeader(int sourcePort, int destinationPort,
                     int checkSum, int length) {
        this.sourcePort = sourcePort;
        this.destinationPort = destinationPort;
        this.checkSum = checkSum;
        this.length = length;
    }

    @Override
    public String toString() {
        return "\n UdpHeader" +
                "\n  sourcePort=" + sourcePort +
                "\n  destinationPort=" + destinationPort +
                "\n  checkSum=" + checkSum +
                "\n  length=" + length +
                "\n";
    }
}
