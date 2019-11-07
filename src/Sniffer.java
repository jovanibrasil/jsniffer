
public class Sniffer {
	static {
		System.loadLibrary("native"); // loads the "native" shared library	
	}

	public static void main(String[] args) {
		// new Sniffer().sayHello();
		
		// System.out.println("2 + 2 = " + new Sniffer().sumIntegers(2, 2));
		// System.out.println(new Sniffer().sayHelloToMe("jovani", false));
		
		// Sniffer helloJni = new Sniffer();
		// helloJni.createUser("Jovani", 1000);
		// UserData ud = new UserData();
		// ud.name = "Jovani Brasil";
		// ud.balance = 1000;
		// System.out.println(helloJni.printUserData(ud));;
		new Sniffer().run();
		
	}

	// Declare a native method sayHello() that receives no arguments and returns
	// void
	// private native void sayHello();
	// private native long sumIntegers(int first, int second);
    // private native String sayHelloToMe(String name, boolean isFemale);
	
    // public native UserData createUser(String name, double balance);
    // public native String printUserData(UserData user);

	public native void run();

    
}
