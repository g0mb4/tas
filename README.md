Toy Two Pass Assembler
======================
The project is based on Yonatan Zilpa's excersie. A brief explanation can be found [here](https://www.magmath.com/english/programming/c_programming_language/projects/two_pass_assembler.php).
The majority of the following is just a direct copy of that site.
Some differences:
+ I'm using hexadecimal (base 16) numeric system insted of octal.
+ .entry MAIN needed to be defined explicitly.
+ There wont be any generated .ent/.ext files, if they are not needed.

Documentation can be found [here](https://g0mb4.github.io/tas/).

# "Hardware"
Our computer architecture consists from CPU (Central Processing Unit), registers and Random Access Memory RAM, where part of the memory is being used as a stack. The size of each word in memory is 16 bits. Arithmetics is to be carried by the '2's complement' method. Our computer machine can only handle integers (Positives or negatives), it doesn't handle real numbers.

## Registers
Our computer machine includes the following list of registers:
+ Eight general registers (**r0**, **r1**, **r2**, **r3**, **r4**, **r5**, **r6**, **r7**)
+ One Program Counter register (**PC**).
+ One Stack Pointer register (**SP**).
+ One Status register (**PSW** - Program Status Word) which has two flags: **carry flag** and **zero flag**.

All registers are 16 bits in size.
The two first bits of the PSW register are C and Z in correspondence
The size of memory is 2000 words (each word is 16 bits in size).
Characters are coded in ASCII.

# Instructions
In our computer machine, instruction is a word (16 bits in size) that carries information about the operator and operands. Although instruction is a string of 16 bits, it can be divided into fields. The following table provides further information about the instruction. The bits are in decimal number system.

| Fields | Operation | Source Operand            ||  Destination Operand      ||
| ------ | --------- | --------------- | ---------| ----------------| -------- |
|        |           | Addressing Mode | Register | Addressing Mode | Register |
| Bits   | 15-12     | 11-9            | 8-6      | 5-3             | 2-0      |

The following table maps operator's name to its corresponding instruction code (opcode).

| Operator     | Opcode |
| ------------ | ------ |
|  ``` mov ``` | 0 	    |
|  ``` cmp ``` | 1	    |
|  ``` add ``` | 2	    |
|  ``` sub ``` | 3	    |
|  ``` mul ``` | 4	    |
|  ``` div ``` | 5	    |
|  ``` lea ``` | 6	    |
|  ``` inc ``` | 7	    |
|  ``` dec ``` | 8	    |
|  ``` jnz ``` | 9	    |
|  ``` jnc ``` | a	    |
|  ``` shl ``` | b	    |
|  ``` prn ``` | c	    |
|  ``` jsr ``` | d	    |
|  ``` rts ``` | e	    |
|  ``` hlt ``` | f	    |

All operators are written in lower case letters, details on the meaning of these operators will be specified later.

+ **Bits 9-11**: This field refers to the addressing mode of the source operand. Depending on the value of this field (numeric values of bits 9-11) , the instruction may refer to additional word (first additional word)
+ **Bits 6-8**: This field refers to the register of the source operand. The field (bits 6-8) maps its numeric value n to register rn.

   *Notice*: If the addressing mode in the source operand does not require the source register, then the source register field are not in use. In such a case the numeric value of the field (bits 6-8) is equal to zero.
+ **Bits 3-5**: This field refers to the addressing mode of the destination operand. Depending on the numeric value of this field (bits 3-5) , the instruction may refer to additional word (second additional word)
+ **Bits 0-2**: This field refers to the register of the destination operand. The field (bits 0-2) maps its numeric value n to register rn.

   *Notice*: If the addressing mode in the destination operand does not require the destination register, then the source register field are not in use. In such a case the numeric value of the field (bits 6-8) is equal to zero.

There are six types of addressing modes in our assembly language, some of these modes require additional information, i.e. additional word. The following table provides information on all types of addressing mode.

| First Word                                        ||| Additional Word | Operand | Way of Writing | &nbsp;&nbsp;Example&nbsp;&nbsp; |
| ----------- | ------------------ | ---------------- | --------------- | ------- | -------------- | ------- |
| Field Value | Name               | Register         |                 |         |                |         |
| 0 | Instant addressing | zero (not in use) | yes | The numeric value of the operand is determined by the numeric value of the additional word. | The operand is a number preceded by the '#' sign. | ``` mov #-1,r2 ``` |
| 1 | Direct addressing | zero (not in use) | yes |	The additional word contains memory address. The numeric value of the operand is the value of this address. | The operand is a label, either declared or expected to be declared later in the file.	| ``` mov x,r2 ``` |
| 2 | Indirect addressing | zero (not in use) | yes | The numeric value of the additional word contains memory address. The value of this address is also a memory address. The value of the second address is the numeric value of the operand. | Indirect addressing is indicated by the '@' sign which appeared just before the label. The label is declared in the same way as in the direct addressing mode. | ``` mov @x,r2 ``` |
| 3 | Relative addressing | zero (not in use) | yes | The additional word contains an integer number (positive or negative) indicating the distance in words from the current command to the address of the operand. | The operand is a label, defined as in the case of direct addressing mode, preceded by the '*' sign. | ``` mov *x,r2 ``` |
| 4 | Direct register addressing | n (positive integer) | no | Register rn contains the value of the operand. | The operand is a legal register name. | ``` mov r1,r2 ``` |
| 5 | Indirect register addressing | n (positive integer)) | no | Register rn contains information on memory address. This memory address contains the operand. | The operand is a legal register name indicated by the '@' sign. | ``` mov @r1,r2 ``` |

