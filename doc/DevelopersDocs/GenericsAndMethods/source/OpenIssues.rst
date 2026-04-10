Best Practices and Open Issues
===============================


Best Practices
-----------------------

The following sections list some best practices using generic data tyopes and
setting up scrpted methods.

Generics
+++++++++++++++++++++++

**Only Use Generic Containers When Needed**

The Generic type in GMAT lets programmers relax type informaton requirements 
for scripted inputs.  The responsiblity for type checking becomes greater on 
the side of the code.  Programmers are better served by requiring that inputs 
observe the target data types, and only use Generic types when input flexibility 
is truly necessary. 

Methods
+++++++++++++++++++++++

**Name Member Methods to Match Scripted Methods** 

The implementation of scripted "methods" is a bit of a shell game.  The 
scripting sets up a sequence that looks something like this:

#. Check is a scripted method is supported
#. Set the method inputs
#. (As needed) Prepare for use when the object is initialized
#. Execute the method during a run *(Not yet implemented)* 

However, this sequence is made through a set of wrapper calls.  There is no code 
that forces the programmer to implement a class method with the scripted method
name.  All that the class needs to support is a set of steps that accomplish the 
intended goal when the script makes an object method call.

To minimize confusion, one best practice is to collect the scripted action into 
a method implemented in the class, and to name that method the same as the 
scripted method name.


Open Development Items
-----------------------

* Methods are object mode only in the current code
   * Parsing is functional
   * All applied actions must be made inside of the class's ``Initialize`` method
   * Methods do not execute during runs 

* GMAT needs a wrapper class for Generics
   * Adding a wrapper will simplify type support
   * The equation implementation is likely to require that a GenericWrapper be 
     added to the code. 



Known Issues and Bugs
-----------------------

* There are no test cases defined for scripted methods
