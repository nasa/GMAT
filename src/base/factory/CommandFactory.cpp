//$Id$
//------------------------------------------------------------------------------
//                            CommandFactory
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
// Author: Wendy Shoan
// Created: 2003/10/09
//
/**
 *  Implementation code for the CommandFactory class, responsible for
 *  creating Command objects.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "CommandFactory.hpp"
#include "GmatGlobal.hpp"     // for IsMatlabAvailable()
#include "NoOp.hpp"           // for NoOp command
#include "BeginMissionSequence.hpp" // for BeginMissionSequence command
#include "Toggle.hpp"         // for Toggle command
#include "Propagate.hpp"      // for Propagate command
#include "Maneuver.hpp"       // for Maneuver command
#include "Target.hpp"         // for Target command
#include "Vary.hpp"           // for Vary command
#include "Achieve.hpp"        // for Achieve command
#include "EndTarget.hpp"      // for EndTarget command
#include "For.hpp"            // for For command
#include "EndFor.hpp"         // for EndFor command
#include "If.hpp"             // for IF command
#include "Else.hpp"           // for Else command
#include "EndIf.hpp"          // for EndIf command
#include "While.hpp"          // for FOR command
#include "EndWhile.hpp"       // for EndFor command
#include "Assignment.hpp"     // for Assignment (GMAT) command  
#include "Report.hpp"         // for Report command
#include "SaveMission.hpp"    // for SaveMission command  
#include "Stop.hpp"           // for Stop command
#include "FindEvents.hpp"     // forFindEvents command
//#include "CallGmatFunction.hpp"   // for CallGmatFunction command
#include "CallBuiltinGmatFunction.hpp"
#include "BeginFiniteBurn.hpp"// for BeginFiniteBurn command
#include "EndFiniteBurn.hpp"  // for EndFiniteBurn command
#include "BeginScript.hpp"    // for BeginScript command
#include "EndScript.hpp"      // for EndScript command
#include "Optimize.hpp"       // for Optimize command
#include "EndOptimize.hpp"    // for EndOptimize command
#include "Minimize.hpp"       // for Minimize command
#include "NonlinearConstraint.hpp" // for NonlinearConstraint command

#include "ClearPlot.hpp"      // for ClearPlot command
#include "PenUp.hpp"          // for PenUp command
#include "PenDown.hpp"        // for PenDown command
#include "MarkPoint.hpp"      // for MarkPoint command
//#include "Global.hpp"       // for Global command
#include "Create.hpp"         // for Create command
#include "Write.hpp"          // for Write command
#include "UpdateDynamicData.hpp" // for UpdateDynamicData command

//******************************************************************************
// ElseIf does not work yet. (2008.08.29)
// The workaround is to use nested If-Else statements.
// The work that needs to be done concerns the conditions, IIRC - WCS
#ifdef __INCLUDE_ELSEIF__
#include "ElseIf.hpp"         // for ElseIf command
#endif
//******************************************************************************

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested command class 
 *
 * @param <ofType>   type of command object to create and return.
 * @param <withName> name of the command (currently not used).
 *
 * @return command object
 *
 * @note As of 2003/10/14, we are ignoring the withname parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatBase* CommandFactory::CreateObject(const std::string &ofType,
                                       const std::string &withName)
{
   return CreateCommand(ofType, withName);
}

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType, 
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested command class 
 *
 * @param <ofType>   type of command object to create and return.
 * @param <withName> name of the command (currently not used).
 *
 * @return command object
 *
 * @note As of 2003/10/14, we are ignoring the withname parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatCommand* CommandFactory::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
    if (ofType == "NoOp")
        return new NoOp;
    if (ofType == "BeginMissionSequence")
        return new BeginMissionSequence;
    else if (ofType == "Propagate")
        return new Propagate;
    else if (ofType == "Toggle")
        return new Toggle;
    else if (ofType == "Maneuver")
        return new Maneuver;
    else if (ofType == "Target")
        return new Target;
    else if (ofType == "Vary")
        return new Vary;
    else if (ofType == "Achieve")
        return new Achieve;
    else if (ofType == "EndTarget")
        return new EndTarget;
    else if (ofType == "For")
        return new For;
    else if (ofType == "EndFor")
        return new EndFor;
    else if (ofType == "While")
        return new While;
    else if (ofType == "EndWhile")
        return new EndWhile;
    else if (ofType == "If")
        return new If;
    else if (ofType == "Else")
        return new Else;
#ifdef __INCLUDE_ELSEIF__
    else if (ofType == "ElseIf")
        return new ElseIf;
#endif
    else if (ofType == "EndIf")
        return new EndIf;
    else if (ofType == "GMAT" || ofType == "Equation" || ofType == "Assignment")
        return new Assignment;
    else if (ofType == "Report")
        return new Report;
    else if (ofType == "SaveMission")
        return new SaveMission;
    // Actual creating of CallFunction is not allowed, but it should
    // be added to allowed creatables so that Interpreter can continue
    // with creating proper CallGmatFunction
    //else if (ofType == "CallFunction")
    //   return new CallFunction;
//    else if (ofType == "CallGmatFunction")
//        return new CallGmatFunction;
    else if (ofType == "CallBuiltinGmatFunction")
       return new CallBuiltinGmatFunction;
    else if (ofType == "BeginFiniteBurn")
        return new BeginFiniteBurn;
    else if (ofType == "EndFiniteBurn")
         return new EndFiniteBurn;
    else if (ofType == "BeginScript")
        return new BeginScript;
    else if (ofType == "EndScript")
         return new EndScript;
    else if (ofType == "Stop")
        return new Stop;
    else if (ofType == "FindEvents")
        return new FindEvents;
    else if (ofType == "Optimize")
        return new Optimize;
    else if (ofType == "EndOptimize")
        return new EndOptimize;
    else if (ofType == "Minimize")
        return new Minimize;
    else if (ofType == "NonlinearConstraint")
        return new NonlinearConstraint;
    else if (ofType == "ClearPlot")
        return new ClearPlot;
    else if (ofType == "PenUp")
        return new PenUp;
    else if (ofType == "PenDown")
        return new PenDown;
    else if (ofType == "MarkPoint")
        return new MarkPoint;
//    else if (ofType == "Global")
//        return new Global;
    else if (ofType == "Create")
        return new Create;
    else if (ofType == "Write")
       return new Write;
    else if (ofType == "UpdateDynamicData")
       return new UpdateDynamicData;
   // add more here .......
   else 
   {
      return NULL;   // doesn't match any known type of command
   }
   
}


//------------------------------------------------------------------------------
//  CommandFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CommandFactory.
 * (default constructor)
 *
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory() :
    Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("Achieve");
      creatables.push_back("Assignment");
      creatables.push_back("BeginFiniteBurn");
      creatables.push_back("BeginMissionSequence");
      sequenceStarters.push_back("BeginMissionSequence");
      creatables.push_back("BeginScript");
      creatables.push_back("CallFunction");
      creatables.push_back("CallBuiltinGmatFunction");
//      creatables.push_back("CallGmatFunction");
      creatables.push_back("ClearPlot");
      creatables.push_back("Create");
      creatables.push_back("Write");
      creatables.push_back("Else");
#ifdef __INCLUDE_ELSEIF__
      creatables.push_back("ElseIf");
#endif
      creatables.push_back("EndFor");
      creatables.push_back("EndIf");
      creatables.push_back("EndOptimize");
      creatables.push_back("EndTarget");
      creatables.push_back("EndWhile");
      creatables.push_back("EndScript");
      creatables.push_back("EndFiniteBurn");
      creatables.push_back("Equation");
      creatables.push_back("For");
      creatables.push_back("If");
      creatables.push_back("GMAT");
//      creatables.push_back("Global");
      creatables.push_back("Maneuver");
      creatables.push_back("MarkPoint");
      creatables.push_back("Minimize");
      creatables.push_back("NonlinearConstraint");
      creatables.push_back("NoOp");
      creatables.push_back("Optimize");
      creatables.push_back("PenUp");
      creatables.push_back("PenDown");
      creatables.push_back("Propagate");
      creatables.push_back("Report");
      creatables.push_back("SaveMission");
      creatables.push_back("ScriptEvent");
      creatables.push_back("Stop");
      creatables.push_back("FindEvents");
      creatables.push_back("Target");
      creatables.push_back("Toggle");
      creatables.push_back("Vary");
      creatables.push_back("While");
      creatables.push_back("UpdateDynamicData");
   }
   
   // Now fill in unviewable commands
   // We don't want to show these commands in the MissionTree menu
   if (unviewables.empty())
   {
      // These commands do nothing
      unviewables.push_back("NoOp");
      unviewables.push_back("BeginMissionSequence");
      
      // These commands show as Equation in the MissionTree menu
      unviewables.push_back("Assignment");
      unviewables.push_back("GMAT");
      
      // These commands show as ScriptEvent in the MissionTree menu
      unviewables.push_back("BeginScript");
      
      // These commands only works in object setup mode and inside a GmatFunction
      unviewables.push_back("Create");

      // Commented out. If this breaks lots of GUI testing uncomment this
      //unviewables.push_back("Write");
      
      // CallFunction is parent command of CallGmatFunction and CallMatlabFunction
      unviewables.push_back("CallFunction");
      
      // These commands are only viewable under Target or Optimize
      unviewables.push_back("Achieve");
      unviewables.push_back("Minimize");
      unviewables.push_back("NonlinearConstraint");
      unviewables.push_back("Vary");
      
      // These commands are automatically created via GUI
      unviewables.push_back("For");
      unviewables.push_back("If");
      unviewables.push_back("Else");
      unviewables.push_back("ElseIf");
      unviewables.push_back("While");
      unviewables.push_back("EndFor");
      unviewables.push_back("EndIf");
      unviewables.push_back("EndOptimize");
      unviewables.push_back("EndTarget");
      unviewables.push_back("EndWhile");
      unviewables.push_back("EndScript");
   }

   GmatType::RegisterType(Gmat::COMMAND, "Command");
}

//------------------------------------------------------------------------------
//  CommandFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CommandFactory.
 *
 * @param <createList> list of creatable command objects
 *
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory(StringArray createList) :
    Factory(createList,Gmat::COMMAND)
{
}

//------------------------------------------------------------------------------
//  CommandFactory(const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the (base) class CommandFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory(const CommandFactory& fact) :
    Factory(fact)
{
}

//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CommandFactory base class.
 *
 * @param <fact> the CommandFactory object whose data to assign to "this" factory.
 *
 * @return "this" CommandFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CommandFactory& CommandFactory::operator= (const CommandFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~CommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the CommandFactory base class.
 */
//------------------------------------------------------------------------------
CommandFactory::~CommandFactory()
{
   // deletes handled by Factory destructor
}


//------------------------------------------------------------------------------
// StringArray GetListOfCreatableObjects(const std::string &qualifier) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of creatable objects
 *
 * Override for the base class method so that sequence starter commands can be
 * identified
 *
 * @param qualifier The subtype for the command, if any
 *
 * @return The list of commands, qualified if requested
 */
//------------------------------------------------------------------------------
StringArray CommandFactory::GetListOfCreatableObjects(
                                  const std::string &qualifier)
{
   if (qualifier != "")
   {
      if (qualifier == "SequenceStarters")
         return sequenceStarters;
   }

   return Factory::GetListOfCreatableObjects(qualifier);
}
