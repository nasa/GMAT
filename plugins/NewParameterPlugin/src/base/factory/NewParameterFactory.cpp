//$Id: NewParameterFactory.cpp 1397 2011-04-21 19:04:45Z  $
//------------------------------------------------------------------------------
//                            NewParameterFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/29
//
/**
 *  Implementation code for the NewParameterFactory class, responsible for creating
 *  the HSL VF13ad optimizer.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "NewParameterFactory.hpp"
#include "MessageInterface.hpp"
#include "ParameterInfo.hpp"

#include "FMDensity.hpp"
#include "FMAcceleration.hpp"
#include "SolverStatus.hpp"
#include "SolverState.hpp"
#include "Torque.hpp"


//#define DEBUG_CREATION


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Solver class. 
 *
 * @param <ofType> type of Solver object to create and return.
 * @param <withName> the name for the newly-created Solver object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
GmatBase* NewParameterFactory::CreateObject(const std::string &ofType,
                                            const std::string &withName)
{
   return CreateParameter(ofType, withName);
}

//------------------------------------------------------------------------------
//  Parameter* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class. 
 *
 * @param <ofType> type of Parameter object to create and return.
 * @param <withName> the name for the newly-created Parameter object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Parameter* NewParameterFactory::CreateParameter(const std::string &ofType,
                                    const std::string &withName)
{
   Parameter *retval = NULL;
   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("Attempting to create a \"%s\" "
            "Parameter...", ofType.c_str());
   #endif

   if (ofType == "Acceleration")
      retval = new FMAcceleration(withName);
   if (ofType == "AccelerationX")
      retval = new FMAccelerationX(withName);
   if (ofType == "AccelerationY")
      retval = new FMAccelerationY(withName);
   if (ofType == "AccelerationZ")
      retval = new FMAccelerationZ(withName);
   if (ofType == "AtmosDensity")
      retval = new FMDensity(withName);
   if (ofType == "SolverStatus")
      retval = new SolverStatus(withName);
   if (ofType == "SolverState")
      retval = new SolverState(withName);
   if (ofType == "BurnTorque")
      retval = new BurnTorque(withName);
   if (ofType == "GravityTorque")
      retval = new GravityTorque(withName);
   if (ofType == "SRPTorque")
      retval = new SRPTorque(withName);
   if (ofType == "TotalTorque")
      retval = new TotalTorque(withName);

   #ifdef DEBUG_CREATION
      MessageInterface::ShowMessage("output pointer is <%p>\n", retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
//  NewParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NewParameterFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
NewParameterFactory::NewParameterFactory() :
    Factory                (Gmat::PARAMETER),
    registrationComplete   (false)
{
   if (creatables.empty())
   {
      creatables.push_back("Acceleration");
      creatables.push_back("AccelerationX");
      creatables.push_back("AccelerationY");
      creatables.push_back("AccelerationZ");
      creatables.push_back("AtmosDensity");
      creatables.push_back("SolverStatus");
      creatables.push_back("SolverState");
      creatables.push_back("BurnTorque");
      creatables.push_back("GravityTorque");
      creatables.push_back("SRPTorque");
      creatables.push_back("TotalTorque");
   }
}

//------------------------------------------------------------------------------
//  NewParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NewParameterFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
NewParameterFactory::NewParameterFactory(StringArray createList) :
   Factory                (createList, Gmat::PARAMETER),
   registrationComplete   (false)
{
}


//------------------------------------------------------------------------------
//  NewParameterFactory(const NewParameterFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class NewParameterFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
NewParameterFactory::NewParameterFactory(const NewParameterFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Acceleration");
      creatables.push_back("AccelerationX");
      creatables.push_back("AccelerationY");
      creatables.push_back("AccelerationZ");
      creatables.push_back("AtmosDensity");
      creatables.push_back("SolverStatus");
      creatables.push_back("SolverState");
      creatables.push_back("BurnTorque");
      creatables.push_back("GravityTorque");
      creatables.push_back("SRPTorque");
      creatables.push_back("TotalTorque");
      registrationComplete = false;
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * NewParameterFactory operator for the NewParameterFactory base class.
 *
 * @param <fact> the NewParameterFactory object that is copied.
 *
 * @return "this" NewParameterFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
NewParameterFactory& NewParameterFactory::operator=(const NewParameterFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~NewParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the NewParameterFactory base class.
 */
//------------------------------------------------------------------------------
NewParameterFactory::~NewParameterFactory()
{
}


//------------------------------------------------------------------------------
// bool DoesObjectTypeMatchSubtype(const std::string &theType,
//                                 const std::string &theSubtype)
//------------------------------------------------------------------------------
/**
 * Checks if a creatable solver type matches a subtype.
 *
 * @param theType The script identifier for the object type
 * @param theSubtype The subtype being checked
 *
 * @return true if the scripted type and subtype match, false if the type does
 *         not match the subtype
 */
//------------------------------------------------------------------------------
bool NewParameterFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
      const std::string &theSubtype)
{
   bool retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetListOfCreatableObjects(const std::string &qualifier)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of Parameters that this factory can create.
 *
 * This method returns the list, and registers the new Parameters in the
 * ParameterInfo database if tehy were not previously registered.
 *
 * @param qualifier Subtype qualifier (not used in this Factory)
 *
 * @return The list of creatable Parameters
 */
//------------------------------------------------------------------------------
StringArray NewParameterFactory::GetListOfCreatableObjects(
      const std::string &qualifier)
{
   // A hack to register the new parameters with the ParameterInfo database
   // prior to the usage of the Parameter in a run.  This piece is needed in
   // order for the Parameter to appear on the GUI before one has been created
   // for use, because the ParameterInfo data is filled from the parameter's
   // constructor.
   //
   /// @todo Parameter registration via construction is klunky and should be
   /// fixed
   if (!registrationComplete)
   {
      Parameter *param = CreateParameter("Acceleration",
            "DefaultSC.DefaultFM.Acceleration");
      delete param;

      param = CreateParameter("AccelerationX",
            "DefaultSC.DefaultFM.AccelerationX");
      delete param;

      param = CreateParameter("AccelerationY",
            "DefaultSC.DefaultFM.AccelerationY");
      delete param;

      param = CreateParameter("AccelerationZ",
            "DefaultSC.DefaultFM.AccelerationZ");
      delete param;

      param = CreateParameter("AtmosDensity",
            "DefaultSC.DefaultFM.AtmosDensity");
      delete param;

      param = CreateParameter("SolverStatus",
            "DefaultSolver.SolverStatus");
      delete param;

      param = CreateParameter("SolverState",
            "DefaultSolver.SolverState");
      delete param;

      param = CreateParameter("BurnTorque",
         "DefaultSC.DefaultFM.BurnTorque");
      delete param;

      param = CreateParameter("GravityTorque",
         "DefaultSC.DefaultFM.GravityTorque");
      delete param;

      param = CreateParameter("SRPTorque",
         "DefaultSC.DefaultFM.SRPTorque");
      delete param;

      param = CreateParameter("TotalTorque",
         "DefaultSC.DefaultFM.TotalTorque");
      delete param;

//      // Here's how I think we might do this -- but this doesn't work:
//      // register parameter names with info
//      ParameterInfo *pInfo = ParameterInfo::Instance();
//      pInfo->Add("AtmosDensity", Gmat::SPACECRAFT, Gmat::ORIGIN,
//            "DefaultSC.AtmosDensity", GmatParam::ATTACHED_OBJ, true, true,
//            false, false, "Atmospheric Density");

      registrationComplete = true;
   }

   return Factory::GetListOfCreatableObjects(qualifier);
}