## Machine Instruction Characterization
Machine instruction may be classified into three different classes (according to the number of operands appeared in each instruction).

## First Class of Operators
The first class contains all machine instructions that get two operands. Any machine instruction that belongs to this class may contain one of the following operators:
```
        mov, cmp, add, sub, mul, div, lea, shl
```
The following table provides further explanation on the operational aspects of these operators:

| Numeric Code | Operator | Description | &nbsp;&nbsp;Example&nbsp;&nbsp;| Example Description |
| ------------ | -------- | ----------- | ------- | ------------------- |
| 0 | ``` mov ``` | Copies the value of the source operand (the first operand) to the destination operand (the second operand).	|  ``` mov A, r1 ``` | Copy the value of A to register r1. |
| 1	| ``` cmp ``` |	Compare between two operands. The cmp operator subtracts the destination operand from the source operand, without saving the subtraction result, it then updates the zero flag, flag z, in the status register, PSW. | ``` cpm A, r1 ``` | If the values of A and r1 are equal, then the zero flag A, in the status register PSW, is turned on. Else the zero flag is turned off. |
| 2 | ``` add ``` |	The destination operand is assigned with the value of the source operand plus the value of the destination operand. | ``` add A, r0 ``` | Register r0 gets the sum of r0 and A. |
| 3	| ``` sub ``` |	The destination operand is assigned with the value of the source operand minus the value of the destination operand. |  ``` sub #3, r1 ``` | Register r1 is assigned with the value of r1 minus 3. |
| 4 | ``` mul ``` | Destination operand assigned with the value of the source operand times the value of destination operand |  ``` mul A, r2 ``` | Register r2 assigned with A times r2. |
| 5 | ``` div ``` |	Destination operand is assigned with the value of destination operand divided by the source operand (destination divided by source) | ``` div A, r2 ``` | Register r2 assigned with r2/A. |
| 6 | ``` lea ``` |	Acronym for 'load effective address'. This operation loads memory address, marked with the label appeared in the first operand to the destination operand. |  ``` lea ABC, r1  ``` | The memory address of label ABC is assigned to register r1. |
| b | ``` shl ``` | Shift bits to the left in the source operand. The number of shifts is determined by the value of the destination operand. |  ``` shl r1, #1 ``` | Register r1 is shifted 1 bit to the left.

## Second Class of Operators
The second class contains all machine instructions that gets one operand. In such cases there is no source operand, thus bits 6-11 are meaningless (their values is zero). Any machine instruction in this class may contain one of the following instruction:
```
        inc, dec, jnz, jnc, prn, jsr
```
The following table provides further explanation on the operational aspects of these operators:

