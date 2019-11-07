
generate-headers:
	javac -h . HelloWorldJNI.java
	
compile:
	#g++ -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux HelloWorldJNI.cpp -o HelloWorldJNI.o
	gcc -c -fPIC -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux HelloWorldJNI.c -o HelloWorldJNI.o

# Include the compiled code into a shared library	
link:
	#g++ -shared -fPIC -o libnative.so HelloWorldJNI.o -lc
	gcc -shared -fPIC -o libnative.so HelloWorldJNI.o -lc
	
run:
	java -cp . -Djava.library.path=./ HelloWorldJNI
	
clean:
	rm *.class *.so *.o	*.log

#run-complete: generate-headers compile link run
#	echo "BUILDING ..."