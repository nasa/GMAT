//$Id$
//------------------------------------------------------------------------------
//                                 CallPythonFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II.
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
/**
 * Definition for the CallPythonFunction command class
 */
//------------------------------------------------------------------------------

#include "InterfaceException.hpp"
#include "CallPythonFunction.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_CONSTRUCTOR
//#define DEBUG_SETGET
//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------
const std::string
CallPythonFunction::PARAMETER_TEXT[PythonFunctionParamCount - CallFunctionParamCount] =
{
   "PythonModule",
   "PythonFunction"
};


const Gmat::ParameterType
CallPythonFunction::PARAMETER_TYPE[PythonFunctionParamCount - CallFunctionParamCount] =
{
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE
};


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CallPythonFunction()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
CallPythonFunction::CallPythonFunction() :
   CallFunction      ("CallPythonFunction"),
   moduleName        (""),
   functionName      (""),
   pythonIf          (NULL)
{
#ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage("CallPythonFunction default constructor.\n");
#endif

}


//------------------------------------------------------------------------------
// CallPythonFunction(const CallPythonFunction& cpf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param cpf The object being copied
 */
//------------------------------------------------------------------------------
CallPythonFunction::CallPythonFunction(const CallPythonFunction& cpf) :
   CallFunction      (cpf),
   moduleName        (cpf.moduleName),
   functionName      (cpf.functionName),
   pythonIf          (cpf.pythonIf)
{
   #ifdef DEBUG_CONSTRUCTOR
      MessageInterface::ShowMessage("CallPythonFunction copy constructor from <%p>.\n", cpf);
   #endif

   dataInput = cpf.dataInput;
   dataReturn = cpf.dataReturn;
}


//------------------------------------------------------------------------------
// CallPythonFunction& operator =(const CallPythonFunction& cpf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param cpf The object used to set structures in this one
 *
 * @return This command, configured to match cpf
 */
