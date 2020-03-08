public class TcpHeader extends Header {

    public int sourcePort;
    public int destinationPort;
    public int sequenceNumber;
    public int acknowledgmentNumber; // if ACK set

    public int dataOffset;
    public int reserved;
    public int ns;
    public int cwr;
    public int ece;
    public int ugr;
    public int ack;
    public int psh;
    public int rst;
    public int syn;
    public int fin;
    public int windowSize;

    public int checkSum;
    public int urgentPointer; // if urg set

    // options // if dataOffset > 5. // Padded at the end with 0 if necessary

    public TcpHeader(int sourcePort, int destinationPort, int sequenceNumber, int acknowledgmentNumber,
                     int dataOffset, int reserved, int ns, int cwr, int ece, int ugr, int ack, int psh,
                     int rst, int syn, int fin, int windowSize, int checkSum, int urgentPointer) {
        this.sourcePort = sourcePort;
        this.destinationPort = destinationPort;
        this.sequenceNumber = sequenceNumber;
        this.acknowledgmentNumber = acknowledgmentNumber;
        this.dataOffset = dataOffset;
        this.reserved = reserved;
        this.ns = ns;
        this.cwr = cwr;
        this.ece = ece;
        this.ugr = ugr;
        this.ack = ack;
        this.psh = psh;
        this.rst = rst;
        this.syn = syn;
        this.fin = fin;
        this.windowSize = windowSize;
        this.checkSum = checkSum;
        this.urgentPointer = urgentPointer;
    }

    @Override
    public String toString() {
        return "TcpHeader{" +
                "sourcePort=" + sourcePort +
                ", destinationPort=" + destinationPort +
                ", \nsequenceNumber=" + sequenceNumber +
                ", \nacknowledgmentNumber=" + acknowledgmentNumber +
                ", dataOffset=" + dataOffset +
                ", reserved=" + reserved +
                ", ns=" + ns +
                ", cwr=" + cwr +
                ", ece=" + ece +
                ", ugr=" + ugr +
                ", ack=" + ack +
                ", psh=" + psh +
                ", rst=" + rst +
                ", syn=" + syn +
                ", fin=" + fin +
                ", windowSize=" + windowSize +
                ", \ncheckSum=" + checkSum +
                ", urgentPointer=" + urgentPointer +
                "}\n";
    }
}