| Numeric Code | Operator | Description | &nbsp;&nbsp;Example&nbsp;&nbsp; | Example Description |
| ------------ | -------- | ----------- | ------- | ------------------- |
| 7 | ``` inc ``` | The operand is increased by one. |  ``` inc r2 ``` | Register r2 is assigned with r2 plus 1. |
| 8 | ``` dec ``` |	The operand is decreased by one. |  ``` dec r2 ``` | Register r2 is assigned with r2 minus 1. |
| 9 | ``` jnz ``` |	Acronym: jump if not zero. The Program Counter register PC is assigned with the source operand if the Z flag, in the Program Status Word register PSW is not zero. |  ``` jnz LINE ``` | If the Z flag (in the PSW register) is not zero, then PC register is assigned with LINE. |
| a | ``` jnc ``` |	Acronym: jump if not carry. The Program Counter register PC is assigned with zero if the C flag, in the Program Status Word register PSW is not 0. |  ``` jnc LINE ``` | If the C flag (in the PSW register) is not zero, then PC register is assigned with LINE. |
| c | ``` prn ``` |	Prints the ASCII equivalent of the operand to the standard output file (stdout). |  ``` prn r1 ``` | The ASCII equivalent character of the value stored in r1 is printed to standard file. |
| d | ``` jsr ``` | Calls a subroutine that pushes register PC to the running time stack and assign the operand to the Program Counter register PC.	|  ``` jsr FUNC ``` | stack[SP] = PC<br> SP = SP-1 <br> PC = FUNC |

## Third Class of Operators
The third class contains all machine instructions that gets no operands. In such cases bits 0-11 are meaningless (their values is zero). Any machine instruction in this class may contain one of the following instruction:
```
        rts, hlt
```
The following table provides further explanation on the operational aspects of these operators:

| Numeric Code | Operator | Description | &nbsp;&nbsp;Example&nbsp;&nbsp; | Example Description |
| ------------ | -------- | ----------- | ------- | ------------------- |
| e | ``` rts ``` | Pops a value from the running time stack and move this value to the Program Counter register. |  ``` rts ``` | SP = SP+1 <br> PC = stack[SP] |
| f | ``` hlt ``` | Halts the program. | ``` hlt  ``` | Halting the program.

## Legal addressing modes
The following table contains information on legal addressing mode for the source and destination operands.

| Operator  | Legal Addressing Modes for the Source Operand | Legal Addressing Modes for the Destination Operand |
| --------- | --------------------------------------------- | -------------------------------------------------- |
| ```mov``` | 0,1,2,3,4,5	                                | 1,2,3,4,5                                          |
| ```cmp``` | 0,1,2,3,4,5	                                | 0,1,2,3,4,5                                        |
| ```add``` | 0,1,2,3,4,5	                                | 1,2,3,4,5                                          |
| ```sub``` | 0,1,2,3,4,5	                                | 1,2,3,4,5                                          |
| ```mul``` | 0,1,2,3,4,5	                                | 1,2,3,4,5                                          |
| ```div``` | 0,1,2,3,4,5	                                | 1,2,3,4,5                                          |
| ```lea``` | 1	                                            | 1,2,3,4,5                                          |
| ```inc``` | No source operand	                            | 1,2,3,4,5                                          |
| ```dec``` | No source operand	                            | 1,2,3,4,5                                          |
| ```jnz``` | No source operand	                            | 1,2,3,5                                            |
| ```jnc``` | No source operand	                            | 1,2,3,5                                            |
| ```shl``` | 1,2,3,4,5	                                    | 0,1,2,3,4,5                                        |
| ```prn``` | No source operand	                            | 0,1,2,3,4,5                                        |
| ```jsr``` | No source operand	                            | 1,2,3,5                                            |
| ```rts``` | No source operand	                            | No source operand                                  |
| ```hlt``` | No source operand	                            | No source operand                                  |

# Statements
Our assembly language is consisted of statements separated by the new line character '\\n'. When we look into a file it appeared to be made out of lines of statements, each statement appeared in its own line.
Our assembly language has four types of statements. These statements described in the following table.

| Type of statement | General Explanation |
| ----------------- | ------------------- |
| Empty Statement | Line with this kind of statement may contains only white spaces: tab character '\\t' or space character ' ' |
| Comment Statement	| The first character in a line with this statement is the semicolon ';' character. This line should be completely ignored by the assembler. |
| Declarative Statement	| This statement is a directive to the assembler program. It does not generate machine instruction. |
| Operation Statement | This statement generates machine instruction that needs to be executed by the CPU. The statement represent machine instruction in symbolic form. |