//------------------------------------------------------------------------------
CallPythonFunction& CallPythonFunction::operator=(const CallPythonFunction& cpf)
{
   #ifdef DEBUG_CONSTRUCTOR
      MessageInterface::ShowMessage("CallPythonFunction assignment command from <%p>.\n", cpf);
   #endif

   if (this != &cpf)
   {
      CallFunction::operator=(cpf);
      moduleName   = cpf.moduleName;
      functionName = cpf.functionName;
      pythonIf = cpf.pythonIf;
      dataInput = cpf.dataInput;
      dataReturn = cpf.dataReturn;
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~CallPythonFunction
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
CallPythonFunction::~CallPythonFunction()
{
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string used for each parameter
 *
 * @param id The ID for the parameter
 *
 * @return The descriptor
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetParameterText(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TEXT[id - CallFunctionParamCount];

   return CallFunction::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter from its string descrition
 *
 * @param str The string associated with the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer CallPythonFunction::GetParameterID(const std::string& str) const
{
   for (Integer i = CallFunctionParamCount; i < PythonFunctionParamCount; ++i)
      if (str == PARAMETER_TEXT[i - CallFunctionParamCount])
         return i;

   return CallFunction::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the type for a parameter
 *
 * @param id The parameter ID
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType CallPythonFunction::GetParameterType(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return PARAMETER_TYPE[id - CallFunctionParamCount];
   else
      return CallFunction::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the string description of a parameter's type
 *
 * @param id The ID for the parameter
 *
 * @return The string describing the type
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetParameterTypeString(const Integer id) const
{
   if (id >= CallFunctionParamCount && id < PythonFunctionParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - CallFunctionParamCount)];
   else
      return CallFunction::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The id for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetStringParameter(const Integer id) const
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::GetStringParameter(id = %d)\n", id);
#endif
   if (id == MODULENAME)
      return moduleName;
   if (id == FUNCTIONNAME)
      return functionName;

   return CallFunction::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const char* value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param id The id for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const Integer id, const char* value)
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::SetStringParameter(id = %d, value = <%s>)\n", id, value);
#endif

   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      /// @todo: Determine if this code should match SetStringParameter below
      functionName = value;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param id The id for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const Integer id,
      const std::string& value)
{
#ifdef DEBUG_SETGET
   MessageInterface::ShowMessage("CallPythonFunction::SetStringParameter(id = %d, std::string &value = <%s>)\n", id, value.c_str());
#endif

   if (id == MODULENAME)
   {
      moduleName = value;
      return true;
   }
   if (id == FUNCTIONNAME)
   {
      /// @todo: Determine if this code should match SetStringParameter above
      /// @note I think this is the one used in the script engine
      functionName = value;
      mFunctionName = "Python." + moduleName + "." + functionName;
      return true;
   }

   return CallFunction::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The script label for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string CallPythonFunction::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const char* value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const std::string& label,
      const char* value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter's value
 *
 * @param label The script label for the parameter
 * @param value The new value for the parameter
 *
 * @return True on success
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialize Python engine
 *
 * This method initializes Python by loading the Python engine and setting the 
 * PYTHONPATH.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::Initialize()
{
	bool ret = false;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::Initialize()\n");
   #endif

   ret = CallFunction::Initialize();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  Base class init complete\n");
   #endif

   try
   {
      pythonIf = PythonInterface::PyInstance();
      FileManager *fileMgr = FileManager::Instance();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("  pythonIf:  %p\n", pythonIf);
      #endif

	   //Initialize Python engine
	   pythonIf->PyInitialize();

	   // Get all Python module paths from the startup file
	   StringArray paths = fileMgr->GetAllPythonModulePaths();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("  Adding %d python paths\n", paths.size());
      #endif

	   pythonIf->PyAddModulePath(paths);
   }
   catch (BaseException &ex)
   {
      throw CommandException("Error in the CallPython command initializaton:\n" +
         ex.GetFullMessage());
   }
   catch (...)
   {
      throw CommandException("An unhandled Python exception was thrown during "
            "initialization");
   }

   //Fill in Inputlist
   Integer sizeIn = FillInputList();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  SizeIn is %d\n", sizeIn);
   #endif

   //Fill in Outputlist
   Integer sizeOut = FillOutputList();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("  SizeOut is %d\n", sizeOut);
   #endif

	return ret;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute as a Python wrapper
 *
 * This method will execute the Python functions in a Python module.
 *
 * @param none
 *
 * @return bool
 */
//------------------------------------------------------------------------------
bool CallPythonFunction::Execute()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::Execute()\n");
   #endif

   // send the in parameters
   std::vector<PyObject *> argIn;
   
   // Prepare the format string specifier (const char *format) to build 
   // Python object.
   SendInParam();
   for (UnsignedInt i = 0; i < dataInput.size(); ++i)
      argIn.push_back(ConvertToPyObject(dataInput.at(i)));
   PyObject* pyRet;

  // Next, call Python function Wrapper
   try
   {
      pyRet = pythonIf->PyFunctionWrapper(moduleName, functionName, argIn);
   }
   catch (BaseException &ex)
   {
      throw CommandException("Error in the CallPython command execution:\n" +
         ex.GetFullMessage());
   }
   catch (...)
   {
      throw CommandException("An unhandled Python exception was thrown during "
         "execution");
   }

   /*------------------------------------------------------------------------*/
   /* GMAT receives Python data following these rules:
    *
    * Floats are passed to GMAT Variables
    *
    * Ints are passed to GMAT Variables
    *
    * Strings are passed to GMAT strings
    *
    * Lists are passed to GMAT arrays (and must be lists of floats) 
    *
    * Lists of lists are passed to GMAT arrays (and must be lists of lists of
    * floats, all of the same dimension)
    *
    * @note: The tuple handling is different from that described here, because
    * Python returns multiple parameters in tuples, and that means mixed types.
    *
    * Tuples must contain numerical data, and are passed to GMAT one-
    * dimensional arrays
    *
    * Tuples of tuples must contain numerical data, and are passed to GMAT 2D 
    * arrays
   --------------------------------------------------------------------------*/
   dataReturn.clear();

   if (pyRet)
   {
      if (PyTuple_Check(pyRet))
      {
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Python has returned a tuple of "
                  "values.\n");
         #endif
         Integer tupleSz = PyTuple_Size(pyRet);
         for (Integer i = 0; i < tupleSz; ++i)
         {
            PyObject *member = PyTuple_GetItem(pyRet, i);
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("   %d: %p\n", i, member);
            #endif
            dataReturn.push_back(ConvertFromPyObject(member));
         }
      }
      else if (PyMemoryView_Check(pyRet))
      {
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Python has returned a memory"
                  "view object\n");
         #endif
      }
      else
      {
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Python has returned a single"
               "item, attempting to convert to GMAT data type.\n");
         #endif
         PyIfVariant curOutput = ConvertFromPyObject(pyRet);
         // Check if no output was provided (meaning variant was std::monostate)
         if (curOutput.index() != 0)
            dataReturn.push_back(ConvertFromPyObject(pyRet));
      }

      Py_DECREF(pyRet);

      // Fill in the output parameters
      GetOutParams();
   }
   else   // when return value is NULL and no exception is caught/handled.
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("Unknown error happened in Python "
               "Interface.\n");
      #endif
   }

   BuildCommandSummary(true);

	return true;
}


//------------------------------------------------------------------------------
// PyIfVariant ConvertFromPyObject(PyObject* member)
//------------------------------------------------------------------------------
/**
 * Converts and returns a PyObject to a GMAT equivalent varient
 *
 * @param member The object being handled.
 *
 * Note that Tuple objects are not handled here -- the calling function breaks
 * the tuple into separate PyObjects and passes them in one at a time.
 *
 * @return retVal The PyIfVariant version of the PyObject
 */
//------------------------------------------------------------------------------
CallPythonFunction::PyIfVariant CallPythonFunction::ConvertFromPyObject(
      PyObject* member)
{
   PyIfVariant retval;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("BuildReturnFromPyObject(%p)\n", member);
   #endif

   if (member != Py_None)
   {
      try // Since we are making Python calls here, wrap in a handler
      {
         // Reals
         if (PyFloat_Check(member))
         {
            retval = PyFloat_AsDouble(member);
         }
         // Integers, passed into real number containers
         else if (PyLong_Check(member))
         {
            retval = PyLong_AsDouble(member);
         }

         // Strings
         #ifdef IS_PY3K
            else if (PyUnicode_Check(member))
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("A Python String was returned.\n");
               #endif
               retval = _PyUnicode_AsString(member);
            }
         #else
            else if (PyBytes_Check(member))
            {
               retval = PyBytes_AsString(member);
            }
         #endif

         // Lists of floats/ints or lists of lists of floats/ints
         else if (PyList_Check(member))
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Return was a list of size %d\n", 
                  PyList_Size(member));
            #endif

            // number of list elements in a list, for example: [ [], [], [] ]
            Integer listSz = PyList_Size(member);
            PyObject *pyItem = PyList_GetItem(member, 0);

            // number of elements in a list, for example: [ 1, 2, 3 ]
            Integer elementSz = PyList_Size(pyItem);

            if (PyList_Check(pyItem))
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Python has returned a list of list "
                     "of Floats/Integers.\n");
               #endif

               Rmatrix retMat(listSz, elementSz);

               for (Integer i = 0; i < listSz; i++)
               {
                  pyItem = PyList_GetItem(member, i);
                  RealArray vItem;
                  for (Integer j = 0; j < elementSz; j++)
                  {
                     Real retElem;
                     PyObject *pyElem = PyList_GetItem(pyItem, j);

                     // If element is a Python Integer/Long, convert to Real
                     if (PyLong_Check(pyElem))
                        retElem = PyLong_AsDouble(pyElem);
                     else if (PyFloat_Check(pyElem))
                        retElem = PyFloat_AsDouble(pyElem);
                     else
                        throw CommandException("An array member received from Python "
                        "is neither a float nor an integer, so GMAT cannot "
                        "process the value returned on the script line\n   \"" +
                        GetGeneratingString(Gmat::NO_COMMENTS) + "\"");

                     #ifdef DEBUG_EXECUTION
                        MessageInterface::ShowMessage("Array element [%d, %d] value in "
                           "output array is %lf\n", i, j, retElem);
                     #endif

                     retMat(i, j) = retElem;
                  }
               }
               retval = retMat;
            }
            else if (PyFloat_Check(pyItem))
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Python has returned a list of "
                     "floats.\n");
               #endif
               Rmatrix retMat(1, listSz);
               for (Integer i = 0; i < listSz; ++i)
               {
                  pyItem = PyList_GetItem(member, i);
                  retMat(0, i) = PyFloat_AsDouble(pyItem);

                  #ifdef DEBUG_EXECUTION
                     MessageInterface::ShowMessage("Value is %lf\n", 
                        retMat(0,i));
                  #endif
               }
               retval = retMat;
            }
            else if (PyLong_Check(pyItem))
            {
               #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Python has returned a list of Integers.\n");
               #endif
               Rmatrix retMat(1, listSz);
               for (Integer i = 0; i < listSz; ++i)
               {
                  pyItem = PyList_GetItem(member, i);
                  retMat(0, i) = PyLong_AsDouble(pyItem);
               }
               retval = retMat;
            }
            else
            {
               // The return type is not handled
               throw CommandException("The list member returned from the Python "
                  "call on the script line\n   \"" +
                  GetGeneratingString(Gmat::NO_COMMENTS) + "\"\nis a type that "
                  "GMAT does not handle.");
            }
         }
         else if (!PyTuple_Check(member))
         {
            // The return type is not handled
            throw CommandException("The returned value from the Python call is a "
               "type not handled by GMAT");
         }
      }
      catch (BaseException &ex)
      {
         throw CommandException(ex.GetFullMessage() + " on the script line\n   \"" +
            GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }
      catch (...)
      {
         throw CommandException("An error was encountered processing return data "
            "from Python");
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// PyObject* ConvertToPyObject(PyIfVariant GMATParam)
//------------------------------------------------------------------------------
/**
 * Converts and returns a GMAT variant as a PyObject
 *
 * @param GMATParam The variant parameter being handled.
 *
 * @return The PyObject version of the PyIfVariant
 */
//------------------------------------------------------------------------------
PyObject* CallPythonFunction::ConvertToPyObject(PyIfVariant GMATParam)
{
   PyObject *pyParam = NULL;

   try
   {
      switch (GMATParam.index())
      {
      case 1: // Real
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Setting new PyObject to the Real "
               "value %lf\n", std::get<Real>(GMATParam));
         #endif
         pyParam = PyFloat_FromDouble(std::get<Real>(GMATParam));
         break;
      case 2: // String
         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("Setting new PyObject to the string "
               "value \"" + std::get<std::string>(GMATParam) + "\"\n");
         #endif
         pyParam =
            PyUnicode_FromString(std::get<std::string>(GMATParam).c_str());
         break;
      case 3: // Rmatrix
      {
         Rmatrix inMat = std::get<Rmatrix>(GMATParam);
         Integer row = inMat.GetNumRows();
         Integer col = inMat.GetNumColumns();
         Integer listRetCode = 0;

         if (row == 1) // List
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Setting new PyObject to a list "
                  "containing data of 1D GMAT Rmatrix\n");
            #endif
            Integer numElem = (row != 1) ? row : col;
            pyParam = PyList_New(numElem);
            for (UnsignedInt i = 0; i < row; ++i)
            {
               for (UnsignedInt j = 0; j < col; ++j)
               {
                  UnsignedInt listIdx = (row != 1) ? i : j;
                  #ifdef DEBUG_EXECUTION
                  MessageInterface::ShowMessage("Array element [%d] value in "
                     "input array is %lf\n", listIdx, inMat(i, j));
                  #endif
                  listRetCode += PyList_SetItem(pyParam, listIdx,
                     PyFloat_FromDouble(inMat(i, j)));
               }
            }
         }
         else // List of lists
         {
            #ifdef DEBUG_EXECUTION
               MessageInterface::ShowMessage("Setting new PyObject to a list "
                  "of lists containing data of 2D GMAT Rmatrix\n");
            #endif
            pyParam = PyList_New(row);
            for (UnsignedInt i = 0; i < row; ++i)
            {
               PyObject* listCols = PyList_New(col);
               for (UnsignedInt j = 0; j < col; ++j)
               {
                  #ifdef DEBUG_EXECUTION
                     MessageInterface::ShowMessage("Array element [%d, %d] value in "
                        "input array is %lf\n", i, j, inMat(i, j));
                  #endif
                  listRetCode +=
                     PyList_SetItem(listCols, j, PyFloat_FromDouble(inMat(i, j)));
               }
               listRetCode += PyList_SetItem(pyParam, i, listCols);
            }
         }

         if (listRetCode)
            throw CommandException("An index has gone out of bounds of the "
               "generated Python list size when filling it with Rmatrix "
               "elements");
         break;
      }
      default:
         throw CommandException("A parameter is not a supported input type for"
            " GMAT's Python interface. Accepted data types are "
            "[Real, String, Array]");
      }
   }
   catch (BaseException &ex)
   {
      throw CommandException(ex.GetFullMessage() + " on the script line\n   \"" +
         GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
   }
   catch (std::bad_variant_access &ex)
   {
      std::string varMsg = ex.what();
      throw CommandException("Received a " + varMsg + " while attempting to "
         "convert a GMAT variant to a Python input on the script line\n   \"" +
         GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
   }
   catch (...)
   {
      throw CommandException("An error was encountered processing input data "
         "for Python");
   }

   return pyParam;
}

//------------------------------------------------------------------------------
// void CallPythonFunction::RunComplete()
//------------------------------------------------------------------------------
/**
 * Finalizes and closes the Python interface
 */
//------------------------------------------------------------------------------
void CallPythonFunction::RunComplete()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("  Calling CallPythonFunction::RunComplete()\n");
   #endif
   if (pythonIf != NULL)
   {
      try 
      {
         pythonIf->PyFinalize();
         pythonIf = NULL;
      }
      catch (BaseException &ex)
      {
         throw CommandException("Error in the CallPython command finalizaton:\n" +
            ex.GetFullMessage());
      }
      catch (...)
      {
         throw CommandException("An unhandled Python exception was thrown during finalization");
      }
   }

   CallFunction::RunComplete();
}


