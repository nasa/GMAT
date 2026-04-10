//$Id$
//------------------------------------------------------------------------------
//                                   FieldOfViewException
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
// Author:Syeda Kazmi
// Created: 5/23/2019
//
/**
 * Exception class used by the  FieldOfView hierarchy.
 */
 //------------------------------------------------------------------------------


#ifndef FieldOfViewException_hpp
#define FieldOfViewException_hpp

#include "gmatdefs.hpp"
#include "BaseException.hpp"

class GMAT_API FieldOfViewException : public BaseException
{
public:

	FieldOfViewException(std::string details = "");
	virtual ~FieldOfViewException();
	FieldOfViewException(const FieldOfViewException &soe);
};

#endif // FieldOfViewException_hpp
#pragma once