## Directive Statement
Directive statement is of the following form:
Directive statement may optionally start with a label, the label has to follow certain syntax rules (to be described later). Directive can start with or without a label, in any case a directive name, preceded by a dot '.' character, must be included. NO whitespace allowed between the '.' character and the directive name. If the directive does include a label, then at least one whitespace character is separating between the label and the '.' character. Following the directive name, whitespace-separated, appearing, in the same line, the directive parameters (the number of parameters is determined by the type of the directive). As mentioned, directive statement may include four types of directive:

1. .data

    The parameter(s) of data is a list of legal numbers separated by a comma ',' character. For example:
```
.data    +7,-57 ,17   ,    9
```
    Notice that any number of whitespace characters may appear between the number(s) and the comma character(s). However, the comma character must separate between two numeric values.
    The '.data' directive statement directs the assembler to allocate space in its data image where the appropriate numeric parameters is to be stored. It also direct the assembler to advance the data counter by the number of parameters (of the '.data' directive). If the '.data' directive has a label name, then this label name is assigned with the value in the data image (before it was advanced) and get inserted to the symbols table. This way we can refer to certain place in the data image using the label name. For instance, if we write
```
XYZ:    .data   +7,-57,17,9
    mov 	XYZ, r1
```
    then register r1 is assigned with the value +7. If we continue to write
```
lea    XYZ, r1
```
    then r1 would have been assigned with the address (in the data image) that stores the +7 value.

2. .string

    The '.string' directive statement gets only one legal string as parameter. The meaning of '.string' directive statement is similar to the '.data' directive statement. The ASCII characters composed the string are coded to their appropriate numeric ASCII values) and get inserted to the data image by their order. At the end a zero value is being inserted, to mark the end of the string. The value of the data counter is to be increase, according to the length of the string + one. If the line includes a label name, then the value of the label name is going to point to the location in memory that stores the ASCII code of the first character of the string, at the same way as it was done for the '.data' string. For instance the directive statement
```
ABC:    .string    "abcdef"
```
    is going to allocate an array of characters of length 7 starting from the address stored in the ABC label name. This "array" is initialized to the ASCII value of characters 'a', 'b', 'c', 'd', 'e', 'f' in correspondence, the array is to be ended with the zero value concatenate to the end of the array.

3. .entry

    The '.entry' directive statement gets one parameter only. This parameter is a label name, declared by other directive statement in the very same file where the The purpose of the '.entry' directive statement is to deal handle cases where a label name defined in an assembly source file A needs to be referred by other assembly source file(s) B, C, D, etc. In this case the '.entry' directive statement, written in the file A, gets the label name as its parameter (the '.entry' directive statement has to have a single parameter). For instance, if an assembly source file A contains the following lines
```
.entry	HELLO
HELLO:  add		#1, r1
```
    then other assembly source file(s), may refer to HELLO label name. Notice that a label at the beginning of the '.entry' directive is meaningless.

4. .extern

    The '.extern' directive statement gets one parameter this parameter is the name of a label name defined in other assembly source file. The purpose of this directive statement is to declare that the label has been defined in other source file and that this assembly source file (the one that contains the '.extern' directive statement) is using it. The correspondence between the value of the label, as appeared in the source file where it was defined, and the operation instruction(s) that are using it as an argument is to be done at linking time.
```
.extern HELLO
```
    Notice that a label at the beginning of the '.extern' directive is meaningless.

## Operation Statement
Operation statement is composed from the following:

1. Optional label.

2. Operation name.

3. Operands (the number of operands may be 0, 1 or 2 depending on the operation).

The length of a statement (of any type) cannot exceed 80 characters.
The name of the operation is to be written in lower case letter, operation name can be one of the 16 operations mentioned above.
After the operation name, separated with whitespace character(s), one or two operands may appear. In the case of two operands, the operands are separated with a comma ',' character. As mentioned before, whitespace character(s) may separate the comma and the operands. Operation statement with two operands has the following form:

