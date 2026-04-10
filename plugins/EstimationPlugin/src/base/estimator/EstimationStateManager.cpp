//$Id: EstimationStateManager.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EstimationStateManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/05
//
/**
 * Implementation code for the estimation state manager.
 */
//------------------------------------------------------------------------------


#include "EstimationStateManager.hpp"
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"
#include "GmatBase.hpp"              // Thrust Scale Factor Solve For
#include "StateConversionUtil.hpp"
#include "Planet.hpp"
#include "Plate.hpp"
#include "ODEModel.hpp"
#include "StringUtil.hpp"
#include "Receiver.hpp"
#include "ErrorModel.hpp"
#include <sstream>                  // for stringstream


//#define DEBUG_ESM_LOADING
//#define DEBUG_STATE_CONSTRUCTION
//#define DEBUG_OBJECT_UPDATES
//#define DUMP_STATE
//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_CLONING
//#define DEBUG_COVARIANCE
//#define DEBUG_INITIALIZATION
//#define DEBUG_STM_MAPPING

//------------------------------------------------------------------------------
// EstimationStateManager(Integer size)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param size The state manager size
 */
//------------------------------------------------------------------------------
EstimationStateManager::EstimationStateManager(Integer size) :
   StateManager         (size),
   psm                  (NULL),
   propagationState     (NULL),
   hasStateOffset       (false),
   stateOffset          (size),
   measMan              (NULL)
{
   // Estimation always use precision time for calculation
   state.SetPrecisionTimeFlag(true);
}


//------------------------------------------------------------------------------
// ~EstimationStateManager()
//------------------------------------------------------------------------------
/**
 * Class destructor
 */
//------------------------------------------------------------------------------
EstimationStateManager::~EstimationStateManager()
{
   CleanUp();
}


//------------------------------------------------------------------------------
// EstimationStateManager(const EstimationStateManager & esm)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param esm The state manager that is being copied
 */
//------------------------------------------------------------------------------
EstimationStateManager::EstimationStateManager(
      const EstimationStateManager & esm) :
   StateManager            (esm),
   solveForNames           (esm.solveForNames),
   solveForObjectNames     (esm.solveForObjectNames),
   solveForIDNames         (esm.solveForIDNames),
   considerNames           (esm.considerNames),
   considerObjectNames     (esm.considerObjectNames),
   considerIDNames         (esm.considerIDNames),
   participantNames        (esm.participantNames),
   stmMap                  (esm.stmMap),
   stmColCount             (esm.stmColCount),
   covMap                  (esm.covMap),
   covColCount             (esm.covColCount),
   hasStateOffset          (esm.hasStateOffset),
   stateOffset             (esm.stateOffset)
{
   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   {
      solveForObjects.push_back(NULL);
      solveForIDs.push_back(-1);
   }
   for (UnsignedInt i = 0; i < considerNames.size(); ++i)
   {
      considerObjects.push_back(NULL);
      considerIDs.push_back(-1);
   }

   // thrust scale factor solve for
   for (std::vector<PhysicalModel*>::const_iterator i = esm.pms.begin();
	   i != esm.pms.end(); ++i)
   {
	   pms.push_back((PhysicalModel*)((*i)->Clone()));
   }
}


void EstimationStateManager::CleanUp()
{
   solveForNames.clear();
   solveForObjectNames.clear();
   solveForIDNames.clear();
   solveForIDs.clear();
   considerNames.clear();
   considerObjectNames.clear();
   considerIDNames.clear();
   considerIDs.clear();
   participantNames.clear();

   measMan = NULL;
   chunks.clear();
   
   // clean up ObjectArray estimationObjectClones;
   for (UnsignedInt i = 0; i < estimationObjectClones.size(); ++i)
   {
      if (estimationObjectClones[i])
         delete estimationObjectClones[i];
   }
   estimationObjectClones.clear();
   
   // clean up ObjectArray solveForObjects;
   solveForObjects.clear();

   // clean up ObjectArray considerObjects;
   considerObjects.clear();


   // clean up ObjectMap stmMap;
   stmMap.clear();

   // clean up Rmatrix stm;
   // clean up Covariance covariance;

   // clean up ObjectMap covMap;
   covMap.clear();
   
   /// Pointers to the FileThrust              Thrust Scale Factor Solve For
   std::vector<PhysicalModel*>    pms;
   for (Integer i = 0; i < pms.size(); ++i)
   {
      if (pms[i])
         delete pms[i];
   }
   pms.clear();

   // clean up PropagationStateManager *psm;
   psm = NULL; 
   // clean up GmatState *propagationState;
   propagationState = NULL;

}


//------------------------------------------------------------------------------
// EstimationStateManager& operator=(const EstimationStateManager & esm)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param esm The state manager assigning data to this one
 *
 * @return This state manager, configured to match esm
 */
//------------------------------------------------------------------------------
EstimationStateManager& EstimationStateManager::operator=(
      const EstimationStateManager & esm)
{
   if (&esm != this)
   {
      StateManager::operator=(esm);
      solveForNames       = esm.solveForNames;
      solveForObjectNames = esm.solveForObjectNames;
      solveForIDNames     = esm.solveForIDNames;
      considerNames       = esm.considerNames;
      considerObjectNames = esm.considerObjectNames;
      considerIDNames     = esm.considerIDNames;
      participantNames    = esm.participantNames;
      stmMap              = esm.stmMap;
      stmColCount         = esm.stmColCount;
      covMap              = esm.covMap;
      covColCount         = esm.covColCount;
      hasStateOffset      = esm.hasStateOffset;
      stateOffset         = esm.stateOffset;
      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
      {
         solveForObjects.push_back(NULL);
         solveForIDs.push_back(-1);
      }
      for (UnsignedInt i = 0; i < considerNames.size(); ++i)
      {
         considerObjects.push_back(NULL);
         considerIDs.push_back(-1);
      }

	  // thrust scale factor solve for
	  for (std::vector<PhysicalModel*>::iterator i = pms.begin();
		  i != pms.end(); ++i)
		  delete (*i);
	  pms.clear();
	  // thrust scale factor solve for
	  for (std::vector<PhysicalModel*>::const_iterator i = esm.pms.begin();
		  i != esm.pms.end(); ++i)
	  {
		  pms.push_back((PhysicalModel*)((*i)->Clone()));
	  }
   }

   return *this;
}


//------------------------------------------------------------------------------
// const StringArray& GetObjectList(std::string ofType)
//------------------------------------------------------------------------------
/**
 * Retrieves the names of all of the objects used by this estimation state
 * manager.
 *
 * @param ofType The type of object names requested.  Pass in the empty string
 *               for all objects.
 *
 * @return The list of objects
 */
//------------------------------------------------------------------------------
const StringArray& EstimationStateManager::GetObjectList(std::string ofType)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstimationStateManager::GetObjectList(%s) "
            "called\n", ofType.c_str());
   #endif
   chunks.clear();

   if (ofType == "")
      chunks = StateManager::GetObjectList(ofType);

   if ((ofType == "") || (ofType == "SolveFor"))
   {
      for (UnsignedInt i = 0; i < solveForObjectNames.size(); ++i)
      {
  	 		#ifdef DEBUG_INITIALIZATION
         	MessageInterface::ShowMessage("ESM processing solveForObjectNames[%d] %s\n", i,
            	   solveForObjectNames[i].c_str());
			#endif

         if (find(chunks.begin(), chunks.end(), solveForObjectNames[i]) ==
               chunks.end())
            chunks.push_back(solveForObjectNames[i]);
      }
   }

   if ((ofType == "") || (ofType == "Consider"))
   {
      for (UnsignedInt i = 0; i < considerObjectNames.size(); ++i)
      {
#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("ESM processing considerObjectNames[%d] %s\n", i,
            considerObjectNames[i].c_str());
#endif

         if (find(chunks.begin(), chunks.end(), considerObjectNames[i]) ==
               chunks.end())
            chunks.push_back(considerObjectNames[i]);
      }
   }

   if ((ofType == "") || (ofType == "Participants"))
   {
      for (UnsignedInt i = 0; i < participantNames.size(); ++i)
      {
#ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("ESM processing participantNames[%d] %s\n", i,
            participantNames[i].c_str());
#endif

         if (find(chunks.begin(), chunks.end(), participantNames[i]) ==
               chunks.end())
            chunks.push_back(participantNames[i]);
      }
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Current Object List:\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", chunks[i].c_str());
   #endif

   return chunks;
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets the object pointers for objects used in this estimation state manager.
 *
 * @param obj The object pointer that is getting set
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetObject(GmatBase *obj)
{
   #ifdef DEBUG_ESM_LOADING
      MessageInterface::ShowMessage("+++Adding <%s,%p> to the ESM\n",
            obj->GetFullName().c_str(), obj);
   #endif

   bool retval = false;

   // If measurement manager has one of these objects (by name), use that one
   GmatBase *clone = measMan->GetClone(obj);
   if (clone != NULL)
      obj = clone;

   // Be sure object is not already in the list
   if (find(objects.begin(), objects.end(), obj) == objects.end())   // It accepts clone objects in objects list
   {
      // Set flag to tell participant using precision time
      obj->SetPrecisionTimeFlag(true);

      objects.push_back(obj);
      current = obj;
      StringArray *objectProps = new StringArray;
      elements[current] = objectProps;

      try
      {
         if (obj->IsOfType(Gmat::FORMATION))
         {
            Integer id = obj->GetParameterID("A1Epoch");
            epochIDs.push_back(id);
         }
         else if (obj->IsOfType(Gmat::PHYSICAL_MODEL))
         {
            // Don't update epochs of force models
            epochIDs.push_back(-1);
         }
         else
         {
            Integer id = obj->GetParameterID("Epoch");
            if (obj->GetParameterType(id) != Gmat::REAL_TYPE)
               id = obj->GetParameterID("A1Epoch");
            epochIDs.push_back(id);
         }
      }
      catch (BaseException&)
      {
         // Epoch is not a valid parameter
         epochIDs.push_back(-1);
      }
   

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("Object set; current points to <%s,%p>\n",
            current->GetName().c_str(), current);
      #endif

   }

   #ifdef DEBUG_ESM_LOADING
      MessageInterface::ShowMessage("   ESM now has %d objects, %d "
            "solveForObjects, and %d solveFors\n", objects.size(), solveForObjects.size(),
            solveForIDNames.size());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool CheckObjects()
//------------------------------------------------------------------------------
/**
 * Checks the object pointers for objects used in this estimation state manager.
 *
 *
 * @return true if the objects solvefors are valid, false if not
 */
 //------------------------------------------------------------------------------
bool EstimationStateManager::CheckObjects()
{
   bool retval = true;
   for (UnsignedInt i = 0; i < solveForObjectNames.size(); ++i)
   {
      bool foundObject = false;
      std::string errmsg = "";
      for (int k = 0; k < objects.size(); ++k)
      {
         GmatBase * obj = objects[k];
         std::string objFullName = obj->GetFullName();
         // Set the object property
         if (solveForObjectNames[i] == objFullName)
         {
            // Set the property ID
            Integer id = obj->GetEstimationParameterID(solveForIDNames[i]);

            // verify valid ID
            if (id == -1)
               errmsg = "Error: Solve-for parameter " + obj->GetName() + "." + solveForIDNames[i] + " does not exist.\n";
            else if (obj->IsEstimationParameterValid(id) == false)
               errmsg = "Error: parameter " + obj->GetName() + "." + solveForIDNames[i] + " is not allowed for use as a solve-for variable in this GMAT version.\n";
            else
            {
               solveForObjects[i] = obj;
               // set the object property
               solveForIDs[i] = id;
               elements[obj]->push_back(solveForIDNames[i]);
               foundObject = true;
            }
         }
      }
      if (foundObject == false)
         throw EstimatorException(errmsg);
   }

#ifdef DEBUG_ESM_LOADING
   MessageInterface::ShowMessage("   ESM now has %d objects, %d "
      "solveForObjects, and %d solveFors\n", objects.size(), solveForObjects.size(),
      solveForIDNames.size());
#endif

   return retval;
}

//------------------------------------------------------------------------------
// void EstimationStateManager::BufferObjects()
//------------------------------------------------------------------------------
/**
 * Clones the objects used in the estimation so that they can be restored later
 */
//------------------------------------------------------------------------------
void EstimationStateManager::BufferObjects(ObjectArray *buffer)
{
#ifdef DEBUG_CLONING
   MessageInterface::ShowMessage("EstimationStateManager::BufferObjects(%p) "
         "called\n", buffer);
#endif

   GmatBase *clone;

   // Clean up internal buffer
   for (UnsignedInt i = 0; i < estimationObjectClones.size(); ++i)
   {
      if (estimationObjectClones[i])
         delete estimationObjectClones[i];
   }
   estimationObjectClones.clear();

   // Clean up external buffer if it is existed
   if (buffer != NULL)
   {
      for (UnsignedInt i = 0; i < buffer->size(); ++i)
         delete (*buffer)[i];
      buffer->clear();
   }

   // Create clones for all objects and put them in internal buffer and external buffer 
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      clone = objects[i]->Clone();
      estimationObjectClones.push_back(clone);
      if (buffer != NULL)
         buffer->push_back(clone->Clone());
   }
}


//------------------------------------------------------------------------------
// void EstimationStateManager::RestoreObjects()
//------------------------------------------------------------------------------
/**
 * Restores the objects from clones created in the BufferObjects() method
 */
