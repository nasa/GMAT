//$Id$
//------------------------------------------------------------------------------
//                               ApiInterpreter
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
// Author: Alex Campbell
// Created: 2023/03/20
//
//------------------------------------------------------------------------------
/**
 * Class definition for the ApiInterpreter
 */
 //------------------------------------------------------------------------------

#ifndef ApiInterpreter_hpp
#define ApiInterpreter_hpp

#include "Interpreter.hpp"
#include "InterpreterException.hpp"
#include "Function.hpp"
#include <stack>            // for Inlcude script and file postion stack

/**
 * The ApiInterpreter class manages the API reading process.
 */
class GMAT_API ApiInterpreter : public Interpreter
{
public:
	static ApiInterpreter*   Instance();

	virtual void Clear();
	virtual bool Interpret();	

	virtual Integer ChangeRunState(const std::string &state, Integer sandboxNum = 1);

	virtual bool Build(Gmat::WriteMode mode);

	GmatCommand* ParseCommandBlock(const std::string &type, const std::string &desc, GmatCommand *inCmd);

protected:

	/// The API interpreter singleton
	static ApiInterpreter *instance;
	/// Flag indicating #Include statement fouond in the resource mode
	bool includeFoundInResource;

	void InitializeApiData();

	ApiInterpreter();
	virtual ~ApiInterpreter();

	virtual void HandleError(const BaseException &e, bool writeLine = true, bool isWarning = false,
		bool showWarning = true);

private:


};

#endif // ApiInterpreter_hpp