| Label           | Operation   | Operands                ||
| --------------- | ----------- | ---------- | ----------- |
|                 |             | Source     | Destination |
| ``` HELLO: ```  | ``` add ``` | ``` r7,``` | ``` B ```   |
| ``` JUMP: ```   | ``` jnc ``` |            | ``` XYZ ``` |
| ``` END: ```    | ``` hlt ``` |            |             |

# Formal Definitions

## Label
Every label must begin with an upper or lower case letter, the rest of the label may contain letters or numbers. The length of the label cannot exceed 30 characters. The label ends with a column ':' character. The column character is not part of the label name it is just a sign representing the end of the character. The label must begin with the first column of the line. Label name cannot have more than one definition. The following labels are written correctly.
```
        hEllo:
        x:
        He78940:
```
Label name cannot be the same as register or operation name.
The label derived its value from the syntax. Label written at the beginning of '.data' or '.string' directive gets the value of the appropriate data counter. Label written at the beginning of an operation statement gets the value of the appropriate operation counter.

## Number
Number is a string of decimal digits (0-9) that may optionally be preceded by either '-' or '+' sign. The number gets its value from its decimal representation represented by the string of digits. For instance the numbers
```
        76, -5, +123
```
can be accepted as numbers. As mentioned, we do not handle rational or real numbers, only integers.

## String
String is a sequence of visible ASCII characters surrounded by double quotation marks. The quotation marks are not part of the string. The string
```
        "Hello World"
```
is an example for legal string.

# Two Pass Assembler
When the assembler is starting to translate code it needs to carry two major assignments. Its first assignment is to identify and translate the operation code and its second assignment is to determine addresses for all data and variables appeared in the source file(s). For instance, when the assembler reads the following code:
```
                 .entry MAIN
        MAIN:    mov    LEN, r1
        	     lea    STR, r2
        LOOP:    jnz    END
        	     prn    @r2
        	     sub    #1, r1
        	     inc    r2
        	     jnc    *LOOP
        END:     hlt
        STR:     .string "abcdef"
        LEN:     .data 6
```

it has to replace the operation names mov, lea, jnz, prn, sub, inc, jnc, hlt with their equivalent binary codes, in addition, the assembler has to replace the symbols STR, LEN, MAIN, LOOP, END with their appropriate addresses that have been allocated for the directive statements.
Assuming that the code in example I has being translated by the assembler and has been stored (operations and directives) in a memory block that starts from address 0000, then this translation can be described as follow:

| Label         | Address | Command         | Operand(s)        | Machine Code |
| ------------- | ------- | --------------- | ----------------- | ------------ |
|               |         | ``` .entry ```  |  ``` MAIN ```     |              |
| ``` MAIN: ``` | 0000    | ``` mov ```     |  ``` LEN, r1 ```  | 0221         |
|               | 0001    |                 |                   | 0014         |
|               | 0002    | ``` lea ```     |  ``` STR, r2 ```  | 6222         |
|               | 0003    |                 |                   | 000d         |
| ``` LOOP: ``` | 0004    | ``` jnz ```     |  ``` END ```      | 9008         |
|               | 0005    |                 |                   | 000c         |
|               | 0006    | ``` prn ```     |  ``` @r2 ```      | c02a         |
|               | 0007    | ``` sub ```     |  ``` #1, r1 ```   | 3021         |
|               | 0008    |                 |                   | 0001         |
|               | 0009    | ``` inc ```     |  ``` r2 ```       | 7022         |
|               | 000a    | ``` jnc ```     |  ``` *LOOP ```    | a018         |
|               | 000b    |                 |                   | fffa         |
| ``` END: ```  | 000c    | ``` hlt ```     |                   | f000         |
| ``` STR: ```  | 000d    | ``` .string ``` |  ``` "abcdef" ``` | 0061         |
|               | 000e    |                 |                   | 0062         |
|               | 000f    |                 |                   | 0063         |
|               | 0010    |                 |                   | 0064         |
|               | 0011    |                 |                   | 0065         |
|               | 0012    |                 |                   | 0066         |
|               | 0013    |                 |                   | 0000         |
| ``` LEN: ```  | 0014    | ``` .data ```   |  ``` 6 ```        | 0006         |

