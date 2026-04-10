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
 * Declares operation of MessageWindow class.
 */
//------------------------------------------------------------------------------
#ifndef MessageWindow_hpp
#define MessageWindow_hpp

#include "Subscriber.hpp"
#include <sstream>

class GMAT_API MessageWindow : public Subscriber
{
public:
   MessageWindow(const std::string &name);
   MessageWindow(const MessageWindow &mw);
   virtual ~MessageWindow(void);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool        IsParameterReadOnly(const Integer id) const;
   
   virtual Integer     GetIntegerParameter(const Integer id) const;
   virtual Integer     SetIntegerParameter(const Integer id,
                                          const Integer value);

protected:
   /// Precision for output of real data
   Integer precision;
   
   enum
   {
      PRECISION = SubscriberParamCount,
      MessageWindowParamCount
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[MessageWindowParamCount - SubscriberParamCount];
   static const std::string
      PARAMETER_TEXT[MessageWindowParamCount - SubscriberParamCount];
   
   virtual bool Distribute(Integer len);
   virtual bool Distribute(const Real * dat, Integer len);
   
   std::stringstream dstream;
};

#endif