//------------------------------------------------------------------------------
void EstimationStateManager::RestoreObjects(ObjectArray *fromBuffer)
{
   #ifdef DEBUG_CLONING
      MessageInterface::ShowMessage("EstimationStateManager::RestoreObjects(%p)"
            " called\n", fromBuffer);
      MessageInterface::ShowMessage("   %d objects and %d clones\n",
            objects.size(), estimationObjectClones.size());
      for (Integer i = 0; i < objects.size(); ++i)
         MessageInterface::ShowMessage("objects[%d] = <%p,%s>\n", i, objects[i], objects[i]->GetName().c_str());
      for (Integer i = 0; i < estimationObjectClones.size(); ++i)
         MessageInterface::ShowMessage("estimationObjectClones[%d] = <%p,%s>\n", i, estimationObjectClones[i], estimationObjectClones[i]->GetName().c_str());
   #endif

   // Set retore buffer to be external buffer
   ObjectArray *restoreBuffer = fromBuffer;

   // Set retore buffer to be internal buffer if external buffer does not exist
   if (fromBuffer == NULL)
   {
      restoreBuffer = &estimationObjectClones;
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Using internal buffer\n");
      #endif
   }

   // The restore buffer has to be the same size of objects
   if (restoreBuffer->size() != objects.size())
   {
      std::stringstream errmsg;
      errmsg << "EstimationStateManager::RestoreObjects(): "
                "Clone size mismatch; there are " << objects.size()
             << " objects and " << restoreBuffer->size() << " clones.";
      throw EstimatorException(errmsg.str());
   }
   
   for (UnsignedInt i = 0; i < restoreBuffer->size(); ++i)
   {
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Setting object %s from clone named "
               "%s\n", objects[i]->GetName().c_str(),
               (*restoreBuffer)[i]->GetName().c_str());
      #endif
      if (objects[i] != (*restoreBuffer)[i])
      {
         if (objects[i]->IsOfType(Gmat::SPACECRAFT))
            ((Spacecraft*)(objects[i]))->operator=(*((Spacecraft*)((*restoreBuffer)[i])));
         else
            (objects[i])->operator= (*((*restoreBuffer)[i]));
      }
      
      #ifdef DEBUG_CLONING
         MessageInterface::ShowMessage("Object <%p,%s> data:\n%s", objects[i], objects[i]->GetName().c_str(),
               objects[i]->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
      #endif
   }
   
   #ifdef DEBUG_CLONING
      MessageInterface::ShowMessage("EstimationStateManager::RestoreObjects() "
            "finished\n");
   #endif
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string prop)
//------------------------------------------------------------------------------
/**
 * Method used to identify SolveFor parameters to the estimation state manager
 *
 * @param prop String identifying the SolveFor parameter
 *
 * @return true if the parameter is accepted, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string prop)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting SolveFor parameter \"%s\"\n",
            prop.c_str());
   #endif
   
   bool retval = false;

   if (find(solveForNames.begin(), solveForNames.end(), prop) ==
         solveForNames.end())
   {
      // Decompose the property name into object name and property ID
      DecomposeParameter(prop, 2);
      if (chunks.size() < 2)
         throw EstimatorException("SolveFor parameter is not properly "
               "formatted; the format entered, \"" + prop +
               "\", should be in the form object.parameter.");

      // Save the property elements and fill out the vectors
      solveForNames.push_back(prop);
      solveForObjectNames.push_back(chunks[0]);
      solveForObjects.push_back(NULL);
      solveForIDNames.push_back(chunks[1]);
      solveForIDs.push_back(-1);

      retval = true;
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstmationStateManager::SetProperty():  Current SolveFor parameters:\n");
   
	   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   		  MessageInterface::ShowMessage("  solve for name = '%s';  object name = '%s';   parameter = '%s'\n", solveForNames[i].c_str(), solveForObjectNames[i].c_str(), solveForIDNames[i].c_str());
   #endif 

   return retval;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string prop, Integer loc)
//------------------------------------------------------------------------------
/**
 * Method used to identify SolveFor parameters to the estimation state manager,
 * potentially replacing a parameter that is already in place.
 *
 * @param prop String identifying the SolveFor parameter
 * @param loc The location in the SolveFor array desired for this property
 *
 * @return true if the parameter is accepted, false if not
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string prop, Integer loc)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting SolveFor parameter %d to \"%s\"\n",
            loc, prop.c_str());
   #endif 
   bool retval = false;

   if (find(solveForNames.begin(), solveForNames.end(), prop) ==
         solveForNames.end())
   {
      // Decompose the property name into object name and property ID
      DecomposeParameter(prop, 2);
      if (chunks.size() < 2)
         throw EstimatorException("SolveFor parameter is not properly "
               "formatted; the format entered, \"" + prop +
               "\", should be in the form object.parameter.");

      // Save the property elements
      if (loc == (Integer)solveForNames.size())
      {
         // Save the property elements and fill out the vectors
         solveForNames.push_back(prop);
         solveForObjectNames.push_back(chunks[0]);
         solveForObjects.push_back(NULL);
         solveForIDNames.push_back(chunks[1]);
         solveForIDs.push_back(-1);
         retval = true;
      }
      else if ((loc < (Integer)solveForNames.size()) && (loc >= 0))
      {
         solveForNames[loc]       = prop;
         solveForObjectNames[loc] = chunks[0];
         solveForObjects[loc]     = NULL;
         solveForIDNames[loc]     = chunks[1];
         solveForIDs[loc]         = -1;
         retval = true;
      }
      else
         throw EstimatorException("Cannot set the SolveFor parameter " + prop);
   }
   else
   {
      // Display a message and do nothing
      MessageInterface::ShowMessage("Solve-for '%s' was set twice to Estimation State Manager. Skip setting...\n", prop.c_str());
      return true;
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstimationStateManager::SetProperty():    Current SolveFor parameters:\n");
      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
	     MessageInterface::ShowMessage("  solve for name = '%s';  object name = '%s';   parameter = '%s'\n", solveForNames[i].c_str(), solveForObjectNames[i].c_str(), solveForIDNames[i].c_str());
   #endif
   
   return retval;
}


bool EstimationStateManager::IsPropertiesSetupCorrect()
{
   if (solveForNames.size() == 0)
      throw EstimatorException("Error: No solvefor parameters are set to estimation.\n");

   StringArray stateSolveFors = { "CartesianState", "KeplerianState" };
   bool hasStateSolveFor = false;

   for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
   {
      if (solveForObjectNames[i] == "")
         throw EstimatorException("Error: '" + solveForNames[i] +"' has an empty object name.\n");

      if (i >= solveForObjects.size()) 
         throw EstimatorException("Error: '" + solveForNames[i] +"' object which is specified in AddSolverFor was not defined in your script.\n");
      else
      {
         if(solveForObjects[i] == NULL)
            throw EstimatorException("Error: '" + solveForNames[i] +"' object which is specified in AddSolverFor was not defined in your script.\n");
      }

      if (solveForIDNames[i] == "")
         throw EstimatorException("Error: '" + solveForNames[i] +"' has an empty parameter name.\n");

      if (i >= solveForIDs.size()) 
         throw EstimatorException("Error: '" + solveForNames[i] +"' parameter which is specified in AddSolverFor was not defined in your script.\n");
      else
      {
         if(solveForIDs[i] == -1)  // Changed from NULL to clear warning
            throw EstimatorException("Error: '" + solveForNames[i] +"' paramter which is specified in AddSolverFor was not defined in your script.\n");
      }

      UnsignedInt startPos = solveForNames[i].find_last_of('.');
      for (UnsignedInt j = 0; j < stateSolveFors.size(); ++j)
      {
         if (solveForNames[i].find(stateSolveFors[j], startPos) != std::string::npos)
            hasStateSolveFor = true;
      }
   }

   if (!hasStateSolveFor)
      throw EstimatorException("Error: No state solvefor parameters are used in estimation.\n");

   return true;
}


//------------------------------------------------------------------------------
// bool SetProperty(std::string sf, GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets a SolveFor parameter associated with a specific object.
 *
 * @param sf The solveFor parameter
 * @param obj The object that supplies the parameter data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(std::string sf, GmatBase *obj)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting SolveFor parameter \"%s\" on object %s <%p>\n",
            sf.c_str(), obj->GetName().c_str(), obj);
   #endif

   bool retval = false;

   if (find(solveForNames.begin(), solveForNames.end(), sf) ==
         solveForNames.end())
   {
      // Save the property elements and fill out the vectors
      solveForNames.push_back(sf);
      solveForObjectNames.push_back(obj->GetName());
      solveForObjects.push_back(obj);
      solveForIDNames.push_back(sf);
      solveForIDs.push_back(-1);

      retval = true;
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("EstmationStateManager::SetProperty():  Current SolveFor parameters:\n");

      for (UnsignedInt i = 0; i < solveForNames.size(); ++i)
           MessageInterface::ShowMessage("  solve for name = '%s';  object name = '%s';   parameter = '%s'\n", solveForNames[i].c_str(), solveForObjectNames[i].c_str(), solveForIDNames[i].c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool SetProperty(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Sets a SolveFor parameter associated with a specific object. Assume that the 
 * object obj has SolveFors paramter containing object's solve-for names. Example: 
 * Sat.SolveFors = Cartesian. The solve-for parameter for this case is Sat.Cartisian
 *
 * @param obj The object that supplies solve-for parameter
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetProperty(GmatBase *obj)
{
#ifdef DEBUG_STATE_CONSTRUCTION
   MessageInterface::ShowMessage("EstimationStateManager::SetProperty(obj = <%s>)\n", obj->GetFullName().c_str());
#endif
   StringArray solforNames = GetSolveForList(obj);
   
   if (solforNames.empty())
      return false;

   for (UnsignedInt i = 0; i < solforNames.size(); ++i)
   {
#ifdef DEBUG_STATE_CONSTRUCTION
	   MessageInterface::ShowMessage("SetProperty for:  solveforNames[%d] = <%s>\n", i, solforNames[i].c_str());
#endif
		   SetProperty(solforNames[i]);

   }
     

   return true;
}


//------------------------------------------------------------------------------
// StringArray GetSolveForList(GmatBase* obj)
//------------------------------------------------------------------------------
/**
* This function is used to create a list of solve-for properties for a given object
*
* @param obj        a pointer to the object 
* return            a list of solve-for properties associated with the input object 
*/
//------------------------------------------------------------------------------
StringArray EstimationStateManager::GetSolveForList(GmatBase* obj)
{
   StringArray participantNames = this->measMan->GetParticipantList();

   StringArray solveforList;
   
   if (obj->IsOfType(Gmat::SPACECRAFT))
   {
      // 1. Load solve-for list for spacecraft
      // 1.1. Load solve-for list from Spacecraft object
      solveforList = obj->GetStringArrayParameter("SolveFors");

      // 1.2. Load solve-for list from Plate objects
      ObjectArray plateList = ((Spacecraft*)obj)->GetRefObjectArray("Plate");
      std::vector<StringArray> constraints = ((Spacecraft*)obj)->GetEqualConstrains();
      for (Integer i = 0; i < plateList.size(); ++i)
      {
         StringArray plateSolvefors = ((Plate*)(plateList[i]))->GetStringArrayParameter("SolveFors");
         for (Integer j = 0; j < plateSolvefors.size(); ++j)
         {
            //MessageInterface::ShowMessage("plate<%s>: %s\n", plateList[i]->GetFullName().c_str(), (plateList[i]->GetName() + "." + plateSolvefors[j]).c_str());
            std::string sfFullName = plateList[i]->GetName() + "." + plateSolvefors[j];
            bool addToList = true;
            for (Integer k = 0; k < constraints.size(); ++k)
            {
               for (Integer kk = 1; kk < constraints[k].size(); ++kk)
               {
                  if (constraints[k][kk] == sfFullName)
                  {
                     addToList = false;
                     break;
                  }
               }

               if (addToList == false)
                  break;
            }

            if (addToList)
               solveforList.push_back(sfFullName);
         }
      }

      // 1.3. Load solve-for list from Hardware objects. For now, only Transmitter
      ObjectArray hardwareList = ((Spacecraft*)obj)->GetRefObjectArray("Hardware");
      for (Integer i = 0; i < hardwareList.size(); ++i)
      {
         StringArray hardwareSolvefors;
         try
         {
            hardwareSolvefors = hardwareList[i]->GetStringArrayParameter("SolveFors");
         }
         catch (...)
         {
            hardwareSolvefors.clear();
         }

         for (Integer j = 0; j < hardwareSolvefors.size(); ++j)
         {
            std::string sfFullName = hardwareList[i]->GetName() + "." + hardwareSolvefors[j];
            solveforList.push_back(sfFullName);
         }
         if (hardwareList[i]->GetTypeName() == "Receiver") // Get Receiver Error Models for SC-SC
         {
            ObjectArray errmodelMap = ((Receiver*)hardwareList[i])->GetRefObjectArray(Gmat::ERROR_MODEL);
            for (int k = 0; k < errmodelMap.size(); ++k)
            {

                  // Given fullName = 'CAN.SimSat_DSNRange_ErrorModel'. It needs to extract 'SimSat' from this string
                  std::string fullName = ((Receiver*)hardwareList[i])->GetFullName();
                  std::string name = ((Receiver*)hardwareList[i])->GetName();
                  std::string s = fullName.substr(0, fullName.size() - name.size() - 1);
                  std::string::size_type pos = s.find_first_of('.');
                  s = s.substr(pos + 1);

                  // If the name of spacecraft ('SimSat') is not in the participants list. It needs to skip setting solve-for
                  bool found = false;
                  for (Integer j = 0; j < participantNames.size(); ++j)
                  {
                     if (participantNames[j] == s)
                     {
                        found = true;
                        break;
                     }
                  }
                  if (!found)
                     continue;

                  // 2.2. Get solve-for list from error models
                  StringArray sfList = ((ErrorModel *)errmodelMap[k])->GetStringArrayParameter("SolveFors");
                  for (UnsignedInt j = 0; j < sfList.size(); ++j)
                     solveforList.push_back(((ErrorModel *)errmodelMap[k])->GetFullName() + "." + sfList[j]);
               
            }

         }
      }
   }
   else if (obj->IsOfType(Gmat::GROUND_STATION))
   {
      // 2. Load solve-for list from ground station
      // 2.1. Get a list of error models
      std::map<std::string,ObjectArray> errorModelMap = ((GroundstationInterface*)obj)->GetErrorModelMap();
      for (std::map<std::string,ObjectArray>::iterator mapIndex = errorModelMap.begin(); mapIndex != errorModelMap.end(); ++mapIndex)
      {
         ObjectArray errorModels = mapIndex->second;
         for (UnsignedInt i = 0; i < errorModels.size(); ++i)
         {
            // Given fullName = 'CAN.SimSat_DSNRange_ErrorModel'. It needs to extract 'SimSat' from this string
            std::string fullName = errorModels[i]->GetFullName();
            std::string name = errorModels[i]->GetName();
            std::string s = fullName.substr(0, fullName.size() - name.size() - 1);
            std::string::size_type pos = s.find_first_of('.');
            s = s.substr(pos + 1);

            // If the name of spacecraft ('SimSat') is not in the participants list. It needs to skip setting solve-for
            bool found = false;
            for (Integer j = 0; j < participantNames.size(); ++j)
            {
               if (participantNames[j] == s)
               {
                  found = true;
                  break;
               }
            }
            if (!found)
               continue;

            // 2.2. Get solve-for list from error models
            StringArray sfList = errorModels[i]->GetStringArrayParameter("SolveFors");
            for(UnsignedInt j = 0; j < sfList.size(); ++j)
               solveforList.push_back(errorModels[i]->GetFullName() + "." + sfList[j]);
         }
      }

      // 2.2. Load solve-for list from Hardware objects.
      ObjectArray hardwareList = ((GroundstationInterface*)obj)->GetRefObjectArray("Hardware");
      for (Integer i = 0; i < hardwareList.size(); ++i)
      {
         StringArray hardwareSolvefors;
         try
         {
            hardwareSolvefors = hardwareList[i]->GetStringArrayParameter("SolveFors");
         }
         catch (...)
         {
            hardwareSolvefors.clear();
         }

         for (Integer j = 0; j < hardwareSolvefors.size(); ++j)
         {
            std::string sfFullName = hardwareList[i]->GetName() + "." + hardwareSolvefors[j];
            solveforList.push_back(sfFullName);
         }
      }
   }

//   else if (obj->IsOfType("ThrustSegment"))                         // Thrust Scale Factor Solve For
   else
   {
	   StringArray sfList = obj->GetStringArrayParameter("SolveFors");
	   for (UnsignedInt j = 0; j < sfList.size(); ++j)
		   solveforList.push_back(obj->GetFullName() + "." + sfList[j]);
   }

   // 3. Add prefix
   for (UnsignedInt i = 0; i < solveforList.size(); ++i)
   {
      if (!obj->IsOfType(Gmat::GROUND_STATION))
         solveforList[i] = obj->GetName() + "." + solveforList[i]; 
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("EstimationStateManager::GetSolveForList - Solve for parameters for <%s>:\n", obj->GetName().c_str());
      for (UnsignedInt i = 0; i < solveforList.size(); ++i)
         MessageInterface::ShowMessage("%d: %s\n", i, solveforList[i].c_str());
   #endif

   return solveforList;
}


//------------------------------------------------------------------------------
// bool SetConsider(std::string prop)
//------------------------------------------------------------------------------
/**
 * Method used to identify Consider parameters to the estimation state manager
 *
 * @param prop String identifying the Consider parameter
 *
 * @return true if the parameter is accepted, false if not
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string prop)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}


//------------------------------------------------------------------------------
// bool SetConsider(std::string prop, Integer loc)
//------------------------------------------------------------------------------
/**
 * Method used to identify Consider parameters to the estimation state manager,
 * potentially replacing a parameter that is already in place.
 *
 * @param prop String identifying the Consider parameter
 * @param loc The location in the Consider array desired for this property
 *
 * @return true if the parameter is accepted, false if not
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string prop, Integer loc)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}

//------------------------------------------------------------------------------
// bool SetConsider(std::string con, GmatBase* obj)
//------------------------------------------------------------------------------
/**
 * Sets a Consider parameter associated with a specific object.
 *
 * This version of the call is not currently used.
 *
 * @param con The Consider parameter
 * @param obj The object that supplies the parameter data
 *
 * @return true on success, false on failure
 *
 * @note Consider parameters are not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::SetConsider(std::string con, GmatBase *obj)
{
   bool retval = false;

   MessageInterface::ShowMessage("Consider parameters are not yet "
         "implemented.\n");

   return retval;
}


//------------------------------------------------------------------------------
// void SetParticipantList(StringArray &p)
//------------------------------------------------------------------------------
/**
 * Passes the names of the participants into the ESM
 *
 * @param p The list of participants
 */
//------------------------------------------------------------------------------
void EstimationStateManager::SetParticipantList(StringArray &p)
{
   participantNames = p;

   // Thrust Scale Factor Solve For
   // Somehow get the names of the thrust segments added to participantNames
   
  
}


//------------------------------------------------------------------------------
// bool BuildState()
//------------------------------------------------------------------------------
/**
 * Assembles the estimation state vector
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::BuildState()
{
   bool retval = false;

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("Entered BuildState()\n");
      MessageInterface::ShowMessage("   ESM now has %d objects, %d "
            "solveForObjects, and %d solveFors\n", objects.size(), solveForObjects.size(),
            solveForIDNames.size());

      MessageInterface::ShowMessage("   There are %d elements:\n", elements.size());
      for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
            i != elements.end(); ++i)
      {
         MessageInterface::ShowMessage("      %s, with %d components\n", i->first->GetName().c_str(), i->second->size());
         for (UnsignedInt j = 0; j < i->second->size(); ++j)
            MessageInterface::ShowMessage("         %s\n", i->second->at(j).c_str());
      }
   #endif

   // Determine the size of the propagation state vector
   stateSize = SortVector();

   if (stateSize <= 0)
   {
      std::stringstream sizeVal;
      sizeVal << stateSize;
      throw EstimatorException("No solve-for parameter is defined for estimator;"
            " estimation is not possible.\n");
   }

   std::map<std::string,Integer> associateMap;
   // Build the associate map
   std::string name;
   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectFullName;
      if (associateMap.find(name) == associateMap.end())
         associateMap[name] = index;
   }

   state.SetSize(stateSize);
   stateOffset.SetSize(stateSize);

   // Build the data structures for the STM and covariance matrix
   stm.SetSize(stateSize, stateSize);
   covariance.SetDimension(stateSize);
   stmColCount = stateSize;
   covColCount = stateSize;

   for (Integer index = 0; index < stateSize; ++index)
   {
      name = stateMap[index]->objectFullName;
      std::stringstream sel("");
      sel << stateMap[index]->subelement;
      state.SetElementProperties(index, stateMap[index]->elementID,
            name + "." + stateMap[index]->elementName + "." + sel.str(),
            associateMap[name]);
   }

   // Initialize the STM matrix
   for (Integer i = 0; i < stateSize; ++i)
      for (Integer j = 0; j < stateSize; ++j)
         if (i == j)
            stm(i,j) = 1.0;
         else
            stm(i,j) = 0.0;

   // Now build the covariance, using the elements the user has set and defaults
   // for the rest
   for (Integer i = 0; i < stateSize;)
   {
      Integer size    = stateMap[i]->length;
      Integer id      = stateMap[i]->parameterID;
      Covariance *cov = stateMap[i]->object->GetCovariance();

      #ifdef DEBUG_COVARIANCE
         MessageInterface::ShowMessage("Found length = %d for id = %d    object = <%p,%s>\n", size, id, stateMap[i]->object, stateMap[i]->object->GetName().c_str());
      #endif

      Rmatrix *subcov = cov->GetCovariance(id);
      if (subcov)
      {
         #ifdef DEBUG_COVARIANCE
            MessageInterface::ShowMessage("Found %d by %d subcovariance: [\n",
                  subcov->GetNumRows(), subcov->GetNumColumns());
         #endif
         for (Integer j = 0; j < size; ++j)
            for (Integer k = 0; k < size; ++k)
               covariance(i + j, i + k) = (*subcov)(j, k);
#ifdef DEBUG_COVARIANCE
         for (Integer j = 0; j < size; ++j)
         {
            for (Integer k = 0; k < size; ++k)
               MessageInterface::ShowMessage("%le   ", (*subcov)(j, k));
            MessageInterface::ShowMessage("\n");
         }
         MessageInterface::ShowMessage("]\n");
#endif
      }
      else
      {
         #ifdef DEBUG_COVARIANCE
            MessageInterface::ShowMessage("Found no subcovariance\n");
         #endif

         if (stateMap[i]->elementName == "CartesianState")
         {
            for (Integer j = 0; j < size; ++j)
            {
               for (Integer k = 0; k < size; ++k)
               {
                  if (j < 3)
                     covariance(i+j,i+k) = (j == k ? 1.0e12 : 0.0);
                  else
                     covariance(i+j,i+k) = (j == k ? 1.0e6 : 0.0);
               }
            }
         }
         else // Other defaults are set to the identity
         {
            for (Integer j = 0; j < size; ++j)
            {
               for (Integer k = 0; k < size; ++k)
               {
                  covariance(i+j,i+k) = (j == k ? 1.0e0 : 0.0);
               }
            }
         }
      }
      i += size;
   }

   #ifdef DEBUG_COVARIANCE
      MessageInterface::ShowMessage("EstimationStateManager covariance info:\n");
      covariance.ShowContent();
   #endif

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage(
            "Estimation state vector has %d elements:\n", stateSize);
      StringArray props = state.GetElementDescriptions();
      for (Integer index = 0; index < stateSize; ++index)
         MessageInterface::ShowMessage("   %d:  %s\n", index,
               props[index].c_str());
   #endif

   #ifdef DUMP_STATE
      MapObjectsToVector();
      MessageInterface::ShowMessage("state.epoch = %.12lf   state.epochGT = %s\n", state.GetEpoch(), state.GetEpochGT().ToString().c_str());
      for (Integer i = 0; i < stateSize; ++i)
         MessageInterface::ShowMessage("State[%02d] = %.12lf, %s %d\n", i, state[i],
               "RefState start =", state.GetAssociateIndex(i));

      MessageInterface::ShowMessage("State Transition Matrix = %s\n",
            stm.ToString().c_str());
      MessageInterface::ShowMessage("Covariance Matrix = %s\n",
            covariance.GetCovariance()->ToString().c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool MapVectorToObjects()
//------------------------------------------------------------------------------
/**
 * Passes estimation state vector data to the associated objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapVectorToObjects()
{
   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage("Mapping vector to objects\n"
            "   Epoch = %s\n", state.GetEpochGT().ToString().c_str());
   #endif
   for (Integer index = 0; index < stateSize; ++index)
   {
      #ifdef DEBUG_OBJECT_UPDATES
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectFullName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf . Its dot = %.15le for object <%s, %s, %p>   parameterID = %d  parameterType = %d\n", index, lbl.c_str(),
            state[index], state.GetStateDot()[index], stateMap[index]->object->GetName().c_str(), stateMap[index]->object->GetFullName().c_str(), stateMap[index]->object, 
            stateMap[index]->parameterID, stateMap[index]->parameterType);
      #endif
   
      Rvector3 acceleration;

      bool prevChoice = true;
      if (stateMap[index]->object->IsOfType("Plate"))
      {
         prevChoice = ((Plate*)stateMap[index]->object)->SetErrorSelection(false);
      }

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
         {
            (stateMap[index]->object)->SetRealParameter(
               stateMap[index]->parameterID, state[index]);
            
            if (stateMap[index]->object->IsOfType(Gmat::SPACECRAFT))
            {
               Integer accelIndex = stateMap[index]->parameterID - (cartesianStateID + 3);

               #ifdef DEBUG_OBJECT_UPDATES
                  MessageInterface::ShowMessage("accelIndex = %d - %d = %d\n",
                     stateMap[index]->parameterID, (cartesianStateID + 3), accelIndex);
               #endif

               if ((stateMap[index]->parameterID - (cartesianStateID + 3)) == 0)
               {
                  acceleration[0] = (state.GetStateDot())[index];
                  acceleration[1] = (state.GetStateDot())[index+1];
                  acceleration[2] = (state.GetStateDot())[index+2];

                  #ifdef DEBUG_OBJECT_UPDATES
                     MessageInterface::ShowMessage("    acceleration = [%.15le   %.15le   %.15le]\n",
                        acceleration[0], acceleration[1], acceleration[2]);
                  #endif

                  ((Spacecraft*)(stateMap[index]->object))->SetAcceleration(acceleration);

                  // clear its value for the next acceleration value setting
                  acceleration.Set(0.0, 0.0, 0.0);
               }
            }
            break;
         }
         case Gmat::RVECTOR_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index],
                     stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateMap[index]->parameterID, state[index],
                     stateMap[index]->rowIndex, stateMap[index]->colIndex);
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." +
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }

      if (stateMap[index]->object->IsOfType("Plate"))
      {
         ((Plate*)stateMap[index]->object)->SetErrorSelection(prevChoice);
      }
   }

   // Update value for constraint variables of spacecraft
   ObjectArray spacecrafts;
   for (Integer i = 0; i < stateSize; ++i)
   {
      if (stateMap[i]->object->IsOfType(Gmat::SPACECRAFT))
      {
         bool found = false;
         for (Integer j = 0; j < spacecrafts.size(); ++j)
         {
            if (stateMap[i]->object == spacecrafts[j])
            {
               found = true;
               break;
            }
         }
         if (found == false)
         {
            ((Spacecraft*)(stateMap[i]->object))->UpdateValueForConstraints();
            spacecrafts.push_back(stateMap[i]->object);
         }
      }
   }

   #ifdef DEBUG_CLONING
   MessageInterface::ShowMessage("After map vector to object:\n");
   for (Integer index = 0; index < objects.size(); ++index)
   {
      MessageInterface::ShowMessage("Object <%p,%s> data:\n%s", objects[index], objects[index]->GetName().c_str(),
         objects[index]->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
   }
   #endif

   GmatEpoch theEpoch = state.GetEpoch();
   GmatTime theEpochGT = state.GetEpochGT();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      if (epochIDs[i] >= 0)
      {
         objects[i]->SetRealParameter(epochIDs[i], theEpoch);
         if (state.HasPrecisionTime())
            objects[i]->SetGmatTimeParameter(epochIDs[i], theEpochGT);
         else
            objects[i]->SetGmatTimeParameter(epochIDs[i], GmatTime(theEpoch));
      }
   }

   return true;
}


//------------------------------------------------------------------------------
// bool MapFullVectorToObjects()
//------------------------------------------------------------------------------
/**
 * Passes estimation state vector data, including reference state offsets,
 * to the associated objects
 *
 * @return true on success
 */
 //------------------------------------------------------------------------------
bool EstimationStateManager::MapFullVectorToObjects()
{
#ifdef DEBUG_OBJECT_UPDATES
   MessageInterface::ShowMessage("Mapping vector with offsets to objects\n"
      "   Epoch = %s\n", state.GetEpochGT().ToString().c_str());
#endif

   Rvector3 acceleration;

   for (Integer index = 0; index < stateSize; ++index)
   {
      Real fullState = state[index];
      if (hasStateOffset)
         fullState += stateOffset[index];

#ifdef DEBUG_OBJECT_UPDATES
      std::stringstream msg("");
      msg << stateMap[index]->subelement;
      std::string lbl = stateMap[index]->objectFullName + "." +
         stateMap[index]->elementName + "." + msg.str() + " = ";
      MessageInterface::ShowMessage("   %d: %s%.12lf (%.12lf + %.12lf)  for object <%s, %p>   parameterID = %d\n", index, lbl.c_str(),
         fullState, state[index], stateOffset[index], stateMap[index]->object->GetFullName().c_str(), stateMap[index]->object, stateMap[index]->parameterID);
#endif

      switch (stateMap[index]->parameterType)
      {
      case Gmat::REAL_TYPE:
      {
         (stateMap[index]->object)->SetRealParameter(
            stateMap[index]->parameterID, fullState);

         if (stateMap[index]->object->IsOfType(Gmat::SPACECRAFT))
         {
            Integer accelIndex = stateMap[index]->parameterID - (cartesianStateID + 3);
            if ((0 <= accelIndex) && (accelIndex < 3))
            {
               acceleration[accelIndex] = (state.GetStateDot())[index];
               if (accelIndex == 2)
               {
                  ((Spacecraft*)(stateMap[index]->object))->SetAcceleration(acceleration);

                  // clear its value for the next acceleration value setting
                  acceleration.Set(0.0, 0.0, 0.0);
               }
            }
         }
         break;
      }
      case Gmat::RVECTOR_TYPE:
         stateMap[index]->object->SetRealParameter(
            stateMap[index]->parameterID, fullState,
            stateMap[index]->rowIndex);
         break;

      case Gmat::RMATRIX_TYPE:
         stateMap[index]->object->SetRealParameter(
            stateMap[index]->parameterID, fullState,
            stateMap[index]->rowIndex, stateMap[index]->colIndex);
         break;

      default:
         std::stringstream sel("");
         sel << stateMap[index]->subelement;
         std::string label = stateMap[index]->objectName + "." +
            stateMap[index]->elementName + "." + sel.str();
         MessageInterface::ShowMessage(
            "%s not set; Element type not handled\n", label.c_str());
      }

   }

#ifdef DEBUG_CLONING
   MessageInterface::ShowMessage("After map vector to object:\n");
   for (Integer index = 0; index < objects.size(); ++index)
   {
      MessageInterface::ShowMessage("Object <%p,%s> data:\n%s", objects[index], objects[index]->GetName().c_str(),
         objects[index]->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
   }
#endif

   GmatEpoch theEpoch = state.GetEpoch();
   GmatTime theEpochGT = state.GetEpochGT();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      if (epochIDs[i] >= 0)
      {
         objects[i]->SetRealParameter(epochIDs[i], theEpoch);
         if (state.HasPrecisionTime())
            objects[i]->SetGmatTimeParameter(epochIDs[i], theEpochGT);
         else
            objects[i]->SetGmatTimeParameter(epochIDs[i], GmatTime(theEpoch));
      }
   }

   return true;
}



//------------------------------------------------------------------------------
// bool EstimationStateManager::MapObjectsToVector()
//------------------------------------------------------------------------------
/**
 * Fills object data into the estimation state vector
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToVector()
{
   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage(
            "EstimationStateManager::MapObjectsToVector() called with state "
            "size %d\n", stateSize);
   #endif
   for (Integer index = 0; index < stateSize; ++index)
   {
      // Check object pointer
      if (stateMap[index]->object == NULL)
      {
         MessageInterface::ShowMessage("Object pointer for %s is NULL; "
               "skipping it for now.\n", stateMap[index]->objectName.c_str());
         continue;
      }
      #ifdef DEBUG_OBJECT_MAPPING
         else
         {
            MessageInterface::ShowMessage("Object pointer for %s is %p\n",
                  stateMap[index]->objectName.c_str(), stateMap[index]->object);
            MessageInterface::ShowMessage("  parameter type = %d   parameterID = %d\n",
               stateMap[index]->parameterType, stateMap[index]->parameterID);
         }
      #endif

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
         {
            state[index] = stateMap[index]->object->GetRealParameter(
               stateMap[index]->parameterID);
            
            // Update stateDot
            //state.GetStateDot()[index] = 0.0;

            if (stateMap[index]->object->IsOfType(Gmat::SPACECRAFT))
            {
               Integer idx = stateMap[index]->parameterID - cartesianStateID;
               if (idx == 5)
               {
                  for (Integer k = 0; k < 3; ++k)
                     (state.GetStateDot())[index - 5 + k] = state[index - 2 + k];

                  Rvector3 accel = ((Spacecraft*)(stateMap[index]->object))->GetAcceleration();
                  for (Integer k = 3; k < 6; ++k)
                     (state.GetStateDot())[index - 5 + k] = accel[k - 3];

                  #ifdef DEBUG_OBJECT_MAPPING
                     for (Integer k = 0; k < 6; ++k)
                     {
                        MessageInterface::ShowMessage("    stateDot[%d] = %.15le\n",
                          index - 5 + k, (state.GetStateDot())[index - 5 + k]);
                     }
                  #endif
               }
            }

            break;
         }
         case Gmat::RVECTOR_TYPE:
            state[index] =
               stateMap[index]->object->GetRealParameter(
                     stateMap[index]->parameterID, stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            state[index] =
               stateMap[index]->object->GetRealParameter(
                     stateMap[index]->parameterID, stateMap[index]->rowIndex,
                     stateMap[index]->colIndex);
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." +
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }
   }

   #ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage("   State mapping complete; moving to epochs\n");
	#endif
	
   // Manage epoch
   GmatEpoch theEpoch = 0.0;
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      // Objects without epoch have a -1 set as their epochID
      if (epochIDs[i] >= 0)
      {
		  if (theEpoch == 0.0)
		  {
			  theEpoch = objects[i]->GetRealParameter(epochIDs[i]);
			  break;
		  }
      }
   }
   
   
   if (state.HasPrecisionTime())
   {
      GmatTime theEpochGT = 0.0;
      for (UnsignedInt i = 0; i < objects.size(); ++i)
      {
         // Objects without epoch have a -1 set as their epochID
         if (epochIDs[i] >= 0)
         {
			 if (theEpochGT == 0.0)
			 {
				 theEpochGT = objects[i]->GetGmatTimeParameter(epochIDs[i]);
				 break;
			 }
         }
      } 
	  state.SetEpoch(theEpochGT.GetMjd());
      state.SetEpochGT(theEpochGT);
   }	
   else 
	   state.SetEpoch(theEpoch);

   #ifdef DEBUG_OBJECT_UPDATES
      MessageInterface::ShowMessage(
            "After mapping objects to vector, contents are\n"
            "   Epoch = %s\n", state.GetEpochGT().ToString().c_str());
      for (Integer index = 0; index < stateSize; ++index)
      {
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectFullName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf.  Its dot is %.15le\n", index, lbl.c_str(),
               state[index], state.GetStateDot()[index]);
      }
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool MapSTMToObjects()
//------------------------------------------------------------------------------
/**
 * Passes state transition matrix data to the objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapSTMToObjects()
{
   bool retval = true;

   #ifdef DEBUG_STM_MAPPING
      MessageInterface::ShowMessage("Setting object STM's to\n");
      for (Integer i = 0; i < stateSize; ++i)
      {
         for (Integer j = 0; j < stateSize; ++j)
            MessageInterface::ShowMessage("   %.12le", stm(i,j));
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   // Fill in the STM based on the objects that comprise the state vector
   GmatBase* obj;
   Integer elementId; //, elementLength;
   for (UnsignedInt h = 0; h < stateMap.size(); ++h)
   {
      obj = stateMap[h]->object;
      if (stateMap[h]->subelement == 1)
      {
         elementId = stateMap[h]->parameterID;
//         elementLength = stateMap[h]->length;

         bool hasDstm = obj->HasDynamicParameterSTM(elementId);

         if (hasDstm)
         {
            Rmatrix* dstm = obj->GetParameterSTM(elementId);
            Integer stmSize = dstm->GetNumRows();
            IntegerArray stmMap = GetSTMMapping(obj, elementId);

            for (Integer row = 0; row < stmSize; row++)
            {
               if (stmMap[row] == -1)
               {
                  Integer id = obj->GetStmRowId(row);
                  std::string paramName = obj->GetParameterText(id);
                  std::string solveforName = obj->GetParameterNameForEstimationParameter(paramName);

                  throw EstimatorException("Error mapping the element \"" + solveforName +
                     "\" in the object \"" + obj->GetName() + "\" from the ESM STM to the object STM\n");
               }
            }

            // Fill in the object's stm from the master stm
            for (Integer i = 0; i < stmSize; ++i)
               for (Integer j = 0; j < stmSize; ++j)
                  (*dstm)(i, j) = stm(stmMap[i], stmMap[j]);

#ifdef DEBUG_STM_MAPPING
            MessageInterface::ShowMessage("Spacecraft %s's STM = \n", obj->GetName().c_str());
            for (Integer i = 0; i < stmSize; ++i)
            {
               MessageInterface::ShowMessage("[");
               for (Integer j = 0; j < stmSize; ++j)
               {
                  MessageInterface::ShowMessage("%.12le   ", (*dstm)(i, j));
               }
               MessageInterface::ShowMessage("]\n");
            }
            for (Integer row = 0; row < stmMap.size(); ++row)
            {
               MessageInterface::ShowMessage("object STM row = %d   ESM STM row = %d\n", row, stmMap[row]);
            }
#endif

         }

      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool MapObjectsToSTM()
//------------------------------------------------------------------------------
/**
 * Fills the estimation state transition matrix data from the objects
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToSTM()
{
#ifdef DEBUG_STM_MAPPING
   MessageInterface::ShowMessage("Start EstimationStateManager::MapObjectsToSTM()\n");
#endif
   bool retval = true;

   // Fill in the STM based on the objects that comprise the state vector
   GmatBase* obj;
   Integer elementId; //, elementLength;
   for (UnsignedInt h = 0; h < stateMap.size(); ++h)
   {
      obj = stateMap[h]->object;
      if (stateMap[h]->subelement == 1)
      {
         elementId = stateMap[h]->parameterID;

         bool hasDstm = obj->HasDynamicParameterSTM(elementId);

#ifdef DEBUG_STM_MAPPING
         Integer elementLength = stateMap[h]->length;
         MessageInterface::ShowMessage("Prepping for STM; element %s for "
            "object %s has ID %d and length %d, and %s a dynamic STM "
            "contribution\n", stateMap[h]->elementName.c_str(),
            obj->GetName().c_str(), elementId, elementLength,
            (hasDstm ? "has" : "does not have"));
#endif

         if (hasDstm)
         {
            Rmatrix* dstm = obj->GetParameterSTM(elementId);
            Integer stmSize = dstm->GetNumRows();
            IntegerArray stmMap = GetSTMMapping(obj, elementId);

            for (Integer row = 0; row < stmSize; row++)
            {
               if (stmMap[row] == -1)
               {
                  Integer id = obj->GetStmRowId(row);
                  std::string paramName = obj->GetParameterText(id);
                  std::string solveforName = obj->GetParameterNameForEstimationParameter(paramName);

                  throw EstimatorException("Error mapping the element \"" + solveforName +
                     "\" in the object \"" + obj->GetName() + "\" from the object STM to the ESM STM\n");
               }
            }
            
            // Fill in the master stm with the current data
            for (Integer i = 0; i < stmSize; ++i)
               for (Integer j = 0; j < stmSize; ++j)
                  stm(stmMap[i], stmMap[j]) = (*dstm)(i, j);

#ifdef DEBUG_STM_MAPPING
            MessageInterface::ShowMessage("Spacecraft %s's STM = \n", obj->GetName().c_str());
            for (Integer i = 0; i < stmSize; ++i)
            {
               MessageInterface::ShowMessage("[");
               for (Integer j = 0; j < stmSize; ++j)
               {
                  MessageInterface::ShowMessage("%.12le   ",(*dstm)(i, j));
               }
               MessageInterface::ShowMessage("]\n");
            }
            for (Integer row = 0; row < stmMap.size(); ++row)
            {
               MessageInterface::ShowMessage("object STM row = %d   ESM STM row = %d\n", row, stmMap[row]);
            }
#endif

         }

      }
   }

#ifdef DEBUG_STM_MAPPING
   MessageInterface::ShowMessage("Loaded object STM's; esm STM now contains\n");
   for (Integer i = 0; i < stateSize; ++i)
   {
      for (Integer j = 0; j < stateSize; ++j)
         MessageInterface::ShowMessage("   %.12le", stm(i, j));
      MessageInterface::ShowMessage("\n");
   }
   MessageInterface::ShowMessage("\n");
#endif
#ifdef DEBUG_STM_MAPPING
   MessageInterface::ShowMessage("End EstimationStateManager::MapObjectsToSTM()\n");
#endif
   return retval;
}


//------------------------------------------------------------------------------
// IntegerArray GetSTMMapping()
//------------------------------------------------------------------------------
/**
 * Gets the element mapping from the STM of the provided object and parameter to
 * the ESM elements
 *
 * @param obj The object to get the STM from
 * @param parameterId The parameter to get the STM with regards to
 *
 * @return The mapping of elements from the STM to the ESM elements. If no mapping
 * can be found for an element, that a value of -1 is returned
 */
 //------------------------------------------------------------------------------
IntegerArray EstimationStateManager::GetSTMMapping(GmatBase* obj, Integer parameterId) const
{
   Rmatrix* dstm = obj->GetParameterSTM(parameterId);
   Integer stmSize = dstm->GetNumRows();
   IntegerArray stmMap(stmSize, -1);

   for (Integer objRow = 0; objRow < stmSize; objRow++)
   {
      bool found = false;
      Integer id = obj->GetStmRowId(objRow);
      std::string paramName = obj->GetParameterText(id);
      std::string solveforName = obj->GetParameterNameForEstimationParameter(paramName);
      id = obj->GetParameterID(solveforName);

      for (Integer idx = 0; idx < stateMap.size(); idx++)
      {
         if (obj == stateMap[idx]->object && id == stateMap[idx]->parameterID)
         {
            found = true;
            stmMap[objRow] = idx;
            break;
         }
      }
      
      if (!found)
      {
         // Special check for THF solve-fors
         for (Integer idx = 0; idx < stateMap.size(); idx++)
         {
            std::string objSolveforName = stateMap[idx]->object->GetParameterNameForEstimationParameter(paramName);
            if (objSolveforName == stateMap[idx]->elementName)
            {
               found = true;
               stmMap[objRow] = idx;

               // Support for non-scalar params
               if (stateMap[idx]->length > 1)
               {
                  for (Integer ix = 1; ix < stateMap[idx]->length; ix++)
                  {
                     objRow++;
                     stmMap[objRow] = idx + ix;
                  }
               }

               break;
            }
         }
      }

      if (!found)
      {
         // Special check for owned object solve-fors
         StringArray parts = GmatStringUtil::SeparateBy(paramName, ".");
         if (parts.size() == 2)
         {
            std::string ownedObjName = parts[0];
            std::string objParamName = parts[1];

            for (Integer idx = 0; idx < stateMap.size(); idx++)
            {
               std::string objSolveforName = stateMap[idx]->object->GetParameterNameForEstimationParameter(objParamName);
               if (ownedObjName == stateMap[idx]->objectName && objSolveforName == stateMap[idx]->elementName)
               {
                  found = true;
                  stmMap[objRow] = idx;
                  // Support for non-scalar params
                  if (stateMap[idx]->length > 1)
                  {
                     for (Integer ix = 1; ix < stateMap[idx]->length; ix++)
                     {
                        objRow++;
                        stmMap[objRow] = idx + ix;
                     }
                  }

                  break;
               }
            }
         }
      }

      if (!found)
      {
         std::string sfFullName = obj->GetName() + '.' + solveforName;
         for (Integer idx = 0; idx < stateMap.size(); idx++)
         {
            std::string elementFullName = stateMap[idx]->object->GetFullName() + stateMap[idx]->elementName;
            if (sfFullName == elementFullName)
            {
               found = true;
               stmMap[objRow] = idx;
               break;
            }
         }
      }

   }

   return stmMap;
}

//------------------------------------------------------------------------------
// bool MapCovariancesToObjects()
//------------------------------------------------------------------------------
/**
 * Passes covariance matrix data to the objects
 *
 * @return true on success
 *
 * @note Covariance mapping is not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapCovariancesToObjects()
{
   for (Integer i = 0; i < stateSize;)
   {
      Integer iSize = stateMap[i]->length;
      Integer iId = stateMap[i]->parameterID;
      Covariance *cov = stateMap[i]->object->GetCovariance();
      Integer iStart = cov->GetSubMatrixLocationStart(iId);

      for (Integer j = 0; j < stateSize;)
      {
         Integer jSize = stateMap[j]->length;

         if (stateMap[i]->object == stateMap[j]->object)
         {
            Integer jId = stateMap[j]->parameterID;
            Integer jStart = cov->GetSubMatrixLocationStart(jId);

            for (UnsignedInt ii = 0U; ii < iSize; ii++)
               for (UnsignedInt jj = 0U; jj < jSize; jj++)
                  (*cov)(iStart + ii, jStart + jj) = covariance(i + ii, j + jj);
         }

         j += jSize;
      }

      i += iSize;
   }

   for (Integer i = 0; i < stateSize;)
   {
      Integer iSize = stateMap[i]->length;
      stateMap[i]->object->MapCovarianceToParameters();
      i += iSize;
   }

   bool retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool MapObjectsToCovariances()
//------------------------------------------------------------------------------
/**
 * Fills the covariance matrix data with data from the objects
 *
 * @return true on success
 *
 * @note Covariance mapping is not yet implemented
 */
//------------------------------------------------------------------------------
bool EstimationStateManager::MapObjectsToCovariances()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// GmatState* GetStateOffset()
//------------------------------------------------------------------------------
/**
 * Retrieves the state offset
 *
 * @return A pointer to the state offset
 */
 //------------------------------------------------------------------------------
GmatState* EstimationStateManager::GetStateOffset()
{
   return &stateOffset;
}


//------------------------------------------------------------------------------
// bool HasStateOffset()
//------------------------------------------------------------------------------
/**
 * Retrieves the value of the hasStateOffset flag
 *
 * @return The value of the hasStateOffset flag
 */
 //------------------------------------------------------------------------------
bool EstimationStateManager::HasStateOffset()
{
   return hasStateOffset;
}


//------------------------------------------------------------------------------
// void SetHasStateOffset(bool hasOffset)
//------------------------------------------------------------------------------
/**
 * Sets the value of the hasStateOffset flag
 *
 * @param hasOffset The value to set the hasStateOffset flag
 */
 //------------------------------------------------------------------------------
void EstimationStateManager::SetHasStateOffset(bool hasOffset)
{
   hasStateOffset = hasOffset;
}


//------------------------------------------------------------------------------
// Rmatrix* GetSTM()
//------------------------------------------------------------------------------
/**
 * Retrieves the estimation state transition matrix
 *
 * @return A pointer to the matrix
 */
//------------------------------------------------------------------------------
Rmatrix* EstimationStateManager::GetSTM()
{
   return &stm;
}

//------------------------------------------------------------------------------
// Rmatrix* GetCovariance()
//------------------------------------------------------------------------------
/**
 * Retrieves the estimation covariance matrix
 *
 * @return A pointer to the matrix
 */
//------------------------------------------------------------------------------
Covariance* EstimationStateManager::GetCovariance()
{
   return &covariance;
}


//------------------------------------------------------------------------------
// void SetMeasurementManager(MeasurementManager *mm)
//------------------------------------------------------------------------------
/**
 * Passes a measurement manager pointer to the estimation state manager
 *
 * @param mm The MeasurementManager pointer
 */
//------------------------------------------------------------------------------
void EstimationStateManager::SetMeasurementManager(MeasurementManager *mm)
{
   measMan = mm;
}


//------------------------------------------------------------------------------
// void DecomposeParameter(std::string &param, Integer howMany)
//------------------------------------------------------------------------------
/**
 * This method breaks a string into several pieces, separating the string at
 * period characters
 *
 * @param param The string that is getting decomposed
 * @param howMany The maximum number of components allowed
 *
 * @return A StringArray containing the substrings
 */
//------------------------------------------------------------------------------
void EstimationStateManager::DecomposeParameter(std::string &param,
      Integer howMany)
{
   chunks.clear();
   size_t loc;             // change from std::string::size_type to size_t in order to compatible with C++98 and C++11

   loc = param.find_last_of('.');
   chunks.push_back(param.substr(0,loc));

   // todo: handle the howMany parameter.  For now, always split into 2 chunks
   // at 1st period
   if (loc != std::string::npos)
      chunks.push_back(param.substr(loc+1));

   #ifdef DEBUG_PARSING
      MessageInterface::ShowMessage("Chunks:\n");
      for (UnsignedInt i = 0; i < chunks.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", chunks[i].c_str());
   #endif
}


//------------------------------------------------------------------------------
// Integer SortVector()
//------------------------------------------------------------------------------
/**
 * This method performs preprocessing and state measurement tasks required by
 * the BuildState() method
 *
 * @return The size of the estimation state vector
 */
//------------------------------------------------------------------------------
Integer EstimationStateManager::SortVector()
{
   StringArray *elementList;
   std::vector<Integer> order;
   std::vector<Integer> idList;
   ObjectArray owners;
   StringArray property;
   std::vector<Integer>::iterator oLoc;

   std::vector<Integer> subElementIndex;

   Integer id = -1;
   Integer size, loc = 0, val;
   stateSize = 0;
   Integer counter = 0;

   // First build a list of the property IDs and objects, measuring state size
   // at the same time
   for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
      i != elements.end(); ++i)
   {
      current = i->first;
      elementList = i->second;

#ifdef DEBUG_OBJECT_MAPPING
      MessageInterface::ShowMessage("Working with %s, %d elements:\n",
         current->GetName().c_str(), elementList->size());
      for (UnsignedInt m = 0; m < elementList->size(); ++m)
         MessageInterface::ShowMessage("   %s\n", elementList->at(m).c_str());
#endif

      for (StringArray::iterator j = elementList->begin();
         j != elementList->end(); ++j)
      {
         id = current->GetEstimationParameterID(*j);
         //         if (id == Gmat::UNKNOWN_STATE)
         //            throw EstimatorException("Unknown state element: " + (*j));

#ifdef DEBUG_OBJECT_MAPPING
         MessageInterface::ShowMessage("Element ID for %s is %d;", j->c_str(),
            id);
#endif

         size = current->GetEstimationParameterSize(id);
#ifdef DEBUG_OBJECT_MAPPING
         MessageInterface::ShowMessage("  size is %d\n", size);
#endif

         stateSize += size;
         std::vector<Integer> tempOrder;
         for (Integer k = 0; k < size; ++k)
         {
            idList.push_back(id);
            owners.push_back(current);
            property.push_back(current->GetParameterNameForEstimationParameter(*j));
            subElementIndex.push_back(k);

            // Put this item in the ordering list.
            // It is sorted by key = <id,subElementIndex> (key = id*1000 + subElementIndex)
            Integer key = id * 1000 + k;
            bool found = false;
            oLoc = tempOrder.begin();
            while (oLoc != tempOrder.end())
            {
               //val = idList[*oLoc];
               val = idList[*oLoc]*1000 + subElementIndex[*oLoc];
               //if (id < val)
               if (key <= val)
               {
                  tempOrder.insert(oLoc, loc);
                  found = true;
                  break;
               }
               //else if (id == val)
               //{
               //   tempOrder.insert(oLoc, loc);
               //   found = true;
               //   break;
               //}
               else
               {
                  ++oLoc;
               }
            }
            if (!found)
               tempOrder.push_back(loc);
            ++loc;
         }
         // Determine what object the estimation is attached to, if any

         std::string assignedObject;
         for(Integer w =0;w<participantNames.size();w++)
         {
            if (current->GetFullName().find(participantNames[w]) !=std::string::npos)
            {
               assignedObject = participantNames[w];
            }
         }

         // Collect the order array together for spacecraft objects
         bool finder = false;
         oLoc = order.begin();
         while (oLoc != order.end())
         {
            if (owners[tempOrder[0]]->GetType() < owners[*oLoc]->GetType())
            {
               order.insert(oLoc, tempOrder.begin(), tempOrder.end());
               finder = true;
               break;
            }
            else if (owners[tempOrder[0]]->GetType() == owners[*oLoc]->GetType())
            {
               if (owners[tempOrder[0]]->GetFullName() < owners[*oLoc]->GetFullName())
               {
                  order.insert(oLoc, tempOrder.begin(), tempOrder.end());
                  finder = true;
                  break;
               }
               else if (owners[tempOrder[0]]->GetFullName() == owners[*oLoc]->GetFullName())
               {
                  if (owners[tempOrder[0]]->GetEstimationParameterID(property[tempOrder[0]]) <= owners[*oLoc]->GetEstimationParameterID(property[*oLoc]))
                  {
                     order.insert(oLoc, tempOrder.begin(), tempOrder.end());
                     finder = true;
                     break;
                  }
               }
            }
            ++oLoc;
         }
         if (!finder)
         {
            order.insert(order.end(), tempOrder.begin(), tempOrder.end());
         }
      }
   }

   ListItem *newItem;
   val = 0;
   stateMap.clear();
   for (Integer i = 0; i < stateSize; ++i)
   {
      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("%d <- %d: %d %s.%s gives ", i, order[i],
               idList[order[i]], owners[order[i]]->GetFullName().c_str(), 
               property[order[i]].c_str());
      #endif

      newItem = new ListItem;
      newItem->objectName  = owners[order[i]]->GetName();
      newItem->objectFullName = owners[order[i]]->GetFullName();
      newItem->elementName = property[order[i]];
      newItem->object      = owners[order[i]];
      newItem->elementID   = idList[order[i]];
      newItem->subelement  = ++val;
      newItem->parameterID =
            owners[order[i]]->GetParameterID(property[order[i]]);
      newItem->parameterType =
            owners[order[i]]->GetParameterType(newItem->parameterID);

      if (newItem->parameterType == Gmat::REAL_TYPE)
         newItem->parameterID += val - 1;

      #ifdef DEBUG_STATE_CONSTRUCTION
         MessageInterface::ShowMessage("[%s, %s, %s, %s, %d, %d, %d, %d]\n",
               newItem->objectName.c_str(),
               newItem->objectFullName.c_str(),
               newItem->elementName.c_str(),
               newItem->associateName.c_str(),
               newItem->elementID,
               newItem->subelement,
               newItem->parameterID,
               newItem->parameterType);
      #endif

      if (newItem->parameterType == Gmat::RVECTOR_TYPE)
      {
         const Rvector mat =
            owners[order[i]]->GetRvectorParameter(property[order[i]]);
         newItem->rowLength = mat.GetSize();
         newItem->rowIndex = val-1;
      }

      if (newItem->parameterType == Gmat::RMATRIX_TYPE)
      {
         const Rmatrix mat =
            owners[order[i]]->GetRmatrixParameter(property[order[i]]);
         newItem->rowLength = mat.GetNumColumns();
         newItem->colIndex = (val-1) % newItem->rowLength;
         newItem->rowIndex = (Integer)((val - 1) / newItem->rowLength);

         #ifdef DEBUG_STATE_CONSTRUCTION
            MessageInterface::ShowMessage(
                  "RowLen = %d, %d -> row %2d  col %2d\n", newItem->rowLength,
                  val, newItem->rowIndex, newItem->colIndex);
         #endif
      }

      newItem->length =
            owners[order[i]]->GetEstimationParameterSize(idList[order[i]]);

      if (val == newItem->length)
         val = 0;

      stateMap.push_back(newItem);
   }

   #ifdef DEBUG_STATE_CONSTRUCTION
      MessageInterface::ShowMessage("State size = %d; map contents:\n",
            stateSize);
      for (std::vector<ListItem*>::iterator i = stateMap.begin();
            i != stateMap.end(); ++i)
         MessageInterface::ShowMessage("   obj = <%p,%s,%s>   objectName = %s   objectFullName = %s  elementName = %s   elementID = %d   subelement = %d of %d,   parameterID = %d\n",
               (*i)->object, ((*i)->object)->GetName().c_str(), ((*i)->object)->GetFullName().c_str(),
               (*i)->objectName.c_str(), (*i)->objectFullName.c_str(), (*i)->elementName.c_str(),
               (*i)->elementID, (*i)->subelement, (*i)->length,
               (*i)->parameterID);
   #endif


   // It needs to speicify ID of spacecraft's CartesianState pramater in order to map acceleration values 
   for (Integer i = 0; i < objects.size(); ++i)
   {
      if (objects[i]->IsOfType(Gmat::SPACECRAFT))
      {
         cartesianStateID = ((Spacecraft*)(objects[i]))->GetParameterID("CartesianState");
         break;
      }
   }

   return stateSize;
}


const std::vector<PhysicalModel*>& EstimationStateManager::GetAllPhysicalModels()
{
	return pms;
}


GmatTime EstimationStateManager::GetEstimationEpochGT()
{
   GmatTime estEpoch;
   if (state.HasPrecisionTime())
      estEpoch = state.GetEpochGT();
   else
      estEpoch = GmatTime(state.GetEpoch());

   return estEpoch;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantState(GmatBase* spaceObj, std::string inStateType)
//------------------------------------------------------------------------------
/**
* This method used to get state of a spacecraft in its own coordinate system and in
* a given state type. For Keplerian state, anomaly is in form of MA (instead of TA)
*
* @param spaceObj      spacecraft from which we need to get state
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
* @param anomalyType   spacefify anomaly type to be "MA" or "TA" for Keplerian anomaly element.
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in 
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
Rvector6 EstimationStateManager::GetParticipantState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType)
{
   Rvector6 outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      
      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      
      // Convert internal state to spacecraft's Cartesian coordinate system
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, cs);
      
      // Convert Cartesian to Keplerian if it needs
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            Planet* pn = (Planet*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, the origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               pn = (Planet*)(ss->GetBody(cbName));
            }
            CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);
            cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);
            delete mj2kCS;

            Real mu = pn->GetRealParameter("Mu");
            outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);     // Keplerian anomaly element is in form of MA (instead of TA)
         }
         else
            outState = outStateCart;
      }
      else if (inStateType == "Keplerian")
      {
         SolarSystem* ss = cs->GetSolarSystem();
         SpacePoint* origin = cs->GetOrigin();
         Planet* pn = (Planet*)origin;
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            // for case CS is GDSTropocentric, the origin is ground station GDS
            std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
            pn = (Planet*)(ss->GetBody(cbName));
         }
         CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);
         cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);
         delete mj2kCS;

         Real mu = pn->GetRealParameter("Mu");
         outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);        // Keplerian anomaly emement is in form of MA (instaed of TA)
      }
      else if (inStateType == "Cartesian")
         outState = outStateCart;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");

      delete cv;
      cv = NULL;
   }

   return outState;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantMJ2000EqState(GmatBase* spaceObj, std::string inStateType)
//------------------------------------------------------------------------------
/**
* This method used to get state of a spacecraft in MJ2000Eq axis and in
* a given state type. For Keplerian state, anomaly is in form of MA (instead of TA)
*
* @param spaceObj      spacecraft from which we need to get state
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
* @param anomalyType   spacefify anomaly type to be "MA" or "TA" for Keplerian anomaly element.
*
* @return            spacecraft's state in MJ2000Eq and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
Rvector6 EstimationStateManager::GetParticipantMJ2000EqState(GmatBase* spaceObj, std::string inStateType, std::string anomalyType)
{
   Rvector6 outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);

      // Convert internal state to spacecraft's Cartesian coordinate system
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      // Get spacecraft's internal coordinate system
      Spacecraft* obj = (Spacecraft*)spaceObj;
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);
      
      // Get spacecraft's state in MJ2000Eq Cartesian coordinate system 
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);
      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;

      // Convert Cartesian to Keplerian if it needs
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;
            Real mu = pn->GetRealParameter("Mu");
            outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);     // Keplerian anomaly element is in form of MA (instead of TA)
         }
         else
            outState = outStateCart;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         outState = StateConversionUtil::CartesianToKeplerian(mu, outStateCart, anomalyType);        // Keplerian anomaly emement is in form of MA (instaed of TA)
      }
      else if (inStateType == "Cartesian")
         outState = outStateCart;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");
      
   }
   
   return outState;
}


//------------------------------------------------------------------------------
// bool SetParticipantState(GmatBase* spaceObj, Rvector6& inputState,
//                          std::string inStateType)
//------------------------------------------------------------------------------
/**
* Method used to set state of a spacecraft. For Keplerian state, anomaly element 
* is in form of MA (instead of TA).
*
* @param spaceObj      spacecraft from which we need to get state
* @param inputState    state of spacecraft in its own coordinate system. For 
*                       Keplerian, anomaly element in the input state is in form of "MA"
* @param inStateType   state type used in the state. Its value can be 
*                       "Cartesian", "Keplerian", or ""
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
bool EstimationStateManager::SetParticipantState(GmatBase* spaceObj, 
    Rvector6& inputState, std::string inStateType)
{
   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      //Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);
      CoordinateConverter* cv = new CoordinateConverter();

      // Convert Keplerian to Cartesian if it needs
      Rvector6 outState;
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            Planet* pn = (Planet*)origin;
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               // for case CS is GDSTropocentric, the origin is ground station GDS
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               pn = (Planet*)(ss->GetBody(cbName));
            }

            Real mu = pn->GetRealParameter("Mu");
            outState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
         }
         else
         {
            // Convert from spacecraft coordiante system to internal coordinate system
            cv->Convert(epoch, inputState, cs, outState, internalcs);
         }
      }
      else if (inStateType == "Keplerian")
      {
         SolarSystem* ss = cs->GetSolarSystem();
         SpacePoint* origin = cs->GetOrigin();
         Planet* pn = (Planet*)origin;
         if (origin->IsOfType(Gmat::GROUND_STATION))
         {
            // for case CS is GDSTropocentric, the origin is ground station GDS
            std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
            pn = (Planet*)(ss->GetBody(cbName));
         }

         Real mu = pn->GetRealParameter("Mu");
         outState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
      }
      else if (inStateType == "Cartesian")
      {
         // Convert from spacecraft coordiante system to internal coordinate system
         cv->Convert(epoch, inputState, cs, outState, internalcs);
      }
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");

      // Set state
      for (Integer j = 0; j < 6; ++j)
         state[i+j] = outState[j];

      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;
   }

   return true;
}


//------------------------------------------------------------------------------
// bool SetParticipantMJ2000EqState(GmatBase* spaceObj, Rvector6& inputState,
//                          std::string inStateType)
//------------------------------------------------------------------------------
/**
* Method used to set MJ2000Eq state of a spacecraft. For Keplerian state, anomaly element
* is in form of MA (instead of TA).
*
* @param spaceObj      spacecraft from which we need to get state
* @param inputState    state of spacecraft in MJ2000Eq coordinate system. For
*                       Keplerian, anomaly element in the input state is in form of "MA"
* @param inStateType   state type used in the state. Its value can be
*                       "Cartesian", "Keplerian", or ""
*
* @return            spacecraft's state in its coordinate system and
*                    If inStateType is "Cartesian", state will be in Cartesian state
*                    If inStateType is "Keplerian", state will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
bool EstimationStateManager::SetParticipantMJ2000EqState(GmatBase* spaceObj,
   Rvector6& inputState, std::string inStateType)
{
   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // Get state of spacecraft object
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      Spacecraft* obj = (Spacecraft*)spaceObj;

      // Create spaceraft's coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }

      // Convert Keplerian to Cartesian if it needs
      Rvector6 tempState;
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;

            Real mu = pn->GetRealParameter("Mu");
            tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
         }
         else
            tempState = inputState;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         tempState = StateConversionUtil::KeplerianToCartesian(mu, inputState, "MA");
      }
      else if (inStateType == "Cartesian")
         tempState = inputState;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");


      // Get internal coordinate system
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Convert from spacecraft coordiante system to internal coordinate system
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outState;
      GmatTime epoch = GetEstimationEpochGT();
      cv->Convert(epoch, tempState, mj2kCS, outState, internalcs);
      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;

      // Set state
      for (Integer j = 0; j < 6; ++j)
         state[i + j] = outState[j];
   }

   return true;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantMJ2000EqStateOffset(GmatBase* spaceObj, std::string inStateType)
//------------------------------------------------------------------------------
/**
* This method used to get state offset of a spacecraft in MJ2000Eq axis and in
* a given state type. For Keplerian state, anomaly is in form of MA (instead of TA)
*
* @param spaceObj      spacecraft from which we need to get state
* @param inStateType   state type used in the state offset. Its value can be
*                       "Cartesian", "Keplerian", or ""
* @param anomalyType   spacefify anomaly type to be "MA" or "TA" for Keplerian anomaly element.
*
* @return            spacecraft's state offset in MJ2000Eq and
*                    If inStateType is "Cartesian", state offset will be in Cartesian state
*                    If inStateType is "Keplerian", state offset will be in Keplerian state
*                    If inStateType is "", state will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
Rvector6 EstimationStateManager::GetParticipantMJ2000EqStateOffset(GmatBase* spaceObj, std::string inStateType, std::string anomalyType)
{
   Rvector6 outStateOffset, outStateFull;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      Rvector6 inStateFull;
      for (Integer j = 0; j < 6; j++)
         inStateFull[j] = state[i + j] + stateOffset[i + j];

      // Convert internal state to spacecraft's Cartesian coordinate system
      outStateFull = inStateFull;
      GmatTime epoch = GetEstimationEpochGT();

      // Get spacecraft's internal coordinate system
      Spacecraft* obj = (Spacecraft*)spaceObj;
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Get spacecraft's state in MJ2000Eq Cartesian coordinate system 
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateFullCart;
      cv->Convert(epoch, inStateFull, internalcs, outStateFullCart, mj2kCS);
      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;

      // Convert Cartesian to Keplerian if it needs
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;
            Real mu = pn->GetRealParameter("Mu");
            outStateFull = StateConversionUtil::CartesianToKeplerian(mu, outStateFullCart, anomalyType);     // Keplerian anomaly element is in form of MA (instead of TA)
         }
         else
            outStateFull = outStateFullCart;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         outStateFull = StateConversionUtil::CartesianToKeplerian(mu, outStateFullCart, anomalyType);        // Keplerian anomaly emement is in form of MA (instaed of TA)
      }
      else if (inStateType == "Cartesian")
         outStateFull = outStateFullCart;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");

      // Subtract out the main portion of the state to leave behind just the offset
      Rvector6 outState = GetParticipantMJ2000EqState(spaceObj, inStateType, anomalyType);
      outStateOffset = outStateFull - outState;
   }

   return outStateOffset;
}


//------------------------------------------------------------------------------
// bool SetParticipantMJ2000EqStateOffset(GmatBase* spaceObj, Rvector6& inputStateOffset,
//                          std::string inStateType)
//------------------------------------------------------------------------------
/**
* Method used to set MJ2000Eq state offset of a spacecraft. For Keplerian state, anomaly element
* is in form of MA (instead of TA).
*
* @param spaceObj      spacecraft from which we need to get state offset
* @param inputStateOffset  state offset of spacecraft in MJ2000Eq coordinate system. For
*                          Keplerian, anomaly element in the input state offset is in form of "MA"
* @param inStateType   state type used in the state offset. Its value can be
*                       "Cartesian", "Keplerian", or ""
*
* @return            spacecraft's state offset in its coordinate system and
*                    If inStateType is "Cartesian", state offset will be in Cartesian state
*                    If inStateType is "Keplerian", state offset will be in Keplerian state
*                    If inStateType is "", state offset will be in state type specified in
*                       spacecraft's DisplayStateType parameter set in script
*
*/
//------------------------------------------------------------------------------
bool EstimationStateManager::SetParticipantMJ2000EqStateOffset(GmatBase* spaceObj,
   Rvector6& inputStateOffset, std::string inStateType)
{
   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // Get state of spacecraft object
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      Spacecraft* obj = (Spacecraft*)spaceObj;

      Rvector6 inputStateFull;
      Rvector6 inputState = GetParticipantMJ2000EqState(spaceObj, inStateType, "MA");
      for (Integer j = 0; j < 6; j++)
         inputStateFull[j] = inputState[j] + inputStateOffset[j];

      // Create spaceraft's coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }

      // Convert Keplerian to Cartesian if it needs
      Rvector6 tempState;
      if (inStateType == "")
      {
         std::string stateType = obj->GetStringParameter("DisplayStateType");
         if (stateType == "Keplerian")
         {
            Planet* pn = (Planet*)origin;

            Real mu = pn->GetRealParameter("Mu");
            tempState = StateConversionUtil::KeplerianToCartesian(mu, inputStateFull, "MA");
         }
         else
            tempState = inputStateFull;
      }
      else if (inStateType == "Keplerian")
      {
         Planet* pn = (Planet*)origin;
         Real mu = pn->GetRealParameter("Mu");
         tempState = StateConversionUtil::KeplerianToCartesian(mu, inputStateFull, "MA");
      }
      else if (inStateType == "Cartesian")
         tempState = inputStateFull;
      else
         throw EstimatorException("Error: Input state type '" + inStateType + "' is invalid. It would be Cartesian, Keplerian, or an empty string.\n");


      // Get internal coordinate system
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create spacecraft's MJ2000Eq Cartesian coordinate system
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem("mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Convert from spacecraft coordiante system to internal coordinate system
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateFull;
      GmatTime epoch = GetEstimationEpochGT();
      cv->Convert(epoch, tempState, mj2kCS, outStateFull, internalcs);
      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;

      // Set state
      for (Integer j = 0; j < 6; ++j)
         stateOffset[i + j] = outStateFull[j] - state[i + j];
   }

   return true;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantCartesianState(GmatBase* spaceObj)
//------------------------------------------------------------------------------
/**
* Method used to convert state of a spacecraft from internal coordinate
* system to the coordinate system specified in script
*
* @param spaceObj    spacecraft from which we need to get state
*
* @return            spacecraft's state in its apparent coordinate system and 
*                    Cartesian state type
*
*/
//------------------------------------------------------------------------------
Rvector EstimationStateManager::GetParticipantCartesianState(GmatBase* spaceObj)
{
   Rvector outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // 1.Get state of spacecraft in its internal coordinate system
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }

      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();

      Spacecraft* obj = (Spacecraft*)spaceObj;
      //std::string stateType = obj->GetStringParameter("DisplayStateType");

      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, cs);
      delete cv;
      cv = NULL;

      outState = outStateCart;
   }

   return outState;
}


