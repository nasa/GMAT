//$Id$
//------------------------------------------------------------------------------
//                                  Target
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Target command class
 */
//------------------------------------------------------------------------------

 
#include "Target.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_TARGETER_PARSING
//#define DEBUG_TARGETER
//#define DEBUG_START_MODE
//#define DEBUG_TARGET_COMMANDS
//#define DEBUG_TARGET_INIT
//#define DEBUG_TARGET_EXEC

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif


//------------------------------------------------------------------------------
//  Target()
//------------------------------------------------------------------------------
/**
 * Creates a Target command.  (default constructor)
 */
//------------------------------------------------------------------------------
Target::Target() :
   SolverBranchCommand("Target"),
//   targeterName       (""),
   targeterConverged  (false),
   targeterInFunctionInitialized (false),
   targeterRunOnce(false),
   TargeterConvergedID(parameterCount),
   targeterInDebugMode(false)
{
   parameterCount += 1; // 2;
   objectTypeNames.push_back("Target");
}


//------------------------------------------------------------------------------
//  ~Target(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Target command.  (destructor)
 */
//------------------------------------------------------------------------------
Target::~Target()
{
}

    
//------------------------------------------------------------------------------
//  Target(const Target& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Target command.  (Copy constructor)
 *
 * @param t The target command that is copied.
 */
//------------------------------------------------------------------------------
Target::Target(const Target& t) :
   SolverBranchCommand (t),
   targeterConverged   (false),
   targeterInFunctionInitialized (false),
   targeterRunOnce (false),
   TargeterConvergedID (t.TargeterConvergedID),
   targeterInDebugMode (t.targeterInDebugMode)
{
   parameterCount = t.parameterCount;
   localStore.clear();
}


//------------------------------------------------------------------------------
//  Target& operator=(const Target& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the target command.
 *
 * @param t The target command that is copied.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Target& Target::operator=(const Target& t)
{
   if (this == &t)
      return *this;
    
   GmatCommand::operator=(t);

   targeterConverged   = false;
   targeterInFunctionInitialized = false;
   targeterRunOnce = false;
   TargeterConvergedID = t.TargeterConvergedID;
   targeterInDebugMode = t.targeterInDebugMode;
   localStore.clear();

   return *this;
}


//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the targeter loop.
 *
 * This method calls the SolverBranchCommand base class method that adds a 
 * commandto the command sequence that branches off of the main mission 
 * sequence.  This extension was needed so that the EndTarget command can be set 
 * to point back to the head of the targeter loop.
 *
 * @param cmd The command that gets appended.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Target::Append(GmatCommand *cmd)
{
   #ifdef DEBUG_TARGETER_PARSING
       MessageInterface::ShowMessage("\nTarget::Append received \"%s\" command",
                                     cmd->GetTypeName().c_str());
   #endif
    
   if (!SolverBranchCommand::Append(cmd))
        return false;
    
   // If at the end of a targeter branch, point that end back to this comand.
   if (cmd->GetTypeName() == "EndTarget") {
      if ((nestLevel == 0) && (branchToFill != -1))  {
         cmd->Append(this);
         // Targeter loop is complete; -1 pops to the next higher sequence.
         branchToFill = -1;
         #ifdef DEBUG_TARGETER_PARSING
             MessageInterface::ShowMessage("\nTarget::Append closing \"%s\"",
                                           generatingString.c_str());
         #endif
      }
      else
         --nestLevel;
   }

   // If it's a nested targeter branch, add to the nest level.
   if (cmd->GetTypeName() == "Target")
      ++nestLevel;

   #ifdef DEBUG_TARGETER_PARSING
       MessageInterface::ShowMessage(
             "\nTarget::Append for \"%s\" nest level = %d",
             generatingString.c_str(), nestLevel);
   #endif

   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Target.
 *
 * @return clone of the Target.
 */
//------------------------------------------------------------------------------
GmatBase* Target::Clone() const
{
   return (new Target(*this));
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& Target::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   generatingString = "";
   
   if (mode != Gmat::NO_COMMENTS)
   {
      generatingString = prefix;
   }
   
   generatingString += "Target " + solverName;
   
   // Handle the option strings
   generatingString += GetSolverOptionText();
   
   generatingString += ";";

   if (mode == Gmat::NO_COMMENTS)
   {
	  InsertCommandName(generatingString);
      return generatingString;
   }

   return SolverBranchCommand::GetGeneratingString(mode, prefix, useName);
}

//---------------------------------------------------------------------------
// bool RenameRefObject(const UnsignedInt type,
//                      const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Target::RenameRefObject(const UnsignedInt type,
                             const std::string &oldName,
                             const std::string &newName)
{   
   SolverBranchCommand::RenameRefObject(type, oldName, newName);
   
   return true;
}

