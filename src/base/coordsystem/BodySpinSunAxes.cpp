//$Id: BodySpinSunAxes.cpp 10276 2012-09-17 16:41:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                  BodySpinSunAxes
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Tuan Dang Nguyen/GSFC
// Created: 2012/09/17
//
/**
 * Implementation of the BodySpinSunAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "GmatBaseException.hpp"
#include "ObjectReferencedAxes.hpp"
#include "BodySpinSunAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

#include "MessageInterface.hpp"

#include <iostream>


using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeConstants;      // for JD offsets, etc.
using namespace GmatMathConstants;      // for RAD_PER_ARCSEC, etc.

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZE
//#define DEBUG_TIME_CALC
//#define DEBUG_ROT_MATRIX

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  BodySpinSunAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base BodySpinSunAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::BodySpinSunAxes(const std::string &itsName) :
   ObjectReferencedAxes("BodySpinSun",itsName)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::BodySpinSunAxes()   default construction.\n");
#endif

   primaryName    = GmatSolarSystemDefaults::SUN_NAME;

   objectTypeNames.push_back("BodySpinSun");
   parameterCount = BodySpinSunAxesParamCount;

   needsCBOrigin  = true;
   usesPrimary    = GmatCoordinate::NOT_USED;
   usesSecondary  = GmatCoordinate::NOT_USED;
}


//------------------------------------------------------------------------------
//  BodySpinSunAxes(const BodySpinSunAxes &bss);
//------------------------------------------------------------------------------
/**
 * Constructs base BodySpinSunAxes structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param gse  BodySpinSunAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::BodySpinSunAxes(const BodySpinSunAxes &bss) :
   ObjectReferencedAxes(bss)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::BodySpinSunAxes()   copy construction.\n");
#endif

}

//------------------------------------------------------------------------------
//  BodySpinSunAxes& operator=(const BodySpinSunAxes &bss)
//------------------------------------------------------------------------------
/**
 * Assignment operator for BodySpinSunAxes structures.
 *
 * @param bss The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const BodySpinSunAxes& BodySpinSunAxes::operator=(const BodySpinSunAxes &bss)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BodySpinSunAxes::operator =\n");
#endif

   if (&bss == this)
      return *this;

   ObjectReferencedAxes::operator=(bss);

   return *this;
}
//------------------------------------------------------------------------------
//  ~BodySpinSunAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
BodySpinSunAxes::~BodySpinSunAxes()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("BodySpinSunAxes::~BodySpinSunAxes()   destruction.\n");
#endif

}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}


GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage BodySpinSunAxes::UsesNutationUpdateInterval() const
{
   //if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      return GmatCoordinate::REQUIRED;
   //return ObjectReferencedAxes::UsesNutationUpdateInterval();
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this BodySpinSunAxes.
 *
 */
//------------------------------------------------------------------------------
bool BodySpinSunAxes::Initialize()
{
#ifdef DEBUG_INITIALIZE
   MessageInterface::ShowMessage("BodySpinSunAxes::Initialize().\n");
#endif

   ObjectReferencedAxes::Initialize();
   InitializeFK5();
   secondary = origin;							// the secondary is the same as the origin for BodySpinSun coordinate system
   primary = solar->GetBody(primaryName);

   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BodySpinSunAxes.
 *
 * @return clone of the BodySpinSunAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BodySpinSunAxes::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("BodySpinSunAxes::Clone().\n");
#endif

   return (new BodySpinSunAxes(*this));
}

bool BodySpinSunAxes::IsParameterReadOnly(const Integer id) const
{
   switch (id)
   {
      case PRIMARY_OBJECT_NAME:
	  case SECONDARY_OBJECT_NAME:
	  case X_AXIS:
	  case Y_AXIS:
	  case Z_AXIS:
		 return true;
         break;
	  default:
         return ObjectReferencedAxes::IsParameterReadOnly(id);
   }
}
   
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 *
 */
