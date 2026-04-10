Scripted Equations as Fields and Method Inputs
==============================================

Beginning with the R202a GMAT release , the GMAT script language includes code
that lets users define numerical settings using equations compatible with the 
equations processed by GMAT's assignment command.  This functionality requires 
some special setting in the code to enable the feature, as described in this 
document.

Features and Use Cases
----------------------
The implementation of the equation handling code meets a set of requirements 
captured in the features described below.  Following the feature description is 
a set of use cases that provide representative examples of this feature.

Features
++++++++

* Fields that support equations are identified with an equation specific type.
* Equations are validated in the Interpreter subsystem's FinalPass code.
* Equation fields do not automatically support pure numeric inputs

  * Equations are constructed using the MathTree infrastructure developed for 
    the assignment command.
  * Numeric support can be added on a case by case basis

* In general, equations are evaluated when the Get*Parameter() method is called.

Equation Examples
+++++++++++++++++

There are two use cases for equations as currently implemented: Setting a field 
value as an equation, and sending an equation into a scripted object method.  

**Use Case 1: Equation Fields**

Developers can identify equation based fields in their code by setting the type
to be an equation.  When a field is set this way, the corresponding script 
access methods allow for object scripting like this:

.. code-block:: matlab
   :caption: Equation Scripting for a Custom Class
   :linenos:

   Create ConstrainedImpulsiveBurn theBurn
   ...
   theBurn.Element3 = 2.5 - Sqrt(theBurn.Element1^2 + theBurn.Element2^2);

For this example, a new impulsive burn class, ConstrainedImpulsiveBurn, has been 
defined that lets a user script an equation for the third component of the burn
using an equation.  In this case, the user has scripted the burn so that the 
total burn magnitude is 2.5 km/s, with the third burn component used to offset 
the first and second in order to keep the total delta-V applied set to 2.5 km/s,
as long as the first two components together have a RSS magnitude smaller than 
2.5 km/s. 


**Use Case 2: Function Methods**

GMAT's scripted method code lets a user incorporate method syntax into the 
simulation language.  This feature is used in the Optimal Control subsystem to
script equations for the point (and, eventually, path) functions used in the 
system.  This lets users set optimal control constraints based on the state of 
the system at select points on a spacecraft's trajectory.  An example of this
scripting is

.. code-block:: matlab
   :caption: Equation Scripting for an Optimal Control boundary condition
   :linenos:

   Create Phase PatchedConicLaunch;
   ...

   Create OptimalControlFunction boundCon
   boundCon.Function = Expression
   boundCon.Type = AlgebraicConstraint
   boundCon.PhaseList = {PatchedConicLaunch.Final}
   boundCon.SetModelParameter('LowerBounds', 11.2);
   boundCon.SetModelParameter('Expression', 'Sqrt(testSat.VX^2 + testSat.VY^2 + testSat.VZ^2)');

This function constrains the speed of the testSat spacecraft at the end of the 
PatchedConicLaunch phase to be at least 11.2 km/s in the default Earth 
mean-of-J2000 equatorial coordinate system.

Coding Equations
----------------
Equation coding in GMAT scripting is a two step process.  The first step is to 
code the core functionality to the point at which a script that uses an equation 
will load.  Once the script can be parsed into a GMAT run, the structures 
necessary for the equation to evaluate during a run need to be coded.

Equation management is slightly more complicated than the usual GMAT object 
field management because equations ate handled differently from the script
pespective than from the run time perspective.  In script processing, the 
expression defining an equation is handled as a string.  During a run, 
the call to the field or method that uses the equation requires evaluation of 
the expressed equation, and returns numeric data.  The hand-off between the 
string parsing for the expression of an equation and the construction of the 
objects and connections required to evaluate the expression is performed in the 
final pass[#]_ through GMAT's script interpreter.  This pass populates a 
MathTree data structure for evaluation during the run.  That structure is then 
used to build the parameters identified in the equation so that they are 
available for use in the run.

.. [*] TBD: Handling for equations entered from a GUI panel.

At run time, the run time objects used to evaluate equations are passed to the
MathTree objects through a series of calls in the GMAT Sandbox during 
initialization.  This makes evaluation of the equation data using the objects 
that model the run possible.

Developers coding equation support into a GMAT class need to know how to set up
these pieces of the code.  The following discussion provides that information.

Setup and Parsing
+++++++++++++++++
Fields that support equations are set in GMAT objects by designating them as
EQUATION_TYPE fields.  An example of this designation is the Expression field in 
the CSALTInterface's OptimalFunction class.  The scripting for the field is 
identified in the class's PARAMETER_TEXT and PARAMETER_TYPE setting as

.. code-block:: c++
   :linenos:

   const std::string
   OptimalFunction::PARAMETER_TEXT[OptimalFunctionParamCount - GmatBaseParamCount] =
   {
     "Type",
     "PhaseList",
     "Expression",
     "LowerBound",
     "UpperBound",
     "ScaleFactors"
   };

   const Gmat::ParameterType
   OptimalFunction::PARAMETER_TYPE[OptimalFunctionParamCount - GmatBaseParamCount] =
   {
     Gmat::STRING_TYPE,        // Type
     Gmat::STRINGARRAY_TYPE,   // Phase List -- was OBJECTARRAY_TYPE
     Gmat::EQUATION_TYPE,      // Expression
     Gmat::RVECTOR_TYPE,       // Lower Bound
     Gmat::RVECTOR_TYPE,       // Upper Bound
     Gmat::RVECTOR_TYPE        // Scale Factors
   };

The third entry here is the item of interest for equation settings.  An example 
of the scripting for this field is the line

.. code-block:: matlab

   MyOptimalFun.Expression = -Sqrt(sat.X^2 + sat.Y^2 + sat.Z^2);

On parsing, the text on the right side of the equals sign is stored in a string,
defined in the OptimalFunction header file as

.. code-block:: c++
   :linenos:

   /// The string that gets evaluated to define the constraint or cost
   std::string  expression;

The GMAT Interpreter subsystem treats this field as a string, using the usual
class methods (GetStringParameter(), SetStringParameter(), etc) to handle the 
scripted text.  During parsing, the string is set and the Interpreter builds the
MathTree components needed to manage the scripted equation.

.. Loading and Running
   +++++++++++++++++++


