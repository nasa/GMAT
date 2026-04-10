//$Id$
//------------------------------------------------------------------------------
//                              GravityField
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
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed Real to Real, int
//           to Integer, etc.
//
//           2004/11/02 D. Conway Thinking Systems, Inc.
//           Changed internal arrays to static arrays to fix memory error.
//
// @note For now, we assume
//       1) we are computing state derivatives along with any
//          STM or A-Matrix derivatives.
/**
 * This is the GravityField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shgravityfield.cpp
// *** Created   : March 17, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C/C++ Source
// Development Environment   : Borland C++ 5.02
// Modification History      : 3/17/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery.  Happy St. Patrick's Day!
// **************************************************************************

#include "GravityField.hpp"
#include "ODEModelException.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "Rvector.hpp"
#include "TimeTypes.hpp"
#include "CoordinateConverter.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "UtilityException.hpp"
#include "FileManager.hpp"
#include <sstream>                 // for <<

//#define DEBUG_GRAVITY_FIELD
//#define DEBUG_GRAVITY_FIELD_DETAILS
//#define DEBUG_FIRST_CALL
//#define DEBUG_GRAV_COORD_SYSTEM
//#define DEBUG_BODY_STATE
//#define DEBUG_GRAVITY_FILE_READ
//#define DEBUG_GRAVITY_EARTH_TIDE
//#define DEBUG_CALCULATE
//#define DEBUG_BODY_DATA
//#define DEBUG_DEGREE_ORDER
//#define DEBUG_DERIVATIVES
//#define DEBUG_CREATE_DELETE
//#define DEBUG_GRAVITY_TORQUE


using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif

const std::string
GravityField::PARAMETER_TEXT[GravityFieldParamCount - HarmonicFieldParamCount] =
{
   "Mu",
   "A",
   "TideFile",
   "TideFileFullPath",
   "TideModel",
};

const Gmat::ParameterType
GravityField::PARAMETER_TYPE[GravityFieldParamCount - HarmonicFieldParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::FILENAME_TYPE,  // "TideFile",
   Gmat::FILENAME_TYPE,  // "TideFileFullPath",
   Gmat::STRING_TYPE,
};
//------------------------------------------------------------------------------
const std::string GravityField::GRAVITY_MODEL_NAMES[NumGravityModels] =
   {
      // Earth Model names
      "EGM-96",
      "JGM-2",
      "JGM-3",
      // Luna Model Names
      "LP-165",
      // Mars Model Names
      "Mars-50C",
      // Venus Model Names
      "MGNP-180U",
      "Other",
      "None"
   };
//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GravityField(const std::string &name, const std::string &bodyName,
//               Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class
 * (default constructor).
 *
 * @param <name> parameter indicating the name of the object.
 * @param <deg>  maximum degree of the polynomials.
 * @param <ord>  maximum order of the polynomials.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const std::string &name, const std::string &forBodyName,
                           Integer maxDeg, Integer maxOrd) :
   HarmonicField          (name, "GravityField", maxDeg, maxOrd),
   mu                     (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
   a                      (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   tideFilename           (""),
   tideFilenameFullPath   (""),
   TideModel              ("None"),
   defaultMu              (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
   defaultA               (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   gfInitialized          (false),
   orderTruncateReported  (false),
   degreeTruncateReported (false),
   offsetBodyOrigin       (false),
   gravityModel           (NULL),
   j2k                    (NULL)
{
   objectTypeNames.push_back("GravityField");
   objectTypeNames.push_back("Gravity");
   bodyName = forBodyName;
   parameterCount = GravityFieldParamCount;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}


//------------------------------------------------------------------------------
//  ~GravityField()
//------------------------------------------------------------------------------
/**
 * This method destroys the GravityField object
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
GravityField::~GravityField()
{
   if (gravityModel)
      delete gravityModel;
   
   if (j2k)
      delete j2k;
}


//------------------------------------------------------------------------------
//  GravityField(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class from the input
 * object.
 * (copy constructor).
 *
 * @param <gf> object to copy to create this new object.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const GravityField &gf) :
    HarmonicField          (gf),
    mu                     (gf.mu),
    a                      (gf.a),
    tideFilename           (gf.tideFilename),
    tideFilenameFullPath   (gf.tideFilenameFullPath),
    TideModel              (gf.TideModel),
    defaultMu              (gf.defaultMu),
    defaultA               (gf.defaultA),
    gfInitialized          (false),
    orderTruncateReported  (gf.orderTruncateReported),
    degreeTruncateReported (gf.degreeTruncateReported),
    offsetBodyOrigin       (gf.offsetBodyOrigin),
    gravityModel           (NULL),
    j2k                    (NULL),
    frv                    (gf.frv),
    trv                    (gf.trv),
    now                    (gf.now),
    nowGT                  (gf.nowGT)
{
   objectTypeNames.push_back("GravityField");
   bodyName = gf.bodyName;


   parameterCount = GravityFieldParamCount;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);

   isInitialized = false;
}


//------------------------------------------------------------------------------
//  GravityField& operator=(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method sets the 'this' object to the value(s) of the input
 * object.
 *
 * @param <gf> object whose values to copy to this object.
 */
