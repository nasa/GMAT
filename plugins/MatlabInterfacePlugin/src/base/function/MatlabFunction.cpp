//$Id$
//------------------------------------------------------------------------------
//                                  MatlabFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the MatlabFunction class.
 */
//------------------------------------------------------------------------------

#include "MatlabFunction.hpp"
#include "FileManager.hpp"       // for GetMatlabFunctionPath()
#include "FileUtil.hpp"          // for ParseFileName(), GetCurrentWorkingDirectory()
#include "StringUtil.hpp"        // for Trim()
#include "MessageInterface.hpp"

//#define DEBUG_MATLAB_FUNCTION
//#define DEBUG_FUNCTION_SET

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// MatlabFunction(std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor.
 *
 * @param <name> name of the MatlabFunction.
 *
 */
//------------------------------------------------------------------------------
MatlabFunction::MatlabFunction(const std::string &name) :
   Function("MatlabFunction", name)
{
   // for initial function path, use FileManager
   FileManager *fm = FileManager::Instance();
   std::string pathname;
   objectTypeNames.push_back("MatlabFunction");

   try
   {
      // matlab uses directory path
      pathname = fm->GetMatlabFunctionPath(name);
      functionPath = pathname;
   }
   catch (GmatBaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      #ifdef DEBUG_MATLAB_FUNCTION
      MessageInterface::ShowMessage(e.GetFullMessage());
      #endif
      
      // see if there is FUNCTION_PATH
      try
      {
         pathname = fm->GetFullPathname("FUNCTION_PATH");
         functionPath = pathname;
      }
      catch (GmatBaseException &e)
      {
         // Use exception to remove Visual C++ warning
         e.GetMessageType();
         #ifdef DEBUG_MATLAB_FUNCTION
         MessageInterface::ShowMessage(e.GetFullMessage());
         #endif
      }
   }
   
   #ifdef DEBUG_MATLAB_FUNCTION
   MessageInterface::ShowMessage
      ("   Matlab functionPath=<%s>\n", functionPath.c_str());
   MessageInterface::ShowMessage
      ("   Matlab functionName=<%s>\n", functionName.c_str());
   #endif
}


//------------------------------------------------------------------------------
// ~MatlabFunction()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MatlabFunction::~MatlabFunction()
{
}


//------------------------------------------------------------------------------
// MatlabFunction(const MatlabFunction &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> object being copied
 */
//------------------------------------------------------------------------------
MatlabFunction::MatlabFunction(const MatlabFunction &copy) :
   Function      (copy)
{
}


//------------------------------------------------------------------------------
// MatlabFunction& MatlabFunction::operator=(const MatlabFunction &right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 *
 * @param <right> object being copied
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
MatlabFunction& MatlabFunction::operator=(const MatlabFunction& right)
{
    if (this == &right)
        return *this;

    Function::operator=(right);

    return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MatlabFunction.
 *
 * @return clone of the MatlabFunction.
 */
//------------------------------------------------------------------------------
GmatBase* MatlabFunction::Clone() const
{
   return (new MatlabFunction(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void MatlabFunction::Copy(const GmatBase* orig)
{
   operator=(*((MatlabFunction *)(orig)));
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a std::string parameter.
 * 
 * @param <id> Integer ID of the parameter.
 * @param <value> New value for the parameter.
 * 
 * @return If value of the parameter was set.
 */
//------------------------------------------------------------------------------
bool MatlabFunction::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_FUNCTION_SET
   MessageInterface::ShowMessage
      ("MatlabFunction::SetStringParameter() entered, id=%d, value=%s\n", id, value.c_str());
   #endif
   
   switch (id)
   {
   case FUNCTION_PATH:
      {
         bool retval = SetMatlabFunctionPath(value);
         
         return retval;
      }
   case FUNCTION_NAME:
      {
         // Remove path if it has one
         functionName = GmatFileUtil::ParseFileName(functionPath);
         return true;
      }
   default:
      return Function::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool MatlabFunction::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool SetMatlabFunctionPath(const std::string &path)
//------------------------------------------------------------------------------
bool MatlabFunction::SetMatlabFunctionPath(const std::string &path)
{
   #ifdef DEBUG_FUNCTION_SET
      MessageInterface::ShowMessage
      ("MatlabFunction::SetMatlabFunctionPath() <%p> entered,"
       "path='%s'\n", this, path.c_str());
   #endif
   
   FileManager *fm = FileManager::Instance();

   // Compose full path if it has relative path.
   // Assuming if first char has '.', it has relative path.
   std::string temp = GmatStringUtil::Trim(path);
   #ifdef DEBUG_FUNCTION_SET
      MessageInterface::ShowMessage("   temp=%s\n", temp.c_str());
   #endif
   if (temp[0] == '.')
   {
      //            std::string currPath = fm->GetCurrentWorkingDirectory();
      // Look relative to the directory that contains the current
      // script first
      std::string currPath = fm->GetGmatWorkingDirectory();
      
      #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   currPath=%s\n", currPath.c_str());
      #endif
      
      if (temp[1] != '.')
         functionPath = currPath + temp.substr(1); // WCS - why no slash here too?
      else
      {
         functionPath = currPath + '/';
         functionPath += temp;
      }
      #ifdef DEBUG_FUNCTION_SET
         MessageInterface::ShowMessage("   functionPath set to %s\n",
                                       functionPath.c_str());
      #endif
      
      // Check to see if this path exists; if not, try relative to the
      // bin folder
      if (!fm->DoesDirectoryExist(functionPath))
      {
         std::string binPath = fm->GetCurrentWorkingDirectory();
         if (temp[1] != '.')
            functionPath = binPath + temp.substr(1); // WCS - why no slash here too?
         else
         {
            functionPath = binPath + '/';
            functionPath += temp;
         }
      }
   }
   else
   {
      functionPath = path;
   }
   
   #ifdef DEBUG_FUNCTION_SET
      MessageInterface::ShowMessage
                        ("   functionPath=<%s>\n", functionPath.c_str());
      MessageInterface::ShowMessage
                        ("   functionName=<%s>\n", functionName.c_str());
   #endif
   
   
   // Add to MatlabFunction path so that nested function can be found
   // Do we need to add to FileManager function path? exclude it (LOJ: 2015.03.17)
   std::string funcPathOnly = GmatFileUtil::ParsePathName(functionPath);
   #ifdef DEBUG_FUNCTION_SET
      MessageInterface::ShowMessage
                        ("   ===> Adding MatlabFunction path '%s' to "
                         "FileManager for function '%s'\n",
                        funcPathOnly.c_str(), GetName().c_str());
   #endif
   fm->AddMatlabFunctionPath(funcPathOnly);
   
   // Remove path for function name
   functionName = GmatFileUtil::ParseFileName(functionPath);
   
   return true;
}

