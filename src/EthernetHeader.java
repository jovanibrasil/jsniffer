public class EthernetHeader {

    public String destMac;
    public String sourceMac;
    public int etherType;
    public Header payload;

    public EthernetHeader(String destMac, String sourceMac, int etherType, Header payload) {
        this.destMac = destMac;
        this.sourceMac = sourceMac;
        this.etherType = etherType;
        this.payload = payload;
    }

    @Override
    public String toString() {
        return "EthernetHeader{" +
                "destMac='" + destMac + '\'' +
                ", sourceMac='" + sourceMac + '\'' +
                ", etherType=" + etherType +
                "}\n";
    }
}
