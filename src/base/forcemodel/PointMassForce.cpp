//$Id$
//------------------------------------------------------------------------------
//                             PointMassForce
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
// *** File Name : PointMassForce.cpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 1/21/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces to support Nystrom code
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated parameter strings to include units
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - GetParameter to GetRealParameter
//                                - SetParameter to SetRealParameter
//                              Removals:
//                                - GetParameterName()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
// **************************************************************************

#include "PointMassForce.hpp"
#include "MessageInterface.hpp"
#include "SolarSystem.hpp"
#include "Rvector6.hpp"
#include "GmatDefaults.hpp"
#include "ODEModelException.hpp"
#include "TimeTypes.hpp"

//#define DEBUG_PMF_BODY 0
//#define DEBUG_PMF_DERV 0
//
//#define DEBUG_FORCE_MODEL
//#define DEBUG_FORCE_ORIGIN
//#define DUMP_PLANET_DATA
//#define DEBUG_INDIRECT_TERM
//#define DEBUG_FIRST_CALL
//#define DEBUG_DERIVATIVES_FOR_SPACECRAFT
//#define DEBUG_TIME_ADDITION
//#define DEBUG_GRAVITY_TORQUE


#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif


//---------------------------------
// static data
//---------------------------------
const std::string
PointMassForce::PARAMETER_TEXT[PointMassParamCount - PhysicalModelParamCount] =
{
   "GravConst",
   "Radius",
   "EstimateMethod",
   "PrimaryBody",
};

