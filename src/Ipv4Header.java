public class Ipv4Header extends Header {

    public int version;
    public int ihl; // internet header length
    public int dscp;
    public int ecn;
    public int totalLength;
    public int identification;
    public int flags;
    public int fragmentOffset;
    public int timeToLive;
    public int protocol;
    public int headerChecksum;
    public String sourceIpAddress;
    public String destIpAddress;
    public int options; // if IHL > 5
    public Header payload;

    public Ipv4Header(int version, int ihl, int dscp, int ecn, int totalLength,
                      int identification, int flags, int fragmentOffset,
                      int timeToLive, int protocol, int headerChecksum,
                      String sourceIpAddress, String destIpAddress, int options,
                      Header payload) {
        this.version = version;
        this.ihl = ihl;
        this.dscp = dscp;
        this.ecn = ecn;
        this.totalLength = totalLength;
        this.identification = identification;
        this.flags = flags;
        this.fragmentOffset = fragmentOffset;
        this.timeToLive = timeToLive;
        this.protocol = protocol;
        this.headerChecksum = headerChecksum;
        this.sourceIpAddress = sourceIpAddress;
        this.destIpAddress = destIpAddress;
        this.options = options;
        this.payload = payload;
    }

    @Override
    public String toString() {
        return "\n Ipv4Header" +
                "\n  version=" + version +
                "\n  ihl=" + ihl +
                "\n  dscp=" + dscp +
                "\n  ecn=" + ecn +
                "\n  totalLength=" + totalLength +
                "\n  identification=" + identification +
                "\n  flags=" + flags +
                "\n  fragmentOffset=" + fragmentOffset +
                "\n  timeToLive=" + timeToLive +
                "\n  protocol=" + protocol +
                "\n  headerChecksum=" + headerChecksum +
                "\n  sourceIpAddress='" + sourceIpAddress + '\'' +
                "\n  destIpAddress='" + destIpAddress + '\'' +
                "\n  options=" + options +
                "\n  payload=" + payload +
                '\n';
    }
}
