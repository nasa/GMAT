//$Id: ITRFAxes.cpp 9513 2012-02-29 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  ITRFAxes.cpp
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
// Author: Tuan Dang Nguyen (NASA/GSFC)
// Created: 2012/02/29
//
/**
 * Implementation of the ITRFAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ITRFAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"
#include "Attitude.hpp"
#include "FileManager.hpp"

using namespace GmatMathUtil;        // for trig functions, etc.
using namespace GmatTimeConstants;   // for JD offsets, etc.

//#define DEBUG_FIRST_CALL
//#define DEBUG_TIME_CALC
//#define DEBUG_MOON_MATRIX
//#define DEBUG_ITRF_MATRICES
//#define DEBUG_ITRF_CHECK_DETERMINANT
//#define DEBUG_ITRF_ROT_MATRIX
//#define DEBUG_ITRF_RECOMPUTE
//#define DEBUG_ITRF_EPOCHS
//#define DEBUG_ITRFAXES_CONSTRUCTION
//#define DEBUG_ITRFAXES_CLONE
//#define DEBUG_ITRFAXES_INITIALIZE
//#define DEBUG_ITRF_SET_REF

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif


//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ITRFAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base ITRFAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
ITRFAxes::ITRFAxes(const std::string &itsName) :
   DynamicAxes("ITRF",itsName),
   iauFile                  (NULL)
{
   objectTypeNames.push_back("ITRFAxes");
   parameterCount = ITRFAxesParamCount;

   baseSystem = "ICRF";

   #ifdef DEBUG_ITRFAXES_CONSTRUCTION
      MessageInterface::ShowMessage("Now constructing ITRFAxes with name '%s'\n",
         itsName.c_str());
   #endif
}


//------------------------------------------------------------------------------
//  ITRFAxes(const ITRFAxes &itrfAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base ITRFAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param itrfAxes  ITRFAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
ITRFAxes::ITRFAxes(const ITRFAxes &itrfAxes) :
   DynamicAxes(itrfAxes),
   iauFile                  (NULL)
{
   #ifdef DEBUG_ITRFAXES_CONSTRUCTION
       MessageInterface::ShowMessage("Now copy constructing ITRFAxes from object (%p) with name '%s'\n", &itrfAxes,
         itrfAxes.GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
//  ITRFAxes& operator=(const ITRFAxes &itrfAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for ITRFAxes structures.
 *
 * @param itrfAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const ITRFAxes& ITRFAxes::operator=(const ITRFAxes &itrfAxes)
{
   if (&itrfAxes == this)
      return *this;
   DynamicAxes::operator=(itrfAxes); 

   iauFile                  = itrfAxes.iauFile;
   return *this;
}


//------------------------------------------------------------------------------
//  ~ITRFAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ITRFAxes::~ITRFAxes()
{
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEopFile(
//                                 const std::string &forBaseSystem) const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ITRFAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesItrfFile() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ITRFAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage ITRFAxes::UsesNutationUpdateInterval() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this ITRFAxes.
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool ITRFAxes::Initialize()
{
   #ifdef DEBUG_ITRFAXES_INITIALIZE
      MessageInterface::ShowMessage("Initialize ITRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

   DynamicAxes::Initialize();
   if (originName == GmatSolarSystemDefaults::EARTH_NAME) InitializeFK5();
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   // create and initialize IAU2000/2006 object:
   if (iauFile == NULL)
	   iauFile = IAUFile::Instance();
   iauFile->Initialize();

   // create and initialize EopFile object:
   // wcs 2016.04.28 this is not how we want to do this since we can now override
   // the EOP filename in the Earth object, so getting the name from the startup
   // file may not be correct.  This eop pointer should never be NULL anyway, 
   // since we set the pointer on creation by the Moderator
//   if (eop == NULL)
//   {
//	   FileManager* fm = FileManager::Instance();
//      // Changed to use FileManager::FindPath() (LOJ: 2014.07.01)
//	   std::string fileName = fm->GetFilename("EOP_FILE");
//      std::string fileNameFullPath = fm->FindPath("", "EOP_FILE", true, true, true);
//      if (fileNameFullPath == "")
//         throw CoordinateSystemException("**** ERROR **** The EOP file '" + fileName + "' does not exist\n");
//      
//	   EopFile* eopFile = new EopFile(fileNameFullPath);
//      if (eopFile == NULL)
//         throw CoordinateSystemException("**** ERROR **** Cannot open EOP file '" + fileName + "'\n");
//      
//	   eopFile->Initialize();
//	   SetEopFile(eopFile);
//   }

   isInitialized = true;

   #ifdef DEBUG_ITRFAXES_INITIALIZE
      MessageInterface::ShowMessage("End initialize ITRFAxes: with name '%s'\n",
         instanceName.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the ITRFAxes.
 *
 * @return clone of the ITRFAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ITRFAxes::Clone() const
{
   #ifdef DEBUG_ITRFAXES_CLONE
   MessageInterface::ShowMessage("Now clone ITRFAxes from object (%p) with name '%s'\n", this,
        this->GetName().c_str());
   #endif

   return (new ITRFAxes(*this));
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ITRFAxes class.  This is
 * overridden from the CoordinateBase version, in order to make sure the origin
 * is a CelestialBody
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ITRFAxes::SetRefObject(GmatBase *obj, const UnsignedInt type,
                            const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_ITRF_SET_REF
   MessageInterface::ShowMessage
      ("ITRFAxes::SetRefObject() <%s>, obj=%p, name=%s\n", GetName().c_str(),
       obj, name.c_str());
   #endif
   if (name == originName)
   {
      if (!obj->IsOfType("CelestialBody"))
      {
         CoordinateSystemException cse("");
         cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                        (obj->GetName()).c_str(),
                       "Origin", "Celestial Body");
         throw cse;
      }
   }
   return DynamicAxes::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Rmatrix33 R1(Real angle)
//------------------------------------------------------------------------------
/**
 * This method is used to calculate the rotation matrix when the frame rotates
 * about the X-axis for a given angle.
 *
 * @param <angle>   angle (in radians) that the frame rotates about the X-axis.
 *
 * @return R1 rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ITRFAxes::R1(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,1.0);	r.SetElement(0,1,0.0);	r.SetElement(0,2,0.0);
	r.SetElement(1,0,0.0);	r.SetElement(1,1,c);	r.SetElement(1,2,s);
	r.SetElement(2,0,0.0);	r.SetElement(2,1,-s);	r.SetElement(2,2,c);

	return r;
}


//------------------------------------------------------------------------------
//  Rmatrix33 R2(Real angle)
//------------------------------------------------------------------------------
/**
 * This method is used to calculate the rotation matrix when the frame rotates
 * about the Y-axis for a given angle.
 *
 * @param <angle>   angle (in radians) that the frame rotates about the Y-axis.
 *
 * @return R1 rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ITRFAxes::R2(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,c);	r.SetElement(0,1,0.0);	r.SetElement(0,2,-s);
	r.SetElement(1,0,0.0);	r.SetElement(1,1,1.0);	r.SetElement(1,2,0.0);
	r.SetElement(2,0,s);	r.SetElement(2,1,0.0);	r.SetElement(2,2,c);

	return r;
}

//------------------------------------------------------------------------------
//  Rmatrix33 R3(Real angle)
//------------------------------------------------------------------------------
/**
 * This method is used to calculate the rotation matrix when the frame rotates
 * about the Z-axis for a given angle.
 *
 * @param <angle>   angle (in radians) that the frame rotates about the Z-axis.
 *
 * @return R1 rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ITRFAxes::R3(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,c);	r.SetElement(0,1,s);	r.SetElement(0,2,0.0);
	r.SetElement(1,0,-s);	r.SetElement(1,1,c);	r.SetElement(1,2,0.0);
	r.SetElement(2,0,0.0);	r.SetElement(2,1,0.0);	r.SetElement(2,2,1.0);

	return r;
}


//------------------------------------------------------------------------------
//  Rmatrix33 Skew(Rvector3 vec)
//------------------------------------------------------------------------------
/**
 * This method is used to calculate the rotation matrix specified by the
 * input skew vector.
 *
 * @param <vec>   skew vector
 *
 * @return        rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 ITRFAxes::Skew(Rvector3 vec)
{
	Rmatrix33 r;
	r.SetElement(0,0,0.0);					r.SetElement(0,1,-vec.GetElement(2));	r.SetElement(0,2,vec.GetElement(1));
	r.SetElement(1,0,vec.GetElement(2));	r.SetElement(1,1,0.0);					r.SetElement(1,2,-vec.GetElement(0));
	r.SetElement(2,0,-vec.GetElement(1));	r.SetElement(2,1,vec.GetElement(0));	r.SetElement(2,2,0.0);

	return r;
}


//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch, 
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//------------------------------------------------------------------------------
void ITRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation) 
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling ITRF::CalculateRotationMatrix at epoch %18.12lf; \n", atEpoch.Get());
   #endif
   Real theEpoch = atEpoch.Get();

   //  Perform time computations and read EOP file
   Real sec2rad = GmatMathConstants::RAD_PER_DEG/3600;
   Real a1MJD = theEpoch;

   Real utcMJD = theTimeConverter->Convert(a1MJD,
                    TimeSystemConverter::A1MJD, TimeSystemConverter::UTCMJD,
                    JD_JAN_5_1941);
   Real offset = JD_JAN_5_1941 - JD_NOV_17_1858;

   Real xp,yp,LOD;
   #ifdef DEBUG_ITRF_ROT_MATRIX
      Real dUT1;
      dUT1 = eop->GetUt1UtcOffset(utcMJD +  offset);
   #endif
   eop->GetPolarMotionAndLod(utcMJD +  offset, xp, yp, LOD);

   xp = xp*sec2rad;
   yp = yp*sec2rad;

   Real ut1MJD = theTimeConverter->Convert(a1MJD,
                    TimeSystemConverter::A1MJD, TimeSystemConverter::UT1,
                    JD_JAN_5_1941);

   // Compute elapsed Julian centuries (UT1)
   Real tDiff = JD_JAN_5_1941 - JD_OF_J2000;
   Real jdUT1 = ut1MJD + JD_JAN_5_1941;

   // convert input A1 MJD to TT MJD (for most calculations)
   Real ttMJD = theTimeConverter->Convert(a1MJD,
                   TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                   JD_JAN_5_1941);

   Real jdTT    = ttMJD + JD_JAN_5_1941; // right?
   // Compute Julian centuries of TDB from the base epoch (J2000)
   // NOTE - this is really TT, an approximation of TDB *********
   Real T_TT    = (ttMJD + tDiff) / DAYS_PER_JULIAN_CENTURY;

   //  Compute the Polar Motion Matrix, W, and Earth Rotation Angle, theta
   Real sPrime = -0.000047*sec2rad*T_TT;
   Rmatrix33 W = R3(-sPrime)*R2(xp)*R1(yp);
   Real theta  = fmod(GmatMathConstants::TWO_PI*(0.7790572732640 + 1.00273781191135448*(jdUT1 - 2451545.0)),GmatMathConstants::TWO_PI);

   //  Compute the precession-nutation matrix
   //  . interpolate the XYs data file
   Real data[3];
   if (iauFile == NULL)
   {
      throw CoordinateSystemException("Error: IAUFile object is NULL. GMAT cannot get IAU data.\n");
   }
   iauFile->GetIAUData(jdTT,data,3,9);
   Real X = data[0]*sec2rad;
   Real Y = data[1]*sec2rad;
   Real s = data[2]*sec2rad;

   // . construct the Precession Nutation matrix
   Real b = 1/(1 + sqrt(1- X*X - Y*Y));
   Rmatrix33 CT;
   CT.SetElement(0,0, 1-b*X*X);	CT.SetElement(0,1, -b*X*Y ); CT.SetElement(0,2,X);
   CT.SetElement(1,0, -b*X*Y);   CT.SetElement(1,1, 1-b*Y*Y); CT.SetElement(1,2,Y);
   CT.SetElement(2,0, -X);       CT.SetElement(2,1, -Y);      CT.SetElement(2,2,(1 - b*(X*X + Y*Y)));
   CT = CT*R3(s);

   //  Form the complete rotation matrix from ITRF to GCRF
   Rmatrix33 R    = CT*R3(-theta)*W;
   Real omegaEarth = 7.292115146706979e-5*(1 - LOD/86400);
   Rvector3 vec(0.0, 0.0, omegaEarth);
   Rmatrix33 Rdot = CT*R3(-theta)*Skew(vec)*W;
   rotMatrix = R;
   rotDotMatrix = Rdot;

   #ifdef DEBUG_ITRF_ROT_MATRIX
      MessageInterface::ShowMessage("a1MJD  = %18.10lf\n",a1MJD);
      MessageInterface::ShowMessage("utcMJD = %18.10lf\n",utcMJD);
      MessageInterface::ShowMessage("dUT1=%18.10e, xp=%18.10e, yp=%18.10e, LOD=%18.10e\n",dUT1,xp,yp,LOD);
      MessageInterface::ShowMessage("ut1MJD = %18.10lf\n",ut1MJD);
      MessageInterface::ShowMessage("ttMJD  = %18.10lf\n",ttMJD);
      MessageInterface::ShowMessage("jdTT   = %18.10lf\n",jdTT);
      MessageInterface::ShowMessage("jdUT1  = %18.10lf\n",jdUT1);
      MessageInterface::ShowMessage("T_TT   = %18.10lf\n\n",T_TT);

      MessageInterface::ShowMessage("sPrime = %18.10lf,  theta = %18.10lf\n",sPrime, theta);
      MessageInterface::ShowMessage("W(0,0)=%18.10lf,  W(0,1)=%18.10lf,  W(0,2)=%18.10lf\n",W.GetElement(0,0),W.GetElement(0,1),W.GetElement(0,2));
      MessageInterface::ShowMessage("W(1,0)=%18.10lf,  W(1,1)=%18.10lf,  W(1,2)=%18.10lf\n",W.GetElement(1,0),W.GetElement(1,1),W.GetElement(1,2));
      MessageInterface::ShowMessage("W(2,0)=%18.10lf,  W(2,1)=%18.10lf,  W(2,2)=%18.10lf\n",W.GetElement(2,0),W.GetElement(2,1),W.GetElement(2,2));

      MessageInterface::ShowMessage("X = %18.10lf,   Y = %18.10lf,   s = %18.10lf\n", X, Y, s);
      MessageInterface::ShowMessage("CT(0,0)=%18.10lf,  CT(0,1)=%18.10lf,  CT(0,2)=%18.10lf\n",CT.GetElement(0,0),CT.GetElement(0,1),CT.GetElement(0,2));
      MessageInterface::ShowMessage("CT(1,0)=%18.10lf,  CT(1,1)=%18.10lf,  CT(1,2)=%18.10lf\n",CT.GetElement(1,0),CT.GetElement(1,1),CT.GetElement(1,2));
      MessageInterface::ShowMessage("CT(2,0)=%18.10lf,  CT(2,1)=%18.10lf,  CT(2,2)=%18.10lf\n",CT.GetElement(2,0),CT.GetElement(2,1),CT.GetElement(2,2));

      MessageInterface::ShowMessage("R(0,0)=%18.10lf,  R(0,1)=%18.10lf,  R(0,2)=%18.10lf\n",R.GetElement(0,0),R.GetElement(0,1),R.GetElement(0,2));
      MessageInterface::ShowMessage("R(1,0)=%18.10lf,  R(1,1)=%18.10lf,  R(1,2)=%18.10lf\n",R.GetElement(1,0),R.GetElement(1,1),R.GetElement(1,2));
      MessageInterface::ShowMessage("R(2,0)=%18.10lf,  R(2,1)=%18.10lf,  R(2,2)=%18.10lf\n",R.GetElement(2,0),R.GetElement(2,1),R.GetElement(2,2));

      MessageInterface::ShowMessage("Rdot(0,0)=%18.10lf,  Rdot(0,1)=%18.10lf,  Rdot(0,2)=%18.10lf\n",Rdot.GetElement(0,0),Rdot.GetElement(0,1),Rdot.GetElement(0,2));
      MessageInterface::ShowMessage("Rdot(1,0)=%18.10lf,  Rdot(1,1)=%18.10lf,  Rdot(1,2)=%18.10lf\n",Rdot.GetElement(1,0),Rdot.GetElement(1,1),Rdot.GetElement(1,2));
      MessageInterface::ShowMessage("Rdot(2,0)=%18.10lf,  Rdot(2,1)=%18.10lf,  Rdot(2,2)=%18.10lf\n\n\n",Rdot.GetElement(2,0),Rdot.GetElement(2,1),Rdot.GetElement(2,2));
   #endif

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
      MessageInterface::ShowMessage("NOW exiting ITRFAxes::CalculateRotationMatrix ...\n");
   #endif

}

//------------------------------------------------------------------------------
//  Rmatrix33 GetRotationMatrix(const A1Mjd &atEpoch,
//                         bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the ICRFAxes system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 *
 * @return rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33  ITRFAxes::GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation)
{
	CalculateRotationMatrix(atEpoch, forceComputation);
	return rotMatrix;
}
