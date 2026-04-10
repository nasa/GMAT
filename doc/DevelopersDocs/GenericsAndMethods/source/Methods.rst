.. _ObjectMethods:

Scripted Methods
=================

The GMAT scrpting language supports methods on objects beginning with the GMAT 
R2020a release.  Object methods are scripted similarly to functions.  The method
is called using the syntax

.. code-block:: matlab
   :caption: A Scripted Method

   Create ObjectType MyObject;
   MyObject.MyMethod(Parm1, Parm2);

   Create Variable Parm1 Parm2;

The class defines the method signature, and exposes its methods using an 
interface defined in GmatBase.  GMAT's interpreter detects the method in two
steps.  First, the possibility that the line of script is a method call is made
when the interpreter finds a pair of parentheses in the line.  The Interpreter 
then breaks the line into pieces, looking for a signature of the form 
``owner.method(...)``.  It locates the owner object.  If found, it asks the 
object if it has a scripted method with the method name on the script line.  
Finally, if the method is supported, the interpreter passes in the arguments for 
the method call.

Scripted methods are implemented using the GmatBase access methods

.. code-block:: c++
   :caption: GmatBase Methods used for Scripted Object Methods
   :linenos:

   class GmatBase
   {
      ...
      // Scripted Object Method Support
      StringArray             GetScriptedMethodList();
      Integer                 GetMethodParameterCount(const std::string &forMethod);
      ObjectTypeArray         GetMethodParameterTypes(const std::string &forMethod);

      virtual bool            SetMethodParameters(const std::string methodName,
                                 const StringArray & args);
      virtual bool            SetMethodParameter(const std::string methodName,
                                 const std::string &arg, Integer index);

      virtual Generic         ExecuteMethod(const std::string &forMethod,
                                 const StringArray &parameters);
      ...
   }

As an example of the implementation, consider the scripting

.. code-block:: matlab
   :caption: Scripting to Set Model Parameters on a GMAT Boundary Function
   :linenos:

   Create BoundaryFunction EarthGravAssist;
   ...
   EarthGravAssist.SetModelParameter('CelestialBody','Earth');
   EarthGravAssist.SetModelParameter('RadiusLowerBound',6000);

Method Parsing
----------------------

This scripting sets up the scripted ``SetModelParameter`` method, passing in 
field names and values.  The interpreter sees the method lines (lines 3 and 4 in 
the example), and checks the BoundaryFunction object to determine if 
``SetModelParameter`` is a recognized scripted function on the object.  After 
validating the function, the interpreter retrieves the number and type of input 
fields expected for the method, and parses the scripting accordingly, passing 
the script entries to the object for processing.

Supported methods are set up in the object constructor.  The code that defines 
the ``SetModelParameter`` method is

.. code-block:: c++
   :caption: Method configuration for the SetModelParameter method
   :linenos:

   // Define the object's methods
   objectMethods.push_back("SetModelParameter");
   // 3 inputs; 2 are required, third is optional for calls that need it
   methodTotalInputCount["SetModelParameter"] = 2;

   ObjectTypeArray parmFieldTypes;
   // Internal method name
   parmFieldTypes.push_back(Gmat::STRING_TYPE);
   // First input; value, or option (e.g. coordinate system)
   parmFieldTypes.push_back(Gmat::GENERIC_TYPE);
   // Optional second input: value if used
   parmFieldTypes.push_back(Gmat::GENERIC_TYPE);
   methodInputs["SetModelParameter"] = parmFieldTypes;

Supported methods are inserted into the list of object methods (line 2), and the 
minimum number of required inputs is set on the ``methodTotalInputCount`` mapping
(line 4).  Finally, the expected type for each input is defined in an array, and
set in the ``methodInputs`` map for the method (lines 6-13).

The class that implements the method provides method configuration support by
overriding the base class's ``SetMethodParameter`` method.  The override for the 
scripting shown above on the ``BoundaryFunction`` is implemented in the 
``GmatPointFunction`` class using the code

.. code-block:: c++
   :caption: Parameter setup code for the SetModelParameter method
   :linenos:

   bool GmatPointFunction::SetMethodParameter(const std::string methodName,
         const std::string &arg, const Integer index, bool &commandComplete)
   {
      bool retval = false;

      Integer parmCount = methodTotalInputCount[methodName];

      // Walk through the supported methods

      commandComplete = (index >= parmCount - 1);
      static std::string currentArg;

      // Here we add handlers for each scriptable method
      if (methodName == "SetModelParameter")
      {
         switch (methodInputs[methodName][index])
         {
         case Gmat::STRING_TYPE:
            {
               GenericArray setting;
               settingMap[arg] = setting;
               currentArg = arg;
            }
            retval = true;
            break;

         case Gmat::GENERIC_TYPE:
            if (settingMap.find(currentArg) != settingMap.end())
            {
               settingMap[currentArg].push_back(GetGeneric(arg));
               retval = true;
            }
            break;

         // Other types are not handled
         default:
            break;
         }
      }
      
      return retval;
   }

Note that even though it is declared virtual, developers do not need to override 
the ``SetMethodParameters`` base class method in most instances.  If special 
handling is needed for the inputs, that method can be overriden.

This completes the parsing for scripted methods.

Execution
----------------------

.. note::  
   Execution of scripted methods during execution of a mission control sequence 
   is not yet supported.  When implemented, this section will be completed.  
   The design has method execution via an override of the base class's 
   ``ExecuteMethod`` method.  At this writing, there is no use case for the
   override, so this feature is deferred.