//------------------------------------------------------------------------------
// Rvector& GetParticipantMJ2000EqCartesianState(GmatBase* spaceObj)
//------------------------------------------------------------------------------
/**
* Method used to convert state of a spacecraft from internal coordinate
* system to MJ2000Eq Cartesian coordinate system
*
* @param spaceObj    spacecraft from which we need to get state
*
* @return            spacecraft's state in MJ2000Eq Cartesian coordinate system
* example: spacecraft's coordinate system is EarthFixed, This function gets
*          spacecraft state in EarthMJ2000Eq coordinate system.
*
*/
//------------------------------------------------------------------------------
Rvector EstimationStateManager::GetParticipantMJ2000EqCartesianState(GmatBase* spaceObj)
{
   Rvector outState;

   if (spaceObj->IsOfType(Gmat::SPACEOBJECT))
   {
      // Get spacecraft object
      Integer i = 0;
      for (; i < stateMap.size(); ++i)
      {
         if (stateMap[i]->object == spaceObj)
            break;
      }
      Spacecraft* obj = (Spacecraft*)spaceObj;

      // Get internal coordinate system
      CoordinateSystem* internalcs = obj->GetInternalCoordSystem();
      if (internalcs == NULL)
         throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

      // Create MJ2000Eq Cartesian coordinate system
      std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
      CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
      if (cs == NULL)
         throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");

      SolarSystem* ss = cs->GetSolarSystem();
      SpacePoint* origin = cs->GetOrigin();
      if (origin->IsOfType(Gmat::GROUND_STATION))
      {
         std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
         origin = ss->GetBody(cbName);
      }
      CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

      // Convert state from internal coordinate system to MJ2000Eq Cartesian coordinate system
      Rvector6 inState(state[i], state[i + 1], state[i + 2], state[i + 3], state[i + 4], state[i + 5]);
      outState = inState;
      GmatTime epoch = GetEstimationEpochGT();
      CoordinateConverter* cv = new CoordinateConverter();
      Rvector6 outStateCart;
      cv->Convert(epoch, inState, internalcs, outStateCart, mj2kCS);

      delete cv;
      delete mj2kCS;
      cv = NULL; mj2kCS = NULL;

      outState = outStateCart;
   }

   return outState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationState()
//-------------------------------------------------------------------------
/**
* This Method used to get estimation state in Cartesian or Keplerian as
* specified by solve-for variable. Cr_Epsilon and Cd_Epsilon will be use
* instead of Cr and Cd.
*
* @return     estimation state in participants' coordinate systems and 
*             state type, Cr_Epsilon, Cd_Epsilon and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";

            // get a solve-for state. Note that: solve-for state in normal equation has Keplerian anomaly element in form of "MA"
            //Rvector outState = GetParticipantState((*map)[i]->object, stateType, "MA");
            Rvector outState = GetParticipantMJ2000EqState((*map)[i]->object, stateType, "MA");
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
      else
      {
         outputState[i] = state[i];
      }
   }

   return outputState;
}


GmatState* EstimationStateManager::SetEstimationState(GmatState& inputState)
{
   const std::vector<ListItem*> *map = GetStateMap();
   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector6 setVal;
            for (Integer j = 0; j < 6; ++j)
               setVal[j] = inputState[i+j];

            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";
            //SetParticipantState((*map)[i]->object, setVal, stateType);
            SetParticipantMJ2000EqState((*map)[i]->object, setVal, stateType);

            i = i + 5;
         }
      }
      else
      {
         state[i] = inputState[i];
      }
   }

   return &state;
}


//-------------------------------------------------------------------------
// GmatState* SetEstimationStateObjects(const GmatState& inputState)
//-------------------------------------------------------------------------
/**
* This is used to directly set the estimation state objects in their base coordinate system.
* The epoch in the input state will be used to set the epoch of the objects
*
* @param inputState The state to set to the objects
*
* @return The estimation state vector
*
*/
//-------------------------------------------------------------------------
GmatState* EstimationStateManager::SetEstimationStateObjects(const GmatState& inputState)
{

   for (Integer index = 0; index < stateSize; ++index)
   {
      Integer stateParameterID = stateMap[index]->parameterID;

      // Get non-epsilon parameter ID if needed
      std::string stateParameterName = (stateMap[index]->object)->GetParameterText(stateParameterID);
      stateParameterName = (stateMap[index]->object)->GetParameterNameFromEstimationParameter(stateParameterName);
      stateParameterID = (stateMap[index]->object)->GetParameterID(stateParameterName);

      #ifdef DEBUG_OBJECT_UPDATES
         std::stringstream msg("");
         msg << stateMap[index]->subelement;
         std::string lbl = stateMap[index]->objectFullName + "." + 
            stateMap[index]->elementName + "." + msg.str() + " = ";
         MessageInterface::ShowMessage("   %d: %s%.12lf  for object <%s, %p>   parameterID = %d\n", index, lbl.c_str(),
            state[index], stateMap[index]->object->GetFullName().c_str(), stateMap[index]->object, stateParameterID);
      #endif

      switch (stateMap[index]->parameterType)
      {
         case Gmat::REAL_TYPE:
            (stateMap[index]->object)->SetRealParameter(
                   stateParameterID, inputState[index]);
            break;

         case Gmat::RVECTOR_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateParameterID, inputState[index],
                     stateMap[index]->rowIndex);
            break;

         case Gmat::RMATRIX_TYPE:
            stateMap[index]->object->SetRealParameter(
                     stateParameterID, inputState[index],
                     stateMap[index]->rowIndex, stateMap[index]->colIndex);
            break;

         default:
            std::stringstream sel("");
            sel << stateMap[index]->subelement;
            std::string label = stateMap[index]->objectName + "." +
                  stateMap[index]->elementName + "." + sel.str();
            MessageInterface::ShowMessage(
                  "%s not set; Element type not handled\n",label.c_str());
      }

   }

   GmatEpoch theEpoch = inputState.GetEpoch();
   GmatTime theEpochGT = inputState.GetEpochGT();
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      if (epochIDs[i] >= 0)
      {
         objects[i]->SetRealParameter(epochIDs[i], theEpoch);
         if (inputState.HasPrecisionTime())
            objects[i]->SetGmatTimeParameter(epochIDs[i], theEpochGT);
         else
            objects[i]->SetGmatTimeParameter(epochIDs[i], GmatTime(theEpoch));
      }
   }

   // Update the state vector from the objects that were just set
   MapObjectsToVector();

   return &state;
}