//------------------------------------------------------------------------------
void BodySpinSunAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                  bool forceComputation) 
{
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Start  BodySpinSunAxes::CalculateRotationMatrix()...\n");
   #endif

   // calculate rotation matrix fixedToMJ200 from body fixed to bodyMJ2000 and its derivative:
   CelestialBody* theBody = solar->GetBody(GetOriginName());
   CoordinateSystem* mj2kcs  = CoordinateSystem::CreateLocalCoordinateSystem("mj2kcs", "MJ2000Eq", theBody,
                                                           NULL, NULL, theBody->GetJ2000Body(), solar);
   CoordinateSystem* bfcs    = CoordinateSystem::CreateLocalCoordinateSystem("bfcs", "BodyFixed", theBody,
                                                           NULL, NULL, theBody->GetJ2000Body(), solar);

   Rvector6 instate(1.0, 0.0, 0.0, 0.0, 0.0, 0.0);
   Rvector6 outstate;
   CoordinateConverter cc;
   cc.Convert(atEpoch, instate, bfcs,
                           outstate, mj2kcs);
   Rmatrix33 fixedToMJ2000 = cc.GetLastRotationMatrix();
   Rmatrix33 fixedToMJ2000Dot = cc.GetLastRotationDotMatrix();

   // Calculate body's rotation axis in inertial coordinate system FK5 (MJ2000):  
   Rmatrix33 omegaMatrix = fixedToMJ2000Dot*fixedToMJ2000.Transpose();
   Rvector3 spinaxisFK5(-omegaMatrix(1,2), omegaMatrix(0,2), -omegaMatrix(0,1));

   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("fixedToMJ2000 =    [ %18.15lf,  %18.15lf,  %18.15lf\n",fixedToMJ2000(0,0), fixedToMJ2000(0,1), fixedToMJ2000(0,2));
      MessageInterface::ShowMessage("                     %18.15lf,  %18.15lf,  %18.15lf\n",fixedToMJ2000(1,0), fixedToMJ2000(1,1), fixedToMJ2000(1,2));
      MessageInterface::ShowMessage("                     %18.15lf,  %18.15lf,  %18.15lf]\n",fixedToMJ2000(2,0), fixedToMJ2000(2,1), fixedToMJ2000(2,2));
      MessageInterface::ShowMessage("fixedToMJ2000Dot = [ %18.15le,  %18.15le,  %18.15le\n",fixedToMJ2000Dot(0,0), fixedToMJ2000Dot(0,1), fixedToMJ2000Dot(0,2));
      MessageInterface::ShowMessage("                     %18.15le,  %18.15le,  %18.15le\n",fixedToMJ2000Dot(1,0), fixedToMJ2000Dot(1,1), fixedToMJ2000Dot(1,2));
      MessageInterface::ShowMessage("                     %18.15le,  %18.15le,  %18.15le]\n",fixedToMJ2000Dot(2,0), fixedToMJ2000Dot(2,1), fixedToMJ2000Dot(2,2));
      MessageInterface::ShowMessage("omegaMatrix = [ %18.15le,  %18.15le,  %18.15le\n",omegaMatrix(0,0), omegaMatrix(0,1), omegaMatrix(0,2));
      MessageInterface::ShowMessage("                %18.15le,  %18.15le,  %18.15le\n",omegaMatrix(1,0), omegaMatrix(1,1), omegaMatrix(1,2));
      MessageInterface::ShowMessage("                %18.15le,  %18.15le,  %18.15le]\n",omegaMatrix(2,0), omegaMatrix(2,1), omegaMatrix(2,2));
	  MessageInterface::ShowMessage("angular velocity vector = [ %18.12le,  %18.12le,  %18.12le]\n", spinaxisFK5[0],  spinaxisFK5[1], spinaxisFK5[2]);
   #endif
   
   spinaxisFK5 = spinaxisFK5.Normalize();
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("spin axis unit vector  = [ %18.12le,  %18.12le,  %18.12le]\n", spinaxisFK5[0],  spinaxisFK5[1], spinaxisFK5[2]);
      MessageInterface::ShowMessage("Step to specify rotation matrix from BodySpinSun coordinate system to FK5:\n");
   #endif
   rvSunVec = primary->GetMJ2000State(atEpoch) - secondary->GetMJ2000State(atEpoch);

   Rvector3 rSun  = rvSunVec.GetR();
   Rvector3 vSun  = rvSunVec.GetV();

   Real     rMag  = rSun.GetMagnitude();
   Rvector3 x = rSun/ rMag;

   Rvector3 yFK5  = Cross(spinaxisFK5,x);
   Real     yMag  = yFK5.GetMagnitude();
   Rvector3 y     = yFK5 / yMag;

   Rvector3 z     = Cross(x,y);
                
   rotMatrix.Set(x[0], y[0], z[0],
                 x[1], y[1], z[1],
                 x[2], y[2], z[2]);
              
   Rvector3 vR    = vSun / rMag;
   Rvector3 xDot  = vR - x * (x * vR);

//   Rvector3 yTmp  = Cross((fixedToMJ2000Dot * spinaxisFK5), x) +						// fix bug GMT-3465
//                    Cross((fixedToMJ2000 * spinaxisFK5), xDot);						   // fix bug GMT-3465
   Rvector3 spinaxisFixed =  fixedToMJ2000.Transpose()*spinaxisFK5;						// fix bug GMT-3465
   Rvector3 yTmp  = Cross((fixedToMJ2000Dot * spinaxisFixed), x) +						// fix bug GMT-3465
                    Cross(spinaxisFK5, xDot);											      // fix bug GMT-3465
   Rvector3 yDot  = (yTmp / yMag) - y * (y * (yTmp / yMag));

   Rvector3 zDot  = Cross(xDot, y) + Cross(x, yDot);
   
  rotDotMatrix.Set(xDot[0],yDot[0],zDot[0],
                   xDot[1],yDot[1],zDot[1],
                   xDot[2],yDot[2],zDot[2]);
                   
   #ifdef DEBUG_ROT_MATRIX
      MessageInterface::ShowMessage("Sun state: %s %s\n", primary->GetName().c_str(), primary->GetMJ2000State(atEpoch).ToString().c_str());
      MessageInterface::ShowMessage("Earth state: %s %s\n", secondary->GetName().c_str(), secondary->GetMJ2000State(atEpoch).ToString().c_str());
      MessageInterface::ShowMessage("Sun R vector(MJ2000) = %lf %lf %lf\n", rvSunVec[0], rvSunVec[1], rvSunVec[2]);
      MessageInterface::ShowMessage("Sun V vector(MJ2000) = %lf %lf %lf\n", rvSunVec[3], rvSunVec[4], rvSunVec[5]);
      MessageInterface::ShowMessage("x-axis unit vector: x = %18.15lf   %18.15lf   %18.15lf\n", x[0], x[1], x[2]);
      MessageInterface::ShowMessage("y-axis unit vector: y = %18.15lf   %18.15lf   %18.15lf\n", y[0], y[1], y[2]);
      MessageInterface::ShowMessage("z-axis unit vector: z = %18.15lf   %18.15lf   %18.15lf\n", z[0], z[1], z[2]);
      MessageInterface::ShowMessage("xDot = %18.15le   %18.15le   %18.15le\n", xDot[0], xDot[1], xDot[2]);
      MessageInterface::ShowMessage("yDot = %18.15le   %18.15le   %18.15le\n", yDot[0], yDot[1], yDot[2]);
      MessageInterface::ShowMessage("zDot = %18.15le   %18.15le   %18.15le\n", zDot[0], zDot[1], zDot[2]);
      MessageInterface::ShowMessage("rotation matrix = \n%lf %lf %lf\n%lf %lf %lf\n%lf %lf %lf\n",
      rotMatrix(0,0), rotMatrix(0,1), rotMatrix(0,2), rotMatrix(1,0), 
      rotMatrix(1,1), rotMatrix(1,2), rotMatrix(2,0), rotMatrix(2,1), 
      rotMatrix(2,2));
      MessageInterface::ShowMessage("rotation matrix (derivative) = \n%le %le %le\n%le %le %le\n%le %le %le\n",
      rotDotMatrix(0,0), rotDotMatrix(0,1), rotDotMatrix(0,2), rotDotMatrix(1,0), 
      rotDotMatrix(1,1), rotDotMatrix(1,2), rotDotMatrix(2,0), rotDotMatrix(2,1), 
      rotDotMatrix(2,2));
      MessageInterface::ShowMessage("Exit  BodySpinSunAxes::CalculateRotationMatrix()...\n");
   #endif

   if (mj2kcs != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (mj2kcs, "mj2kcs", "BodySpinSunAxes::CalculateRotationMatrix()",
          "deleting localCoordSystem");
      #endif
      delete mj2kcs;
   }
   if (bfcs != NULL)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (bfcs, "bfcs", "BodySpinSunAxes::CalculateRotationMatrix()",
          "deleting localCoordSystem");
      #endif
      delete bfcs;
   }
}
