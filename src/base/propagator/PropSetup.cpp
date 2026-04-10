//$Id$
//------------------------------------------------------------------------------
//                              PropSetup
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
// Author: Linda Jun
// Created: 2003/10/15
//
/**
 * Defines propagator setup operations.
 */
//------------------------------------------------------------------------------

#include "PropSetup.hpp"

#include "ODEModel.hpp"
#include "Propagator.hpp"

#include "PropSetupException.hpp"
#include "PhysicalModel.hpp"
#include "RungeKutta89.hpp"
#include "PointMassForce.hpp"
#include "StringUtil.hpp"               // for GmatStringUtil::Replace()
#include "MessageInterface.hpp"

//#define DEBUG_PROPSETUP
//#define DEBUG_PROPSETUP_SET
//#define DEBUG_PROPSETUP_GET
//#define DEBUG_PROPSETUP_CLONE
//#define DEBUG_PROPSETUP_DELETE
//#define DEBUG_PROPSETUP_GEN_STRING
//#define DEBUG_FUNCTION_CREATION
//#define DEBUG_CLONES
//#define DEBUG_RENAME


//#ifndef DEBUG_MEMORY 
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/**
 * @note
 * Since we set some Propagator's property through PropSetup, such as
 * 'Propagator.InitialStepSize', properties owned by owning objects were
 * added here so that Validator can create corresponding element wrappers 
 * without going through owning object's property list to make Validator
 * job easy. The Validator will simply call GetParameterID() of PropSetup
 * to find out valid property or not. (LOJ: 2008.06.12)
 */

const std::string
PropSetup::PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount] =

{
   "FM",
   "Type",
   "InitialStepSize",
   "Accuracy",
   "ErrorThreshold",
   "SmallestInterval",
   "MinStep",
   "MaxStep",
   "MaxStepAttempts",
   "LowerError",
   "TargetError",
   "StopIfAccuracyIsViolated",
   "StepSize",
   "CentralBody",
   "EpochFormat",
   "StartEpoch",
   "MinimumReduction",
   "MaximumReduction",
   "MinimumTolerance"
};


const Gmat::ParameterType
PropSetup::PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,  // "FM"
   Gmat::OBJECT_TYPE,  // "Type"
   Gmat::REAL_TYPE,    // "InitialStepSize",
   Gmat::REAL_TYPE,    // "Accuracy",
   Gmat::REAL_TYPE,    // "ErrorThreshold",
   Gmat::REAL_TYPE,    // "SmallestInterval",
   Gmat::REAL_TYPE,    // "MinStep",
   Gmat::REAL_TYPE,    // "MaxStep",
   Gmat::INTEGER_TYPE, // "MaxStepAttempts",
   Gmat::REAL_TYPE,    // "LowerError",
   Gmat::REAL_TYPE,    // "TargetError",
   Gmat::BOOLEAN_TYPE, // "StopIfAccuracyIsViolated"
   Gmat::REAL_TYPE,    // "StepSize",
   Gmat::OBJECT_TYPE,  // "CentralBody",
   Gmat::STRING_TYPE,  // "EpochFormat",
   Gmat::STRING_TYPE,  // "StartEpoch",
   Gmat::REAL_TYPE,    // "MinimumReduction",
   Gmat::REAL_TYPE,    // "MaximumReduction",
   Gmat::REAL_TYPE,    // "MinimumTolerance"
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
PropSetup::PropSetup(const std::string &name)
   : GmatBase(Gmat::PROP_SETUP, "PropSetup", name),
   mInitialized                (false),
   mMcsCreated                 (false),
   includeODEModelInGenString  (true),
   mPropagatorName             ("InternalPropagator"),
   mODEModelName               ("InternalODEModel"),
   mPropagator                 (NULL),
   mODEModel                   (NULL)
{
   // GmatBase data
   objectTypes.push_back(Gmat::PROP_SETUP);
   objectTypeNames.push_back("PropSetup");
   
   parameterCount = PropSetupParamCount;

   // Allow PropSetup1 = PropSetup 2 assignment in command mode
   blockCommandModeAssignment = false;

   // Propagator is named or unnamed owned object which means that Propagator is not
   // created by Create command but by handling owned object in the Interpreter.
   ownedObjectCount += 1;
   
   //mInitialized = false;
   //mMcsCreated = false;
   //includeODEModelInGenString = true;
   
   // Name it Internal* so that they can be deleted when new Propagator or ODEModel
   // is set. These names are not actual names but tells whether they can be deleted or not.
   // When Propagator or ForceModes is cloned these names are set to "" so that they
   // can be deleted.
   //mPropagatorName = "InternalPropagator";
   //mODEModelName = "InternalODEModel";
   
   // Create default Integrator and ODEModel
   mPropagator = new RungeKutta89("RungeKutta89");
   mODEModel = new ODEModel(mODEModelName);
   PhysicalModel *pmf = new PointMassForce;
   
   #ifdef DEBUG_PROPSETUP   
   MessageInterface::ShowMessage
      ("PropSetup::PropSetup() adding <%p><PointMassForce> to  "
       "<%p><ODEModel>'InternalODEModel\n", pmf, mODEModel);
   #endif
   
   mODEModel->AddForce(pmf);
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (mPropagator, "RungeKutta89", "PropSetup::PropSetup()",
       "mPropagator = new RungeKutta89()", this);
   MemoryTracker::Instance()->Add
      (mODEModel, mODEModelName, "PropSetup::PropSetup()",
       "mODEModel = new ODEModel()", this);
   MemoryTracker::Instance()->Add
      (pmf, "Earth", "PropSetup::PropSetup()", "*pmf = new PointMassForce", this);
   #endif
}


