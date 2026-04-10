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
 * Class implementation for the ApiInterpreter
 */
 //------------------------------------------------------------------------------

#include "ApiInterpreter.hpp"
#include "MessageInterface.hpp"
#include "Moderator.hpp"
#include "MathParser.hpp"
#include "NoOp.hpp"
#include "CommandUtil.hpp"     // for GmatCommandUtil::GetCommandSeqString()
#include "StringUtil.hpp"      // for GmatStringUtil::
#include "TimeTypes.hpp"       // for GmatTimeUtil::FormatCurrentTime()
#include "FileManager.hpp"     // for GetGmatIncludePath()
#include "FileUtil.hpp"        // for IsPathAbsolute()
#include "GmatGlobal.hpp"      // for SetIncludeFoundInScriptResource()
#include "UserDefinedFunction.hpp" // for AddFunctionObject()
#include <sstream>             // For stringstream, used to check for non-ASCII chars
#include <algorithm>           // for find()

// to allow object creation in command mode, such as inside ScriptEvent
//#define __ALLOW_OBJECT_CREATION_IN_COMMAND_MODE__


//#define DEBUG_READ_FIRST_PASS
//#define DEBUG_DELAYED_BLOCK
//#define DEBUG_PARSE
//#define DEBUG_PARSE_INCLUDE
//#define DEBUG_PARSE_ASSIGNMENT
//#define DEBUG_PARSE_FOOTER
//#define DEBUG_SET_COMMENTS
//#define DEBUG_SCRIPT_WRITING
//#define DEBUG_SCRIPT_WRITING_PARAMETER
//#define DEBUG_SECTION_DELIMITER
//#define DEBUG_SCRIPT_WRITING_COMMANDS
//#define DBGLVL_SCRIPT_READING 2
//#define DBGLVL_GMAT_FUNCTION 2
//#define DEBUG_COMMAND_MODE_TOGGLE
//#define DEBUG_ENCODING_CHAR
//#define DEBUG_INCLUDE
//#define DEBUG_SCRIPTED_METHODS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

ApiInterpreter *ApiInterpreter::instance = NULL;


//------------------------------------------------------------------------------
// ApiInterpreter* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor for the ApiInterpreter singleton.
 *
 * @return Pointer to the singleton.
 */
 //------------------------------------------------------------------------------
ApiInterpreter* ApiInterpreter::Instance()
{
	if (!instance)
		instance = new ApiInterpreter();
	return instance;
}


//------------------------------------------------------------------------------
// ApiInterpreter()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
 //------------------------------------------------------------------------------
ApiInterpreter::ApiInterpreter() :
	Interpreter()
{
#ifdef DEBUG_INSTANCE
	MessageInterface::ShowMessage("ApiInterpreter::ApiInterpreter() <%p> entered\n", this);
#endif

	InitializeApiData();
	Initialize();
	
	ObjectMap* om = theModerator->GetConfiguredObjectMap();
	SolarSystem* ss = theModerator->GetSolarSystemInUse(1);
	SetObjectMap(om);
	SetSolarSystemInUse(ss);

#ifdef DEBUG_INSTANCE
	MessageInterface::ShowMessage("ApiInterpreter::ApiInterpreter() <%p> leaving\n", this);
#endif
}


//------------------------------------------------------------------------------
// ~ApiInterpreter()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
 //------------------------------------------------------------------------------
ApiInterpreter::~ApiInterpreter()
{
}

//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 * Clears scirpt related data
 */
 //------------------------------------------------------------------------------
void ApiInterpreter::Clear()
{
	InitializeApiData();
}

bool ApiInterpreter::Interpret()
{
#if DBGLVL_SCRIPT_READING
	MessageInterface::ShowMessage
	("ApiInterpreter::Interpret() entered\n   Calling Initialize()\n");
#endif

	Initialize();
	

	return (true);
}

//------------------------------------------------------------------------------
// bool Build()
//------------------------------------------------------------------------------
/**
 * Writes the currently configured data to an output stream.
 *
 * @return true if the file parses successfully, false on failure.
 */
 //------------------------------------------------------------------------------
bool ApiInterpreter::Build(Gmat::WriteMode mode)
{
	if (!initialized)
		Initialize();

	// set configured object map first
	SetConfiguredObjectMap();
	return true;
}



//------------------------------------------------------------------------------
// void InitializeScriptData()
//------------------------------------------------------------------------------
/**
 * Initializes script related data.
 */
 //------------------------------------------------------------------------------
void ApiInterpreter::InitializeApiData()
{
#ifdef DEBUG_INIT
	MessageInterface::ShowMessage
	("ApiInterpreter::InitializeScriptData() this=<%p> entered\n", this);
#endif
	

#ifdef DEBUG_INIT
	MessageInterface::ShowMessage("ApiInterpreter::InitializeScriptData() this=<%p> leaving\n", this);
#endif
}

//------------------------------------------------------------------------------
// Integer ChangeRunState(const std::string &state, Integer sandboxNum)
//------------------------------------------------------------------------------
/**
 * Calls Moderator to change run state.
 *
 * @param <state> run state string ("Stop", "Pause", "Resume")
 * @param <snadobxNum> sandbox number
 *
 * @return a status code
 *    1 = successful, <0 = error (tbd)
 */
 //------------------------------------------------------------------------------
Integer ApiInterpreter::ChangeRunState(const std::string &state, Integer sandboxNum)
{
	return theModerator->ChangeRunState(state, sandboxNum);
}

//------------------------------------------------------------------------------
// bool ParseCommandBlock(const StringArray &chunks, GmatCommand *inCmd,
//                        GmatBase *obj)
//------------------------------------------------------------------------------
/*
 * Parses the command block.
 *
 * @param  chunks  Input string array to be used in the parsing
 * @param  inCmd   Input command pointer to be used to append the new command
 * @param  obj     Ouput object pointer if created
 */
 //------------------------------------------------------------------------------
GmatCommand* ApiInterpreter::ParseCommandBlock(const std::string &type, const std::string &desc, GmatCommand *inCmd)
{
	bool retval = true;
	GmatCommand *obj = NULL;

	if (type == "Breakpoint")
	{
		MessageInterface::ShowMessage("Command type \"" + type + "\" is not supported for the API\n");
		return obj;
	}

	obj = CreateCommand(type, desc, retval, inCmd);
	return obj;
}


//------------------------------------------------------------------------------
// void HandleError(const BaseException &e, bool writeLine, bool isWarning ...)
//------------------------------------------------------------------------------
void ApiInterpreter::HandleError(const BaseException &e, bool writeLine,
	bool isWarning,
	bool showWarning)
{	
	MessageInterface::ShowMessage
	("   => Caught exception: '%s'\n", e.GetFullMessage().c_str());
	
	Interpreter::HandleError(e, writeLine, isWarning, showWarning);	
}
