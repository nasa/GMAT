//$Id$
//------------------------------------------------------------------------------
//                                SpaceObjectException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: 2004/07/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the SpaceObject hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef SpaceObjectException_hpp
#define SpaceObjectException_hpp

#include "BaseException.hpp"


class GMAT_API SpaceObjectException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // SpaceObjectException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   SpaceObjectException(const std::string &details = "") :
      BaseException("SpaceObject Exception Thrown: ", details)
   {
   }

   //---------------------------------------------------------------------------
   // ~SpaceObjectException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   ~SpaceObjectException()
   {
   }

   //---------------------------------------------------------------------------
   // SpaceObjectException(const SpaceObjectException &soe)
   //---------------------------------------------------------------------------
   /**
    * Copy constructor.
    *
    * @param <soe> Exception used to generate this one.
    */
   //---------------------------------------------------------------------------
   SpaceObjectException(const SpaceObjectException &soe) :
      BaseException(soe)
   {
   }
};

#endif // SpaceObjectException_hpp
