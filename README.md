
# CaPOS: A simple virtual machine

## Emory Hufbauer
## Project for CS485G Section 001: Systems Programming

### Contents:

+ This README
+ The capos.c source file
+ A makefile
+ A directory containing five test cases (see *Tests* below) 

### Usage:

Running `make` in the main directory should build the project on most Unix OSs. It will generate a single executable file, called `capos`. It executes a .capexe binary file and prints its output:

	`./capos file.capexe`

### Purpose:

This program was created to execute a binary file representing a program created using a simplified instruction set specified by the professor, which he called CaP. It includes arithmetic, printing, and jumping instructions, with support for variables and constants. CaP programs can be written in a human-readable assembly-like format called capsrc, and then compiled into capexe binaries by the capcc program provided by the professor.

### Design:

Instead of reading and executing the binary simultaneously, this program first reads the binary and parses it into a data structure, and then interprets that data structure. This both separates the file-parsing logic from the execution logic, making both simpler, and speeds up iteration operations by removing dependency on the file system when repeatedly executing instructions.

This program loads both variables and constants into memory at parse time, and replaces references to them in the executable data structure with pointers to the memory that they are contained in. This way, all distinction between constants and variables is done at parse time, allowing the run-time logic to be greatly simplified. Because constants are stored in a statically allocated array, the memory capacity for constants, like that for variables, is limited to 32kb. This makes it possible to store 8192 variables and 8192 contstants simutaneously. If more than 8192 constants or a variable with an address greater than 8191 are created, the program will issue a memory overflow error and abort.

This implementation also uses `uint32_t`s instead of ordinary unsigned ints, making memory behavior more consistent across platforms, and providing a data type well-equipped to store the capexe format's native 32-bit words.

Variables are initialized and set to 0 at parse time.

The printchars instruction in this implementation reads a 32-bit number as a list of four characters. It does this by using a bitmask to isolate the relevant bits of the number, and then shifts them to the right and casts them to `char`s. Example: Reading the second-from-the-right character of a 32-bit number:

	0x57696e64
	0x00006e00 & 0x0000ff00
		=0x00006e00
	0x00006e00 >> 8
		=0x6e
	(char) 0x6e
		i

### Tests:

This project contains five test cases in the directory 'tests'. Each test consists of a .capexe file to be run, the source for that file (.capsrc), and a file containing the expected output (.out). Short descriptions of the tests are listed below:

+ Test 1: A simple arithmetic test from the project description.
+ Test 2: A test for the printchars function.
+ Test 3: A long, randomly generated arithmetic test.
+ Test 4: A jump statement test from the project description.
+ Test 5: An implementation of Euclid's GCD algorithm using jump statements.