const Gmat::ParameterType
PointMassForce::PARAMETER_TYPE[PointMassParamCount - PhysicalModelParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// PointMassForce::PointMassForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for point mass gravitational model
 */
//------------------------------------------------------------------------------
PointMassForce::PointMassForce(const std::string &name) :
   PhysicalModel          (Gmat::PHYSICAL_MODEL, "PointMassForce", name),
   mu                     (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
   estimationMethod       (1.0),
   isPrimaryBody          (true),
   satCount               (0)
{
   parameterCount = PointMassParamCount;
   dimension = 6 * satCount;
   body = NULL;
   
   objectTypeNames.push_back("PointMassForce");

   // create default body
   bodyName = GmatSolarSystemDefaults::EARTH_NAME; //loj: 5/20/04 added
   
   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);

   hasTimeJacobian = true;
}

//------------------------------------------------------------------------------
// PointMassForce::~PointMassForce()
//------------------------------------------------------------------------------
/**
 * Destructor for the point mass gravitational model
 */
//------------------------------------------------------------------------------
PointMassForce::~PointMassForce()
{
   //delete theBody; //loj: 5/20/04
}

//------------------------------------------------------------------------------
// PointMassForce::PointMassForce(const PointMassForce& pmf)
//------------------------------------------------------------------------------
/**
 * The copy constructor
 * Note that the actual states are not copied over for this implementation.
 *
 * @param pmf   The original that is being copied
 */
//------------------------------------------------------------------------------
PointMassForce::PointMassForce(const PointMassForce& pmf) :
   PhysicalModel          (pmf),
   mu                     (pmf.mu),
   estimationMethod       (pmf.estimationMethod),
   isPrimaryBody          (pmf.isPrimaryBody),
   bodyrv                 (pmf.bodyrv),
   orig                   (pmf.orig),
   rv                     (pmf.rv),
   now                    (pmf.now),
   nowGT                  (pmf.nowGT),
   satCount               (pmf.satCount)
{
   parameterCount = PointMassParamCount;
   dimension = pmf.dimension;
   epoch = pmf.epoch;
   isInitialized = false;
}

//------------------------------------------------------------------------------
// PointMassForce& PointMassForce::operator= (const PointMassForce& pmf)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 * Note that the actual states are not copied over for this implementation.
 *
 * @param pmf   The original that is being copied
 */
//------------------------------------------------------------------------------
PointMassForce& PointMassForce::operator= (const PointMassForce& pmf)
{
   if (&pmf == this)
      return *this;
      
   PhysicalModel::operator=(pmf);
   epoch            = pmf.epoch;
   mu               = pmf.mu;
   dimension        = pmf.dimension;
   isInitialized    = false;
   elapsedTime      = pmf.elapsedTime;
   estimationMethod = pmf.estimationMethod;
   isPrimaryBody    = pmf.isPrimaryBody;
   bodyrv           = pmf.bodyrv;
   orig             = pmf.orig;
   rv               = pmf.rv;
   now              = pmf.now;
   nowGT            = pmf.nowGT;
   satCount         = pmf.satCount;

   return *this;
}

//------------------------------------------------------------------------------
// bool PointMassForce::Initialize()
//------------------------------------------------------------------------------
/**
 * Method used to ititialize the state data
 */
//------------------------------------------------------------------------------
bool PointMassForce::Initialize()
{
   //MessageInterface::ShowMessage("PointMassForce::Initialize() entered\n");
   
   PhysicalModel::Initialize();
    
   if (solarSystem != NULL)
   {
      //MessageInterface::ShowMessage(
      //   "PointMassForce::Initialize() bodyName=%s\n", bodyName.c_str());
      
      body = solarSystem->GetBody(bodyName); //loj: 5/7/04 added
       
      if (body != NULL)
      {
         mu = body->GetGravitationalConstant();
         
         #if DEBUG_PMF_BODY
             MessageInterface::ShowMessage
                ("PointMassForce::Initialize() setting mu=%f for type=%s, "
                 "name=%s\n", mu, body->GetTypeName().c_str(), 
                 body->GetName().c_str());
         #endif
         
         #ifdef DEBUG_FORCE_MODEL
            MessageInterface::ShowMessage(
               "%s%s%s%16le\n",
               "Point mass body ", body->GetName().c_str(),
               " has mu = ", mu);
         #endif
      }
      else
      {
         MessageInterface::ShowMessage(
            "PointMassForce::Initialize() body \"%s\" is not in the solar "
            "system\n", bodyName.c_str());
         isInitialized = false;
         throw ODEModelException("PointMassForce::Initialize() body \"" +
            bodyName + "\" is not in the solar system\n");
      }
   }
   else
   {
      MessageInterface::ShowMessage(
         "PointMassForce::Initialize() solarSystem is NULL\n");
      isInitialized = false;
      throw ODEModelException(
         "PointMassForce::Initialize() solarSystem is NULL\n");
   }
   
   Integer i6;
   for (Integer i = 0; i < satCount; i++) 
   {
      i6 = cartesianStart + i*6;
      modelState[i6]   = 7000.0 + 200.0 * i;
      modelState[i6+1] = 300.0 * i;
      modelState[i6+2] = 1000.0 - 100.0 * i;
      modelState[i6+3] = 0.0;
      modelState[i6+4] = 8.0 + 0.1 * i;    // 7.61 km/s makes first one circular
      modelState[i6+5] = 0.0;
   }

   return true;
}

//------------------------------------------------------------------------------
// bool PointMassForce::GetDerivatives(Real * state, Real dt, Integer order)
//------------------------------------------------------------------------------
/**
 * Method used to apply the point mass force model to the state
 * 
 * This method sets the first derivative for 1 or more spacecraft.  The model
 * applies point mass gravity, with the gravitating point located at position
 * \f$(0, 0, 0)\f$.  A later implementation may allow for locations other than 
 * the origin.
 * 
 * The first derivatives map as follows:
 * 
 * \f[ {{d}\over{dt}} \pmatrix{\vec r \cr\vec v\cr} = 
 *     \pmatrix{\vec v \cr -{{GM\vec r}\over{r^3}}\cr} \f] 
 * 
 * @param state  The current state vector
 * @param dt     The time offset for the calculations
 * @param order  Order of the derivative being calculated
 */
//------------------------------------------------------------------------------
bool PointMassForce::GetDerivatives(Real * state, Real dt, Integer order, 
      const Integer id)
{
   #ifdef DEBUG_PMF_DERV
      MessageInterface::ShowMessage("Evaluating PointMassForce; "
         "state pointer = %p, time offset = %le, order = %d, id = %d " 
         "satCount = %d, epoch = %le\n", state, dt, order, id, satCount, epoch);
      MessageInterface::ShowMessage("state = [");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("%.12lf  ", state[i]);
      MessageInterface::ShowMessage("]\n");
   #endif
   
   Integer i6, s6, a6, ix;

   if (fillCartesian || fillSTM || fillAMatrix)
   {
      if (order > 2)
         return false;
   
      if ((state == NULL) || (deriv == NULL) || (theState == NULL))
         // throw("Arrays not yet initialized -- exiting");
         return false;
   
      Real radius, r3, mu_r, rbb3, mu_rbb, a_indirect[3];

      now = epoch + (elapsedTime + dt)/GmatTimeConstants::SECS_PER_DAY;
      nowGT = epochGT; nowGT.AddSeconds(elapsedTime); nowGT.AddSeconds(dt);

      #ifdef DEBUG_TIME_ADDITION
         MessageInterface::ShowMessage("%12s epoch: %.12lf from %.12lf + (%lf "
               "+ %lf) sec\n", bodyName.c_str(), now.Get(), epoch, elapsedTime,
               dt);
      #endif

      Real relativePosition[3];
      if (hasPrecisionTime)
      {
         bodyrv = body->GetState(nowGT);
         orig = forceOrigin->GetState(nowGT);
      }
      else
      {
         bodyrv = body->GetState(now);
         orig = forceOrigin->GetState(now);
      }

      #ifdef DUMP_PLANET_DATA
         MessageInterface::ShowMessage("%s, %17.12lf, %17.12lf, %17.12lf, "
            "%17.12lf, %17.16lf, %17.16lf, %17.16lf, %s, %17.12lf, %17.12lf, "
            "%17.12lf, %17.12lf, %17.16lf, %17.16lf, %s, %17.12lf, %17.12lf, "
            "%17.12lf, %17.12lf, %17.16lf, %17.16lf\n", 
            body->GetName().c_str(), now.Get(), bodyrv[0], bodyrv[1], bodyrv[2], 
            bodyrv[3], bodyrv[4], bodyrv[5], "SC_Data", state[0], state[1], 
            state[2], state[3], state[4], state[5], "origin", orig[0], orig[1],
            orig[2], orig[3], orig[4], orig[5]);
      #endif

      const Real *brv = bodyrv.GetDataVector(), *orv = orig.GetDataVector();
      Real rv[6];
      rv[0] = brv[0] - orv[0];
      rv[1] = brv[1] - orv[1];
      rv[2] = brv[2] - orv[2];
      rv[3] = brv[3] - orv[3];
      rv[4] = brv[4] - orv[4];
      rv[5] = brv[5] - orv[5];
      
      if (fillTimeJacobian)
      {
         Real rvmag = sqrt(rv[0] * rv[0] + rv[1] * rv[1] + rv[2] * rv[2]);
         Real rvunit[3];
         rvunit[0] = rv[0] / rvmag;
         rvunit[1] = rv[1] / rvmag;
         rvunit[2] = rv[2] / rvmag;

         // Calculate the unit vector of the position of the spacecraft w/r/t
         // the point mass body
         Real rvrel[3];
         rvrel[0] = rv[0] - state[0];
         rvrel[1] = rv[1] - state[1];
         rvrel[2] = rv[2] - state[2];

         Real rvrelmag = sqrt(rvrel[0] * rvrel[0] + rvrel[1] * rvrel[1] +
            rvrel[2] * rvrel[2]);
         Real rvrelunit[3];
         rvrelunit[0] = rvrel[0] / rvrelmag;
         rvrelunit[1] = rvrel[1] / rvrelmag;
         rvrelunit[2] = rvrel[2] / rvrelmag;

         for (Integer i = 0; i < stmRowCount; ++i)
            timeJacobian[i] = 0.0;

         // If this point mass is not the central body, calculate the n-body
         // time Jacobian
         if (rvmag != 0.0)
         {
            Real timeJacElement;
            for (Integer i = 0; i < 3; ++i)
            {
               for (Integer j = 0; j < 3; ++j)
               {
                  if (i == j)
                  {
                     timeJacElement = (1.0 - 3.0 * rvrelunit[i] *
                        rvrelunit[j]) /
                        pow(rvrelmag, 3) - (1.0 - 3.0 * rvunit[i] *
                        rvunit[j]) / pow(rvmag, 3);
                  }
                  else
                  {
                     timeJacElement = (-3.0 * rvrelunit[i] * rvrelunit[j] / pow(rvrelmag, 3)) -
                        (-3.0 * rvunit[i] * rvunit[j]) /
                        pow(rvmag, 3);
                  }
                  timeJacobian[i + 3] += mu * timeJacElement * rv[j + 3];
               }
            }
         }
      }

      // The vector from the force origin to the gravitating body
      // Precalculations for the indirect effect term
      rbb3 = rv[0]*rv[0]+rv[1]*rv[1]+rv[2]*rv[2];
      if (rbb3 != 0.0) 
      {
         //rbb3 *= sqrt(rbb3);
        rbb3 = sqrt(rbb3 * rbb3 * rbb3);
         mu_rbb = mu / rbb3;
         a_indirect[0] = mu_rbb * rv[0];
         a_indirect[1] = mu_rbb * rv[1];
         a_indirect[2] = mu_rbb * rv[2];
      }
      else
         a_indirect[0] = a_indirect[1] = a_indirect[2] = 0.0;
   
      #ifdef DEBUG_FORCE_ORIGIN
         MessageInterface::ShowMessage(
            "Epoch:  %16.11lf\n  Origin:  [%s]\n  J2KBod:  [%s]\n",
            now.Get(), orig.ToString().c_str(), bodyrv.ToString().c_str());
         MessageInterface::ShowMessage(
            "Now = %16.11lf rbb3 = %16.11le rv = [%16lf %16lf %16lf]\n",
            now.Get(), rbb3, rv[0], rv[1], rv[2]);
      #endif

      #ifdef DEBUG_INDIRECT_TERM
         MessageInterface::ShowMessage("Indirect term for %s with mu %.15le:\n",
               body->GetName().c_str(), mu);
         MessageInterface::ShowMessage("   Origin   = [%16le %16le %16le]\n",
               orig[0], orig[1], orig[2]);
         MessageInterface::ShowMessage("   Position = [%16le %16le %16le]\n",
               bodyrv[0], bodyrv[1], bodyrv[2]);
         MessageInterface::ShowMessage("   a_indirect = [%16le %16le %16le]\n",
               body->GetName().c_str(), a_indirect[0],
               a_indirect[1], a_indirect[2]);
      #endif

      #if DEBUG_PMF_BODY
          ShowBodyState("PointMassForce::GetDerivatives() BEFORE compute " +
                        body->GetName(), now.GetReal(), Rvector6(rv));
      #endif
   
      #if DEBUG_PMF_DERV
          ShowDerivative("PointMassForce::GetDerivatives() BEFORE compute", state,
             satCount);
      #endif
      
      if (fillCartesian)
      {
         for (Integer i = 0; i < satCount; i++) 
         {
            i6 = cartesianStart + i * 6;
            
            relativePosition[0] = rv[0] - state[ i6 ];
            relativePosition[1] = rv[1] - state[i6+1];
            relativePosition[2] = rv[2] - state[i6+2];
      
            r3 = relativePosition[0]*relativePosition[0] + 
                 relativePosition[1]*relativePosition[1] + 
                 relativePosition[2]*relativePosition[2];
            
            radius = sqrt(r3);
            r3 *= radius;
            mu_r = mu / r3;
      
            #ifdef DEBUG_INDIRECT_TERM
               MessageInterface::ShowMessage("   Raw acc for sp %d:  ", i);
               MessageInterface::ShowMessage("[%16le %16le %16le]\n",
                     relativePosition[0] * mu_r, relativePosition[1] * mu_r,
                     relativePosition[2] * mu_r);
               MessageInterface::ShowMessage("   Corrected acc =     [%16le "
                     "%16le %16le]\n",
                     relativePosition[0] * mu_r - a_indirect[0],
                     relativePosition[1] * mu_r - a_indirect[1],
                     relativePosition[2] * mu_r - a_indirect[2]);
            #endif

            if (order == 1) 
            {
               // Do dv/dt first, in case deriv = state
               deriv[3 + i6] = relativePosition[0] * mu_r - a_indirect[0];
               deriv[4 + i6] = relativePosition[1] * mu_r - a_indirect[1];
               deriv[5 + i6] = relativePosition[2] * mu_r - a_indirect[2];

               /// ODEModel now fills in the velocity part, so removed it here
               // dr/dt = v, but only fill this piece for the central body
               //if (rbb3 == 0.0)
               //{
               //   deriv[i6]     = state[3 + i6];
               //   deriv[1 + i6] = state[4 + i6];
               //   deriv[2 + i6] = state[5 + i6];
               //}
               //else
               deriv[i6] = deriv[1 + i6] = deriv[2 + i6] = 0.0;
               #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
                  if ((dt == 0) && (i6 == 0))                  
                     MessageInterface::ShowMessage("***PMF AX in GD   : "
                        "%.12lf * (%.12le / (%.12le)^3)- %.12le = %.12le\n", 
                        relativePosition[0], mu, r3, a_indirect[0], deriv[3]);
               #endif
            } 
            else 
            {
               // Feed accelerations to corresponding components directly for RKN
               deriv[ i6 ] = relativePosition[0] * mu_r - a_indirect[0]; 
               deriv[i6+1] = relativePosition[1] * mu_r - a_indirect[1]; 
               deriv[i6+2] = relativePosition[2] * mu_r - a_indirect[2]; 
               deriv[i6+3] = 0.0; 
               deriv[i6+4] = 0.0; 
               deriv[i6+5] = 0.0; 
            }
         }
      }
      #if DEBUG_PMF_DERV
         ShowDerivative("PointMassForce::GetDerivatives() AFTER compute", state, 
            satCount);
      #endif
   
      #ifdef DEBUG_FORCE_MODEL
//         MessageInterface::ShowMessage(
//            "%s%s%s%16.10lf%s%16.10lf, %16.10lf, %16.10lf%s%16.10lf, %16.10lf, "
//            "%16.10lf%s%16.10le, %16.10le, %16.10le]\n",
//            "Point mass force for ", body->GetName().c_str(), " at epoch ",
//            now.Get(), "\n   Sat position:  [", state[0], state[1], state[2],
//            "]\n   Body position: [", rv[cartIndex + 0], rv[cartIndex + 1],
//            rv[cartIndex + 2], "]\n   Acceleration:  [", deriv[cartIndex + 3],
//            deriv[cartIndex + 4], deriv[cartIndex + 5]);
      #endif
      if (fillSTM || fillAMatrix)
      {
         Integer associate, element;
         Integer aiCount = (fillSTM ? stmCount : aMatrixCount);

			s6 = stmStart;
			a6 = aMatrixStart;

         for (Integer i = 0; i < aiCount; ++i)
         {
				i6 = (fillSTM ? s6 : a6);
            associate = theState->GetAssociateIndex(i6);
            
            relativePosition[0] = rv[0] - state[ associate ];
            relativePosition[1] = rv[1] - state[associate+1];
            relativePosition[2] = rv[2] - state[associate+2];
      
            r3 = relativePosition[0]*relativePosition[0] + 
                 relativePosition[1]*relativePosition[1] + 
                 relativePosition[2]*relativePosition[2];
            
            radius = sqrt(r3);
            r3 *= radius;
            mu_r = mu / r3;
            
				// Get Spacecraft object
				Spacecraft *sc = (Spacecraft*)scObjs[i];

				// Create aTilde matrix
				stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
				Integer stmSize = stmRowCount * stmRowCount;
				Real *aTilde = new Real[stmSize];
            for (Integer i = 0; i < stmRowCount; ++i)
            {
               ix = i * stmRowCount;
               for (Integer j = 0; j < stmRowCount; ++j)
                  aTilde[ix+j] = 0.0;
            }
            
				// Calculate A-tilde
            // Math spec, equ 6.69, broken into separate pieces
            ix = stmRowCount * 3;
            aTilde[ix] = - mu_r + 3.0 * mu_r / (radius*radius) *
                             relativePosition[0] * relativePosition[0];
            
            aTilde[ix+1] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[0] * relativePosition[1];
            
            aTilde[ix+2] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[0] * relativePosition[2];
            
            ix = stmRowCount * 4;
            aTilde[ix] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[1] * relativePosition[0];
            
            aTilde[ix+1] = - mu_r + 3.0 * mu_r / (radius*radius) *
                             relativePosition[1] * relativePosition[1];
            
            aTilde[ix+2] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[1] * relativePosition[2];
            
            ix = stmRowCount * 5;
            aTilde[ix] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[2] * relativePosition[0];
            
            aTilde[ix+1] = 3.0 * mu_r / (radius*radius) *
                             relativePosition[2] * relativePosition[1];
            
            aTilde[ix+2] = - mu_r + 3.0 * mu_r / (radius*radius) *
                             relativePosition[2] * relativePosition[2];

// Moved to ODEModel so upper half of STM and A-Matrix are correctly managed
//            // Now Phi_dot = A_tilde Phi
//            if (fillSTM)
//            {
//               for (Integer j = 0; j < 6; ++j)
//               {
//                  for (Integer k = 0; k < 6; ++k)
//                  {
//                     element = j * 6 + k;
//                     deriv[i6+element] = 0.0;
//                     for (Integer l = 0; l < 6; ++l)
//                     {
//                        deriv[i6+element] += aTilde[j*6+l] * state[i6+l*6+k];
//                     }
//                  }
//               }
//            }
            for (Integer j = 0; j < stmRowCount; ++j)
            {
               for (Integer k = 0; k < stmRowCount; ++k)
               {
                  element = j * stmRowCount + k;
                  if (fillSTM)
                     //deriv[i6+element] = aTilde[element];
						   deriv[s6 + element] = aTilde[element];
                  if (fillAMatrix)
                     deriv[a6 + element] = aTilde[element];
               }
            }

				delete[] aTilde;
				if (fillSTM)
					s6 = s6 + stmSize;
				if (fillAMatrix)
					a6 = a6 + stmSize;

         }
      }
   }
   
   return true;
}

//------------------------------------------------------------------------------
// bool PointMassForce::GetComponentMap(Integer * map, Integer order) const
//------------------------------------------------------------------------------
/**
 * Used to get the mapping in the state variable between components 
 * 
 * This method is used to obtain a mapping between the elements of the state
 * vector.  It is used, for instance, to map the velocity components to the
 * position components for a spacecraft state vector, for the 
 * Runge-Kutta-Nystrom integrators.  The default implementation simply returns 
 * false.
 * 
 * @param map    Array that will contain the mapping of the elements
 * @param order  The order for the mapping (1 maps 1st derivatives to their base 
 *               components, 2 maps 2nd derivatives, and so on)
 * 
 * @return Returns true if a mapping was made, false otherwise.  A false return 
 *         value can be used to indicate that the requested map is not 
 *         available, and verefore that the model may not be appropriate for the
 *         requested operations.
 */
//------------------------------------------------------------------------------
bool PointMassForce::GetComponentMap(Integer * map, Integer order) const
{
   Integer i6;

   if (order != 1)
      return false;

   for (Integer i = 0; i < satCount; i++) 
   {
      i6 = cartesianStart + i * 6;

      map[ i6 ] = i6 + 3;
      map[i6+1] = i6 + 4;
      map[i6+2] = i6 + 5;
      map[i6+3] = -1;
      map[i6+4] = -1;
      map[i6+5] = -1;
   }

   return true;
}

//------------------------------------------------------------------------------
// Real PointMassForce::EstimateError(Real * diffs, Real * answer) const
//------------------------------------------------------------------------------
/**
 * Interface used to estimate the error in the current step
 *
 * The method calculates the largest local estimate of the error from the 
 * integration given the components of the differences calculated by the
 * integrator.  It returns the largest error estimate found.  
 *  
 * The default implementation returns the largest single relative error 
 * component found based on the input arrays.  In other words, the 
 * implementation provided here returns the largest component of the following
 * vector:
 *
 * \f[\vec \epsilon = |{{{EE}_n}\over{x_n^f - x_n^i}}|\f]
 *  
 * There are several alternatives that users of this class can implement: the 
 * error could be calculated based on the largest error in the individual 
 * components of the state vector, as the magnitude of the state vector (that 
 * is, the L2 (rss) norm of the error estimate vector).  The estimated error 
 * should never be negative, so a return value less than 0.0 can be used to 
 * indicate an error condition.
 *    
 * @param diffs  Array of differences calculated by the integrator.  This array 
 *                must be the same size as the state vector
 * @param answer Candidate new state from the integrator
 *
 */
//------------------------------------------------------------------------------
Real PointMassForce::EstimateError(Real * diffs, Real * answer) const
{
   if (estimationMethod == 1.0)
      return PhysicalModel::EstimateError(diffs, answer);

   Real retval = 0.0, err, mag, vec[3];

   for (Integer i = 0; i < dimension; i += 3) 
   {
      vec[0] = answer[ i ] - modelState[ i ];
      vec[1] = answer[i+1] - modelState[i+1];
      vec[2] = answer[i+2] - modelState[i+2];

      mag = vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];        
      err = diffs[i]*diffs[i] + diffs[i+1]*diffs[i+1] + diffs[i+2]*diffs[i+2];
      if (mag > 0.0)
         err = sqrt(err / mag);
      else
         err = sqrt(err);
      if (err > retval)
         retval = err;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 PointMassForce::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   Rvector6 dv;

   Real *j2kState = sc->GetState().GetState();
   Real state[6];
   Real now = sc->GetEpoch();
   GmatTime nowgt = sc->GetEpochGT();

   if (hasPrecisionTime)
      BuildModelStateGT(nowgt, state, j2kState);
   else
      BuildModelState(now, state, j2kState);

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("Point Mass %s: Pos = [%lf %lf %lf] -> ",
               bodyName.c_str(), state[0], state[1], state[2]);
   #endif


   Real radius, r3, mu_r, rbb3, mu_rbb, a_indirect[3];

   Real relativePosition[3];
   Rvector6 bodyrv, orig;
   if (hasPrecisionTime)
   {
      bodyrv = body->GetState(nowgt);
      orig = forceOrigin->GetMJ2000State(nowgt);
   }
   else
   {
      bodyrv = body->GetState(now);
      orig = forceOrigin->GetMJ2000State(now);
   }

   const Real *brv = bodyrv.GetDataVector(), *orv = orig.GetDataVector();
   Real rv[3];
   rv[0] = brv[0] - orv[0];
   rv[1] = brv[1] - orv[1];
   rv[2] = brv[2] - orv[2];

   // The vector from the force origin to the gravitating body
   // Precalculations for the indirect effect term
   rbb3 = rv[0]*rv[0]+rv[1]*rv[1]+rv[2]*rv[2];
   if (rbb3 != 0.0)
   {
      //rbb3 *= sqrt(rbb3);
     rbb3 = sqrt(rbb3 * rbb3 * rbb3);
      mu_rbb = mu / rbb3;
      a_indirect[0] = mu_rbb * rv[0];
      a_indirect[1] = mu_rbb * rv[1];
      a_indirect[2] = mu_rbb * rv[2];
   }
   else
      a_indirect[0] = a_indirect[1] = a_indirect[2] = 0.0;

   {
      //relativePosition[0] = rv[0] - state[0];
      //relativePosition[1] = rv[1] - state[1];
      //relativePosition[2] = rv[2] - state[2];
      relativePosition[0] = - state[0];
      relativePosition[1] = - state[1];
      relativePosition[2] = - state[2];

      r3 = relativePosition[0]*relativePosition[0] +
           relativePosition[1]*relativePosition[1] +
           relativePosition[2]*relativePosition[2];

      radius = sqrt(r3);
      r3 *= radius;
      mu_r = mu / r3;

      dv[3] = relativePosition[0] * mu_r - a_indirect[0];
      dv[4] = relativePosition[1] * mu_r - a_indirect[1];
      dv[5] = relativePosition[2] * mu_r - a_indirect[2];
      #ifdef DEBUG_DERIVATIVES_FOR_SPACECRAFT
         MessageInterface::ShowMessage("***PMF AX in GD4SC: %.12lf * %.12le -"
            " %.12le = %.12le\n", relativePosition[0], mu_r, a_indirect[0], 
            dv[3]);
      #endif
   }

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("Accel = [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   return dv;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
 /**
* This method returns a clone of the PointMassForce.
 *
 * @return clone of the PointMassForce.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PointMassForce::Clone() const
{
   return (new PointMassForce(*this));
}

//------------------------------------------------------------------------------
// std::string PointMassForce::GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PointMassForce::GetParameterText(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < PointMassParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer PointMassForce::GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PointMassForce::GetParameterID(const std::string &str) const
{
   for (Integer i = PhysicalModelParamCount; i < PointMassParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType PointMassForce::GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PointMassForce::GetParameterType(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < PointMassParamCount))
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string PointMassForce::GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PointMassForce::GetParameterTypeString(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < PointMassParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterTypeString(id);
}


// All read only for now
bool PointMassForce::IsParameterReadOnly(const Integer id) const
{
   return true;
}


//------------------------------------------------------------------------------
// Real PointMassForce::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value 
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real PointMassForce::GetRealParameter(const Integer id) const
{
   if (id == MU)
      return mu;
    
   if (id == RADIUS)
      return 0.0;     // Not used in this implementation

   //if (id == flatteningParameter)
   //    return 0.0;     // Not used in this implementation

   //if (id == poleRadiusParameter)
   //    return 0.0;     // Not used in this implementation

   if (id == ESTIMATE_METHOD)
      return estimationMethod;

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real PointMassForce::SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value 
 * 
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real PointMassForce::SetRealParameter(const Integer id, const Real value)
{
   if (id == MU) 
   {
      mu = value;
      return true;
   }

   if (id == RADIUS)
      return false;   // Not used in this implementation

   if (id == ESTIMATE_METHOD) 
   {
      if ((value == 1.0) || (value == 2.0)) 
      {
         estimationMethod = value;
         return true;
      }
      return false;
   }

   return PhysicalModel::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string PointMassForce::GetStringParameter(const Integer id) const
{
//   switch (id)
//   {
//   case BODY:
//      return bodyName;
//   default:
      return PhysicalModel::GetStringParameter(id);
//   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string PointMassForce::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool PointMassForce::SetStringParameter(const Integer id, 
                                        const std::string &value)
{
   //MessageInterface::ShowMessage(
   //   "PointMassForce::SetStringParameter() id = %d, value = %s\n",
   //   id, value.c_str());

//   switch (id)
//   {
//   case BODY:
//      SetBodyName(value);
//      return true;
//   default:
      return PhysicalModel::SetStringParameter(id, value);
//   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label,
//                         const std::string &value)
//------------------------------------------------------------------------------
bool PointMassForce::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  bool value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool PointMassForce::GetBooleanParameter(const Integer id) const
{
   if (id == PRIMARY_BODY)       return isPrimaryBody;

   return PhysicalModel::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
//  bool  SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets the bool parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> bool value for the requested parameter.
 *
 * @return  the boolean set.
 *
 */
 //------------------------------------------------------------------------------
bool PointMassForce::SetBooleanParameter(const Integer id, const bool value)
{
    if (id == PRIMARY_BODY)
    {
        isPrimaryBody = value;
        return isPrimaryBody;
    }

   return PhysicalModel::SetBooleanParameter(id,value);
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative 
 * information for a specified type.
 * 
 * @param id State Element ID for the derivative type
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool PointMassForce::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "PointMassForce checking for support for id %d\n", id);
   #endif
      
   if (id == Gmat::CARTESIAN_STATE)
      return true;
   
   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;
   
   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state 
 * vector, so that the derivative information can be placed in the correct place 
 * in the derivative vector.
 * 
 * @param id         State Element ID for the derivative type
 * @param index      Starting index in the state vector for this type of derivative
 * @param quantity   Number of objects that supply this type of data
 * @param totalSize  For sizable types, the size to use.  For example, for STM,
 *                   this is the number of elements in all STMs
 * 
 * @return true if the type is supported, false otherwise. 
 */
//------------------------------------------------------------------------------
bool PointMassForce::SetStart(Gmat::StateElementId id, Integer index, 
                      Integer quantity, Integer totalSize)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("PointMassForce setting start data for id "
            "= %d to index %d; %d objects identified\n", id, index, quantity);
   #endif
   
   bool retval = false;
   
   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianStart = index;
         cartesianCount = quantity;
         fillCartesian = true;
         retval = true;
         break;
         
      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
			totalSTMSize = totalSize;
         retval = true;
         break;
         
      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
			totalSTMSize = totalSize;
         retval = true;
         break;

      default:
         break;
   }
   
   return retval;
}

//------------------------------------------------------------------------------
// Rvector3 GetTorquesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Calculates and returns the torque values resulting from a point mass
 * acting on the spacecraft at the current epoch
 *
 * @param sc The spacecraft the torques are acting on
 *
 * @return The torque vector [Tx Ty Tz]
 */
//------------------------------------------------------------------------------
Rvector3 PointMassForce::GetTorquesForSpacecraft(Spacecraft *sc)
{
   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage(
         "Entering PointMassForce::GetTorquesForSpacecraft\n");
      MessageInterface::ShowMessage(
         "Calculating torque produced from celestial body %s\n\n",
         bodyName.c_str());
   #endif

   // Get the moment of inertia tensor
   Rmatrix33 scMOITensor = sc->GetSystemMOI();

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Moment of inertia tensor of spacecraft:\n"
         "(%lf, %lf, %lf\n"
         " 0, %lf, %lf\n"
         " 0, 0, %lf)\n\n", scMOITensor(0, 0), scMOITensor(0, 1),
         scMOITensor(0, 2), scMOITensor(1, 1), scMOITensor(1, 2),
         scMOITensor(2, 2));
   #endif

   // Get the distance between the body and the spacecraft
   Real dist;
   Real *j2kState = sc->GetState().GetState();
   Real satState[6];
   epoch = sc->GetEpoch();
   epochGT = sc->GetEpochGT();

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Spacecraft j2k state:\n"
         "(%lf, %lf, %lf, %lf, %lf, %lf)\n\n", j2kState[0], j2kState[1],
         j2kState[2], j2kState[3], j2kState[4], j2kState[5]);
   #endif

   if (hasPrecisionTime)
      BuildModelStateGT(epochGT, satState, j2kState);
   else
      BuildModelState(epoch, satState, j2kState);

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Spacecraft state w.r.t. body:\n"
         "(%lf, %lf, %lf, %lf, %lf, %lf)\n\n", satState[0], satState[1],
         satState[2], satState[3], satState[4], satState[5]);
      MessageInterface::ShowMessage("Body gravitational parameter: %lf\n\n",
         body->GetGravitationalConstant());
   #endif

   dist = GmatMathUtil::Sqrt(satState[0] * satState[0] +
      satState[1] * satState[1] + satState[2] * satState[2]);

   // Nadir vector is the opposite direction of the body to sc vector
   Rvector3 nadirVec(-satState[0] / dist, -satState[1] / dist,
      -satState[2] / dist);

   // Calculate torque
   Rvector3 gravityTorque = Cross(((3.0 * body->GetGravitationalConstant() /
      pow(dist, 3.0)) * nadirVec), scMOITensor * nadirVec);

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Gravity Torque:\n"
         "(%.12e, %.12e, %.12e)\n\n", gravityTorque(0),
         gravityTorque(1), gravityTorque(2));
   #endif

   return gravityTorque;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void ShowBodyState(const std::string &header, Real time, Rvector6 &rv);
