public class ArpHeader extends Header {

    public int htype;// hardware type
    public int ptype;// protocol type
    public int hlen;// hardware address length
    public int plen;// protocol address length
    public int oper; // operation
    public int sha; // sender hardware address
    public int spa; // sender protocol address
    public int tha; // target hardware address
    public int tpa; // target protocol address

    public ArpHeader(int htype, int ptype, int hlen, int plen,
                     int oper, int sha, int spa, int tha, int tpa) {
        this.htype = htype;
        this.ptype = ptype;
        this.hlen = hlen;
        this.plen = plen;
        this.oper = oper;
        this.sha = sha;
        this.spa = spa;
        this.tha = tha;
        this.tpa = tpa;
    }

    @Override
    public String toString() {
        return "\n ArpHeader" +
                "\n  htype=" + htype +
                "\n  ptype=" + ptype +
                "\n  hlen=" + hlen +
                "\n  plen=" + plen +
                "\n  oper=" + oper +
                "\n  sha=" + sha +
                "\n  spa=" + spa +
                "\n  tha=" + tha +
                "\n  tpa=" + tpa +
                "\n";
    }
}
