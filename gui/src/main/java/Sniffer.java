import java.io.File;

public class Sniffer {

	static {
		try {
			File lib = new File("libs/" + System.mapLibraryName("core"));
			System.load(lib.getAbsolutePath());
		} catch (UnsatisfiedLinkError err) {
			// todo show message
		}
	}

	public native void run();
	public native String[] getPrintBuffer();
	public native Statistics getStatistics();
	public native EthernetHeader[] getPacketsFromBuffer(int quantity);

}