//------------------------------------------------------------------------------
// Integer FillInputList()
//------------------------------------------------------------------------------
/**
 * Fills in input parameters
 *
 * This method will fill in input parameters declared in script.
 *
 * @param none
 *
 * @return Integer
 */
//------------------------------------------------------------------------------
Integer CallPythonFunction::FillInputList()
{
   GmatBase *mapObj;

   mInputList.clear();
  
   StringArray ar = GetStringArrayParameter(ADD_INPUT);
   StringArray::iterator it;

   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("The CallPythonFunction command cannot find "
               "the parameter " + *it + " in script line\n   \"" +
               GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }
         
      if (mapObj->IsOfType(Gmat::PARAMETER))
         mInputList.push_back((Parameter *)mapObj);
      else
         throw CommandException("The input field " + (*it) + " was not "
               "recognized as a valid input to the Python interface.");
   }
      
   return mInputList.size();
}


//------------------------------------------------------------------------------
// Integer FillOutputList()
//------------------------------------------------------------------------------
/**
 * Fills in output parameters
 *
 * This method will fill in output parameters declared in script.
 *
 * @param none
 *
 * @return Integer
 */
//------------------------------------------------------------------------------
Integer CallPythonFunction::FillOutputList()
{
   GmatBase *mapObj;

   mOutputList.clear();

   StringArray ar = GetStringArrayParameter(ADD_OUTPUT);
   StringArray::iterator it;

   for (it = ar.begin(); it != ar.end(); ++it)
   {
      if ((mapObj = FindObject(*it)) == NULL)
      {
         throw CommandException("The CallPythonFunction command cannot find "
                                "the parameter " + *it + " in script line\n   \"" +
                                GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
      }

      if (mapObj->IsOfType(Gmat::PARAMETER))
         mOutputList.push_back((Parameter *)mapObj);
      else
         throw CommandException("The output field " + (*it) + " was not "
               "recognized as a valid output from the Python interface on the "
               "script line\n   \"" + GetGeneratingString(Gmat::NO_COMMENTS) +
               "\"");
   }

   return mOutputList.size();
}


//------------------------------------------------------------------------------
// void SendInParam()
//------------------------------------------------------------------------------
/**
 * Fills in format string and input parameters for Python function
 *
 * This method will fill in format string and input parameters
 *
 * @param none
 */
//------------------------------------------------------------------------------
void CallPythonFunction::SendInParam()
{
   dataInput.clear();
   for (unsigned int i = 0; i < mInputList.size(); i++)
   {
      Parameter *param = mInputList[i];
      Gmat::ParameterType type = param->GetReturnType();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Parameter %d, %s, type %d\n", i,
               param->GetName().c_str(), type);
      #endif

      switch (type)
      {
         case Gmat::REAL_TYPE:
         {       
            dataInput.push_back(param->EvaluateReal());
            break;
         }

         case Gmat::STRING_TYPE:
         {
            dataInput.push_back(param->EvaluateString());
            break;
         }

         case Gmat::RMATRIX_TYPE:
         {
            // Array or array element
            bool entireArray = (mInputNames[i].find("(") == std::string::npos);

            #ifdef DEBUG_INITIALIZATION
               MessageInterface::ShowMessage("%d: %s, an array%s\n", i,
                     mInputNames[i].c_str(), (entireArray ? "" : " element"));
            #endif

            Array *arr = (Array *) param;
            UnsignedInt inRow = arr->GetRowCount();
            UnsignedInt inCol = arr->GetColCount();

            if (entireArray)
            {
               if (arr != NULL)
                  dataInput.push_back(param->EvaluateRmatrix());
               else
                  throw CommandException("The Python input parameter " + param->GetName() +
                        " should contain an array, but does not");
            }
            else  // Array element: Passes as a float/Real
            {
               std::string elementStr = mInputNames[i].substr(mInputNames[i].find("(")+1);
               if (elementStr.find(")") == std::string::npos)
                  throw CommandException("The parameter " + param->GetName() +
                        " appears to be an array element, but matching "
                        "parentheses were not found in the scripting \"" +
                        mInputNames[i] + "\" on line\n   " +
                        GetGeneratingString(Gmat::NO_COMMENTS));
               elementStr = elementStr.substr(0, elementStr.find(")"));

               // Scripted row/col values (indexed from 1)
               UnsignedInt row = 1, col = 1;
               bool is2D = (elementStr.find(",") != std::string::npos);
               std::stringstream data(elementStr);
               data >> row;
               if (is2D)
               {
                  data.str(elementStr.substr(elementStr.find(",")+1));
                  data >> col;
               }

               #ifdef DEBUG_INITIALIZATION
                  MessageInterface::ShowMessage("Row %d, col %d\n", row, col);
               #endif

               // In GMAT, 1D arrays are a single row, and indexed by col number
               if (inRow == 1)
               {
                  col = row;
                  row = 1;
               }

               if ((row > inRow) || (col > inCol))
                  throw CommandException("The input array element " + mInputNames[i] +
                        " exceeds the dimensions of the specified array on line\n   " +
                        GetGeneratingString(Gmat::NO_COMMENTS));

               dataInput.push_back(arr->GetRealParameter(std::string("SingleValue"),
                  row - 1, col - 1));
            }
            break;
         }

         default:
            throw CommandException("The parameter " + param->GetName() +
                        ", with type " + PARAM_TYPE_STRING[type] +
                        ", is not a valid input type for GMAT's Python "
                        "interface.");
            break;
      }
   }
}