If the assembler maintains a table of all the operation names and their corresponding binary codes, then all operation names can be easily converted. Whenever the assembler reads an operation name it can simply use the table to find its equivalent binary code. In order to carry the same conversion for the addresses of symbols the assembler has to build similar table.
For instance, in example I, prior to reading the source file(s) the assembler has no way to know that the LOOP symbol relates to address 0004.
Thus, in regards to all symbols that have been defined by the programmer, the assembler has to accomplish two separate tasks. The first task is to build a table of all symbols and their related numeric values, and the second is to replace all the symbols, appeared in the source file(s) with the numeric values of the address fields. This two assignments can be achieved by performing two separate scans (passes) on the source file(s). In the first pass the assembler builds a table of symbols, this table correspond address to each symbol.
In the second pass the assembler translate the source file(s) into binary machine code.
Notice that the two passes are done by the assembler, during translation (in the assembly time), before the linking process.
After the translation process, the program may be linked and load to memory for execution.

## First pass
In the first pass, each instruction is being substituted with its appropriate code and the table of symbols is being built. The rest of the code are left untouched. The code should be loaded at address zero. After applying the first pass on example I, we should get the following result

The table of symbols:

| Name | Value | Image       |
| ---- | ----- | ----------- |
| MAIN | 0000  | instruction |
| LOOP | 0004  | instruction |
| END  | 000c  | instruction |
| STR  | 0000  | data        |
| LEN  | 0007  | data        |

List of entries:

| Name | Value |
| ---- | ----- |
| MAIN | ????  |

Data image:

| Address | Value |
| ------- | ----- |
| 0000    | 0061  |
| 0001    | 0062  |
| 0002    | 0063  |
| 0003    | 0064  |
| 0004    | 0065  |
| 0005    | 0066  |
| 0006    | 0000  |
| 0007    | 0006  |

Instruction image:

| Address | Value |
| ------- | ----- |
| 0000    | 0221  |
| 0001    | ????  |
| 0002    | 6222  |
| 0003    | ????  |
| 0004    | 9008  |
| 0005    | ????  |
| 0006    | c02a  |
| 0007    | 3021  |
| 0008    | ????  |
| 0009    | 7022  |
| 000a    | a018  |
| 000b    | ????  |
| 000c    | f000  |

## Second pass
Applying the second pass on the code of example I yields the following final results:

| Name | Value | Image       |
| ---- | ----- | ----------- |
| MAIN | 0000  | object code |
| LOOP | 0004  | object code |
| END  | 000c  | object code |
| STR  | 000d  | object code |
| LEN  | 0014  | object code |

List of entries:

| Name | Value |
| ---- | ----- |
| MAIN | 0000  |

Object code:

| Address | Machine Word |
| ------- | ------------ |
| 0000    | 0221         |
| 0001    | 0014         |
| 0002    | 6222         |
| 0003    | 000d         |
| 0004    | 9008         |
| 0005    | 000c         |
| 0006    | c02a         |
| 0007    | 3021         |
| 0008    | 0001         |
| 0009    | 7022         |
| 000a    | a018         |
| 000b    | fffa         |
| 000c    | f000         |
| 000d    | 0061         |
| 000e    | 0062         |
| 000f    | 0063         |
| 0010    | 0064         |
| 0011    | 0065         |
| 0012    | 0066         |
| 0013    | 0000         |
| 0014    | 0006         |

When the assembler program is done an object code is generated this object code is to be sent to a linker program. The purpose of the linker program is described as follows:

1. To allocate the program with place in memory (allocation).
2. To link the object file into one executable file (linking)
3. To change addresses according to the loading place (relocation)
4. To physically load the code into memory.

After the linker program is done the program can be loaded to memory and is ready to run. We are not going to make further discussion on how the linker program works.

# The format of output files
The object file written by the assembler provides informations about machine's memory. The first instruction is to be inserted to memory address 0, the second instruction is to be inserted to be inserted to memory address 2,3 or 4 (depending on the length of the first instruction) and so fourth until the translation of the last instruction. The next memory address, after the last translated instruction, contains the data that were built by the '.data' and '.string' instructions, their order of appearance in memory depends on their precedence of appearance in the source file (first instruction occupies first free memory in a rising order).

