//------------------------------------------------------------------------------
//                            ThrustSegmentParameterFactory
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
 *  Implementation code for the ThrustSegmentParameterFactory class, responsible 
 *  for creating the thrust segment parameters.
 */
 //------------------------------------------------------------------------------

#include "ThrustFileDefs.hpp"
#include "gmatdefs.hpp"
#include "ThrustSegmentParameterFactory.hpp"
#include "MessageInterface.hpp"
#include "ParameterInfo.hpp"
#include "ThrustSegmentParameters.hpp"



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
GmatBase* ThrustSegmentParameterFactory::CreateObject(const std::string &ofType,
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
Parameter* ThrustSegmentParameterFactory::CreateParameter(const std::string &ofType,
   const std::string &withName)
{
   Parameter *retval = NULL;
#ifdef DEBUG_CREATION
   MessageInterface::ShowMessage("Attempting to create a \"%s\" "
      "Parameter...", ofType.c_str());
#endif
   //NOTE: Many of the parameters here have been disabled due to a bug with duplicate names. 
   //The undelying code for these have been left in and once a solution is developed, these can be uncommented in this file to allow for normal use.

   if (ofType == "ThrustScaleFactor")
      retval = new ThrustSegmentScaleFactor(withName);
   //if (ofType == "ThrustScaleFactorSigma")
      //retval = new TSFSigma(withName);
   //if (ofType == "ApplyThrustScaleToMassFlow")
      //retval = new FMAccelerationY(withName);
   //if (ofType == "MassFlowScaleFactor")
      //retval = new MassFlowScaleFactor(withName);
   //if (ofType == "MassSource")
      //retval = new FMDensity(withName);
   //if (ofType == "ThrustAngleConstraintVector")
      //retval = new ThrustAngleConstraint(withName);
   if (ofType == "ThrustAngle1")
      retval = new ThrustAngle1(withName);
   if (ofType == "ThrustAngle2")
      retval = new ThrustAngle2(withName);
   //if (ofType == "ThrustAngle1Sigma")
      //retval = new ThrustAngle1Sigma(withName);
  // if (ofType == "ThrustAngle2Sigma")
      //retval = new ThrustAngle2Sigma(withName);
   //if (ofType == "TSF_Epsilon")
      //retval = new TSFEpsilon(withName);
   //if (ofType == "StartEpoch")
      //retval = new StartEpoch(withName);
   //if (ofType == "EndEpoch")
      //retval = new EndEpoch(withName);

#ifdef DEBUG_CREATION
   MessageInterface::ShowMessage("output pointer is <%p>\n", retval);
#endif

   return retval;
}


//------------------------------------------------------------------------------
//  ThrustSegmentParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ThrustSegmentParameterFactory.
 * (default constructor)
 */
 //------------------------------------------------------------------------------
ThrustSegmentParameterFactory::ThrustSegmentParameterFactory() :
   Factory(Gmat::PARAMETER),
   registrationComplete(false)
{
   if (creatables.empty())
   {
      creatables.push_back("ThrustScaleFactor");
      //creatables.push_back("ThrustScaleFactorSigma");
      //creatables.push_back("ApplyThrustScaleToMassFlow");
      //creatables.push_back("MassFlowScaleFactor");
      //creatables.push_back("MassSource");
      //creatables.push_back("ThrustAngleConstraintVector");
      creatables.push_back("ThrustAngle1");
      creatables.push_back("ThrustAngle2");
      //creatables.push_back("ThrustAngle1Sigma");
      //creatables.push_back("ThrustAngle2Sigma");
      //creatables.push_back("TSF_Epsilon");
      //creatables.push_back("StartEpoch");
      //creatables.push_back("EndEpoch");
   }
}

//------------------------------------------------------------------------------
//  ThrustSegmentParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ThrustSegmentParameterFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
 //------------------------------------------------------------------------------
ThrustSegmentParameterFactory::ThrustSegmentParameterFactory(StringArray createList) :
   Factory(createList, Gmat::PARAMETER),
   registrationComplete(false)
{
}


//------------------------------------------------------------------------------
//  ThrustSegmentParameterFactory(const ThrustSegmentParameterFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ThrustSegmentParameterFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
 //------------------------------------------------------------------------------
ThrustSegmentParameterFactory::ThrustSegmentParameterFactory(const ThrustSegmentParameterFactory& fact) :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ThrustScaleFactor");
      //creatables.push_back("ThrustScaleFactorSigma");
      //creatables.push_back("ApplyThrustScaleToMassFlow");
      //creatables.push_back("MassFlowScaleFactor");
      //creatables.push_back("MassSource");
      //creatables.push_back("ThrustAngleConstraintVector");
      creatables.push_back("ThrustAngle1");
      creatables.push_back("ThrustAngle2");
      //creatables.push_back("ThrustAngle1Sigma");
      //creatables.push_back("ThrustAngle2Sigma");
      //creatables.push_back("TSF_Epsilon");
      //creatables.push_back("StartEpoch");
      //creatables.push_back("EndEpoch");
      registrationComplete = false;
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ThrustSegmentParameterFactory operator for the ThrustSegmentParameterFactory base class.
 *
 * @param <fact> the ThrustSegmentParameterFactory object that is copied.
 *
 * @return "this" ThrustSegmentParameterFactory with data set to match the input factory (fact).
 */
 //------------------------------------------------------------------------------
ThrustSegmentParameterFactory& ThrustSegmentParameterFactory::operator=(const ThrustSegmentParameterFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~ThrustSegmentParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ThrustSegmentParameterFactory base class.
 */
 //------------------------------------------------------------------------------
ThrustSegmentParameterFactory::~ThrustSegmentParameterFactory()
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
bool ThrustSegmentParameterFactory::DoesObjectTypeMatchSubtype(const std::string &theType,
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
StringArray ThrustSegmentParameterFactory::GetListOfCreatableObjects(
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
      Parameter *param = CreateParameter("ThrustScaleFactor",
         "DefaultTS.ThrustScaleFactor");
      delete param;

      //param = CreateParameter("ThrustScaleFactorSigma",
         //"DefaultTS.ThrustScaleFactorSigma");
      //delete param;

      //param = CreateParameter("ApplyThrustScaleToMassFlow",
         //"DefaultTS.ApplyThrustScaleToMassFlow");
      //delete param;

      //param = CreateParameter("MassFlowScaleFactor",
         //"DefaultTS.MassFlowScaleFactor");
      //delete param;

      //param = CreateParameter("MassSource",
         //"DefaultTS.MassSource");
      //delete param;

      //param = CreateParameter("ThrustAngleConstraintVector",
         //"DefaultTS.ThrustAngleConstraintVector");
      //delete param;

      param = CreateParameter("ThrustAngle1",
         "DefaultTS.ThrustAngle1");
      delete param;

      param = CreateParameter("ThrustAngle2",
         "DefaultTS.ThrustAngle2");
      delete param;

      //param = CreateParameter("ThrustAngle1Sigma",
         //"DefaultTS.ThrustAngle1Sigma");
      //delete param;

      //param = CreateParameter("ThrustAngle2Sigma",
         //"DefaultTS.ThrustAngle2Sigma");
      //delete param;

      //param = CreateParameter("TSF_Epsilon",
         //"DefaultTS.TSF_Epsilon");
      //delete param;
      
      //param = CreateParameter("StartEpoch",
         //"DefaultTS.StartEpoch");
      //delete param;
      
      //param = CreateParameter("EndEpoch",
         //"DefaultTS.EndEpoch");
      //delete param;

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
