//------------------------------------------------------------------------------
//                            TdmErrorHandler
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
// Author: Farideh Farahnak
// Created: 2014/7/22
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS.
//
/**
 * ErrorHandler class used by the TdmReadWriter class.
 */
//------------------------------------------------------------------------------

#ifndef TdmErrorHandler_hpp
#define TdmErrorHandler_hpp

#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/sax/ErrorHandler.hpp"
#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/util/XMLString.hpp"
#include <string>

#include "estimation_defs.hpp"

using namespace xercesc;

XERCES_CPP_NAMESPACE_USE

/**
* Class that implements the XML Error Handler
*
* This class implements the basic interface ErrorHandler
* functions to handle and report any SAX parsing errors. 
* A SAX parser must register an instance of this class
* by using the parser's setErrorHandler method.
*/
class  ESTIMATION_API TdmErrorHandler : public ErrorHandler
{
public:
   TdmErrorHandler();
   ~TdmErrorHandler();
   TdmErrorHandler(const TdmErrorHandler &teh);

   /// override pure virtual ErrorHandler base class member functions
   virtual void fatalError(const SAXParseException &ex);
   virtual void error(const SAXParseException &ex);
   virtual void warning(const SAXParseException &ex);
   virtual void resetErrors();

private:
   /// Called upon error occurrences during the parsing
   void reportParseException(const SAXParseException &ex);
   /// Error types
   XMLErrorReporter::ErrTypes errType;
   /// Error name, mapped to the error type
   std::string errName;
};

#endif   //TdmErrorHandler_hpp