//------------------------------------------------------------------------------
// PropSetup(const PropSetup &ps)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
PropSetup::PropSetup(const PropSetup &ps) : 
   GmatBase(ps),
   mInitialized                 (false),
   psm                          (ps.psm),
   mMcsCreated                  (ps.mMcsCreated),
   includeODEModelInGenString   (ps.includeODEModelInGenString),
   mPropagatorName              (ps.mPropagatorName),
   mODEModelName                (""),
   mPropagator                  (NULL),
   mODEModel                    (NULL)
{
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("PropSetup::PropSetup(copy) <%p>'%s' entered, ps.Propagator=<%p>, ps.ODEModel=<%p>\n",
       this, GetName().c_str(), ps.mPropagator, ps.mODEModel);
   #endif
   
   ownedObjectCount = ps.ownedObjectCount;
   
   // PropSetup data
   //mInitialized = false;
   //mMcsCreated = ps.mMcsCreated;
   //includeODEModelInGenString = ps.includeODEModelInGenString;
   //mPropagatorName = ps.mPropagatorName;
   //mODEModelName = "";
   //mPropagator = NULL;
   //mODEModel = NULL;
   
   if (ps.mPropagator != NULL)
      mPropagatorName = ps.mPropagator->GetName();
   if (ps.mODEModel != NULL)
      mODEModelName = ps.mODEModel->GetName();
   
   // In instructor function, mPropagator and mODEModel are not created yet so we do not delete them.
   //// first delete old propagator and ODEModel (loj: 2008.11.04)
   //DeleteOwnedObject(PROPAGATOR);
   //DeleteOwnedObject(ODE_MODEL);
   ClonePropagator(ps.mPropagator);
   CloneODEModel(ps.mODEModel);
   
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("PropSetup::PropSetup(copy) exiting\n   Propagator=<%p><%s> '%s'\n   "
       "   ODEModel=<%p><%s> '%s'\n", mPropagator,
       mPropagator ? mPropagator->GetTypeName().c_str() : "NULL",
       mPropagator ? mPropagator->GetName().c_str() : "NULL", mODEModel,
       mODEModel ? mODEModel->GetTypeName().c_str() : "NULL",
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
}

//------------------------------------------------------------------------------
// PropSetup& operator= (const PropSetup &ps)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
PropSetup& PropSetup::operator= (const PropSetup &ps)
{
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("PropSetup::operator=() <%p>'%s' entered, Propagator=<%p>, ODEModel=<%p>\n",
       this, GetName().c_str(), mPropagator, mODEModel);
   MessageInterface::ShowMessage
      ("   ps.Propagator=<%p>, ps.ODEModel=<%p>\n", ps.mPropagator, ps.mODEModel);
   #endif
   
   if (this == &ps)
      return *this;
   
   GmatBase::operator=(ps);
   
   ownedObjectCount = ps.ownedObjectCount;
   
   // PropSetup data
   mInitialized               = false;
   mMcsCreated                = ps.mMcsCreated;
   includeODEModelInGenString = ps.includeODEModelInGenString;
   mPropagatorName            = ps.mPropagatorName;
   
   psm                        = ps.psm;
   
   if (ps.mPropagator != NULL)
      mPropagatorName = ps.mPropagator->GetName();

   mODEModelName = "";
   if (ps.mODEModel != NULL)
      mODEModelName = ps.mODEModel->GetName();
   
   // first delete old propagator and ODEModel
   DeleteOwnedObject(PROPAGATOR, true);
   DeleteOwnedObject(ODE_MODEL, true);
   ClonePropagator(ps.mPropagator);
   CloneODEModel(ps.mODEModel);
   
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("PropSetup::operator=() exiting, Propagator=<%p><%s> '%s'\n   "
       "ODEModel=<%p><%s> '%s'\n", mPropagator,
       mPropagator ? mPropagator->GetTypeName().c_str() : "NULL",
       mPropagator ? mPropagator->GetName().c_str() : "NULL", mODEModel,
       mODEModel ? mODEModel->GetTypeName().c_str() : "NULL",
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
   
   return *this;
}

//------------------------------------------------------------------------------
// virtual ~PropSetup()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PropSetup::~PropSetup()
{
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("PropSetup::~PropSetup() entered, Propagator=<%p>, ODEModel=<%p>\n",
       mPropagator, mODEModel);
   #endif
   
   DeleteOwnedObject(PROPAGATOR, true);
   DeleteOwnedObject(ODE_MODEL, true);
   
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage("PropSetup::~PropSetup() exiting\n");
   #endif
}

//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * @return true if pointers of Propagator and ODEModel are not NULL and
 *    there is at least one Force in the ODEModel; false otherwise.
 */
//------------------------------------------------------------------------------
bool PropSetup::IsInitialized()
{
   return mInitialized;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator()
//------------------------------------------------------------------------------
/**
 *@return internal Propagator pointer
 */
//------------------------------------------------------------------------------
Propagator* PropSetup::GetPropagator()
{
   #ifdef DEBUG_PROPSETUP_GET
   MessageInterface::ShowMessage
      ("PropSetup::GetPropagator() mPropagator=<%p>, name='%s'\n",
       mPropagator, (mPropagator == NULL ? "N/A" :
       mPropagator->GetName().c_str()));
   #endif
   
   return mPropagator;
}

//------------------------------------------------------------------------------
// ODEModel* GetODEModel()
//------------------------------------------------------------------------------
/**
 *@return internal ODEModel pointer
 */
//------------------------------------------------------------------------------
ODEModel* PropSetup::GetODEModel()
{
   #ifdef DEBUG_PROPSETUP_ODEMODEL
   MessageInterface::ShowMessage
      ("PropSetup::GetODEModel() returning <%p>'%s'\n", mODEModel,
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
   return mODEModel;
}

//------------------------------------------------------------------------------
// PropagationStateManager* GetPropStateManager()
//------------------------------------------------------------------------------
/**
 *@return The PropagationStateManager for this PropSetup
 */
//------------------------------------------------------------------------------
PropagationStateManager* PropSetup::GetPropStateManager()
{
   return &psm;
}

//------------------------------------------------------------------------------
// void SetPropagator(Propagator *propagator)
//------------------------------------------------------------------------------
/**
 * Sets internal propagator pointer to given propagator.
 *
 *@param <*propagator> propagator pointer to set internal propagator to
 */
//------------------------------------------------------------------------------
void PropSetup::SetPropagator(Propagator *propagator, bool fromGUI)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetPropagator() this=<%p> '%s' entered, mPropagator=<%p>, "
       "propagator=<%p>\n", this, GetName().c_str(), mPropagator, propagator);
   MessageInterface::ShowMessage("   Prop name is '%s', PropSetup %s\n",
         mPropagatorName.c_str(), (mMcsCreated ? "was MCS Created" :
         "was not MCS Created"));
   #endif

   if (!fromGUI)
   {
      if ((mPropagatorName != "InternalPropagator") && !mMcsCreated)
      {
         if (propagator != NULL)
         {
            if (propagator->GetTypeName() != mPropagator->GetTypeName())
               throw PropSetupException("You cannot change the owned Integrator "
                     "or Analytic Propagator after setting it once");
         }
      }
   }

   if (propagator == NULL)
      throw PropSetupException("SetPropagator() failed: propagator is NULL");
   
   DeleteOwnedObject(PROPAGATOR, true);
//   mPropagator = propagator;

   // Not cloned here; the propagator is owned by the PropSetup, and doesn't
   // get configured -- need to track this re: memory management
   ClonePropagator(propagator);

   if (mPropagator->UsesODEModel() == false)
      DeleteOwnedObject(ODE_MODEL, true);
   
   // Check if this propagator has error control, warn the user if not
   if (mPropagator->IsOfType("Integrator"))
   {
      if (!mPropagator->UsesErrorControl() && fromGUI)
      {
         MessageInterface::ShowMessage("**** WARNING **** " +
            mPropagator->GetTypeName() + " is an integrator with no "
            "error control. The accuracy setting will be ignored.\n");
      }
   }

   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetPropagator() this=<%p> '%s' leaving, mPropagator=<%p>, "
       "propagator=<%p>\n", this, GetName().c_str(), mPropagator, propagator);
   #endif
}

//------------------------------------------------------------------------------
// void SetODEModel(ODEModel *ODEModel)
//------------------------------------------------------------------------------
/**
 * Sets internal force model pointer to given force model.
 *
 * @param odeModel ODEModel pointer to set internal force model to
 */
//------------------------------------------------------------------------------
void PropSetup::SetODEModel(ODEModel *odeModel)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetODEModel() this=<%p> '%s' entered, mODEModel=<%p>'%s', "
       "odeModel=<%p>'%s'\n", this, GetName().c_str(), mODEModel, mODEModel->GetName().c_str(),
       odeModel, odeModel->GetName().c_str());
   #endif
   
   DeleteOwnedObject(ODE_MODEL, true);
   if (mPropagator->UsesODEModel())
   {
      CloneODEModel(odeModel);      // Makes clone or sets pointer to NULL
      mODEModelName = odeModel->GetName();
   }
   
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetODEModel() exiting, mODEModel=<%p>\n", mODEModel);
   #endif
}


