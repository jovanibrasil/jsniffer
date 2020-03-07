public class Ipv4Header {

    protected int totalLength;
    protected Header header;

    public Ipv4Header(int totalLength, Header header) {
        this.totalLength = totalLength;
        this.header = header;
    }

    @Override
    public String toString() {
        return "Ipv4Header{" +
                "totalLength=" + totalLength +
                ", header=" + header +
                "}\n";
    }
}
