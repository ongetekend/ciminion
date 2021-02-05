# CIMINION Implementation


Reference implementation of CIMINION in C++. 

The code was tested in Unix-like systems, and the following instructions are provided for Unix only.

## Dependencies

The source code depends on three external librairies to build in Unix-like systems. 

The prerequisities are having:
- ```sudo``` rights,

- ```G++``` tool,

- ``` GNU make ``` tool, and

- ``` xsltproc ``` tool.

The librairies to install are:

- The Multiprecision arithmetic (used by NTL) depends on the GMP 6.2.0 librairies (most Unix distributions come with GMP installed).

	- Download: https://gmplib.org/

	- How to install: https://gmplib.org/manual/Installing-GMP#Installing-GMP 

- The finite field arithmetic depends on the library NTL 11.4.3. Its source code and installation instructions can be found at: 

	- Download: https://www.shoup.net/ntl/download.html
	
	- How to install: https://www.shoup.net/ntl/doc/tour-unix.html 

- The SHAKE256 implementation is taken from: 

    - Download: https://github.com/XKCP/XKCP
	
	- How to install: https://github.com/XKCP/XKCP/blob/master/README.markdown


## Compilation

To compile the code:

- first go the ```reference/``` directory,

- build with 

	```
	make && make install
	```

- then run the binary with 

	``` 
	./bin/main 
	```

To clean up the binaries and object files:

``` 
make clean 
```


## Changing order finite field

The order of the finite field can be changed.

For that:
 - go to ./src/main.ccp 

 - change the relevant variables in the test_ZZ_* methods
 
 - compile
 

## Licence

The source code is distributed under the licence CC0.
See the file LICENSE for more details.