//------------------------------------------------------------------------------
// void AddForce(PhysicalModel *force)
//------------------------------------------------------------------------------
/**
 * Adds a force to force model.
 */
//------------------------------------------------------------------------------
void PropSetup::AddForce(PhysicalModel *force)
{
   mODEModel->AddForce(force);
}


//------------------------------------------------------------------------------
// PhysicalModel* GetForce(Integer index)
//------------------------------------------------------------------------------
/**
 * @return Force pointer give by index, NULL if invalid index
 */
//------------------------------------------------------------------------------
PhysicalModel* PropSetup::GetForce(Integer index)
{
   return mODEModel->GetForce(index);      
}


//------------------------------------------------------------------------------
// Integer GetNumForces()
//------------------------------------------------------------------------------
/**
 * @return number of forces in the force model
 */
//------------------------------------------------------------------------------
Integer PropSetup::GetNumForces()
{
   return mODEModel->GetNumForces();
}


//------------------------------------------------------------------------------
// void AddPropObject(GmatBase* sat)
//------------------------------------------------------------------------------
/**
 * Add an object capable of being propagated to the propagation system.
 *
 * @param sat The object to add to the propagation system
 */
 //------------------------------------------------------------------------------
void PropSetup::AddPropObject(GmatBase* sat)
{
   psm.SetObject(sat);
}

//------------------------------------------------------------------------------
// void RemovePropObject(GmatBase* sat)
//------------------------------------------------------------------------------
/**
 * Remove an object from the propagation system.
 *
 * API users call this method to remove a spacecraft from propagation
 *
 * @param sat The object to remove from the propagation system
 *
 * @retval True is sat was removed, false if not
 */
//------------------------------------------------------------------------------
bool PropSetup::RemovePropObject(GmatBase* sat)
{
   return psm.RemoveObject(sat);
}

