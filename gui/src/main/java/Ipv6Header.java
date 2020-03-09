public class Ipv6Header extends Header {

    public int version;
    public int trafficClass;
    public int flowLabel;
    public int payloadLength;
    public int nextHeader;
    public int hopLimit;
    public String sourceAddress;
    public String destinationAddress;
    public Header payload;

    public Ipv6Header(int version, int trafficClass, int flowLabel, int payloadLength,
                      int nextHeader, int hopLimit, String sourceAddress,
                      String destinationAddress, Header payload) {
        this.version = version;
        this.trafficClass = trafficClass;
        this.flowLabel = flowLabel;
        this.payloadLength = payloadLength;
        this.nextHeader = nextHeader;
        this.hopLimit = hopLimit;
        this.sourceAddress = sourceAddress;
        this.destinationAddress = destinationAddress;
        this.payload = payload;
    }

    @Override
    public String toString() {
        return "\n com.jovani.Ipv6Header" +
                "\n  version=" + version +
                "\n  trafficClass=" + trafficClass +
                "\n  flowLabel=" + flowLabel +
                "\n  payloadLength=" + payloadLength +
                "\n  nextHeader=" + nextHeader +
                "\n  hopLimit=" + hopLimit +
                "\n  sourceAddress='" + sourceAddress + '\'' +
                "\n  destinationAddress='" + destinationAddress + '\'' +
                "\n";
    }
}
