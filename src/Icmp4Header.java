public class Icmp4Header extends Header {

    public int type;
    public int code;
    public int checksum;

    public Icmp4Header(int type, int code, int checksum) {
        this.type = type;
        this.code = code;
        this.checksum = checksum;
    }

    @Override
    public String toString() {
        return "\n Icmp4Header" +
                "\n  type=" + type +
                "\n  code=" + code +
                "\n  checksum=" + checksum +
                "\n";
    }
}