//-------------------------------------------------------------------------
// GmatState* SetEstimationCartesianStateParticipant(GmatState& inputState)
//-------------------------------------------------------------------------
/**
* This is used to directly set the estimation state objects in the Cartesian
* coordinate system specified by solve-for variable. The epoch in the
* input state will be used to set the epoch of the objects
*
* @param inputState The state to set to the objects
*
* @return The estimation state vector
*
*/
//-------------------------------------------------------------------------
GmatState* EstimationStateManager::SetEstimationCartesianStateParticipant(const GmatState& inputState)
{
   GmatState objectState = inputState;

   for (Integer i = 0; i < stateSize; ++i)
   {
      if ((stateMap[i]->elementName == "CartesianState") ||
         (stateMap[i]->elementName == "KeplerianState") ||
         (stateMap[i]->elementName == "Position"))
      {
         if (stateMap[i]->subelement == 1)
         {
            Rvector6 inState(inputState[i], inputState[i + 1], inputState[i + 2], inputState[i + 3], inputState[i + 4], inputState[i + 5]);
            Rvector6 outState = inState;
            GmatTime epoch = inputState.GetEpochGT();

            Spacecraft* obj = (Spacecraft*)stateMap[i]->object;

            std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
            CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

            if (cs == NULL)
               throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
            if (internalcs == NULL)
               throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

            CoordinateConverter* cv = new CoordinateConverter();
            Rvector6 outStateCart;
            cv->Convert(epoch, inState, cs, outStateCart, internalcs);
            delete cv;
            cv = NULL;

            outState = outStateCart;

            for (Integer j = 0; j < 6; ++j)
               objectState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return SetEstimationStateObjects(objectState);
}


//-------------------------------------------------------------------------
// GmatState* SetEstimationMJ2000EqCartesianStateParticipant(GmatState& inputState)
//-------------------------------------------------------------------------
/**
* This is used to directly set the estimation state objects in the Cartesian
* coordinate system specified by solve-for variable. The epoch in the
* input state will be used to set the epoch of the objects
*
* @param inputState The state to set to the objects
*
* @return The estimation state vector
*
*/
//-------------------------------------------------------------------------
GmatState* EstimationStateManager::SetEstimationMJ2000EqCartesianStateParticipant(const GmatState& inputState)
{
   GmatState objectState = inputState;

   for (Integer i = 0; i < stateSize; ++i)
   {
      if ((stateMap[i]->elementName == "CartesianState") ||
         (stateMap[i]->elementName == "KeplerianState") ||
         (stateMap[i]->elementName == "Position"))
      {
         if (stateMap[i]->subelement == 1)
         {
            Rvector6 inState(inputState[i], inputState[i + 1], inputState[i + 2], inputState[i + 3], inputState[i + 4], inputState[i + 5]);
            Rvector6 outState = inState;
            GmatTime epoch = inputState.GetEpochGT();

            Spacecraft* obj = (Spacecraft*)stateMap[i]->object;

            std::string csName = obj->GetRefObjectName(Gmat::COORDINATE_SYSTEM);
            CoordinateSystem* cs = (CoordinateSystem*)obj->GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
            CoordinateSystem* internalcs = obj->GetInternalCoordSystem();

            SolarSystem* ss = cs->GetSolarSystem();
            SpacePoint* origin = cs->GetOrigin();
            if (origin->IsOfType(Gmat::GROUND_STATION))
            {
               std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
               origin = ss->GetBody(cbName);
            }
            CoordinateSystem* mj2kCS = CoordinateSystem::CreateLocalCoordinateSystem(origin->GetName() + "mj2kCS", "MJ2000Eq", origin, NULL, NULL, cs->GetJ2000Body(), ss);

            if (cs == NULL)
               throw EstimatorException("Coordinate system for " + obj->GetName() + " is not set\n");
            if (internalcs == NULL)
               throw EstimatorException("Internal coordinate system for " + obj->GetName() + " is not set\n");

            CoordinateConverter* cv = new CoordinateConverter();
            Rvector6 outStateCart;
            cv->Convert(epoch, inState, mj2kCS, outStateCart, internalcs);
            delete cv;
            delete mj2kCS;
            cv = NULL; mj2kCS = NULL;

            outState = outStateCart;

            for (Integer j = 0; j < 6; ++j)
               objectState[i + j] = outState[j];

            i = i + 5;
         }
      }
   }

   return SetEstimationStateObjects(objectState);
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationStateOffset()
//-------------------------------------------------------------------------
/**
* This Method used to get estimation state offset in Cartesian or Keplerian
* as specified by solve-for variable. Cr_Epsilon and Cd_Epsilon will be use
* instead of Cr and Cd.
*
* @return     estimation state offset in participants' coordinate systems
*              andstate type, Cr_Epsilon, Cd_Epsilon and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationStateOffset()
{
   GmatState outputStateOffset;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateOffsetElement;
   outputStateOffset.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";

            // get a solve-for state. Note that: solve-for state in normal equation has Keplerian anomaly element in form of "MA"
            //Rvector outState = GetParticipantState((*map)[i]->object, stateType, "MA");
            Rvector outStateOffset = GetParticipantMJ2000EqStateOffset((*map)[i]->object, stateType, "MA");
            for (Integer j = 0; j < 6; ++j)
               outputStateOffset[i + j] = outStateOffset[j];

            i = i + 5;
         }
      }
      else
      {
         outputStateOffset[i] = stateOffset[i];
      }
   }

   return outputStateOffset;
}


GmatState* EstimationStateManager::SetEstimationStateOffset(GmatState& inputStateOffset)
{
   const std::vector<ListItem*> *map = GetStateMap();
   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector6 setVal;
            for (Integer j = 0; j < 6; ++j)
               setVal[j] = inputStateOffset[i + j];

            std::string stateType = "Cartesian";
            if ((*map)[i]->elementName == "KeplerianState")
               stateType = "Keplerian";
            //SetParticipantState((*map)[i]->object, setVal, stateType);
            SetParticipantMJ2000EqStateOffset((*map)[i]->object, setVal, stateType);

            i = i + 5;
         }
      }
      else
      {
         stateOffset[i] = inputStateOffset[i];
      }
   }

   return &stateOffset;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationCartesianState()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr_Epsilon and
* Cd_Epsilon.
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr_Epsilon, Cd_Epsilon and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationCartesianState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector outState = GetParticipantCartesianState((*map)[i]->object);
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
      else
      {
         outputState[i] = state[i];
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationMJ2000EqCartesianState()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to MJ2000Eq
* Cartesian coordinate system. For report, it reports Cr_Epsilon and
* Cd_Epsilon.
*
* @return     estimation state in MJ2000Eq Cartesian coordinate systems,
*             Cr_Epsilon, Cd_Epsilon and bias
* example: spacecraft's coordinate system is EarthFixed, This function gets
*          spacecraft state in EarthMJ2000Eq coordinate system.
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationMJ2000EqCartesianState()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if (((*map)[i]->elementName == "CartesianState") ||
         ((*map)[i]->elementName == "KeplerianState") ||
         ((*map)[i]->elementName == "Position"))
      {
         if ((*map)[i]->subelement == 1)
         {
            Rvector outState = GetParticipantMJ2000EqCartesianState((*map)[i]->object);
            for (Integer j = 0; j < 6; ++j)
               outputState[i + j] = outState[j];

            i = i + 5;
         }
      }
      else
      {
         outputState[i] = state[i];
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationMJ2000EqCartesianStateForReport()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to MJ2000Eq
* Cartesian coordinate system. For report, it reports Cr and Cd.
*
* @return     estimation state in MJ2000Eq Cartesian coordinate systems,
*             Cr, Cd and bias
* example: spacecraft's coordinate system is EarthFixed, This function gets
*          spacecraft state in EarthMJ2000Eq coordinate system.
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationMJ2000EqCartesianStateForReport()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->object->IsOfType("Plate"))
      {
         if ((*map)[i]->elementName == "AreaCoefficient")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("AreaCoefficient");
         }
         else if ((*map)[i]->elementName == "Area")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("Area");
         }
         else if ((*map)[i]->elementName == "LitFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("LitFraction");
         }
         else if ((*map)[i]->elementName == "SpecularFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("SpecularFraction");
         }
         else if ((*map)[i]->elementName == "DiffuseFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("DiffuseFraction");
         }
      }
      else
      {
         // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
            else if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADSRPScaleFactor");
         }
         else if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
            else if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADDragScaleFactor");
         }
         else if ((*map)[i]->elementName == "AtmosDensity_Epsilon")
         {
            outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("AtmosDensityScaleFactor");
         }
         else if (GmatStringUtil::EndsWith((*map)[i]->elementName, ".TSF_Epsilon")) {    // <segmentName>.TSF_Epsilon
            StringArray parts = GmatStringUtil::SeparateBy((*map)[i]->elementName, ".");
            std::string tsfName = parts.at(0) + ".ThrustScaleFactor";
            outputState[i] = (*map)[i]->object->GetRealParameter(tsfName);
         }
         else if ((*map)[i]->elementName == "Bias")
            outputState[i] = state[i];
         else if (((*map)[i]->elementName == "CartesianState") ||
            ((*map)[i]->elementName == "KeplerianState") ||
            ((*map)[i]->elementName == "Position"))
         {
            if ((*map)[i]->subelement == 1)
            {
               Rvector outState = GetParticipantMJ2000EqCartesianState((*map)[i]->object);
               for (Integer j = 0; j < 6; ++j)
                  outputState[i + j] = outState[j];

               i = i + 5;
            }
         }
         else
            outputState[i] = state[i];
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationCartesianStateForReport()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr and Cd.
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr, Cd, and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationCartesianStateForReport()
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->object->IsOfType("Plate"))
      {
         if ((*map)[i]->elementName == "AreaCoefficient")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("AreaCoefficient");
         }
         else if ((*map)[i]->elementName == "Area")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("Area");
         }
         else if ((*map)[i]->elementName == "LitFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("LitFraction");
         }
         else if ((*map)[i]->elementName == "SpecularFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("SpecularFraction");
         }
         else if ((*map)[i]->elementName == "DiffuseFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("DiffuseFraction");
         }
      }
      else
      {
         // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
            else if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADSRPScaleFactor");
         }
         else if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
            else if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADDragScaleFactor");
         }
         else if ((*map)[i]->elementName == "AtmosDensity_Epsilon")
         {
            outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("AtmosDensityScaleFactor");
         }
         else if (GmatStringUtil::EndsWith((*map)[i]->elementName, ".TSF_Epsilon")) {    // <segmentName>.TSF_Epsilon
            StringArray parts = GmatStringUtil::SeparateBy((*map)[i]->elementName, ".");
            std::string tsfName = parts.at(0) + ".ThrustScaleFactor";
            outputState[i] = (*map)[i]->object->GetRealParameter(tsfName);
         }
         else if ((*map)[i]->elementName == "Bias")
            outputState[i] = state[i];
         else if (((*map)[i]->elementName == "CartesianState") ||
            ((*map)[i]->elementName == "KeplerianState") ||
            ((*map)[i]->elementName == "Position"))
         {
            if ((*map)[i]->subelement == 1)
            {
               Rvector outState = GetParticipantCartesianState((*map)[i]->object);
               for (Integer j = 0; j < 6; ++j)
                  outputState[i + j] = outState[j];

               i = i + 5;
            }
         }
         else
         {
            outputState[i] = state[i];
         }
      }
   }

   // Set epoch
   outputState.SetPrecisionTimeFlag(true);

   GmatTime theEpochGT = 0.0;
   for (UnsignedInt i = 0; i < objects.size(); ++i)
   {
      // Objects without epoch have a -1 set as their epochID
      if (epochIDs[i] >= 0)
      {
		  if (theEpochGT == 0.0)
		  {
			  theEpochGT = objects[i]->GetGmatTimeParameter(epochIDs[i]);
			  break;
		  }
      }
   }

   outputState.SetEpoch(theEpochGT.GetMjd());
   outputState.SetEpochGT(theEpochGT);

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationKeplerianStateForReport(std::string anomalyType)
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr and Cd.
*
* @param anomalyType    form of anomaly is used. It would be "TA" or "MA".
*                       Default value is "TA".
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr, Cd, and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationKeplerianStateForReport(std::string anomalyType)
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->object->IsOfType("Plate"))
      {
         if ((*map)[i]->elementName == "AreaCoefficient")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("AreaCoefficient");
         }
         else if ((*map)[i]->elementName == "Area")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("Area");
         }
         else if ((*map)[i]->elementName == "LitFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("LitFraction");
         }
         else if ((*map)[i]->elementName == "SpecularFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("SpecularFraction");
         }
         else if ((*map)[i]->elementName == "DiffuseFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("DiffuseFraction");
         }
      }
      else
      {
         // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
            else if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADSRPScaleFactor");

         }
         else if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
            else if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADDragScaleFactor");

         }
         else if ((*map)[i]->elementName == "AtmosDensity_Epsilon")
         {
            outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("AtmosDensityScaleFactor");

         }
         else if (GmatStringUtil::EndsWith((*map)[i]->elementName, ".TSF_Epsilon")) {    // <segmentName>.TSF_Epsilon
            StringArray parts = GmatStringUtil::SeparateBy((*map)[i]->elementName, ".");
            std::string tsfName = parts.at(0) + ".ThrustScaleFactor";
            outputState[i] = (*map)[i]->object->GetRealParameter(tsfName);
         }
         else if ((*map)[i]->elementName == "Bias")
            outputState[i] = state[i];
         else if (((*map)[i]->elementName == "CartesianState") ||
            ((*map)[i]->elementName == "KeplerianState") ||
            ((*map)[i]->elementName == "Position"))
         {
            if ((*map)[i]->subelement == 1)
            {
               Rvector6 outState = GetParticipantMJ2000EqState((*map)[i]->object, "Keplerian", anomalyType);
               for (Integer j = 0; j < 6; ++j)
                  outputState[i + j] = outState[j];

               i = i + 5;
            }
         }
         else
         {
            outputState[i] = state[i];
         }
      }
   }

   return outputState;
}


//-------------------------------------------------------------------------
// GmatState& GetEstimationStateForReport()
//-------------------------------------------------------------------------
/**
* This Method used to convert result of estimation state to participants'
* coordinate system and state type. For report, it reports Cr and Cd 
* instead of Cr_Epsilon and Cd_Epsilon.
*
* @param anomalyType    form of anomaly is used. It would be "TA" or "MA". 
*                       Default value is "TA".
*
* @return     estimation state in participants' coordinate systems and
*             state type, Cr, Cd, and bias
*
*/
//-------------------------------------------------------------------------
GmatState EstimationStateManager::GetEstimationStateForReport(std::string anomalyType)
{
   GmatState outputState;
   const std::vector<ListItem*> *map = GetStateMap();

   Real outputStateElement;
   outputState.SetSize(map->size());

   for (UnsignedInt i = 0; i < map->size(); ++i)
   {
      if ((*map)[i]->object->IsOfType("Plate"))
      {
         if ((*map)[i]->elementName == "AreaCoefficient")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("AreaCoefficient");
         }
         else if ((*map)[i]->elementName == "Area")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("Area");
         }
         else if ((*map)[i]->elementName == "LitFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("LitFraction");
         }
         else if ((*map)[i]->elementName == "SpecularFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("SpecularFraction");
         }
         else if ((*map)[i]->elementName == "DiffuseFraction")
         {
            outputState[i] = ((Plate*)(*map)[i]->object)->GetRealParameter("DiffuseFraction");
         }
      }
      else
      {
         // Get Cr and Cd instead of Cr_Epsilon and Cd_Epsilon
         if ((*map)[i]->elementName == "Cr_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cr");
            else if (((Spacecraft*)(*map)[i]->object)->GetSRPShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADSRPScaleFactor");
         }
         else if ((*map)[i]->elementName == "Cd_Epsilon")
         {
            if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "Spherical")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("Cd");
            else if (((Spacecraft*)(*map)[i]->object)->GetDragShapeModel() == "SPADFile")
               outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("SPADDragScaleFactor");
         }
         else if ((*map)[i]->elementName == "AtmosDensity_Epsilon")
         {
            outputState[i] = ((Spacecraft*)(*map)[i]->object)->GetRealParameter("AtmosDensityScaleFactor");
         }
         else if (GmatStringUtil::EndsWith((*map)[i]->elementName, ".TSF_Epsilon")) {    // <segmentName>.TSF_Epsilon
            StringArray parts = GmatStringUtil::SeparateBy((*map)[i]->elementName, ".");
            std::string tsfName = parts.at(0) + ".ThrustScaleFactor";
            outputState[i] = ((ODEModel*)(*map)[i]->object)->GetRealParameter(tsfName);
         }
         else if (((*map)[i]->elementName == "CartesianState") ||
            ((*map)[i]->elementName == "KeplerianState") ||
            ((*map)[i]->elementName == "Position"))
         {
            if ((*map)[i]->subelement == 1)
            {
               // In estimation report file or GmatLog.txt anomaly is always in "TA" form
               Rvector outState = GetParticipantState((*map)[i]->object, "", anomalyType);     // inStateType = "" that means get participant state in its own DisplayStateType
               for (Integer j = 0; j < 6; ++j)
                  outputState[i + j] = outState[j];

               i = i + 5;
            }
         }
         else if ((*map)[i]->elementName == "Bias")
         {
            outputState[i] = state[i];
         }
         else
         {
            outputState[i] = state[i];
         }
      }
   }

   outputState.SetEpochGT(state.GetEpochGT());
   outputState.SetEpoch(state.GetEpoch());

   return outputState;
}



//--------------------------------------------------------------------------------------------
// Rmatrix66 CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
//--------------------------------------------------------------------------------------------
/**
* This function is use to calculate derivative state conversion matrix for a spacecraft state.
* It converts from Cartesian to Keplerian (with mean anomaly) coordiate system.
*
* @param obj      it is a spacecraft object
* @param state    Cartesian state of the spacecraft
*
* return          6x6 derivative state conversion matrix [dX/dK]
*/
//--------------------------------------------------------------------------------------------
Rmatrix66 EstimationStateManager::CartesianToKeplerianCoverianceConvertionMatrix(GmatBase* obj, const Rvector6 state)
{
   // 1. Get mu value 
   Spacecraft* spacecraft = (Spacecraft*)obj;
   CoordinateSystem* cs = (CoordinateSystem*)(spacecraft->GetRefObject(Gmat::COORDINATE_SYSTEM, ""));
   SolarSystem* ss = cs->GetSolarSystem();
   SpacePoint* origin = cs->GetOrigin();
   Planet* pn = (Planet*)origin;
   if (origin->IsOfType(Gmat::GROUND_STATION))
   {
      // for case CS is GDSTropocentric, the origin is ground station GDS
      std::string cbName = ((GroundstationInterface*)origin)->GetStringParameter("CentralBody");
      pn = (Planet*)(ss->GetBody(cbName));
   }

   Real mu = pn->GetRealParameter("Mu");

   // 2. Specify conversion matrix
   Rmatrix66 convMatrix = StateConversionUtil::CartesianToKeplerianDerivativeConversion(mu, state);

   return convMatrix;
}



//--------------------------------------------------------------------
//Rmatrix CartToSolveForStateConversionDerivativeMatrix()
//--------------------------------------------------------------------
/**
* This function is used to get matrix [dX/dS] which converts derivative
* in Cartesian state to Solve-for state. For Keplerian anomaly element,
* it is in form of MA.
*
* @return        matrix [dX/dS]
*/
//--------------------------------------------------------------------
Rmatrix EstimationStateManager::CartToSolveForStateConversionDerivativeMatrix()
{
   // 1.Get list of solve-for elements
   const std::vector<ListItem*>* items = GetStateMap();

   // 2. Get current estimation MJ2000Eq Cartesian state
   GmatState estCartState = GetEstimationMJ2000EqCartesianState();

   // 3.Calculate conversion matrix
   Integer size = items->size();
   Rmatrix conversion;
   conversion.SetSize(size, size);
   for (Integer i = 0; i < items->size();)
   {
      ListItem* item = items->at(i);
      if (item->elementName == "KeplerianState")
      {
         // 3.1.1. Get spacecraft object
         GmatBase* obj = item->object;

         // 3.1.2. Specify Cartesian state of the spacecraft
         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
         //MessageInterface::ShowMessage("   Use cartesian state of spacecraft <%s> to calculate [dX/dS]: [%.12lf   %.12lf   %.12lf   %.12lf   %.12lf   %.12lf\n", obj->GetName().c_str(), inState[0], inState[1], inState[2], inState[3], inState[4], inState[5]);

         // 3.1.3. Get conversion matrix [dX/dK] for the spacecraft
         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);

         // 3.1.4. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
               conversion(i + row, i + col) = coversion66(row, col);
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else if (item->elementName == "CartesianState")
      {
         // 3.1.4. Set 6x6 identity matrix to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
            {
               if (row == col)
                  conversion(i + row, i + col) = 1.0;
            }
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else
      {
         conversion(i, i) = 1.0;
         ++i;
      }
   }

   return conversion;
}


////--------------------------------------------------------------------
////Rmatrix CartToKeplConversionDerivativeMatrix()
////--------------------------------------------------------------------
///**
//* This function is used to get matrix [dX/dK] which converts derivative
//* in Cartesian state to Keplerian state. For other variable such as
//* bias, Cr_Epsilon, and Cd_Epsilon, their submatrix is identity matrix.
//* Keplerian anomaly element is MA.
//*
//* @return        matrix [dX/dK]
//*/
////--------------------------------------------------------------------
//Rmatrix EstimationStateManager::CartToKeplConversionDerivativeMatrix()
//{
//   // 1.Get list of solve-for elements
//   const std::vector<ListItem*>* items = GetStateMap();
//
//   // 2. Get current estimation MJ2000Eq Cartesian state
//   GmatState estCartState = GetEstimationMJ2000EqCartesianState();
//
//   // 3.Calculate conversion matrix
//   Integer size = items->size();
//   Rmatrix conversion;
//   conversion.SetSize(size, size);
//   for (Integer i = 0; i < items->size();)
//   {
//      ListItem* item = items->at(i);
//      if ((item->elementName == "KeplerianState") || (item->elementName == "CartesianState"))     // if solve-for is a spacecraft state
//      {
//         // 3.2.1. Get spacecraft object
//         GmatBase* obj = item->object;
//
//         // 3.1.2. Specify Cartesian state of the spacecraft
//         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//         // 3.1.3. Get conversion matrix [dX/dK] for the spacecraft. Keplerian anomaly element is MA.
//         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);
//
//         // 3.1.4. Set value to the submatrix
//         for (Integer row = 0; row < 6; ++row)
//         {
//            for (Integer col = 0; col < 6; ++col)
//               conversion(i + row, i + col) = coversion66(row, col);
//         }
//
//         // 3.1.5. Move index to the next element
//         i = i + 6;
//      }
//      else
//      {
//         conversion(i, i) = 1.0;
//         ++i;
//      }
//   }
//
//   return conversion;
//}


//--------------------------------------------------------------------
//Rmatrix SolveForStateToKeplConversionDerivativeMatrix()
//--------------------------------------------------------------------
/**
* This function is used to get matrix [dS/dK] which converts derivative
* in solve-for state to Keplerian state. For other variable such as
* bias, Cr_Epsilon, and Cd_Epsilon, their submatrix is identity matrix
*
* @return        matrix [dS/dK]
*/
//--------------------------------------------------------------------
Rmatrix EstimationStateManager::SolveForStateToKeplConversionDerivativeMatrix()
{
   // 1.Get list of solve-for elements
   const std::vector<ListItem*>* items = GetStateMap();

   // 2. Get current estimation MJ2000Eq Cartesian state
   GmatState estCartState = GetEstimationMJ2000EqCartesianState();

   // 3.Calculate conversion matrix
   Integer size = items->size();
   Rmatrix conversion;
   conversion.SetSize(size, size);
   for (Integer i = 0; i < items->size();)
   {
      ListItem* item = items->at(i);
      if (item->elementName == "KeplerianState")
      {
         // 3.1.1. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
            {
               if (row == col)
                  conversion(i + row, i + col) = 1.0;
            }
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else if (item->elementName == "CartesianState")
      {
         // 3.1.1. Get spacecraft object
         GmatBase* obj = item->object;

         // 3.1.2. Specify Cartesian state of the spacecraft
         Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);

         // 3.1.3. Get conversion matrix for the spacecraft
         Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);

         // 3.1.4. Set value to the submatrix
         for (Integer row = 0; row < 6; ++row)
         {
            for (Integer col = 0; col < 6; ++col)
               conversion(i + row, i + col) = coversion66(row, col);
         }

         // 3.1.5. Move index to the next element
         i = i + 6;
      }
      else
      {
         conversion(i, i) = 1.0;
         ++i;
      }
   }

   return conversion;
}


std::map<Integer,Integer> EstimationStateManager::GetEqualConstrainsList()
{
   // Get full names of all constrains
   std::vector<StringArray> constrainsList;
   ObjectArray sats;
   GetStateObjects(sats, Gmat::SPACECRAFT);
   for (Integer i = 0; i < sats.size(); ++i)
   {
      std::vector<StringArray> constrains = ((Spacecraft*)sats[i])->GetEqualConstrains();
      for (Integer j = 0; j < constrains.size(); ++j)
      {
         StringArray names = constrains[j];
         for (Integer k = 0; k < names.size(); ++k)
            names[k] = sats[i]->GetName() + "." + names[k];            // example: "estSat" + ". " + "P1.AreaCoefficient"
         constrainsList.push_back(names);
      }
   }

   //MessageInterface::ShowMessage("List of constraints:\n");
   //for (Integer i = 0; i < constrainsList.size(); ++i)
   //{
   //   MessageInterface::ShowMessage("%d: ", i);
   //   for (Integer j = 0; j < constrainsList[i].size(); ++j)
   //      MessageInterface::ShowMessage("<%s>  ", constrainsList[i].at(j).c_str());
   //   MessageInterface::ShowMessage("\n");
   //}

   // Get full names of solvefor variables
   StringArray fullNameList;
   for (Integer i = 0; i < stateSize; ++i)
   {
      std::stringstream ss;
      std::string elementName = stateMap[i]->elementName;
      if ((elementName == "CartesianState") || (elementName == "KeplerianState"))
      {
         // element name has to be the name of spacecraft's coordinate system following by state element name
         // Get spacecraft's coordinate system name
         ss << ((Spacecraft*)(stateMap[i]->object))->GetRefObject(Gmat::COORDINATE_SYSTEM, "")->GetName() << ".";

         if (elementName == "KeplerianState")
         {
            switch (stateMap[i]->subelement)
            {
            case 1:
               ss << "SMA";
               break;
            case 2:
               ss << "ECC";
               break;
            case 3:
               ss << "INC";
               break;
            case 4:
               ss << "RAAN";
               break;
            case 5:
               ss << "AOP";
               break;
            case 6:
               ss << "TA";
               //ss << anomalyType;
               break;
            }
         }
         else
         {
            switch (stateMap[i]->subelement)
            {
            case 1:
               ss << "X";
               break;
            case 2:
               ss << "Y";
               break;
            case 3:
               ss << "Z";
               break;
            case 4:
               ss << "VX";
               break;
            case 5:
               ss << "VY";
               break;
            case 6:
               ss << "VZ";
               break;
            }
         }

         elementName = ss.str();
      }

      std::string fullName = stateMap[i]->object->GetFullName() + "." + elementName;
      ////MessageInterface::ShowMessage("------>   %d: <%s>\n", i, fullName.c_str());
      fullNameList.push_back(fullName);
   }

   //MessageInterface::ShowMessage("Full name list:\n");
   //for (Integer i = 0; i < fullNameList.size(); ++i)
   //{
   //   MessageInterface::ShowMessage("%d: <%s>\n", i, fullNameList[i].c_str());
   //}



   // Create information matrix's indexes associated to the constrains
   Integer index = 0;
   std::map<Integer,Integer> indexMap;
   for (Integer i = 0; i < fullNameList.size(); ++i)
   {
      // Check fullNameList[i] to be in constrainsList
      std::string name = fullNameList[i];
      Integer constrRow = -1;
      Integer constrCol = -1;
      for (Integer j = 0; j < constrainsList.size(); ++j)
      {
         for (Integer k = 0; k < constrainsList[j].size(); ++k)
         {
            if (name == constrainsList[j][k])
            {
               constrRow = j;
               constrCol = k;
               break;
            }
         }
         if (constrRow >= 0)
            break;
      }


      // if it is in constrainsList
      if (constrRow >= 0)
      {
         // Check it is the first element in the constrain or not
         Integer theFirst = -1;
         for (Integer j = 0; j < constrainsList[constrRow].size(); ++j)
         {
            std::string eleName = constrainsList[constrRow][j];
            for (Integer k = 0; k < i; ++k)
            {
               if (eleName == fullNameList[indexMap[k]])
               {
                  theFirst = indexMap[k];
                  break;
               }
            }
            if (theFirst >= 0)
               break;
         }

         if (theFirst >= 0)
            indexMap[i] = theFirst;
         else
         {
            indexMap[i] = index;
            ++index;
         }
      }
      else
      {
         indexMap[i] = index;
         ++index;
      }
   }

   return indexMap;
}


//--------------------------------------------------------------------
// bool HasElementsOfType(const std::string &type)
//--------------------------------------------------------------------
/**
* CHeck if any element in the EstimationStateManager is of a specific type
*
* @param type The type to check
*
* @return True if the input type is an element, false if not
*/
//--------------------------------------------------------------------
bool EstimationStateManager::HasElementsOfType(const std::string &type)
{
   StringArray *elementList, refObjs;

   // Iterate through each element and the list of elements
   for (std::map<GmatBase*, StringArray*>::iterator i = elements.begin();
      i != elements.end(); ++i)
   {
      current = i->first;
      elementList = i->second;

      refObjs = current->GetRefObjectNameArray(GmatType::GetTypeId(type));

      for (StringArray::iterator j = elementList->begin();
         j != elementList->end(); ++j)
      {
         if ((find(refObjs.begin(), refObjs.end(), *j) != refObjs.end()))
            return true;
      }
   }

   return false;
}

StringArray EstimationStateManager::GetObjectSolveFors(std::string objName)
{
   StringArray retVal = {};
   for (int i = 0; i < solveForNames.size(); i++)
   {
      if (solveForNames[i].substr(0, solveForNames[i].find('.')) == objName)
         retVal.push_back(solveForNames[i].substr(solveForNames[i].find('.') + 1));
   }
   return retVal;
}


////--------------------------------------------------------------------
////Rmatrix SolveForStateToDisplStateTypeConversionDerivativeMatrix()
////--------------------------------------------------------------------
///**
//* This function is used to get matrix [dS/dD] which converts derivative
//* in solve-for state to the state specified in spacecraft's
//* DisplayStateType. For other variable such as bias, Cr_Epsilon, and
//* Cd_Epsilon, their submatrix is identity matrix.
//*
//* @return        matrix [dS/dD]
//*/
////--------------------------------------------------------------------
//Rmatrix EstimationStateManager::SolveForStateToDisplStateTypeConversionDerivativeMatrix()
//{
//   // 1.Get list of solve-for elements
//   const std::vector<ListItem*>* items = GetStateMap();
//
//   // 2. Get current estimation state in MJ2000Eq Cartesian coordinate system
//   GmatState estCartState = GetEstimationMJ2000EqCartesianState();
//
//   // 3.Calculate conversion matrix
//   Integer size = items->size();
//   Rmatrix conversion;
//   conversion.SetSize(size, size);
//   for (Integer i = 0; i < items->size();)
//   {
//      ListItem* item = items->at(i);
//      if (item->elementName == "KeplerianState")
//      {
//         // Case 1: solve-for state is Keplerian
//         if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Keplerian")
//         {
//            // if solve-for state and display state type are the same, coversion submatrix will be identity matrix
//            // 3.1.1.1 Set 6x6 identity matrix to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//               {
//                  if (row == col)
//                     conversion(i + row, i + col) = 1.0;
//               }
//            }
//
//            // 3.1.1.2. Move index to the next element
//            i = i + 6;
//         }
//         else if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Cartesian")
//         {
//            // if they are not the same, specify [dK/dX] matrix
//            // 3.1.2.1. Get spacecraft object
//            GmatBase* obj = item->object;
//
//            // 3.1.2.2. Specify Cartesian state of the spacecraft
//            Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//            // 3.1.2.3. Get conversion matrix [dK/dX] for the spacecraft
//            Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState).Inverse();
//
//            // 3.1.2.4. Set value to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//                  conversion(i + row, i + col) = coversion66(row, col);
//            }
//
//            // 3.1.2.5. Move index to the next element
//            i = i + 6;
//         }
//      }
//      else if (item->elementName == "CartesianState")
//      {
//         // Case 2: solve-for state is Cartesian
//         if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Cartesian")
//         {
//            // if solve-for state and display state type are the same, coversion submatrix will be identity matrix
//            // 3.2.1.1 Set 6x6 identity matrix to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//               {
//                  if (row == col)
//                     conversion(i + row, i + col) = 1.0;
//               }
//            }
//
//            // 3.2.1.2. Move index to the next element
//            i = i + 6;
//         }
//         else if (((Spacecraft*)(item->object))->GetStringParameter("DisplayStateType") == "Keplerian")
//         {
//            // 3.2.2.1. Get spacecraft object
//            GmatBase* obj = item->object;
//
//            // 3.2.2.2. Specify Cartesian state of the spacecraft
//            Rvector6 inState(estCartState[i], estCartState[i + 1], estCartState[i + 2], estCartState[i + 3], estCartState[i + 4], estCartState[i + 5]);
//
//            // 3.2.2.3. Get conversion matrix [dX/dK] for the spacecraft
//            Rmatrix66 coversion66 = CartesianToKeplerianCoverianceConvertionMatrix(obj, inState);
//
//            // 3.2.2.4. Set value to the submatrix
//            for (Integer row = 0; row < 6; ++row)
//            {
//               for (Integer col = 0; col < 6; ++col)
//                  conversion(i + row, i + col) = coversion66(row, col);
//            }
//
//            // 3.2.2.5. Move index to the next element
//            i = i + 6;
//         }
//      }
//      else
//      {
//         conversion(i, i) = 1.0;
//         ++i;
//      }
//   }
//
//   return conversion;
//}


