//$Id: GravData.cpp 9811 2011-08-29 21:15:18Z wendys-dev $
//------------------------------------------------------------------------------
//                                  GravData
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
// contract, Task Order 28.
//
// Author: Darrel Conway, Thinking Systems, Inc
// Created: 2013/05/24  (Based on EnvData)
//
/**
 * Implements class which provides data from a force model, such as
 * acceleration, atmospheric density, current or average F10.7 value,
 * relativistic correction, etc.
 */
//------------------------------------------------------------------------------
#include "polyhedrongravitymodel_defs.hpp"
#include "GravData.hpp"
#include "ODEModel.hpp"
#include "ParameterException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

#include "PolyhedronGravityModel.hpp"

//#define DEBUG_ODEDATA_INIT 1
//#define DEBUG_ODEDATA_RUN  1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real GravData::GRAV_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

const std::string
GravData::VALID_OBJECT_TYPE_LIST[GravDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "ODEModel"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// GravData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GravData::GravData() :
   RefData        (),
   mSpacecraft    (NULL),
   mSolarSystem   (NULL),
   mModel         (NULL),
   cbForce        (NULL),
   body           (NULL),
   forceType      (UNDEFINED),
   bodyRadius     (6378.14)
{
}


//------------------------------------------------------------------------------
// GravData(const GravData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param copy The GravData object being copied.
 */
//------------------------------------------------------------------------------
GravData::GravData(const GravData &copy) :
   RefData        (copy),
   mSpacecraft    (copy.mSpacecraft),
   mSolarSystem   (copy.mSolarSystem),
   mModel         (copy.mModel),
   cbForce        (copy.cbForce),
   body           (copy.body),
   forceType      (copy.forceType),
   bodyRadius     (copy.bodyRadius)
{
}


//------------------------------------------------------------------------------
// GravData& operator= (const GravData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param right the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
GravData& GravData::operator= (const GravData &right)
{
   if (this != &right)
   {
      RefData::operator=(right);

      mSpacecraft = right.mSpacecraft;
      mSolarSystem = right.mSolarSystem;
      mModel = right.mModel;
      cbForce = right.cbForce;
      body = right.body;
      forceType = right.forceType;
      bodyRadius = right.bodyRadius;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
// ~GravData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GravData::~GravData()
{
   //MessageInterface::ShowMessage("==> GravData::~GravData()\n");
}


//------------------------------------------------------------------------------
// Real GetEnvReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrieves requested data for the current epoch and location of the Spacecraft
 *
 * @param str The string identifying the desired data
 *
 * @return The value for the requested data
 */
//------------------------------------------------------------------------------
Real GravData::GetGravReal(const std::string &str)
{
   if (mSpacecraft == NULL || mSolarSystem == NULL || mModel == NULL)
      InitializeRefObjects();

   Real retval = 0.0;

   if (str == "SurfaceHeight")
   {
      // SurfaceHeight: a measure of the altitude of from body, based on model
      // Default the return to be outside of the body
      retval = 1000.0;

      if ((mModel != NULL) && (mModel->IsOfType(Gmat::ODE_MODEL)))
      {
         #ifdef DEBUG_ODEDATA_RUN
            MessageInterface::ShowMessage("ODEModel %s Found\n",
                  mModel->GetName().c_str());
         #endif

         // Find the gravity model associated with the force model origin
         if (cbForce == NULL)
         {
            ODEModel *ode = (ODEModel*)mModel;
            PhysicalModel *force;
            std::string cbName = ode->GetStringParameter("CentralBody");
            Integer count = ode->GetNumForces();
            for (Integer i = 0; i < count; ++i)
            {
               force = ode->GetForce(i);
               if (force->IsOfType("PointMassForce") ||
                   force->IsOfType("GravityBase"))
               {
                  if (force->GetBodyName() == cbName)
                  {
                     cbForce = force;
                     if (force->IsOfType("PointMassForce"))
                     {
                        forceType = POINT_MASS;
                        body = force->GetBody();
                        bodyRadius = body->GetEquatorialRadius();
                     }

                     if (force->IsOfType("GravityField"))
                     {
                        forceType = HARMONIC;
                        body = force->GetBody();
                        bodyRadius = body->GetEquatorialRadius();
                     }

                     if (force->IsOfType("PolyhedronGravityModel"))
                     {
                        forceType = POLYHEDRAL;
                        body = force->GetBody();
                        bodyRadius = body->GetEquatorialRadius();
                        cbForce = force;
                        cbForce->SetSolarSystem(mSolarSystem);
                        cbForce->Initialize();
                     }

                     break;
                  }
               }
            }
         }

         GmatEpoch epoch  = mSpacecraft->GetEpoch();
         Real *scState = mSpacecraft->GetState().GetState();
         Rvector6 cbState = body->GetState(epoch);
         Rvector3 state;

         state[0] = scState[0] - cbState[0];
         state[1] = scState[1] - cbState[1];
         state[2] = scState[2] - cbState[2];

         Real dist = state.Norm();

         if (cbForce != NULL)
         {
            switch (forceType)
            {
            case POINT_MASS:
               retval = dist - bodyRadius;
               break;

            case HARMONIC:
               /// @todo Change to geodetic altitude here
               retval = dist - bodyRadius;
               break;

            case POLYHEDRAL:
               if (dist < bodyRadius)
                  retval = ((PolyhedronGravityModel*)cbForce)->GetAltitude(state, epoch);
               else
                  retval = dist - bodyRadius;
               break;

            case UNDEFINED:
            default:
               break;
            }
         }
      }
   }
   else
      throw ParameterException("GravData::GetEnvReal() Unknown "
                  "parameter name: " + str);

   return retval;
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* GravData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool GravData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<GravDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == GravDataObjectCount)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void GravData::InitializeRefObjects()
{
   #if DEBUG_ODEDATA_INIT
   MessageInterface::ShowMessage("GravData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   if (mSpacecraft == NULL)
      throw ParameterException
         ("GravData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("GravData::InitializeRefObjects() Cannot find SolarSystem object\n");

   // set force model
   std::string modelName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[PHYSICAL_MODEL]));
   if (modelName != "")
   {
      #if DEBUG_ODEDATA_INIT
         MessageInterface::ShowMessage
            ("GravData::InitializeRefObjects() getting modelName:%s pointer.\n",
             modelName.c_str());
      #endif
         
      mModel =
         (ODEModel*)FindFirstObject(VALID_OBJECT_TYPE_LIST[PHYSICAL_MODEL]);
      
      if (!mModel)
         throw ParameterException
            ("GravData::InitializeRefObjects() parameter dependent force model "
             "not found in the Configuration: " + modelName + "\n");

   }
   if (mModel == NULL)
      throw ParameterException
         ("GravData::InitializeRefObjects() Cannot find ODE Model object\n");

   if (cbForce != NULL)
   {
      cbForce->SetSolarSystem(mSolarSystem);
      cbForce->Initialize();
   }
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool GravData::IsValidObjectType(UnsignedInt type)
{
   for (int i=0; i<GravDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;
}