//------------------------------------------------------------------------------
// void GetOutParams()
//------------------------------------------------------------------------------
/**
 * Get output parameters
 *
 * Checks match between returned data and GMAT expectations, and fills in output parameters
 *
 * @return void
 */
//------------------------------------------------------------------------------
void CallPythonFunction::GetOutParams()
{
   std::stringstream messages;

   // First validate that the right number of parameters was returned
   if (dataReturn.size() != mOutputList.size())
   {
      messages << "Python returned " << dataReturn.size()
               << " output parameters, and GMAT expected "
               << mOutputList.size() << " returned values.";

      throw CommandException(messages.str());
   }

   Parameter *param;

   try
   {
      for (UnsignedInt i = 0; i < dataReturn.size(); ++i)
      {
         param = mOutputList[i];

         switch (dataReturn[i].index())
         {
         case 1: // Real
            if (param->GetReturnType() != Gmat::REAL_TYPE)
            {
               messages << "The type returned from Python, a real number, "
                  "does not match the type expected by GMAT, a "
                  << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
               continue;
            }
            param->SetReal(std::get<Real>(dataReturn[i]));
            break;

         case 2: // String
            if (param->GetReturnType() != Gmat::STRING_TYPE)
            {
               messages << "The type returned from Python, a string, does not "
                  "match the type expected by GMAT, a "
                  << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
               continue;
            }
            param->SetString(std::get<std::string>(dataReturn[i]));
            break;

         case 3: // Rmatrix
            if (param->GetReturnType() != Gmat::RMATRIX_TYPE)
            {
               messages << "The type returned from Python, an array, does not "
                  "match the type expected by GMAT, a "
                  << PARAM_TYPE_STRING[param->GetReturnType()] << ".\n";
               continue;
            }
            {
               // Validate size matches
               Rmatrix pyMatrix = std::get<Rmatrix>(dataReturn[i]);
               Integer pyRowCount = pyMatrix.GetNumRows();
               Integer pyColCount = pyMatrix.GetNumColumns();
               Rmatrix rvMatrix = param->EvaluateRmatrix();
               Integer expectedRowCount = rvMatrix.GetNumRows();
               Integer expectedColCount = rvMatrix.GetNumColumns();

               if ((expectedColCount != pyColCount) ||
                  (expectedRowCount != pyRowCount))
               {
                  messages << "Size mismatch in the array returned from "
                     "Python at index "
                     << i << ": the returned array has the size ("
                     << pyRowCount << ", " << pyColCount
                     << ") and GMAT expected an array of size ("
                     << expectedRowCount << ", " << expectedColCount
                     << ").\n";
                  break;
               }

               rvMatrix = pyMatrix;
               param->SetRmatrix(rvMatrix);
            }
            break;

         default:
            messages << "Type mismatch in the returned data at index "
               << i << ": the returned data is not a type handled by the "
               "Python interface.";
            break;
         }
      }
   }
   catch (std::bad_variant_access &ex)
   {
      std::string varMsg = ex.what();
      throw CommandException("Received a " + varMsg + " while attempting to "
         "send a GMAT variant to a GMAT parameter input on the script "
         "line\n   \"" + GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
   }
   catch (...)
   {
      throw CommandException("An error was encountered sending variants "
         "generated from Python outputs to GMAT parameters");
   }

   if (messages.str() != "")
      throw CommandException(messages.str() + "See the script line\n   \"" +
            GetGeneratingString(Gmat::NO_COMMENTS) + "\"");
}
