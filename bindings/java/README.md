This is the java bindings for Secure ECC-based DID intersection.

You should install java SDK first.
and change `JAVA_HOME` of the Makefile to be your own JAVA SDK path.

Run the following command:
```
make
```
It uses gcc to compile the source code and generates the libSecureUnionID.so on Linux
or libSecureUnionID.dylib on Mac.
```
make build
```
It uses java command to compile the java source code into java classes.
```
make run
```
It runs the demo.