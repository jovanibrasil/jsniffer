generate-headers:
	javac -h . Sniffer.java
	javac SnifferFrame.java Main.java

compile:
	#g++ -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux Sniffer.cpp -o Sniffer.o
	gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux Sniffer.c -o Sniffer.o

# Include the compiled code into a shared library
link:
	#g++ -shared -fPIC -o libnative.so Sniffer.o -lc
	gcc -shared -fPIC -o libnative.so Sniffer.o -lc

clean:
	rm *.class *.so *.o	*.log

build: generate-headers compile link

# The user must be root
run:
	java -cp . -Djava.library.path=./ Main