//------------------------------------------------------------------------------
GravityField& GravityField::operator=(const GravityField &gf)
{
   if (&gf == this)
      return *this;

   HarmonicField::operator=(gf);
   mu                     = gf.mu;
   a                      = gf.a;
   tideFilename           = gf.tideFilename;
   tideFilenameFullPath   = gf.tideFilenameFullPath;
   TideModel              = gf.TideModel;
   defaultMu              = gf.defaultMu;
   defaultA               = gf.defaultA;
   bodyName               = gf.bodyName;
   gfInitialized          = false;  // is that what I want to do?
   orderTruncateReported  = gf.orderTruncateReported;
   degreeTruncateReported = gf.degreeTruncateReported;
   offsetBodyOrigin       = gf.offsetBodyOrigin;
//   if ((gravityModel) && (gravityModel->GetFilename() == "")) delete gravityModel;  // delete only Body ones
   gravityModel           = gf.gravityModel;
//   gravityModel           = NULL;
   j2k                    = NULL;
   frv                    = gf.frv;
   trv                    = gf.trv;
   now                    = gf.now;
   nowGT                  = gf.nowGT;

   isInitialized = false;

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the object.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::Initialize()
{
   if (gfInitialized && hMinitialized) return true;
   if (!HarmonicField::Initialize())
   {
      throw ODEModelException(
            "GravityField: Legendre Polynomial initialization failed!");
   }

   degreeTruncateReported = false;
   orderTruncateReported  = false;

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   // JPD code added here
   if (!body)
   {
      body = solarSystem->GetBody(bodyName);
      if (!body)
         throw ODEModelException("Body \"" + bodyName +
               "\" undefined for Gravity Field.");
   }
   
   if (!j2k)
   {
      CelestialBody* earth = solarSystem->GetBody("Earth");
      if (!earth) throw ODEModelException("Earth undefined for Gravity Field.");
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("EarthMJ2000Eq", "MJ2000Eq",
                              earth, NULL, NULL, earth, solarSystem);
   }
   
//   // The required distance from the origin to avoid numerical instability, this
//   // value is compared to the L1 distance norm in GetDerivatives to make sure
//   // that the offset is acceptable.  This piece is currently disabled
//   minimumDistance = body->GetEquatorialRadius() * 0.5;

   if (body->GetName() != fixedCS->GetOriginName())
   {
      offsetBodyOrigin = true;
   }

   if ((!fileRead) || (!gravityModel))
   {
      Integer fileDegree = 0;
      Integer fileOrder  = 0;
      mu = body->GetGravitationalConstant();
      a  = body->GetEquatorialRadius();
      if (IsBlank(filename.c_str()))
      {
         std::string errmsg = "There is no gravity file specified for GravityField ";
         errmsg += instanceName + "\n";
         throw ODEModelException(errmsg);
      }
      else
      {
         #ifdef DEBUG_GRAVITY_FILE_READ
            MessageInterface::ShowMessage("Now getting HarmonicGravity with filename = %s, a = %12.10f, mu = %12.10f\n",
                  filename.c_str(), a, mu);
         #endif

         // Changed to open filenameFullPath (LOJ: 2014.06.26)
         //gravityModel = GetGravityFile(filename,a,mu);
         gravityModel = GetHarmonicGravity(filenameFullPath,tideFilenameFullPath,a,mu,body->GetName(), true);
         if (!gravityModel)
         {
            std::string errmsg = "Gravity file ";
            //errmsg += filename + " cannot be opened or read.\n";
            errmsg += filenameFullPath + " cannot be opened or read.\n";
            throw ODEModelException(errmsg);
         }
         else
         {
            #ifdef DEBUG_GRAVITY_FILE_READ
               MessageInterface::ShowMessage("--- HarmonicGravity object retrieved is <%p>\n", gravityModel);
            #endif
            fileDegree =   gravityModel->GetNN();
            fileOrder  =   gravityModel->GetMM();
            mu         = - gravityModel->GetFactor();
            a          =   gravityModel->GetFieldRadius();
         }
      }
      // truncate the degree and/or order, if necessary
      #ifdef DEBUG_DEGREE_ORDER
         MessageInterface::ShowMessage("for body %s, fileDegree = %d, degree = %d\n",
               (body->GetName()).c_str(), fileDegree, degree);
         MessageInterface::ShowMessage("for body %s, fileOrder  = %d, order  = %d\n",
               (body->GetName()).c_str(), fileDegree, degree);
      #endif
      if (fileDegree < degree)   // for now, do not truncate ...
      {
         std::stringstream ss("");
         ss << "In GravityField " << instanceName << ", requested degree (";
         ss << degree << ") is greater than degree (" << fileDegree;
         ss << ") on file " << filename << std::endl;
         throw ODEModelException(ss.str());
      }
      if (fileOrder < order)
      {
         std::stringstream ss("");
         ss << "In GravityField " << instanceName << ", requested order (";
         ss << order << ") is greater than order (" << fileOrder;
         ss << ") on file " << filename << std::endl;
         throw ODEModelException(ss.str());
      }
      if (order > degree)
      {
		 std::stringstream ss("");
         ss << "*** WARNING *** In " + GetName();
		 ss << " the Order (" 
         << order 
         << ") is greater than Degree ("
         << degree 
         << ") so truncating Order to ";
         MessageInterface::ShowMessage("%s %d\n",ss.str().c_str(), degree);
		 order = degree;
      }
      if (degree < 0)
         throw ODEModelException("Invalid degree in GravityField: Degree < 0\n");
      if (order < 0)
         throw ODEModelException("Invalid order in GravityField: Degree < 0\n");
      fileRead = true;
   }
   #ifdef DEBUG_BODY_DATA
      MessageInterface::ShowMessage("for body %s, mu = %12.10f, a = %12.10f\n",
                                   (body->GetName()).c_str(), mu, a);
   #endif

   gfInitialized = true;
   return true;
}
//------------------------------------------------------------------------------
//  bool GetDerivatives(Real * state, Real dt, int dvorder)
//------------------------------------------------------------------------------
/**
* This method initializes the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::GetDerivatives(Real * state, Real dt, Integer dvorder,
      const Integer id)
{
   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GravityField(%s) inputs:\n"
            "   state = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n"
            "   dt = %.10lf\n   dvorder = %d\n",
            instanceName.c_str(), state[0], state[1], state[2], state[3],
            state[4], state[5], dt, dvorder);
      }
   #endif

   // We may want to do this down the road:
//   if (fabs(state[0]) + fabs(state[1]) + fabs(state[2]) < minimumDistance)
//      throw ODEModelException("A harmonic gravity field is being computed "
//            "inside of the " + bodyName + ", which is not allowed");

   if ((dvorder > 2) || (dvorder < 1))
      return false;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("%s %d %s %le %s  %le %le %le %le %le %le\n",
          "Entered GravityField::GetDerivatives with order", dvorder, "dt = ",
          dt, "and state\n",
          state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("cartesianCount = %d, stmCount = %d, aMatrixCount = %d\n",
            cartesianCount, stmCount, aMatrixCount);
      MessageInterface::ShowMessage("fillCartesian = %s, fillSTM = %s, fillAMatrix = %s\n",
            (fillCartesian? "true" : "false"), (fillSTM? "true" : "false"), (fillAMatrix? "true" : "false"));
      MessageInterface::ShowMessage("cartesianStart = %d, stmStart = %d, aMatrixStart = %d\n",
            cartesianStart, stmStart, aMatrixStart);
   #endif

/// @todo Optimize this code -- May be possible to make GravityField calculations more efficient


   if ((cartesianCount < 1)  && (stmCount < 1) && (aMatrixCount < 1))
      throw ODEModelException(
         "GravityField requires at least one spacecraft.");

   // todo: Move into header; this flag is used to decide if the velocity terms
   // are copied into the position derivatives for first order integrators, so
   // when the GravityField is set to work at non-central bodies, the detection
   // will need to happen in initialization.
   Real satState[6];
   Integer nOffset;

   if (hasPrecisionTime)
   {
      nowGT = epochGT;
      nowGT.AddSeconds(elapsedTime);
      nowGT.AddSeconds(dt);
   }
   else
      now = epoch + (elapsedTime + dt)/GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_GRAV_COORD_SYSTEM
       MessageInterface::ShowMessage(
         "------ body = %s\n------ inputCS = %s\n%s\n------ targetCS = %s\n%s"
         "\n------ fixedCS = %s\n%s\n",
         body->GetName().c_str(),     (inputCS == NULL? "NULL" : inputCS->GetName().c_str()), inputCS->GetGeneratingString(Gmat::NO_COMMENTS).c_str(),
         (targetCS == NULL? "NULL" : targetCS->GetName().c_str()), targetCS->GetGeneratingString(Gmat::NO_COMMENTS).c_str(), (fixedCS == NULL? "NULL" : fixedCS->GetName().c_str()), fixedCS->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif



   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
    	 CelestialBody *targetBody = (CelestialBody*) targetCS->GetOrigin();
    	 CelestialBody *fixedBody  = (CelestialBody*) fixedCS->GetOrigin();
         MessageInterface::ShowMessage(
            "   Epoch = %.12lf\n   targetBody = %s\n   fixedBody = %s\n",
            now.Get(), targetBody->GetName().c_str(),
            fixedBody->GetName().c_str());
         MessageInterface::ShowMessage(
            "------ body = %s\n------ inputCS = %s\n------ targetCS = %s\n"
            "------ fixedCS = %s\n",
            body->GetName().c_str(),     inputCS->GetName().c_str(),
            targetCS->GetName().c_str(), fixedCS->GetName().c_str());
      }
   #endif


   if (fillCartesian || fillAMatrix || fillSTM)
   {
      // See assumption 1, above
      if ((cartesianCount < stmCount) || (cartesianCount < aMatrixCount))
      {
         throw ODEModelException("GetDerivatives: cartesianCount < stmCount or aMatrixCount\n");
      }
      Real originacc[3] = { 0.0,0.0,0.0 };  // JPD code
      Rmatrix33 origingrad (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      Rmatrix33 emptyGradient(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      Rmatrix33 gradnew (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      if (body != forceOrigin)
      {
         Real originstate[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
         Calculate(dt,originstate,originacc,origingrad);
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> origingrad = %s\n", origingrad.ToString().c_str());
#endif
      }


		Integer i6 = (fillSTM ? stmStart : aMatrixStart);
      for (Integer n = 0; n < cartesianCount; ++n)
      {
         nOffset = cartesianStart + n * stateSize;
         for (Integer i = 0; i < 6; ++i)
            satState[i] = state[i+nOffset];

         Real accnew[3];  // JPD code
         gradnew = emptyGradient;
         Calculate(dt,satState,accnew,gradnew);
         if (body != forceOrigin)
         {
            for (Integer i=0;  i<=2;  ++i)
               accnew[i] -= originacc[i];
            gradnew -= origingrad;
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> body not equal to forceOrigin\n");
#endif
         }
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> gradnew (%d) = %s\n", n, gradnew.ToString().c_str());
#endif
         
         // Fill Derivatives
         switch (dvorder)
         {
            case 1:
               deriv[0+nOffset] = satState[3];
               deriv[1+nOffset] = satState[4];
               deriv[2+nOffset] = satState[5];
               deriv[3+nOffset] = accnew[0];
               deriv[4+nOffset] = accnew[1];
               deriv[5+nOffset] = accnew[2];
               break;

            case 2:
               deriv[0+nOffset] = accnew[0];
               deriv[1+nOffset] = accnew[1];
               deriv[2+nOffset] = accnew[2];
               deriv[3+nOffset] = 0.0;
               deriv[4+nOffset] = 0.0;
               deriv[5+nOffset] = 0.0;
               break;
         }

         for (Integer index = 0; index < 6; ++index)
         {

            // This message needs to show up when the derivative is NaN
            if (GmatMathUtil::IsNaN(deriv[nOffset + index]))
            {
               std::string derivName = "acceleration";
               if ((dvorder == 1)&& (index < 3))
                  derivName = "velocity";

               MessageInterface::ShowMessage("The force model \"%s\" "
                  "returns a disallowed value for spacecraft \"%s\". "
                  "The %s value is %lf for the position vector "
                  "[%.12lf  %.12lf  %.12lf]km at epoch %.12lf A1Mjd.\n",
                  GetTypeName().c_str(), scObjs[n]->GetName().c_str(),
                  derivName.c_str(), deriv[nOffset + index],
                  satState[0], satState[1], satState[2], (hasPrecisionTime ? epochGT.GetMjd() : epoch));
            }
         }


#ifdef DEBUG_DERIVATIVES
         for (Integer ii = 0 + nOffset; ii < 6+nOffset; ii++)
                     MessageInterface::ShowMessage("------ FM deriv[%d] = %12.10le\n", ii, deriv[ii]);
#endif
         if (fillSTM)
         {
            Integer element;
            // @todo Add the use of the GetAssociateIndex() method here to get index into state array
            //       (See assumption 1, above)
            if (n <= stmCount)
            {
					Integer ix;

					// Get spacecraft object
					Spacecraft* sc = (Spacecraft*)scObjs[n];

					//Create aTilde
					stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
					Integer stmSize = stmRowCount * stmRowCount;
					Real *aTilde;
					aTilde = new Real[stmSize];
               for (Integer i = 0; i < stmRowCount; ++i)
               {
                  ix = i * stmRowCount;
                  for (Integer j = 0; j < stmRowCount; ++j)
                     aTilde[ix+j] = 0.0;
               }

					// Calculate A-tilde
					// fill in the lower left quadrant of the upper 6x6 with the calculated gradient values
               ix = stmRowCount * 3;
               aTilde[ix]   = gradnew(0,0);
               aTilde[ix+1] = gradnew(0,1);
               aTilde[ix+2] = gradnew(0,2);
               ix = stmRowCount * 4;
               aTilde[ix]   = gradnew(1,0);
               aTilde[ix+1] = gradnew(1,1);
               aTilde[ix+2] = gradnew(1,2);
               ix = stmRowCount * 5;
               aTilde[ix]   = gradnew(2,0);
               aTilde[ix+1] = gradnew(2,1);
               aTilde[ix+2] = gradnew(2,2);

               for (Integer j = 0; j < stmRowCount; j++)
               {
                  for (Integer k = 0; k < stmRowCount; k++)
                  {
                     element = j * stmRowCount + k;
#ifdef DEBUG_DERIVATIVES
                     MessageInterface::ShowMessage("------ FM deriv[%d] = %12.10le\n", (i6+element), aTilde[element]);
#endif
                     deriv[i6+element] = aTilde[element];
                  }
               }

					delete[] aTilde;
					i6 = i6 + stmSize;
            }
         }

         if (fillAMatrix)
         {
            Integer element;
            // @todo Add the use of the GetAssociateIndex() method here to get index into state array
            //       (See assumption 1, above)
            if (n <= aMatrixCount)
            {
					Integer ix;

					// Get spacecraft object
					Spacecraft* sc = (Spacecraft*)scObjs[n];

					//Create aTilde
					stmRowCount = sc->GetIntegerParameter("FullSTMRowCount");
					Integer stmSize = stmRowCount * stmRowCount;
					Real *aTilde;
					aTilde = new Real[stmSize];
               for (Integer i = 0; i < stmRowCount; ++i)
               {
                  ix = i * stmRowCount;
                  for (Integer j = 0; j < stmRowCount; ++j)
                     aTilde[ix+j] = 0.0;
               }

					// Calculate A-tilde
					// fill in the lower left quadrant of the upper 6x6 with the calculated gradient values
               ix = stmRowCount * 3;
               aTilde[ix]   = gradnew(0,0);
               aTilde[ix+1] = gradnew(0,1);
               aTilde[ix+2] = gradnew(0,2);
               ix = stmRowCount * 4;
               aTilde[ix]   = gradnew(1,0);
               aTilde[ix+1] = gradnew(1,1);
               aTilde[ix+2] = gradnew(1,2);
               ix = stmRowCount * 5;
               aTilde[ix]   = gradnew(2,0);
               aTilde[ix+1] = gradnew(2,1);
               aTilde[ix+2] = gradnew(2,2);

               for (Integer j = 0; j < stmRowCount; j++)
               {
                  for (Integer k = 0; k < stmRowCount; k++)
                  {
                     element = j * stmRowCount + k;
#ifdef DEBUG_DERIVATIVES
                     MessageInterface::ShowMessage("------ FM deriv[%d] = %12.10le\n", (i6+element), aTilde[element]);
#endif
                     deriv[i6+element] = aTilde[element];
                  }
               }

					delete[] aTilde;
					i6 = i6 + stmSize;
				}
         }

      }  // end for
   }

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         if (body->GetName() == "Mars")
         {
         MessageInterface::ShowMessage(
            "   GravityField[%s <> %s] --> mu = %lf, origin = %s, [%.10lf %.10lf "
            "%.10lf %.16lf %.16lf %.16lf]\n",
            instanceName.c_str(), body->GetName().c_str(), mu,
            targetCS->GetOriginName().c_str(),
            deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
         firstCallFired = true;
         }
      }
   #endif

   return true;
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
Rvector6 GravityField::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   Rvector6 dv;
   Real satState[6];

   Real theEpoch = sc->GetEpoch();
   GmatTime theEpochGT = sc->GetEpochGT();
   Real *j2kState = sc->GetState().GetState();

   epoch = theEpoch;
   epochGT = theEpochGT;

   Rmatrix33 gradnew (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

   for (Integer i = 0; i < 6; ++i)
      satState[i] = j2kState[i];

   // Make sure to use the state relative to the force model origin
   Rvector6 ostate = forceOrigin->GetMJ2000State(theEpoch);

   satState[0] -= ostate[0];
   satState[1] -= ostate[1];
   satState[2] -= ostate[2];
   satState[3] -= ostate[3];
   satState[4] -= ostate[4];
   satState[5] -= ostate[5];

   Real accnew[3];
   Calculate(0.0, satState, accnew, gradnew);

   // Translate to the force origin
   if ((body != forceOrigin) && (inputCS->GetOriginName() != fixedCS->GetOriginName()))
   {
      Real originstate[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
      Real originacc[3] = {0.0, 0.0, 0.0};
      Rmatrix33 origingrad (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      Calculate(0.0, originstate, originacc, origingrad);
      for (Integer i=0;  i<=2;  ++i)
         accnew[i] -= originacc[i];
      gradnew -= origingrad;
   }

   // Fill Derivatives
   dv[0] = satState[3];
   dv[1] = satState[4];
   dv[2] = satState[5];
   dv[3] = accnew[0];
   dv[4] = accnew[1];
   dv[5] = accnew[2];

   #ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("spacecraft deriv = [");
      for (Integer ii = 0; ii < 6; ++ii)
      {
         if (ii > 0)
            MessageInterface::ShowMessage(", ");
         MessageInterface::ShowMessage("%12.10le", dv[ii]);
      }
      MessageInterface::ShowMessage("]\n");
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("GravityField Accel: [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   epoch = theEpoch;
   epochGT = theEpochGT;

   return dv;
}


//------------------------------------------------------------------------------
//  bool GetBodyAndMu(std::string &itsName, Real &itsMu)
//------------------------------------------------------------------------------
/**
* This method returns the body name and mu value (from the gravity file).
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::GetBodyAndMu(std::string &itsName, Real &itsMu)
{
   itsName = bodyName;
   itsMu   = mu;
   return true;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GravityField.
 *
 * @return clone of the GravityField.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GravityField::Clone(void) const
{
   return (new GravityField(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterText(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TEXT[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer     GravityField::GetParameterID(const std::string &str) const
{
   // Write deprecated message once per GMAT session
   static bool tideModelFirstWarning = true;

   if (str == "EarthTideModel")
   {
      if (tideModelFirstWarning)
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"EarthTideModel\" field of GravityField is "
             "deprecated and will be removed from a future build; "
             "please use \"TideModel\" instead.\n");
         tideModelFirstWarning = false;
      }
      return TIDE_MODEL;
   }

   for (Integer i = HarmonicFieldParamCount; i < GravityFieldParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HarmonicFieldParamCount])
         return i;
   }
   return HarmonicField::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType
GravityField::GetParameterType(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TYPE[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
bool GravityField::IsParameterReadOnly(const Integer id) const
{
   // All read only for now except degree and order
   if (id < HarmonicFieldParamCount)
      return HarmonicField::IsParameterReadOnly(id);
   
   if (id == TIDE_FILENAME)
      return false;

   if ((id == TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::EARTH_NAME)) return false;
   if ((id == TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::MOON_NAME)) return false;
   if ((id == TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::MERCURY_NAME)) return false;
   if ((id == TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::VENUS_NAME)) return false;
   if ((id == TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::MARS_NAME)) return false;

   return true;
}


//------------------------------------------------------------------------------
// bool IsParameterValid(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GravityField::IsParameterValid(const Integer id,
                                     const std::string &value)
{
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("GravityField::IsParameterValid() entered, id=%d, value='%s'\n", id, value.c_str());
   #endif
   
   bool retval = true;
   if (id == TIDE_FILENAME)
   {
      #ifdef DEBUG_VALIDATION
      MessageInterface::ShowMessage("   Validating TIDE_FILENAME\n");
      #endif
      if (!SetTideFilename(value, true))
         retval = false;
}
   
   #ifdef DEBUG_VALIDATION
   MessageInterface::ShowMessage
      ("GravityField::IsParameterValid() returning %d\n", retval);
   #endif
   return retval;
}

//------------------------------------------------------------------------------
// bool IsParameterValid(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* @see GmatBase
*/
//------------------------------------------------------------------------------
bool GravityField::IsParameterValid(const std::string &label,
    const std::string &value)
{
    return IsParameterValid(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::GetRealParameter(const Integer id) const
{
   if (id == MU)       return mu;
   if (id == A)        return a;

   return HarmonicField::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::SetRealParameter(const Integer id,
                                           const Real value)
{
   if (id == MU)       return (mu     = value);
   if (id == A)        return (a      = value);

   return HarmonicField::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real GravityField::SetRealParameter(const std::string &label,
                                    const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
std::string GravityField::GetStringParameter(const Integer id) const
{
   if (id == TIDE_MODEL) return TideModel;
   else if (id == TIDE_FILENAME) return tideFilename;
   else if (id == TIDE_FILE_FULLPATH) return tideFilenameFullPath;
   return HarmonicField::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// Real SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
bool GravityField::SetStringParameter(const Integer id,
                                      const std::string &value)
{
   if (id == TIDE_FILENAME)
   {
      #ifdef DEBUG_HARMONIC_FIELD_FILENAME
      MessageInterface::ShowMessage
         ("HarmonicField::SetStringParameter() new TideFile = '%s'\n", value.c_str());
      #endif

      if (GmatStringUtil::IsBlank(value))
      {
         return SetTideFilename(value);
      }

      std::string newValue = value;

      // if value has no file extension, add .tide as default (loj: 2008.10.14)
      if (value.find(".") == value.npos)
         newValue = value + ".tide";

      return SetTideFilename(newValue);
   }
   if (id == TIDE_MODEL)
   {
       HarmonicGravity *gm = GetHarmonicGravity(filenameFullPath, tideFilenameFullPath, a, mu, bodyName, true);

       if (gm != NULL)
       {
           for (int i = 0; i <= HarmonicGravity::ETideCount - 1; ++i)
           {
               if (HarmonicGravity::ETideString[i] == value)
               {
                   if (gm->HaveTideModel(i))
                   {
                       if (HarmonicGravity::ETideString[i] != "None" && gm->IsZeroTide() && !gm->IsTideFree())
                       {
                           MessageInterface::ShowMessage
                              ("*** WARNING *** It appears tide modeling is being used with a zero-tide potential model. "
                               "This may cause an undesired double-counting of the permanent tide effect. "
                               "Please see the GMAT User's Guide for more information.\n");
                       }
                       TideModel = value;
                       delete gm;
                       return true;
                   }
                   else
                   {
                      if (value == "SolidAndPole")
                      {
                         delete gm;
                         throw ODEModelException(
                            "The selected PrimaryBody does not support the \"" +
                            value + "\" tide option.");
                      }
                      else
                      {
                         delete gm;
                         throw ODEModelException(
                            "The selected PrimaryBody or loaded tide model "
                            "does not support the \"" +value + "\" tide option.");
                      }
                   }
               }
           }
           delete gm;
           throw ODEModelException("The selected tide option \"" +
               value + "\" is invalid.");
       }
       else
       {
           throw ODEModelException("Unable to select tide option to use "
               "until tide model is loaded.");
       }
   }
   return HarmonicField::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
std::string GravityField::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string &label, const char *value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
bool GravityField::SetStringParameter(const std::string &label,
                                      const char *value)
{
   return SetStringParameter(GetParameterID(label), std::string(value));
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string &label, const std::string value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
bool GravityField::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
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
bool GravityField::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "GravityField checking for support for id %d\n", id);
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
 *                   this is the number of STM's elements
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool GravityField::SetStart(Gmat::StateElementId id, Integer index,
                      Integer quantity, Integer totalSize)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("GravityFiels setting start data for id = "
            "%d to index %d; %d objects identified\n", id, index, quantity);
   #endif

   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
//         satCount       = quantity;
         cartesianCount = quantity;
         cartesianStart = index;
         fillCartesian  = true;
         retval         = true;
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount       = quantity;
         stmStart       = index;
         fillSTM        = true;
			totalSTMSize   = totalSize;
         retval         = true;
         break;

      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount   = quantity;
         aMatrixStart   = index;
         fillAMatrix    = true;
			totalSTMSize = totalSize;
         retval         = true;
         break;

      default:
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::IsBlank(char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}
//--------------------------------------------------------------------
bool GravityField::IsBlank(const std::string &aLine)
{
   Integer i;
   for (i=0;i<(int)aLine.length();i++)
   {
      if (!isspace(aLine.at(i))) return false;
   }
   return true;
}
//--------------------------------------------------------------------
void GravityField::InverseRotate (Rmatrix33& rot, const Real in[3], Real out[3])
{
   const Real *rm = rot.GetDataVector();
   const Real  rmt[9] = {rm[0], rm[3], rm[6],
      rm[1], rm[4], rm[7],
      rm[2], rm[5], rm[8]};
   for (Integer p = 0; p < 3; ++p)
   {
      Integer p3 = 3*p;
      out[p] = rmt[p3]   * in[0] +
      rmt[p3+1] * in[1] +
      rmt[p3+2] * in[2];
   }
   
}
//------------------------------------------------------------------------------
void GravityField::GetTideData (Real dt, const std::string bodyname, 
   Real pos[3], Real& mukm)
{
   
   CelestialBody* body  = solarSystem->GetBody(bodyname);
   if (!body)
      throw ODEModelException("Solar system does not contain the " + bodyname + 
         " Sun for Tide model.");


   Rvector6 stateinertial;
   if (hasPrecisionTime)
      stateinertial = body->GetMJ2000State(nowGT);
   else
      stateinertial = body->GetMJ2000State(now);

   Rvector6 state;
   if (!j2k)
   {
      throw ODEModelException("Inertial coordinate system was not initialized.");
   }

   //BodyFixed coordinate should use the correct gravity frame and not a user defined frame
   fixedCS->SetForGravityUpdate(true);
   
   if (hasPrecisionTime)
      cc.Convert(nowGT, stateinertial, j2k, state, fixedCS);
   else
      cc.Convert(now, stateinertial, j2k, state, fixedCS);
   
   //BodyFixed coordinate should use the correct gravity frame and not a user defined frame
   fixedCS->SetForGravityUpdate(false);
   
   state.GetR(pos);
   mukm   = body->GetGravitationalConstant();
}


//------------------------------------------------------------------------------
void GravityField::Calculate (Real dt, Real state[6], 
                              Real acc[3], Rmatrix33& grad)
{
   #ifdef DEBUG_CALCULATE
      MessageInterface::ShowMessage(
            "Entering Calculate with dt = %12.10f, state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            dt, state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("   acc = %12.10f  %12.10f  %12.10f\n", acc[0], acc[1], acc[2]);
   #endif
   
   Real jday, now;
   GmatTime jdayGT, nowGT;
   if (hasPrecisionTime)
   {
      jdayGT = epochGT + GmatTimeConstants::JD_JAN_5_1941;
      jdayGT.AddSeconds(elapsedTime);
      jdayGT.AddSeconds(dt);
      nowGT = epochGT; nowGT.AddSeconds(elapsedTime); nowGT.AddSeconds(dt);
   }
   else
   {
      jday = epoch + GmatTimeConstants::JD_JAN_5_1941 +
         (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
      now = epoch + (elapsedTime + dt) / GmatTimeConstants::SECS_PER_DAY;
      jdayGT = jday;
      nowGT = now;
   }

   // convert to body fixed coordinate system
   Real tmpState[6];

   //BodyFixed coordinate should use the correct gravity frame and not a user defined frame
   fixedCS->SetForGravityUpdate(true);
   if (hasPrecisionTime)
      cc.Convert(nowGT, state, inputCS, tmpState, fixedCS);  // which CSs to use here???
   else
      cc.Convert(now, state, inputCS, tmpState, fixedCS);  // which CSs to use here???
   //BodyFixed coordinate should use the correct gravity frame and not a user defined frame
   fixedCS->SetForGravityUpdate(false);
   #ifdef DEBUG_CALCULATE
      MessageInterface::ShowMessage(
            "After Convert, jday = %s, now = %s, and tmpState = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            jdayGT.ToString().c_str(), nowGT.ToString().c_str(), tmpState[0], tmpState[1], tmpState[2], tmpState[3], tmpState[4], tmpState[5]);
   #endif
   Rmatrix33 rotMatrix = cc.GetLastRotationMatrix();
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---->>>> rotMatrix = %s\n", rotMatrix.ToString().c_str());
   #endif
   // tide body pos and mu
   Real sunpos[3]   = {0.0,0.0,0.0};
   Real otherpos[3] = {0.0,0.0,0.0};
   Real sunmukm     = 0.0;
   Real othermukm   = 0.0; 
   // Acceleration
   Real      rotacc[3];
   Rmatrix33 rotgrad;
   Integer   tideLevel = -1;
   if (gravityModel != NULL)
      {
      for (int i=0;  i<=HarmonicGravity::ETideCount-1;  ++i)
         if (HarmonicGravity::ETideString[i] == TideModel)
            if (gravityModel->HaveTideModel(i))
               tideLevel = i;
      }
   else
      tideLevel = 0;

   if (tideLevel > 0)
      {
      // Always do Sun
      GetTideData (dt,GmatSolarSystemDefaults::SUN_NAME,sunpos,sunmukm);
      // Other is central body for any moon, or Moon for Earth
      std::string cb = body->GetCentralBody();
      if (bodyName == GmatSolarSystemDefaults::EARTH_NAME) // We are Earth, do moon
         GetTideData (dt,GmatSolarSystemDefaults::MOON_NAME,otherpos,othermukm);
      else if (cb == GmatSolarSystemDefaults::SUN_NAME) 
         ;   // We are other planet, no big moon
      else
         {
         GetTideData (dt,cb,otherpos,othermukm);  // We are a moon, do planet(cb)
         }
      }
   // Get xp and yp from the EOP file
   Real xp, yp, lod;
   Real utcmjd;
   GmatTime utcmjdGT;
   if (hasPrecisionTime)
   {
      utcmjdGT = theTimeConverter->Convert(nowGT, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
         GmatTimeConstants::JD_JAN_5_1941);
      eop->GetPolarMotionAndLod(utcmjdGT, xp, yp, lod);
   }
   else
   {
      utcmjd = theTimeConverter->Convert(now, TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
         GmatTimeConstants::JD_JAN_5_1941);
      eop->GetPolarMotionAndLod(utcmjd, xp, yp, lod);
   }

   bool computeMatrix = fillAMatrix || fillSTM;

   if (hasPrecisionTime)
      gravityModel->CalculateFullField(jdayGT.GetMjd(), tmpState, degree, order, tideLevel,
         sunpos, sunmukm, otherpos, othermukm,
         xp, yp, computeMatrix, stmLimit, rotacc, rotgrad);
   else
      gravityModel->CalculateFullField (jday, tmpState, degree, order, tideLevel, 
         sunpos, sunmukm, otherpos, othermukm,
         xp, yp, computeMatrix, stmLimit, rotacc, rotgrad);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("after CalculateFullField, rotgrad = %s\n", rotgrad.ToString().c_str());
   #endif
   /*
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate pos= %20.14f %20.14f %20.14f\n",
    tmpState[0],tmpState[1],tmpState[2]);
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(0,0),rotgrad(0,1),rotgrad(0,2));
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(1,0),rotgrad(1,1),rotgrad(1,2));
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(2,0),rotgrad(2,1),rotgrad(2,2));
    */
   
   // Convert back to target CS
   InverseRotate (rotMatrix,rotacc,acc);
   grad = rotMatrix.Transpose() * rotgrad * rotMatrix;
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("at end of Calculate, after rotation, grad = %s\n", grad.ToString().c_str());
   #endif
}
//------------------------------------------------------------------------------
// GmatGrav::GravityModelType GetModelType(const char *filename, const char *forBody)
//------------------------------------------------------------------------------
GravityField::GravityModelType GravityField::GetModelType
   (const char *filename, const char *forBody)
{
   return GetModelType(std::string(filename), std::string(forBody));
}

//------------------------------------------------------------------------------
// GmatGrav::GravityModelType GetModelType(const std::string &filename, const std::string &forBody)
//------------------------------------------------------------------------------
/*
 * Returns recognized gravity model type for input body name, by reading
 * comment line(s).  NOTE - this will only work for COF format files.  Inputting
 * the name of a file with another format (e.g. GRV) will result in a return value
 * of GFM_OTHER.
 *
 * Assumptions:
 *    GFM_EGM96       contains "egm96"
 *    GFM_JGM2        contains "JGM-02"
 *    GFM_JGM3        contains "JGM-03"
 *    GFM_LP165P      contains "lp165p"
 *    GFM_GMM1        contains "GMM-1"        [future?]
 *    GFM_GMM2B       contains "GMM-2B"       [future?]
 *    GFM_MARS50C     contains "Mars-50c"
 *    GFM_MGN75HSAAP  contains "MGN75HSAAP"   [future?]
 *    GFM_MGNP180U    contains "MGNP180U"
 *
 *
 * @param   filename  Input file name
 * @param   forBody   Body Name
 * @return  Gravity model type
 *
 */
//------------------------------------------------------------------------------
GravityField::GravityModelType GravityField::GetModelType 
   (const std::string &filename, const std::string &forBody)
{
   #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("Entering GetModelType, filename = %s, forBody = %s\n",
            filename.c_str(), forBody.c_str());
   #endif
   if (GmatStringUtil::IsBlank(filename, true))
     return GFM_NONE;

   if (filename.find(".cof") == std::string::npos) return GFM_OTHER;

   std::ifstream inStream (filename.c_str());
   if (!inStream)
      throw GravityFileException("Cannot open gravity file \"" + filename + "\"");

   std::string line;
   GravityModelType gfm = GFM_OTHER;

   while (!inStream.eof())
   {
      getline(inStream, line);

      // Make upper case, so we can check for certain keyword
      line = GmatStringUtil::ToUpper(line);

      #ifdef DEBUG_GRAVITY_FILE
      MessageInterface::ShowMessage("   => line=<%s>\n", line.c_str());
      #endif

      // Read comment lines
      if (line[0] == 'C' || line[0] == '#') // do we need
      {
         if (forBody == GmatSolarSystemDefaults::EARTH_NAME)
         {
            if (line.find("EGM96") != line.npos)
            {
               gfm = GFM_EGM96;
               break;
            }
            else if (line.find("JGM-02") != line.npos)
            {
               gfm = GFM_JGM2;
               break;
            }
            else if (line.find("JGM-03") != line.npos)
            {
               gfm = GFM_JGM3;
               break;
            }
         }
         else if (forBody == GmatSolarSystemDefaults::MOON_NAME)
         {
            if (line.find("LP165P") != line.npos)
            {
               gfm = GFM_LP165P;
               break;
            }
         }
         else if (forBody == GmatSolarSystemDefaults::MARS_NAME)
         {
            if (line.find("MARS-50C") != line.npos)
            {
               gfm = GFM_MARS50C;
               break;
            }
         }
         else if (forBody == GmatSolarSystemDefaults::VENUS_NAME)
         {
            if (line.find("MGNP180U") != line.npos)
            {
               gfm = GFM_MGNP180U;
               break;
            }
         }
      }
   }

   inStream.close();

   #ifdef DEBUG_GRAVITY_FILE
   MessageInterface::ShowMessage("GravityFileUtil::GetModelType() returning %d (%s)\n", gfm, GRAVITY_MODEL_NAMES[gfm].c_str());
   #endif


   return gfm;
}
//------------------------------------------------------------------------------
HarmonicGravity* GravityField::GetHarmonicGravity 
  (const std::string& filename, const std::string& tideFilename,
   const Real &radius, const Real &mukm, const std::string& bodyname,
   const bool& loadCoefficients)
   {
   HarmonicGravity* hg = new HarmonicGravity (filename,tideFilename,radius,mukm,bodyname,loadCoefficients);
   if (hg->GetNN() == 0) return NULL;
   return hg;
   }
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool SetTideFilename(const std::string &fn, bool validateOnly = false)
//------------------------------------------------------------------------------
/**
* This method sets the tide filename for this GravityField object.
*
* @param <fn> file name to use to get tide coefficients, etc.
*
* @return flag indicating success of the operation.
*/
//------------------------------------------------------------------------------
bool GravityField::SetTideFilename(const std::string &fn, bool validateOnly)
{
#ifdef DEBUG_HARMONIC_FIELD_FILENAME
    MessageInterface::ShowMessage
        ("GravityField::SetTideFilename() entered for %s\n   tideFilenameFullPath = %s\n   "
        "tideFilename = %s\n   newname  = %s\n   validateOnly = %s\n", bodyName.c_str(),
        tideFilenameFullPath.c_str(), tideFilename.c_str(), fn.c_str(), validateOnly ? "true" : "false");
    MessageInterface::ShowMessage("   potPath  = %s\n", potPath.c_str());
#endif

    bool hasDefaultIndicator = false;
    std::string newfn;
    if (fn.substr(0, 6) == "DFLT__")  // Must match Interpreter static const std::string defaultIndicator
    {
        newfn = fn.substr(6, fn.npos - 6);
        hasDefaultIndicator = true;
    }
    else
    {
        newfn = fn;
        hasDefaultIndicator = false;
    }
    if (tideFilename != newfn)
    {
#ifdef DEBUG_HARMONIC_FIELD_FILENAME
        MessageInterface::ShowMessage
            ("GravityField::SetTideFilename(): hasDefaultIndicator = %s, newfn = %s\n",
            (hasDefaultIndicator ? "true" : "false"), newfn.c_str());
        MessageInterface::ShowMessage("   potPath  = %s\n", potPath.c_str());
#endif

        std::string newFile = newfn;
        std::string fullPath;
        std::string tideFileType = GmatStringUtil::ToUpper(GetBodyName()) + "_POT_PATH";

        // Do not write informational file location message if default file (LOJ: 2014.06.25)
        fullPath =
            GmatBase::GetFullPathFileName(newFile, GetName(), newfn, tideFileType,
            true, "", false, !hasDefaultIndicator);

        // std::ifstream potfile(filename.c_str());
        //std::ifstream potfile(filenameFullPath.c_str());
        //if (!potfile) 
        if (fullPath == "" && newFile != "")
        {
            lastErrorMessage = "The file name \"" + newFile + "\" does not exist";
            if (!validateOnly)
                throw ODEModelException(lastErrorMessage);
        }

        if (!validateOnly)
        {
            tideFilename = newfn;
            tideFilenameFullPath = fullPath;
        }
    }

#ifdef DEBUG_HARMONIC_FIELD_FILENAME
    MessageInterface::ShowMessage
        ("GravityField::SetTideFilename() returning true, tideFilename = %s\n   "
        "tideFilenameFullPath = '%s'\n", tideFilename.c_str(), tideFilenameFullPath.c_str());
#endif

    fileRead = false;
    usingDefaultFile = hasDefaultIndicator;
    return true;
}

//------------------------------------------------------------------------------
// Rvector3 GetTorquesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Calculates and returns the torque values resulting from a gravity field 
 * acting on the spacecraft at the current epoch
 *
 * @param sc The spacecraft the torques are acting on
 *
 * @return The torque vector [Tx Ty Tz]
 */
//------------------------------------------------------------------------------
Rvector3 GravityField::GetTorquesForSpacecraft(Spacecraft *sc)
{
   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage(
         "Entering GravityField::GetTorquesForSpacecraft\n");
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
   Real currEpoch = sc->GetEpoch();
   GmatTime currEpochGT = sc->GetEpochGT();

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Spacecraft j2k state:\n"
         "(%lf, %lf, %lf, %lf, %lf, %lf)\n\n", j2kState[0], j2kState[1],
         j2kState[2], j2kState[3], j2kState[4], j2kState[5]);
   #endif

   if (hasPrecisionTime)
      BuildModelStateGT(currEpochGT, satState, j2kState);
   else
      BuildModelState(currEpoch, satState, j2kState);

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

   Rvector3 gravityTorque = Cross(((3.0 * body->GetGravitationalConstant() /
      pow(dist, 3.0)) * nadirVec), scMOITensor * nadirVec);

   #ifdef DEBUG_GRAVITY_TORQUE
      MessageInterface::ShowMessage("Gravity Torque:\n"
         "(%.12e, %.12e, %.12e)\n\n", gravityTorque(0),
         gravityTorque(1), gravityTorque(2));
   #endif

   return gravityTorque;
}
