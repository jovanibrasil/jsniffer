import javax.swing.SwingUtilities;

public class Main {

	public static void main(String[] args) {
		Sniffer sniffer = new Sniffer(); // ADD must use singleton pattern
		SwingUtilities.invokeLater(() -> {
			SnifferFrame frame = new SnifferFrame("Sniffer", sniffer);
		});
	}
	
}