//------------------------------------------------------------------------------
// void PrepareInternals()
//------------------------------------------------------------------------------
/**
 * Prepares the internal objects: the PropagationStateManager, Propagator
 * and ODEModel associated with the PropSetup.
 */
 //------------------------------------------------------------------------------
void PropSetup::PrepareInternals()
{
   // Use precision time setting from ODE model if it exists
   if (mODEModel)
   {
      psm.GetState()->SetPrecisionTimeFlag(mODEModel->HasPrecisionTime());
      psm.BuildState();

      // Pass the state manager to the dynamics model
      mODEModel->SetPropStateManager(&psm);
      mODEModel->SetState(psm.GetState());

      // Assemble all of the force model objects together
      mODEModel->Initialize();

      // Finish the force model setup
      mODEModel->BuildModelFromMap();
      mODEModel->UpdateInitialData();
   }
   else
   {
      psm.BuildState();
   }

   // Initialize the Propagator components
   Initialize();
   mPropagator->Initialize();
}

//------------------------------------------------------------------------------
// const std::string* GetParameterList() const
//------------------------------------------------------------------------------
/**
 * @return pointer to parameter name list
 */
//------------------------------------------------------------------------------
const std::string* PropSetup::GetParameterList() const
{
   return PARAMETER_TEXT;
}


