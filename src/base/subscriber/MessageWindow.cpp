//$Id$
//------------------------------------------------------------------------------
//                                  MessageWindow
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Author: Linda Jun
// Created: 2003/11/6
//
/**
 * Defines operation of MessageWindow class.
 */
//------------------------------------------------------------------------------
#include <iomanip>
#include "MessageWindow.hpp"
#include "MessageInterface.hpp" // for ShowMessage()

//---------------------------------
// static data
//---------------------------------
const std::string
MessageWindow::PARAMETER_TEXT[MessageWindowParamCount - SubscriberParamCount] =
{
   "Precision"
}; 

const Gmat::ParameterType
MessageWindow::PARAMETER_TYPE[MessageWindowParamCount - SubscriberParamCount] =
{
   Gmat::INTEGER_TYPE,
};

//------------------------------------------------------------------------------
// MessageWindow(const MessageWindow &mw)
//------------------------------------------------------------------------------
MessageWindow::MessageWindow(const MessageWindow &mw)
   : Subscriber (mw),
     precision  (mw.precision)
{
   // GmatBase data
   //parameterCount = MessageWindowParamCount;
   dstream.precision(precision);
   dstream.setf(std::ios::fixed);
}

//------------------------------------------------------------------------------
// MessageWindow(const std::string &name)
//------------------------------------------------------------------------------
MessageWindow::MessageWindow(const std::string &name)
   : Subscriber ("MessageWindow", name),
     precision  (10)
{
   // GmatBase data
   parameterCount = MessageWindowParamCount;
   dstream.precision(precision);
   dstream.setf(std::ios::fixed);
}

//------------------------------------------------------------------------------
// ~MessageWindow(void)
//------------------------------------------------------------------------------
MessageWindow::~MessageWindow(void)
{
    dstream.flush();
}

//------------------------------------------------------------------------------
// bool MessageWindow::Distribute(int len)
//------------------------------------------------------------------------------
bool MessageWindow::Distribute(int len)
{
   dstream.str("");
   
   if (len == 0)
      //dstream << data;
      return false;
   else
      for (int i = 0; i < len; ++i)
         dstream << data[i];
   
   dstream.flush();
   MessageInterface::ShowMessage(dstream.str());
   return true;
}

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool MessageWindow::Distribute(const Real * dat, Integer len)
{
    dstream.str("");
    dstream.flush();
    
    if (len == 0)
    {
        return false;
    }
    else
    {
        for (int i = 0; i < len-1; ++i)
            dstream << dat[i] << "  ";
        
        dstream << dat[len-1] << std::endl;
    }

    MessageInterface::ShowMessage(dstream.str());
    //dstream.str("");
    //dstream << "line # " << MessageInterface::GetNumberOfMessageLines() << std::endl;
    //MessageInterface::ShowMessage(dstream.str());
    return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MessageWindow.
 *
 * @return clone of the MessageWindow.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MessageWindow::Clone() const
{
   return (new MessageWindow(*this));
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string MessageWindow::GetParameterText(const Integer id) const
{
   if (id >= SubscriberParamCount && id < MessageWindowParamCount)
      return PARAMETER_TEXT[id - MessageWindowParamCount];
   else
      return Subscriber::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer MessageWindow::GetParameterID(const std::string &str) const
{
   for (int i=SubscriberParamCount; i<MessageWindowParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SubscriberParamCount])
         return i;
   }
   
   return Subscriber::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType MessageWindow::GetParameterType(const Integer id) const
{
   if (id >= SubscriberParamCount && id < MessageWindowParamCount)
      return PARAMETER_TYPE[id - SubscriberParamCount];
   else
      return Subscriber::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string MessageWindow::GetParameterTypeString(const Integer id) const
{
   if (id >= SubscriberParamCount && id < MessageWindowParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id - SubscriberParamCount)];
   else
      return Subscriber::GetParameterTypeString(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool MessageWindow::IsParameterReadOnly(const Integer id) const
{
   if (id == UPPER_LEFT || id == SIZE || id == RELATIVE_Z_ORDER || id == MINIMIZED)
      return true;

   return Subscriber::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer MessageWindow::GetIntegerParameter(const Integer id) const
{
    switch (id)
    {
    case PRECISION:
        return precision;
    default:
        return Subscriber::GetIntegerParameter(id);
    }
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer MessageWindow::SetIntegerParameter(const Integer id, const Integer value)
{
    switch (id)
    {
    case PRECISION:
        if (value > 0)
        {
            precision = value;
            dstream.precision(precision);
        }
        return precision;
    default:
        return Subscriber::GetIntegerParameter(id);
    }
}
