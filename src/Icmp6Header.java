public class Icmp6Header {

    public int type;
    public int code;
    public int checksum;

    public Icmp6Header(int type, int code, int checksum) {
        this.type = type;
        this.code = code;
        this.checksum = checksum;
    }

    @Override
    public String toString() {
        return "\n Icmp6Header" +
                "\n  type=" + type +
                "\n  code=" + code +
                "\n  checksum=" + checksum +
                "\n";
    }
}
