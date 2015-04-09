# Extended-precision-integer-calculator
Foundations of Algorithm assignment

Most modern cryptography relies on doing arithmetic with very large numbers. One example is the RSA
algorithm, used by secure websites to prove that the server really is who it’s claiming to be. RSA can
also be used for encrypting messages. Either way, its security relies on the assumption that it’s hard to
factorize a number that is the product of two large primes, where “large” means hundreds of decimal
digits.
The problem is, the data type int in C can only store values up to 231􀀀1 = 2;147;483;647. If larger
values must be manipulated accurately, a different representation for integers is required. Your task in
this project is to develop an integer calculator that works with extended-precision integer values. The
calculator has a simple interface, and 26 “variables” (or memories) into which values can be stored. For
example a session with your calculator (where > is the prompt from the system) might look like:

> a = 2147483647
> a ?
2147483647
> a + 1
> a ?
2147483648
> b = 999999999999999
> c = 1000000000000000000
> c + b
> c ?
1000999999999999999
> b ?
999999999999999
> a ?
2147483648
> ^D

Note the extremely limited syntax: constants or other variables can be assigned to variables using an
“=” operator; or variable values can be altered using a single operator and another variable or constant
(and “+” is the only one implemented in the skeleton; or variable values can be printed using “?”. The
variable in question is always the first thing specified on each input line; then a single operator; and then
(for most operators) either another variable name, or an extended-precision integer constant.
To allow storage of very large integers, your calculator needs to make use arrays of ASCII characters,
with one decimal digit stored per character in the array. You are to design a suitable structure and
1
representation, assuming that values of up to INTSIZE digits are to be handled. Other information might
also be required in each “number”, including a count of the number of digits, and a sign.
The expected input format for all stages is identical – a sequence of simple calculator commands of
the type shown above.
