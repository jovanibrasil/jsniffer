public class Sniffer {

	static {
		System.loadLibrary("native"); // loads the "native" shared library	
	}

	public native void run();
	public native String[] getPrintBuffer();
	public native Statistics getStatistics();
	public native Ipv4Header[] getPacketsFromBuffer(int quantity);

}
