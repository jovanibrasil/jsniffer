public class Ipv4Header extends Header {

    public int totalLength;
    public Header payload;

    public Ipv4Header(int totalLength, Header payload) {
        this.totalLength = totalLength;
        this.payload = payload;
    }

    @Override
    public String toString() {
        return "Ipv4Header{" +
                "totalLength=" + totalLength +
                ", header=" + payload +
                "}\n";
    }
}
