//$Id$
//------------------------------------------------------------------------------
//                              BaseException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/09
//
/**
 * This class provides base exception class, from which all GMAT exceptions must 
 * be derived.
 */
//------------------------------------------------------------------------------
#ifndef BaseException_hpp
#define BaseException_hpp

#include "utildefs.hpp"
#include <string>

class GMATUTIL_API BaseException
{
public:
   virtual std::string GetFullMessage() const;
   virtual std::string GetDetails() const;
   virtual bool IsFatal() const;
   virtual void SetMessage(const std::string &message);
   virtual void SetDetails(const std::string &details);
   virtual void SetFatal(bool fatal);
   virtual void SetDetails(const char *details, ...);
   virtual Gmat::MessageType GetMessageType();
   virtual void SetMessageType(Gmat::MessageType mt);
   
   const BaseException& operator=(const std::string &newMessage);
   
   
   static const int MAX_MESSAGE_LENGTH = 3000;
   
protected:
   BaseException(const std::string& message = "",
         const std::string &details = "",
         // Change to this when the repeated instances are fixed:
//         Gmat::MessageType mt = Gmat::ERROR_);
         // Change to this if it's problematic:
         Gmat::MessageType mt = Gmat::GENERAL_);
   BaseException(const BaseException& be);
   virtual ~BaseException();
   const BaseException& operator=(const BaseException& be);
   
private:
   std::string theMessage;
   std::string theDetails;
   Gmat::MessageType msgType;
   bool isFatal;
};
#endif // BaseException_hpp

