
generate-headers:
	javac -h . Sniffer.java
	
compile:
	#g++ -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux Sniffer.cpp -o Sniffer.o
	gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux Sniffer.c -o Sniffer.o

# Include the compiled code into a shared library	
link:
	#g++ -shared -fPIC -o libnative.so Sniffer.o -lc
	gcc -shared -fPIC -o libnative.so Sniffer.o -lc
	
run:
	java -cp . -Djava.library.path=./ Sniffer
	
clean:
	rm *.class *.so *.o	*.log

#run-complete: generate-headers compile link run
#	echo "BUILDING ..."