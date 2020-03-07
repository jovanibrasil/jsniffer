import javax.swing.SwingUtilities;

public class Main {

	public static void main(String[] args) {
		UdpHeader udpHeader = new UdpHeader(0, 0, 0, 0);

		Sniffer sniffer = new Sniffer(); // ADD must use singleton pattern
		SwingUtilities.invokeLater(() -> {
			SnifferFrame frame = new SnifferFrame("Sniffer", sniffer);
		});
	}
	
}