//------------------------------------------------------------------------------
//                              Substr
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
// Developed jointly by NASA / GSFC and Pearl River Technologies
//
// Author: Peter Candell
// Created: 2024.07.10
//
/**
 * Declares Substr class.
 */
 //------------------------------------------------------------------------------
#ifndef Substr_hpp
#define Substr_hpp

#include "StringFunctionNode.hpp"

class GMAT_API Substr : public StringFunctionNode
{
public:
	Substr(const std::string& name);
	virtual ~Substr();
	Substr(const Substr& copy);
	Substr& operator=(const Substr& ss);

	// inherited from GmatBase
	virtual GmatBase* Clone() const;

	// inherited from MathNode
	virtual std::string EvaluateString();
};

#endif // Substr_hpp