//------------------------------------------------------------------------------
// Integer GetParameterCount(void) const
//------------------------------------------------------------------------------
Integer PropSetup::GetParameterCount(void) const
{
   Integer count = parameterCount;
   return count;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference objects used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool PropSetup::RenameRefObject(const UnsignedInt type,
                                const std::string &oldName,
                                const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("PropSetup::RenameRefObject() entered, this=<%p>'%s', type=%d, "
       "oldName='%s', newName='%s'\n", this, GetName().c_str(), type,
       oldName.c_str(), newName.c_str());
   MessageInterface::ShowMessage
      ("   mODEModelName='%s', mODEModel=<%p><%s>'%s'\n", mODEModelName.c_str(),
       mODEModel, mODEModel ? mODEModel->GetTypeName().c_str() : "NULL",
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
   
   // Rename ODE model name
   if (mODEModelName.find(oldName) != mODEModelName.npos)
   {
      mODEModelName = GmatStringUtil::Replace(mODEModelName, oldName, newName);
   }
   
   // Rename actual ODE model name of ODEModel pointer
   if (mODEModel)
   {
      std::string modelName = mODEModel->GetName();
      if (modelName.find(oldName) != modelName.npos)
      {
         modelName = GmatStringUtil::Replace(modelName, oldName, newName);
         mODEModel->SetName(modelName);
      }
   }
   
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("PropSetup::RenameRefObject() returning true, mODEMoedlName='%s', "
       "mODEModel=<%p><%s>'%s'\n", mODEModelName.c_str(), mODEModel,
       mODEModel ? mODEModel->GetTypeName().c_str() : "NULL",
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
   
   return true;
}


//---------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                           const std::string &name = "");
//---------------------------------------------------------------------------
/*
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool PropSetup::SetRefObject(GmatBase *obj, const UnsignedInt type,
                             const std::string &name)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetRefObject() entered, obj=<%p><%s> '%s', type=%d, name='%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   switch (type)
   {
   case Gmat::PROPAGATOR:
      SetPropagator((Propagator*)obj);
      return true;
   case Gmat::ODE_MODEL:
      SetODEModel((ODEModel*)obj);
      {  // Manage case statement scoping issues
         std::string refName = obj->GetName();
         if ((refName != "") && (refName != "InternalODEModel"))
            mODEModelName = refName;
      }
      return true;
   default:
      return false;
   }
}


//------------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This method returns the unnamed objects owned by the PropSetup.
 *
 * The current implementation only contains one PropSetup owned object: the 
 * Propagator.
 * 
 * @return Pointer to the owned object.
 */
//------------------------------------------------------------------------------
GmatBase* PropSetup::GetOwnedObject(Integer whichOne)
{
   // Propagator is named or unnamed owned object(loj: 2008.11.05)
   if (whichOne == ownedObjectCount - 1)   // If this works, make it more usable
      return mPropagator;
   return GmatBase::GetOwnedObject(whichOne);
}


//---------------------------------------------------------------------------
// bool IsOwnedObject(Integer id) const
//---------------------------------------------------------------------------
bool PropSetup::IsOwnedObject(Integer id) const
{
   if (id == PROPAGATOR || id == ODE_MODEL)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PropSetup.
 *
 * @return clone of the PropSetup.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PropSetup::Clone(void) const
{
   return (new PropSetup(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void PropSetup::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((PropSetup *)(orig)));
   instanceName = name;
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool PropSetup::HasRefObjectTypeArray()
{
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& PropSetup::GetRefObjectTypeArray()
{
   // We need to add in the property order since Interpreter querrys for
   // object type using property id
   refObjectTypes.clear();
   
   // Fill with UNKNOWN_OBJECT since GmatBase no longer has 0 parameters (LOJ: 2009.11.03)
   for (Integer i = 0; i < GmatBaseParamCount; i++)
      refObjectTypes.push_back(Gmat::UNKNOWN_OBJECT);
   
   refObjectTypes.push_back(Gmat::ODE_MODEL);
   refObjectTypes.push_back(Gmat::PROPAGATOR);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the member forces.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& PropSetup::GetRefObjectNameArray(const UnsignedInt type)
{
   refObjectNames.clear();
   if (mPropagatorName != "" && mPropagatorName != "InternalPropagator")
      if (type == Gmat::PROPAGATOR || type == Gmat::UNKNOWN_OBJECT)
         refObjectNames.push_back(mPropagatorName);
   if (mODEModelName != "" && mODEModelName != "InternalODEModel" &&
       mODEModelName != "InternalForceModel")
      if (type == Gmat::ODE_MODEL || type == Gmat::UNKNOWN_OBJECT)
         refObjectNames.push_back(mODEModelName);
   return refObjectNames;
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PropSetup::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PropSetup::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<PropSetupParamCount; i++)
   {
      if (str == PropSetup::PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool PropSetup::IsParameterReadOnly(const Integer id) const
{
   if ((id == ODE_MODEL) || (id == PROPAGATOR))
   {
      if ((id == ODE_MODEL) && (mPropagator != NULL))
         if (!mPropagator->UsesODEModel())
            return true;
      return false;
   }
   else if ((id >= INITIAL_STEP_SIZE) && (id <= BULIRSCH_MINIMUMTOLERANCE))
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool PropSetup::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool PropSetup::IsParameterCommandModeSettable(const Integer id) const
{
   switch (id)
   {
      case ANALYTIC_CENTRALBODY:
      case ANALYTIC_EPOCHFORMAT:
      case ANALYTIC_STARTEPOCH:
      case ACCURACY:
      case INITIAL_STEP_SIZE:
      case ERROR_THRESHOLD:
      case SMALLEST_INTERVAL:
      case MIN_STEP:
      case MAX_STEP:
      case MAX_STEP_ATTEMPTS:
      case LOWER_ERROR:
      case TARGET_ERROR:
      case ANALYTIC_STEPSIZE:
      case BULIRSCH_MINIMUMREDUCTION:
      case BULIRSCH_MAXIMUMREDUCTION:
      case BULIRSCH_MINIMUMTOLERANCE:
      case STOP_IF_ACCURACY_VIOLATED:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->IsParameterCommandModeSettable(actualId);
         }
   default:
      return GmatBase::IsParameterCommandModeSettable(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetStringParameter(const Integer id) const
{
   #ifdef DEBUG_PROPSETUP_GET
   MessageInterface::ShowMessage
      ("PropSetup::GetStringParameter() '%s' entered, id=%d\n",
            GetParameterText(id).c_str(), id);
   #endif
   std::string name;
   switch (id)
   {
   case PROPAGATOR:
      if (mPropagator)
         name = mPropagator->GetTypeName();
      else
         name = "UndefinedPropagator";
      break;
   case ODE_MODEL:
      if (mODEModel)
      {
         name = mODEModelName;
         if (name == "")
            name = mODEModel->GetName();
      }
      else
         name = "InternalODEModel";
      break;
   case ANALYTIC_CENTRALBODY:
   case ANALYTIC_EPOCHFORMAT:
   case ANALYTIC_STARTEPOCH:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
      return mPropagator->GetStringParameter(actualId);
   }
   default:
      return GmatBase::GetStringParameter(id);
   }
   #ifdef DEBUG_PROPSETUP_GET
   MessageInterface::ShowMessage
      ("PropSetup::GetStringParameter() '%s' returning '%s'\n",
            GetParameterText(id).c_str(), name.c_str());
   #endif
   return name;
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool PropSetup::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetStringParameter() '%s', id=%d, value='%s'\n",
       GetName().c_str(), id, value.c_str());
   #endif
      
   switch (id)
   {
   case PROPAGATOR:
      mPropagatorName = value;
      return true;
   case ODE_MODEL:
      mODEModelName = value;
      return true;

   case ANALYTIC_CENTRALBODY:
   case ANALYTIC_EPOCHFORMAT:
   case ANALYTIC_STARTEPOCH:
   {
      // Get actual id
      Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
      return mPropagator->SetStringParameter(actualId, value);
   }

   default:
      return GmatBase::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool PropSetup::SetStringParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/*
 * This method provides call-through to propagator
 */
//------------------------------------------------------------------------------
Real PropSetup::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case ACCURACY:
      case INITIAL_STEP_SIZE:
      case ERROR_THRESHOLD:
      case SMALLEST_INTERVAL:
      case MIN_STEP:
      case MAX_STEP:
      case MAX_STEP_ATTEMPTS:
      case LOWER_ERROR:
      case TARGET_ERROR:
      case ANALYTIC_STEPSIZE:
      case BULIRSCH_MINIMUMREDUCTION:
      case BULIRSCH_MAXIMUMREDUCTION:
      case BULIRSCH_MINIMUMTOLERANCE:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->GetRealParameter(actualId);
         }
      default:
         return GmatBase::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real PropSetup::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/*
 * This method provides call-through to propagator
 */
//------------------------------------------------------------------------------
Real PropSetup::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
      MessageInterface::ShowMessage("PropSetup::SetRealParameter(%d <%s>, %lf) "
            "called\n", id, GetParameterText(id).c_str(), value);
   #endif
   switch (id)
   {
      case ACCURACY:
      case INITIAL_STEP_SIZE:
      case ERROR_THRESHOLD:
      case SMALLEST_INTERVAL:
      case MIN_STEP:
      case MAX_STEP:
      case MAX_STEP_ATTEMPTS:
      case LOWER_ERROR:
      case TARGET_ERROR:
      case ANALYTIC_STEPSIZE:
      case BULIRSCH_MINIMUMREDUCTION:
      case BULIRSCH_MAXIMUMREDUCTION:
      case BULIRSCH_MINIMUMTOLERANCE:
         {
            // Check if this propagator has error control, warn the user if not
            if (mPropagator->IsOfType("Integrator"))
            {
               if (!mPropagator->UsesErrorControl() && id == ACCURACY)
               {
                  MessageInterface::ShowMessage("**** WARNING **** " +
                     mPropagator->GetTypeName() + " is an integrator with no "
                     "error control. The accuracy setting will be ignored.\n");
               }
            }

            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->SetRealParameter(actualId, value);
         }
   default:
      return GmatBase::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real PropSetup::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer PropSetup::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
      case MAX_STEP_ATTEMPTS:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->GetIntegerParameter(actualId);
         }
   default:
      return GmatBase::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer PropSetup::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer PropSetup::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
      case MAX_STEP_ATTEMPTS:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->SetIntegerParameter(actualId, value);
         }
   default:
      return GmatBase::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
Integer PropSetup::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param id The parameter ID
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::GetBooleanParameter(const Integer id) const
{
   switch (id)
   {
      case STOP_IF_ACCURACY_VIOLATED:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->GetBooleanParameter(actualId);
         }
   default:
      return GmatBase::GetBooleanParameter(id);
   }
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param id The parameter ID
 * @param value The new parameter setting
 *
 * @return the value after setting
 */
//------------------------------------------------------------------------------
bool PropSetup::SetBooleanParameter(const Integer id, const bool value)
{
   switch (id)
   {
      case STOP_IF_ACCURACY_VIOLATED:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->SetBooleanParameter(actualId, value);
         }
   default:
      return GmatBase::SetBooleanParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Booleans
 *
 * @param id The parameter ID
 * @param index The index into the array
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::GetBooleanParameter(const Integer id, const Integer index) const
{
   return GmatBase::GetBooleanParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter in an array
 *
 * @param id The parameter ID
 * @parm value The new parameter value
 * @param index The index into the array
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::SetBooleanParameter(const Integer id, const bool value,
      const Integer index)
{
   return GmatBase::SetBooleanParameter(id, value, index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param label The parameter's script string
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param label The parameter's script string
 * @param value The new parameter setting
 *
 * @return the value after setting
 */
//------------------------------------------------------------------------------
bool PropSetup::SetBooleanParameter(const std::string &label,
                                         const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label,
//                                          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Booleans
 *
 * @param label The parameter's script string
 * @param index The index into the array
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::GetBooleanParameter(const std::string &label,
                                         const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter in an array
 *
 * @param label The parameter's script string
 * @parm value The new parameter value
 * @param index The index into the array
 *
 * @return the current value
 */
//------------------------------------------------------------------------------
bool PropSetup::SetBooleanParameter(const std::string &label, const bool value,
      const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets mInitialized to true if pointers of Propagator and ODEModel are not
 * NULL and there is at least one Force in the ODEModel; false otherwise
 */
//------------------------------------------------------------------------------
bool PropSetup::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PropSetup::Initialize() entered \n");
   #endif
   mInitialized = true;

   if (mPropagator == NULL)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() mPropagator is NULL\n");
      #endif
      mInitialized = false;
   }
   
   if (mODEModel == NULL)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() mODEModel is NULL\n");
      #endif
      mInitialized = false;
   }
   else if (mODEModel->GetNumForces() == 0)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() NumForces is 0\n");
      #endif
      mInitialized = false;
   }
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "PropSetup::Initialize() initialized = %d\n", mInitialized);
   #endif
   
   if (mInitialized == true)
   {
      mPropagator->SetPhysicalModel(mODEModel);
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() after SetPhysicalModel(%s) \n",
            mODEModel->GetName().c_str());
      #endif

//      mPropagator->Initialize();
      
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() after mPropagator->Initialize() \n");
      #endif
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Applies a user action
 *
 * PropSetup uses this method to set the flag for instances created in the
 * MissionControlSequence (i.e. in command mode), so that those instances can
 * accept properties that otherwise are only settable in object mode.
 */
//------------------------------------------------------------------------------
bool PropSetup::TakeAction(const std::string &action,
      const std::string &actionData)
{
   if (action == "WasMcsCreated")
   {
      #ifdef DEBUG_FUNCTION_CREATION
         MessageInterface::ShowMessage("PropSetup::TakeAction: %s was MCS "
               "created for <%p>\n", instanceName.c_str(), this);
      #endif
      mMcsCreated = true;
      return true;
   }
   
   if (action == "IncludeODEModel")
   {
      includeODEModelInGenString = true;
      return true;
   }
   
   if (action == "ExcludeODEModel")
   {
      includeODEModelInGenString = false;
      return true;
   }
   
   return GmatBase::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//            const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Provides special handling for the scripting for PropSetups.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the script used to construct the PropSetup.
 */
//------------------------------------------------------------------------------
const std::string& PropSetup::GetGeneratingString(Gmat::WriteMode mode,
            const std::string &prefix, const std::string &useName)
{
   #ifdef DEBUG_PROPSETUP_GEN_STRING
   MessageInterface::ShowMessage
      ("PropSetup::GetGeneratingString() <%p>'%s' entered, mODEModel=<%p> '%s'\n",
       this, GetName().c_str(), mODEModel,
       mODEModel ? mODEModel->GetName().c_str() : "NULL");
   #endif
   std::string gen, fmName = "", temp;
   bool showODEModel = false;
   bool propUsesODEModel = true;

   if (mPropagator != NULL)
      propUsesODEModel = mPropagator->UsesODEModel();
   if (mODEModel != NULL)
   {
      temp = mODEModel->GetName();
      if ((temp == "") && propUsesODEModel)
      {
         fmName = instanceName + "_ODEModel";
         showODEModel = true;
      }
      else
      {
         fmName = temp;
         if (fmName != "")
            showODEModel = true;
      }

      // For Gmat::SCRIPTING which saves to script file, we need to write
      // ODEModels first so it is handled in the ScriptInterpreter.
      if (mode == Gmat::SHOW_SCRIPT)
         showODEModel = true;
      
      #ifdef DEBUG_PROPSETUP_GEN_STRING
         MessageInterface::ShowMessage("includeODEModelInGenString = %s, "
               "showODEModel = %s, fmName = \"%s\"\n",
               (includeODEModelInGenString ? "true" : "false"),
               (showODEModel ? "true" : "false"), fmName.c_str());
      #endif

      if (showODEModel && includeODEModelInGenString)
      {
         gen = mODEModel->GetGeneratingString(mode, prefix, fmName) + "\n";
      }
   }
   
   // Temporarily rename propagator to the type name so the Type field fills
   std::string pname = "";
   if (mPropagator != NULL)
   {
      pname = mPropagator->GetName();
      mPropagator->SetName(mPropagator->GetTypeName());
   }

   gen += GmatBase::GetGeneratingString(mode, prefix, useName);
   generatingString = gen;
   
   // Reset the name
   if (mPropagator != NULL)
      mPropagator->SetName(pname);

   #ifdef DEBUG_PROPSETUP_GEN_STRING
   MessageInterface::ShowMessage
      ("PropSetup::GetGeneratingString() <%p>'%s' returning\n%s\n", this,
       GetName().c_str(), generatingString.c_str());
   #endif
   return generatingString;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool PropSetup::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void PropSetup::UpdateClonedObject(GmatBase *obj)
{
   #ifdef DEBUG_CLONES
      MessageInterface::ShowMessage("PropSetup::UpdateClonedObject(%s) "
            "called\n", obj->GetName().c_str());
   #endif

   if (obj->IsOfType(Gmat::ODE_MODEL) && (mODEModel != NULL))
   {
      #ifdef DEBUG_CLONES
         MessageInterface::ShowMessage("Setting ODEModel on Propsetup %s\n",
               instanceName.c_str());
      #endif
      if (obj->GetName() == mODEModel->GetName())
      {
         mODEModel->operator=(*((ODEModel*)obj));
         isInitialized = false;
      }
   }

   if (obj->IsOfType(Gmat::PROP_SETUP))
   {
      #ifdef DEBUG_CLONES
         MessageInterface::ShowMessage("Setting ODEModel on Propsetup %s to "
               "match %s\n", instanceName.c_str(), obj->GetName().c_str());
      #endif

      ODEModel *odem = ((PropSetup*)obj)->GetODEModel();
      if ((odem != NULL) && (mODEModel != NULL))
      {
         if (odem->GetName() == mODEModel->GetName())
         {
            mODEModel->operator=(*odem);
            isInitialized = false;
         }
      }
   }
}


//------------------------------------------------------------------------------
// void UpdateClonedObjectParameter(GmatBase *obj, Integer updatedParameterId)
//------------------------------------------------------------------------------
/**
 * This method changes a single parameter on an owned clone
 *
 * @param obj The master object holding the new parameter value
 * @param updatedParameterId The ID of the updated parameter
 */
//------------------------------------------------------------------------------
void PropSetup::UpdateClonedObjectParameter(GmatBase *obj,
      Integer updatedParameterId)
{
   #ifdef DEBUG_CLONES
      MessageInterface::ShowMessage("PropSetup::UpdateClonedObject(%s) "
            "called\n", obj->GetName().c_str());
   #endif

   if ((obj->IsOfType(Gmat::ODE_MODEL) ||
       obj->IsOfType(Gmat::PROP_SETUP)) && (mODEModel != NULL))
   {
      #ifdef DEBUG_CLONES
         MessageInterface::ShowMessage("Setting parameter on %s on Propsetup "
               "%s\n", obj->GetTypeName().c_str(), instanceName.c_str());
      #endif
      if (obj->GetName() == mODEModel->GetName())
      {

         mODEModel->CopyParameter(*obj, updatedParameterId);
         isInitialized = false;
      }
   }
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// void ClonePropagator(Propagator *prop)
//------------------------------------------------------------------------------
void PropSetup::ClonePropagator(Propagator *prop)
{
   #ifdef DEBUG_PROPSETUP_CLONE
   MessageInterface::ShowMessage
      ("PropSetup::ClonePropagator() <%p>'%s' entered, prop=<%p>\n", this,
       GetName().c_str(), prop);
   #endif
   if (prop != NULL)
   {
      mPropagatorName = "";
      mPropagator = (Propagator *)(prop->Clone());
      mPropagator->SetName(instanceName+"_Clone"); //LOJ: Appended _Clone to name (2014.12.16)
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (mPropagator, mPropagatorName, "PropSetup::ClonePropagator()",
          "mPropagator = prop->Clone()", this);
      #endif
   }
   else
   {
      mPropagatorName = "";
      mPropagator = NULL;
   }
   #ifdef DEBUG_PROPSETUP_CLONE
   MessageInterface::ShowMessage
      ("PropSetup::ClonePropagator() exiting, mPropagatorName='%s', mPropagator=<%p>\n",
       mPropagatorName.c_str(), mPropagator);
   #endif
}


//------------------------------------------------------------------------------
// void CloneODEModel(ODEModel *fm)
//------------------------------------------------------------------------------
void PropSetup::CloneODEModel(ODEModel *fm)
{
   #ifdef DEBUG_PROPSETUP_CLONE
   MessageInterface::ShowMessage
      ("PropSetup::CloneODEModel() <%p>'%s' entered, fm=<%p>, mODEModel=<%p>, "
       "mODEModelName='%s'\n", this, GetName().c_str(), fm, mODEModel,
       mODEModelName.c_str());
   #endif
   if (fm != NULL)
   {
      mODEModelName = "";
      mODEModel = (ODEModel *)(fm->Clone());
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (mODEModel, mODEModelName, "PropSetup::CloneODEModel()",
          "mODEModel = fm->Clone()", this);
      #endif
   }
   else
   {
      mODEModelName = "";
      mODEModel = NULL;
   }
   #ifdef DEBUG_PROPSETUP_CLONE
   MessageInterface::ShowMessage
      ("PropSetup::CloneODEModel() exiting, mODEModelName='%s', mODEModel=<%p>\n",
       mODEModelName.c_str(), mODEModel);
   #endif
}


//------------------------------------------------------------------------------
// void DeleteOwnedObject(Integer id, bool forceDelete)
//------------------------------------------------------------------------------
/**
 * Deletes internal or cloned owned object. Owned objects are named Internal*
 * in the consturctor. When Propagator or ForceModes is cloned their names are
 * set to "" so that those can be deleted.
 *
 * @param  id  Id indicating PROPAGATOR or FORCE_MODEL
 * @param  forceDelete  If this is true, it will be deleted without checking (false)
 */
//------------------------------------------------------------------------------
void PropSetup::DeleteOwnedObject(Integer id, bool forceDelete)
{
   #ifdef DEBUG_PROPSETUP_DELETE
   MessageInterface::ShowMessage
      ("PropSetup::DeleteOwnedObject() <%p>'%s' entered, id=%d, forceDelete=%d\n",
       this, GetName().c_str(), id, forceDelete);
   #endif
   
   // Since Propagator and ODEModel are cloned delete them here. (loj: 2008.11.05)
   if (id == PROPAGATOR)
   {
      #ifdef DEBUG_PROPSETUP_DELETE
      MessageInterface::ShowMessage
         ("   mPropagator=<%p>, mPropagatorName='%s'\n",
          mPropagator, mPropagatorName.c_str());
      #endif
      if (mPropagator != NULL)
      {
         if (forceDelete ||
             (!forceDelete &&
              (mPropagatorName == "" || mPropagatorName == "InternalPropagator")))
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (mPropagator, mPropagatorName, "PropSetup::DeleteOwnedObject()",
                "deleting mPropagator", this);
            #endif
            delete mPropagator;
            mPropagatorName = "";
            mPropagator = NULL;
         }
         else
         {
            #ifdef DEBUG_PROPSETUP_DELETE
            MessageInterface::ShowMessage
               ("   =====> mPropagator=<%p> was not deleted\n", mPropagator);
            #endif
         }
      }
   }
   else if (id == ODE_MODEL)
   {
      #ifdef DEBUG_PROPSETUP_DELETE
      MessageInterface::ShowMessage
         ("   mODEModel=<%p>, mODEModelName='%s'\n",
          mODEModel, mODEModel ? mODEModel->GetName().c_str() : "NULL");
      #endif
      if (mODEModel != NULL)
      {
         // delete cloned ODEModel 
         if (forceDelete ||
             (!forceDelete &&
              (mODEModelName == "" || mODEModelName == "InternalODEModel")))
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (mODEModel, mODEModelName, "PropSetup::DeleteOwnedObject()",
                "deleting mODEModel", this);
            #endif
            delete mODEModel;
            mODEModelName = "";
            mODEModel = NULL;
         }
         else
         {
            #ifdef DEBUG_PROPSETUP_DELETE
            MessageInterface::ShowMessage
               ("   =====> mODEModel=<%p> was not deleted\n", mODEModel);
            #endif
         }
      }
   }
   
   #ifdef DEBUG_PROPSETUP_DELETE
   MessageInterface::ShowMessage
      ("PropSetup::DeleteOwnedObject() <%p>'%s' exiting\n", this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
// Integer GetOwnedObjectId(Integer id, UnsignedInt objType) const
//------------------------------------------------------------------------------
/**
 * Returns property id of owned object.
 */
//------------------------------------------------------------------------------
Integer PropSetup::GetOwnedObjectId(Integer id, UnsignedInt objType) const
{
   Integer actualId = -1;
   
   try
   {
      if (objType == Gmat::PROPAGATOR)
      {
         if (mPropagator == NULL)
            throw PropSetupException
               ("PropSetup::GetOwnedObjectId() failed: Propagator is NULL");
         
         actualId = mPropagator->GetParameterID(GetParameterText(id));

         #ifdef DEBUG_SET_PROPAGATOR_PARAMETER
            MessageInterface::ShowMessage("   Actual ID is %d\n", actualId);
			#endif
      }
      else if (objType == Gmat::ODE_MODEL)
      {
         if (mODEModel == NULL)
            throw PropSetupException
               ("PropSetup::GetOwnedObjectId() failed: ODEModel is NULL");
         
         actualId = mODEModel->GetParameterID(GetParameterText(id));
      }
   }
   catch (BaseException &)
   {
      throw;
   }
   
   return actualId;
}


bool PropSetup::SetPrecisionTimeFlag(bool onOff)
{
   mPropagator->SetPrecisionTimeFlag(onOff);
   // Ephemeris propagators do not have ODE models
   if (mODEModel)
      mODEModel->SetPrecisionTimeFlag(onOff);
   psm.GetState()->SetPrecisionTimeFlag(onOff);

   return GmatBase::SetPrecisionTimeFlag(onOff);
}