## The object code file (.oc)
The object file is composed out of lines of text. The first line contains (in octal) the length of the code and the length of data, both are in terms of memory words. Those two numbers must be separated by white space. Each of the next lines provides information on the content of memory address (in octal form) starting from memory address 0. In addition, for each memory address, occupied by instruction (not data), there appear additional information for the linker. This additional information could be one of the following three characters: 'e' 'a' or 'r'. The character 'a' designates the fact that the content of the memory address is absolute and does not depend on where the file is to be loaded (the assembler assumes it to start from memory address 0). The character 'r' designates the fact that memory address is relocatable and should be added with the appropriate offset, in regards to where the file is to be loaded. The offset is the first memory address from which the first instruction of the program is to be loaded. The letter 'd' designates the fact that the content of the file depends on external variable, the linker program is to take care on the insertion of the appropriate value.

## The entries file (.ent)
The entries file is composed out of lines of text. Each line contains the entry name and value, as it was computed for this file.

## The externals file (.ext)
The externals file is composed out of lines of text. Each line contains the name and memory address of the external variable.

## Binary file (.bin)
The binary file contains the object code in binary (non-text) format. It can't be created, if the source code contains .extern directives.

## Example files
### test
Prints the string "abcdef".
*test.as*
```
; test.as
; Prints the string "abcdef".

		.entry MAIN		; file contains the definition of MAIN
MAIN:	mov	 LEN, r1	; move LEN(6) to r1
		lea	 STR, r2	; load the address of STR to r2
LOOP:	jnz	 END		; if not zero, jump to END
		prn	 @r2		; print the character at the memory location that r2 holds
		sub	 #1, r1		; r1 = r1 - 1
		inc	 r2			; r2 = r2 + 1
		jnc	 *LOOP		; jump to LOOP of carry flag is not set (sub sets it)
END:	hlt				; end of the program
STR:	.string	 "abcdef"	; string to print
LEN:	.data	6			; length of the string
```
*test.oc*
```
d 8
0000 0221 a
0001 0014 r
0002 6222 a
0003 000d r
0004 9008 a
0005 000c r
0006 c02a a
0007 3021 a
0008 0001 a
0009 7022 a
000a a018 a
000b fffa a
000c f000 a
000d 0061  
000e 0062  
000f 0063  
0010 0064  
0011 0065  
0012 0066  
0013 0000  
0014 0006
```
*test.ent*
```
MAIN 0000
```
### ps
The main routine of the program of reversing string "abcdef".
*ps.as*
```
; ps.as
; Includes main routine of reversing string "abcdef"

MAIN:	lea STR, STRADD
        jsr	COUNT
        jsr	PRTSTR
        mov	*STRADD, LASTCHAR
        add	LEN, LASTCHAR
        dec	LASTCHAR
        jsr	REVERSE
        jsr	PRTSTR
        hlt

.entry	STRADD
.entry	MAIN
.extern REVERSE
.extern PRTSTR
.extern COUNT

STRADD:	  	.data 0
STR:		.string "abcdef"
LASTCHAR: 	.data 0
LEN:		.data 0
```
*ps.oc*
```
14 a
0000 6208 a
0001 0015 r
0002 0014 r
0003 d008 a
0004 0000 e
0005 d008 a
0006 0000 e
0007 0608 a
0008 000d a
0009 001c r
000a 2208 a
000b 001d r
000c 001c r
000d 8008 a
000e 001c r
000f d008 a
0010 0000 e
0011 d008 a
0012 0000 e
0013 f000 a
0014 0000  
0015 0061  
0016 0062  
0017 0063  
0018 0064  
0019 0065  
001a 0066  
001b 0000  
001c 0000  
001d 0000  
```
*ps.ent*
```
STRADD 0014
MAIN 0000
```
*ps.ext*
```
COUNT 0004
PRTSTR 0006
REVERSE 0010
PRTSTR 0012
```

# Usage of tas

```
tas <options> source-file
```
where the options are:
```
-l : prints debugging lists after each pass
-n : creates NO output files
-b : creates binary output file
-h : shows this text
```

# Compilation of tas

*Windows*
```
cd tas
mkdir build
cd build
cmake ..
tas.sln
```
*Linux*
```
cd tas
mkdir build
cd build
cmake ..
make
```
