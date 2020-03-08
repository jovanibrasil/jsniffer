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
        return "Ipv6Header{" +
                "version=" + version +
                ", trafficClass=" + trafficClass +
                ", flowLabel=" + flowLabel +
                ", payloadLength=" + payloadLength +
                ", nextHeader=" + nextHeader +
                ", hopLimit=" + hopLimit +
                ", sourceAddress='" + sourceAddress + '\'' +
                ", destinationAddress='" + destinationAddress + '\'' +
                "}\n";
    }
}
