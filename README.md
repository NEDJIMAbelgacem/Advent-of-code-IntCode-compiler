## Advent of Code Arithmetic expressions to IntCode compiler :
This is my implementation of an intcode compiler. It supports all the opcodes from day 5 and some additional instructions like Subtraction, division, modulus, less than or equals, greater than, greater than or equals.

## supported expressions :
- Supported operators are : +, -, *, /, %, >, <, >=, <=, ==
- Nesting operations in parentheses is also possible
- It supports also the ternary operator b ? op1 : op2 used in mapy languages. I thought about this just to use jump instructions 

## How to use :
Just compile with a C++ compiler 
then run it and give your arithmetic expression, it will output the IntCode and its result