// Parameter access methods

//------------------------------------------------------------------------------
//  std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter names.
 *
 * @param <id> the ID of the parameter.
 *
 * @return the text string for the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetParameterText(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return "Targeter";
    
   return SolverBranchCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter IDs.
 *
 * @param <str> the text description of the parameter.
 *
 * @return the integer ID for the parameter.
 */
//------------------------------------------------------------------------------
Integer Target::GetParameterID(const std::string &str) const
{
   if (str == "Targeter")
      return SOLVER_NAME_ID;
   if (str == "TargeterConverged")
      return TargeterConvergedID;
    
   return SolverBranchCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter types.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return the type of the parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Target::GetParameterType(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return Gmat::STRING_TYPE;
   if (id == TargeterConvergedID)
      return Gmat::BOOLEAN_TYPE;
    
   return SolverBranchCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for parameter type data description.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return a string describing the type of the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetParameterTypeString(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   if (id == TargeterConvergedID)
      return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];
    
   return SolverBranchCommand::GetParameterTypeString(id);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Read accessor for string parameters.
 *
 * @param <id> the integer ID of the parameter.
 *
 * @return the string contained in the parameter.
 */
//------------------------------------------------------------------------------
std::string Target::GetStringParameter(const Integer id) const
{
   if (id == SOLVER_NAME_ID)
      return solverName;
    
   return SolverBranchCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Write accessor for string parameters.
 *
 * @param <id> the integer ID of the parameter.
 * @param <value> the new string stored in the parameter.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Target::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVER_NAME_ID) {
      solverName = value;
      return true;
   }
    
   return SolverBranchCommand::SetStringParameter(id, value);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or false if the parameter is
 *         not boolean.
 *
 * @todo Setup the GmatBase Get/Set methods to throw exceptions for invalid
 *       parameter accesses.
 */
//---------------------------------------------------------------------------
bool Target::GetBooleanParameter(const Integer id) const
{
   if (id == TargeterConvergedID)
      return targeterConverged;
      
   return SolverBranchCommand::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  std::string GetRefObjectName(const UnsignedInt type) const
//------------------------------------------------------------------------------
/**
 * Retrieve the name of a reference object.
 *
 * @param <type> The type of object that is being looked up.
 *
 * @return the object's name.
 */
//------------------------------------------------------------------------------
std::string Target::GetRefObjectName(const UnsignedInt type) const
{
   if (type == Gmat::SOLVER)
      return solverName;
   return SolverBranchCommand::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// bool SetRefObjectName(const UnsignedInt type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Set the name of a reference object.
 *
 * @param <type> The type of object.
 * @param <name> The name of the object.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool Target::SetRefObjectName(const UnsignedInt type,
                              const std::string &name)
{
   if (type == Gmat::SOLVER) {
      solverName = name;
      return true;
   }
   return SolverBranchCommand::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the targeter.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Target::Initialize()
{
   #ifdef DEBUG_TARGET_INIT
   MessageInterface::ShowMessage
      ("Target::Initialize() entered, theSolver = <%p>'%s', isInitialized = %d\n"
       "   targeterInFunctionInitialized = %d\n", theSolver, 
       theSolver ? theSolver->GetName().c_str() : "NULL", isInitialized,
       targeterInFunctionInitialized);
   #endif
   
   GmatBase *mapObj = NULL;
   cloneCount = 0;

   if ((mapObj = FindObject(solverName)) == NULL) 
   {
      std::string errorString = "Target command cannot find targeter \"";
      errorString += solverName;
      errorString += "\"";
      throw CommandException(errorString, Gmat::ERROR_);
   }
   
   // Clone the targeter for local use
   #ifdef DEBUG_TARGET_INIT
   MessageInterface::ShowMessage
      ("Target::Initialize() cloning mapObj <%p>'%s'\n", mapObj,
       mapObj->GetName().c_str());
   MessageInterface::ShowMessage
      ("mapObj maxIter=%d\n",
       mapObj->GetIntegerParameter(mapObj->GetParameterID("MaximumIterations")));
   #endif 

   // Delete the old cloned solver
   if (theSolver)
   {
	   if (theSolver->GetBooleanParameter(theSolver->GetParameterID("Nested")))
	   {
		   branchExecuting = false;
		   commandExecuting = false;
		   return true;		   
	   }
	   else {		
#ifdef DEBUG_MEMORY
		   MemoryTracker::Instance()->Remove
		   (theSolver, "local solver", "Target::Initialize()",
			   "deleting local cloned solver");
#endif
		   delete theSolver;
	   }
   }
   
   theSolver = (Solver *)(mapObj->Clone());
   if (theSolver != NULL)
      ++cloneCount;

   #ifdef DEBUG_TARGET_INIT
   MessageInterface::ShowMessage
      ("Target::Initialize() theSolver <%p>'%s', cloneCount=%d\n", theSolver,
       theSolver->GetName().c_str(), cloneCount);
   #endif
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (theSolver, theSolver->GetName(), "Target::Initialize()",
       "theSolver = (Solver *)(mapObj->Clone())");
   #endif
   
   theSolver->TakeAction("ResetInstanceCount");
   mapObj->TakeAction("ResetInstanceCount");
   
   theSolver->TakeAction("IncrementInstanceCount");
   mapObj->TakeAction("IncrementInstanceCount");
   
   if (theSolver->GetStringParameter("ReportStyle") == "Debug")
      targeterInDebugMode = true;
   theSolver->SetStringParameter("SolverMode", 
         GetStringParameter(SOLVER_SOLVE_MODE));
   theSolver->SetStringParameter("ExitMode", 
         GetStringParameter(SOLVER_EXIT_MODE));
   
   // Set the local copy of the targeter on each node
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *current;
   specialState = Solver::INITIALIZING;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;

      #ifdef DEBUG_TARGET_COMMANDS
         Integer nodeNum = 0;
      #endif
      while ((current != NULL) && (current != this))
      {
         #ifdef DEBUG_TARGET_COMMANDS
            MessageInterface::ShowMessage(
               "   Target Command %d:  %s\n", ++nodeNum, 
               current->GetTypeName().c_str());       
         #endif
         if ((current->GetTypeName() == "Vary") || 
             (current->GetTypeName() == "Achieve"))
            current->SetRefObject(theSolver, Gmat::SOLVER, solverName);
         current = current->GetNext();
      }
   }

   bool retval = SolverBranchCommand::Initialize();
   
   if (retval == true) {
      // Targeter specific initialization goes here:
      if (FindObject(solverName) == NULL) 
      {
         std::string errorString = "Target command cannot find targeter \"";
         errorString += solverName;
         errorString += "\"";
         throw CommandException(errorString);
      }

      retval = theSolver->Initialize();
   }
   
   // Set this to true for two-mode function parsing (LOJ: 2015.03.09)
   //targeterInFunctionInitialized = false;
   targeterInFunctionInitialized = true;
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this targeting loop.
 *
 * This method (will eventually) runs the targeter state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Target::Execute()
{
   #ifdef DEBUG_TARGET_EXEC
   MessageInterface::ShowMessage
      ("\nTarget::Execute() entered, theSolver=<%p>'%s', ", (GmatBase*)theSolver,
       theSolver->GetName().c_str());
   MessageInterface::ShowMessage
      ("maxIter=%d\n",
       theSolver->GetIntegerParameter(theSolver->GetParameterID("MaximumIterations")));
   MessageInterface::ShowMessage
      ("currentFunction=<%p>'%s'\n",
       currentFunction, currentFunction ? ((GmatBase*)currentFunction)->GetName().c_str() : "NULL");
   MessageInterface::ShowMessage
      ("targeterInFunctionInitialized=%d\n", targeterInFunctionInitialized);
   #endif
   
   // If targeting inside a function, we need to reinitialize since the local solver is
   // cloned in Initialize(). All objects including solvers are initialized in
   // assignment command which happens after Target::Initialize(). (LOJ: 2009.03.17)
   if (currentFunction != NULL && !targeterInFunctionInitialized)
   {
      Initialize();
      targeterInFunctionInitialized = true;
   }
   
   bool retval = true;
   
   // Drive through the state machine.
   Solver::SolverState state = theSolver->GetState();
   
   #ifdef DEBUG_TARGET_COMMANDS
      MessageInterface::ShowMessage("TargetExecute(%c%c%c%d)\n",
         (commandExecuting?'Y':'N'),
         (commandComplete?'Y':'N'),
         (branchExecuting?'Y':'N'),
         state);
      MessageInterface::ShowMessage("   targeterConverged=%d\n", 
            targeterConverged);
   #endif
      

   bool prepareForRun = true;
   // Attempt to reset if recalled   
   if (commandComplete)
   {
      commandComplete = false;
      commandExecuting = false;
      specialState = Solver::INITIALIZING;
      prepareForRun = false;
   }  

   if (!commandExecuting) 
   {
      #ifdef DEBUG_TARGET_COMMANDS
          MessageInterface::ShowMessage(
          "Entered Targeter while command is not executing\n");
      #endif

      FreeLoopData();
      if (prepareForRun)
         StoreLoopData();

      retval = SolverBranchCommand::Execute();

      #ifdef DEBUG_TARGETER
         MessageInterface::ShowMessage("Resetting the Differential Corrector\n");
      #endif

      theSolver->TakeAction("Reset");
      state = theSolver->GetState();
   }
   
   if (branchExecuting)
   {
	  theSolver->SetBooleanParameter(theSolver->GetParameterID("Nested"),true);
      retval = ExecuteBranch();
	  theSolver->SetBooleanParameter(theSolver->GetParameterID("Nested"), false);

      if (!branchExecuting)
      {
         if ((state == Solver::FINISHED) || (specialState == Solver::FINISHED))
         {
            PenDownSubscribers();
            LightenSubscribers(1);
            commandComplete = true;
         }
         else
         {
            PenUpSubscribers();
         }
      }
   }
   else
   {
      GmatCommand *currentCmd;
   
      ChangeRunState(Gmat::SOLVING);
      
      switch (startMode)
      {
         case RUN_INITIAL_GUESS:
            #ifdef DEBUG_START_MODE
               MessageInterface::ShowMessage(
                     "Running as RUN_INITIAL_GUESS, specialState = %d, currentState = %d\n",
                     specialState, theSolver->GetState());
            #endif
            switch (specialState) 
            {
               case Solver::INITIALIZING:
                  // Finalize initialization of the targeter data
                  currentCmd = branch[0];
                  targeterConverged = false;
                  while (currentCmd != this)  
                  {
                     std::string type = currentCmd->GetTypeName();
                     if ((type == "Target") || (type == "Vary") ||
                         (type == "Achieve"))
                        currentCmd->Execute();
                     currentCmd = currentCmd->GetNext();
                  }

                  StoreLoopData();
                  specialState = Solver::NOMINAL;
                  break;
                     
               case Solver::NOMINAL:
                  // Execute the nominal sequence
                  if (!commandComplete) 
                  {
                     branchExecuting = true;
                     ResetLoopData();
                  }
                  specialState = Solver::RUNSPECIAL;
                  break;
                  
               case Solver::RUNSPECIAL:
                  // Run once more to publish the data from the converged state
                  if (!commandComplete)
                  {
                     ResetLoopData();
                     branchExecuting = true;
                     ChangeRunState(Gmat::SOLVEDPASS);
                  }
                  theSolver->Finalize();
                  specialState = Solver::FINISHED;

                  // Final clean-up
                  targeterConverged = true;
                  break;
                  
               case Solver::FINISHED:
                  specialState = Solver::INITIALIZING;
                  break;

               default:
                  break;
            }                     
            break;
            
         case RUN_SOLUTION:
            #ifdef DEBUG_START_MODE
               MessageInterface::ShowMessage(
                     "Running as RUN_SOLUTION, state = %d\n", state);
            #endif
            throw SolverException(
                  "Run Solution is not yet implemented for the Target "
                  "command\n");
            break;
         
         case RUN_AND_SOLVE:
         default:
            #ifdef DEBUG_START_MODE
               MessageInterface::ShowMessage(
                     "Running as RUN_AND_SOLVE or default, state = %d\n", 
                     state);
            #endif
            switch (state) 
            {
               case Solver::INITIALIZING:
                  // Finalize initialization of the targeter data
                  currentCmd = branch[0];
                  targeterConverged = false;
                  StoreLoopData();
                  while (currentCmd != this)  
                  {
                     std::string type = currentCmd->GetTypeName();
                     if ((type == "Target") || (type == "Vary") ||
                         (type == "Achieve"))
                     {
                        currentCmd->Execute();
                        if ((type == "Vary") && (targeterRunOnce))
                           currentCmd->TakeAction("SolverReset");
                     }
                     currentCmd = currentCmd->GetNext();
                  }
                  GetActiveSubscribers();
                  SetSubscriberBreakpoint();
                  break;
                     
               case Solver::NOMINAL:
                  // Execute the nominal sequence
                  if (!commandComplete) 
                  {
                     branchExecuting = true;
                     ApplySubscriberBreakpoint();
                     PenDownSubscribers();
                     LightenSubscribers(1);
                     ResetLoopData();
                  }
                  break;
                     
               case Solver::CHECKINGRUN:
                  // Check for convergence; this is done in the targeter state
                  // machine, so this case is a NoOp for the Target command
                  break;
         
               case Solver::PERTURBING:
                  branchExecuting = true;
                  ApplySubscriberBreakpoint();
                  PenDownSubscribers();
                  LightenSubscribers(4);
                  ResetLoopData();
                  break;
                     
               case Solver::CALCULATING:
                  // Calculate the next set of variables to use; this is 
                  // performed in the targeter -- nothing to be done here
                  break;
                     
               case Solver::FINISHED:
                  // Final clean-up
                  targeterConverged = true;
                  targeterRunOnce = true;
                  
                  // Run once more to publish the data from the converged state
                  if (!commandComplete)
                  {
                     ResetLoopData();
                     branchExecuting = true;
                     ApplySubscriberBreakpoint();
                     PenDownSubscribers();
                     LightenSubscribers(1);
                     ChangeRunState(Gmat::SOLVEDPASS);
                  }
                  break;
                     
               case Solver::ITERATING:     // Intentional fall-through
               default:
                  throw CommandException(
                     "Invalid state in the Targeter state machine");
            }
            break;
      }
   }
   
   if (!branchExecuting)
   {
      theSolver->AdvanceState();
      theSolver->ReportProgress(listeners, Solver::UNDEFINED_STATE);

      if (theSolver->GetState() == Solver::FINISHED)
      {
         publisher->FlushBuffers();
         targeterConverged = true;
      }
   }
   
   // Pass spacecraft data to the targeter for reporting in debug mode
   if (targeterInDebugMode)
   {
      std::string dbgData = "";
      for (ObjectArray::iterator i = localStore.begin(); i < localStore.end(); 
           ++i)
      {
         dbgData += (*i)->GetGeneratingString() + "\n---\n";
      }
      theSolver->SetDebugString(dbgData);
   }
   BuildCommandSummary(true);
   
   #ifdef DEBUG_TARGET_EXEC
   MessageInterface::ShowMessage
      ("Target::Execute() returning %d, theSolver=<%p>'%s'\n", retval,
       theSolver, theSolver->GetName().c_str());
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Tells the sequence that the run was ended, possibly before reaching the end.
 */
//------------------------------------------------------------------------------
void Target::RunComplete()
{
   if (theSolver != NULL)
      theSolver->Finalize();
   
   // Free local data (LOJ: 2009.03.17)
   FreeLoopData();
   
   SolverBranchCommand::RunComplete();
}