//------------------------------------------------------------------------------
void  PointMassForce::ShowBodyState(const std::string &header, Real time,
                                    Rvector6 &rv)
{
   #if DEBUG_PMF_BODY
      static int debugCount1 = 0;
      static bool showBodyState = true;
      
      if (showBodyState)
      {
         MessageInterface::ShowMessage("%s\n", header.c_str());
         MessageInterface::ShowMessage(
            ">>>>>=======================================\n");
         MessageInterface::ShowMessage("time=%f  rv=%s\n", time,
            rv.ToString().c_str());
         MessageInterface::ShowMessage(
            "=======================================<<<<<\n");
         
         debugCount1++;
         if (debugCount1 > 10)
            showBodyState = false;
      }
   #endif
}


//------------------------------------------------------------------------------
// void ShowDerivative(const std::string &header, Real *state, Integer satCount)
//------------------------------------------------------------------------------
void  PointMassForce::ShowDerivative(const std::string &header, Real *state,
                                     Integer satCount)
{
   #if DEBUG_PMF_DERV
      static int debugCount2 = 0;
      static bool showDeriv = true;
      
      if (showDeriv)
      {
         MessageInterface::ShowMessage("%s\n", header.c_str());
         MessageInterface::ShowMessage(
            ">>>>>=======================================\n");
         Integer i6;
         Rvector6 stateVec = Rvector6(state);
      
         for (Integer i = 0; i < satCount; i++) 
         {
            i6 = cartesianStart + i * 6;
            MessageInterface::ShowMessage
               ("sc#=%d  state=%s\n", i, stateVec.ToString().c_str());
         
            MessageInterface::ShowMessage
               ("deriv=%f %f %f %f %f %f\n", deriv[i6], deriv[i6+1], 
                deriv[i6+2], deriv[i6+3], deriv[i6+4], deriv[i6+5]);
         }
         MessageInterface::ShowMessage(
            "=======================================<<<<<\n");
         
         debugCount2++;
         if (debugCount2 > 10)
            showDeriv = false;
      }
   #endif
}

