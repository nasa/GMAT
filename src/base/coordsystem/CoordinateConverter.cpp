//$Id$
//------------------------------------------------------------------------------
//                                  CoordinateConverter
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
// Author: Wendy C. Shoan
// Created: 2004/12/27
//
/**
 * Implementation of the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateConverter.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "Rvector.hpp"
#include "TimeTypes.hpp"
#include "ICRFFile.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FIRST_CALL
//#define DEBUG_TO_FROM
//#define DEBUG_BASE_SYSTEM
//#define DEBUG_ROT_DOT_MATRIX
//#define DEBUG_ICRF_TOFK5

#ifdef DEBUG_TO_FROM
   #include "MessageInterface.hpp"
#endif

#ifdef DEBUG_FIRST_CALL 
   #include "MessageInterface.hpp"
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateConverter();
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateConverter objects.
 * (default constructor).
 */
//---------------------------------------------------------------------------
CoordinateConverter::CoordinateConverter():
   specifyRotMatrixDeriv (false),
   lastIcrfToFk5Epoch    (0.0)
{
}

//---------------------------------------------------------------------------
//  CoordinateConverter(const CoordinateConverter &coordCvt);
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateConverter classes, by copying 
 * the input instance (copy constructor).
 *
 * @param coordCvt  CoordinateConverter instance to copy to create "this" 
 *                  instance.
 */
//---------------------------------------------------------------------------
CoordinateConverter::CoordinateConverter(const CoordinateConverter &coordCvt):
   specifyRotMatrixDeriv  (coordCvt.specifyRotMatrixDeriv),
   lastIcrfToFk5Epoch(coordCvt.lastIcrfToFk5Epoch)
{
}

//---------------------------------------------------------------------------
//  CoordinateConverter& operator=(const CoordinateConverter &coordCvt)
//---------------------------------------------------------------------------
/**
 * Assignment operator for CoordinateConverter objects.
 *
 * @param coordCvt The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateConverter& CoordinateConverter::operator=(
                           const CoordinateConverter &coordCvt)
{
   if (&coordCvt == this)
      return *this;

   specifyRotMatrixDeriv = coordCvt.specifyRotMatrixDeriv;
   lastIcrfToFk5Epoch    = coordCvt.lastIcrfToFk5Epoch;
   return *this;
}
//---------------------------------------------------------------------------
//  ~CoordinateConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateConverter::~CoordinateConverter()
{
   // nothing to do ... hm ... hm ... hm ... hm ... la-de-da-de-da
   
}

//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateConverter::Initialize()
{
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
}


//---------------------------------------------------------------------------
//  bool SetToCalculateRotMatrixDeriv(bool turnOn) 
//---------------------------------------------------------------------------
/**
* Turn on/off flag to indicate GMAT will calculate d[R]/dX derivative of
* rotation matrix or not
*
* @param turnOn  flag indicates to calculate d[R]/dX
*
* @return        previous flag value
*/
//---------------------------------------------------------------------------
bool CoordinateConverter::SetToCalculateRotMatrixDeriv(bool turnOn)
{
	bool prevFlag = specifyRotMatrixDeriv;
	specifyRotMatrixDeriv = turnOn;
	return prevFlag;
}


//------------------------------------------------------------------------------
//  bool  Convert(const A1Mjd &epoch, const Rvector &inState,
//                CoordinateSystem *inCoord, Rvector &outState,
//                CoordinateSystem *outCoord, 
//                bool forceComputation, bool omitTranslation) 
//------------------------------------------------------------------------------
/**
 * This method converts the inState vector from coordIn CoordinateSystem
 * to the coordOut CoordinateSystem, at the input epoch, and returns the
 * result in outState.
 *
 * @param epoch            time for which to do the conversion.
 * @param inState          input State (in inCoord system).
 * @param inCoord          pointer to the input CoordinateSystem.
 * @param outState         resulting vector, in the outCoord system.
 * @param outCoord         pointer to the output CoordinateSystem.
 * @param forceComputation force the computation whether it's time to do
 *                         it or not (default is false)
 * @param omitTranslation  omit the translation whether coincident or not
 *                         (default is false)
 *
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool CoordinateConverter::Convert(const A1Mjd &epoch, const Rvector &inState,
                          CoordinateSystem *inCoord, Rvector &outState,
                          CoordinateSystem *outCoord, 
                          bool forceComputation, bool omitTranslation)
{
   const Real *in = inState.GetDataVector();
   Real *out = new Real[outState.GetSize()];

   #ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage("in: %f %f %f %f %f %f\n", in[0], in[1], in[2], in[3], in[4], in[5]);
   MessageInterface::ShowMessage("out: %f %f %f %f %f %f\n", out[0], out[1], out[2], out[3], out[4], out[5]);
   #endif
   
   if (Convert(epoch, in, inCoord, out, outCoord, forceComputation, omitTranslation))
   {
      outState.Set(out);
      delete [] out;
      return true;
   }
   
   delete [] out;
   return false;

}


bool CoordinateConverter::Convert(const GmatTime &epoch, const Rvector &inState,
   CoordinateSystem *inCoord, Rvector &outState,
   CoordinateSystem *outCoord,
   bool forceComputation, bool omitTranslation)
{
   const Real *in = inState.GetDataVector();
   Real *out = new Real[outState.GetSize()];

#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage("in: %f %f %f %f %f %f\n", in[0], in[1], in[2], in[3], in[4], in[5]);
   MessageInterface::ShowMessage("out: %f %f %f %f %f %f\n", out[0], out[1], out[2], out[3], out[4], out[5]);
#endif

   if (Convert(epoch, in, inCoord, out, outCoord, forceComputation, omitTranslation))
   {
      outState.Set(out);
      delete[] out;
      return true;
   }

   delete[] out;
   return false;

}


//------------------------------------------------------------------------------
// bool Convert(const A1Mjd &epoch, const Real *inState,
//              CoordinateSystem *inCoord, Real *outState,
//              CoordinateSystem *outCoord, bool forceComputation = false,
//              bool omitTranslation = false)
//------------------------------------------------------------------------------
/**
 * This method converts the inState vector from coordIn CoordinateSystem
 * to the coordOut CoordinateSystem, at the input epoch, and returns the
 * result in outState.
 *
 * @param epoch            time for which to do the conversion.
 * @param inState          input State (in inCoord system).
 * @param inCoord          pointer to the input CoordinateSystem.
 * @param outState         resulting vector, in the outCoord system.
 * @param outCoord         pointer to the output CoordinateSystem.
 * @param forceComputation force the computation whether it's time to do
 *                         it or not (default is false)
 * @param omitTranslation  omit the translation whether coincident or not
 *                         (default is false)
 *
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool CoordinateConverter::Convert(const A1Mjd &epoch, const Real *inState,
                          CoordinateSystem *inCoord, Real *outState,
                          CoordinateSystem *outCoord, 
                          bool forceComputation, bool omitTranslation)
{
   if ((!inCoord) || (!outCoord))
      throw CoordinateSystemException(
         "Undefined coordinate system - conversion not performed.");

	bool inCoordPrevFlag = inCoord->SetCalculateRotMatrixDeriv(specifyRotMatrixDeriv);
	bool outCoordPrevFlag = outCoord->SetCalculateRotMatrixDeriv(specifyRotMatrixDeriv);
   
   #ifdef DEBUG_TO_FROM
      MessageInterface::ShowMessage
         ("In Convert(const A1Mjd & ...) function, inCoord is %s(%p) and outCoord is %s(%p)\n",
         (inCoord->GetName()).c_str(), inCoord, (outCoord->GetName()).c_str(),
          outCoord);
      MessageInterface::ShowMessage
         ("   forceComputation=%d, omitTranslation=%d\n", forceComputation,
          omitTranslation);
   #endif
   if (inCoord->GetName() == outCoord->GetName())
   {
      // assuming state is size 6 here!!!
      for (Integer i=0;i<6;i++) outState[i] = inState[i];
      lastRotMatrix.Set(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);
      return true;
   }
   
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "Coordinate conversion check:\n   %s --> %s\n", 
            inCoord->GetName().c_str(), outCoord->GetName().c_str());
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State    = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
      }
   #endif
   
   #ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("   inCoord->GetOrigin=%s(%p), outCoord->GetOrigin=%s(%p)\n",
       inCoord->GetOrigin()->GetName().c_str(), inCoord->GetOrigin(),
       outCoord->GetOrigin()->GetName().c_str(), outCoord->GetOrigin());
   #endif
   
   // call coordinate system methods to convert - allow exceptions to
   // percolate up (to be caught at a higher level)
   bool sameOrigin = (inCoord->GetOrigin() == outCoord->GetOrigin() ? 
   //bool sameOrigin = (inCoord->GetOrigin()->GetName() == outCoord->GetOrigin()->GetName() ? 
         true : false);
   
   bool coincident = sameOrigin || omitTranslation;
   Real intState[6];
   
   #ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("   sameOrigin=%d, omitTranslation=%d, coincident=%d\n",
       sameOrigin, omitTranslation, coincident);
   #endif
   
   inCoord->ToBaseSystem(epoch, inState, intState, coincident, forceComputation);
   #ifdef DEBUG_TO_FROM
      MessageInterface::ShowMessage
         ("In Convert, sameOrigin is %s,and coincident is %s\n",
         (sameOrigin? "TRUE" : "FALSE"),
         (coincident? "TRUE" : "FALSE"));
      MessageInterface::ShowMessage("inState = %18.10f   %18.10f   %18.10f\n",
            inState[0], inState[1], inState[2]);
      MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
            inState[3], inState[4], inState[5]);
      MessageInterface::ShowMessage("intState = %18.10f   %18.10f   %18.10f\n",
            intState[0], intState[1], intState[2]);
      MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
            intState[3], intState[4], intState[5]);
   #endif

   std::string inBaseName  = inCoord->GetBaseSystem();
   std::string outBaseName = outCoord->GetBaseSystem();
   #ifdef DEBUG_BASE_SYSTEM
      MessageInterface::ShowMessage("inBase system  = %s\n", inBaseName.c_str());
      MessageInterface::ShowMessage("outBase system = %s\n", outBaseName.c_str());
   #endif
   Real baseState[6];

   if (inBaseName != outBaseName)
   {
      ConvertFromBaseToBase(epoch, inCoord->GetSolarSystem(), inBaseName, outBaseName, intState, baseState);
   }
   else
   {
      for (unsigned int ii = 0; ii < 6; ii++)
         baseState[ii] = intState[ii];
   }
   #ifdef DEBUG_BASE_SYSTEM
      MessageInterface::ShowMessage("baseState = %18.10f   %18.10f   %18.10f\n",
            baseState[0], baseState[1], baseState[2]);
      MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
            baseState[3], baseState[4], baseState[5]);
      MessageInterface::ShowMessage(" ... about to call outCoord->FromBaseSystem ... outCoord is %sNULL\n",
            (outCoord? "NOT " : ""));
      MessageInterface::ShowMessage("epoch = %le, coincident = %s, forceComputation = %s\n", epoch.Get(),
            (coincident? "true" : "false"), (forceComputation? "true" : "false"));
   #endif

   outCoord->FromBaseSystem(epoch, baseState, outState, coincident, forceComputation);
   #ifdef DEBUG_TO_FROM
      MessageInterface::ShowMessage("outState = %18.10f   %18.10f   %18.10f\n",
            outState[0], outState[1], outState[2]);
      MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
            outState[3], outState[4], outState[5]);
   #endif
   
   // last rotation matrix
   // R1 * R2T
   Real R1[9];
   Real R2[9];
   Real lrm[3][3];
   Integer p3;

   inCoord->GetLastRotationMatrix(R1);
   outCoord->GetLastRotationMatrix(R2);
   Real  R2T[9] = {R2[0], R2[3], R2[6],
                   R2[1], R2[4], R2[7],
                   R2[2], R2[5], R2[8]};
   
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         lrm[p][q] = R2T[p3]   * R1[q]   +
                     R2T[p3+1] * R1[q+3] +
                     R2T[p3+2] * R1[q+6];
      }
   }
   lastRotMatrix.Set(lrm[0][0],lrm[0][1],lrm[0][2],
                     lrm[1][0],lrm[1][1],lrm[1][2],
                     lrm[2][0],lrm[2][1],lrm[2][2]);
 

   // last rotation Dot matrix
   // R1*R2Dot + R1dot*R2
   Real R1dot[9];
   Real R2dot[9];
   inCoord->GetLastRotationDotMatrix(R1dot);
   outCoord->GetLastRotationDotMatrix(R2dot);

   #ifdef DEBUG_ROT_DOT_MATRIX
      MessageInterface::ShowMessage("R1dot = %le %le %le %le %le %le %le %le %le\n",
            R1dot[0],R1dot[1],R1dot[2],R1dot[3],R1dot[4],R1dot[5],R1dot[6],R1dot[7],R1dot[8]);
      MessageInterface::ShowMessage("R2dot = %le %le %le %le %le %le %le %le %le\n",
            R2dot[0],R2dot[1],R2dot[2],R2dot[3],R2dot[4],R2dot[5],
            R2dot[6],R2dot[7],R2dot[8]);
   #endif
   Real  R2dotTR1[3][3];
   Real  R2TR1dot[3][3];
   Real  R2dotT[9] = {R2dot[0], R2dot[3], R2dot[6],
                      R2dot[1], R2dot[4], R2dot[7],
                      R2dot[2], R2dot[5], R2dot[8]};
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         R2dotTR1[p][q] = R2dotT[p3]   * R1[q]   +
                          R2dotT[p3+1] * R1[q+3] +
                          R2dotT[p3+2] * R1[q+6];
      }
   }
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         R2TR1dot[p][q] = R2T[p3]   * R1dot[q]   +
                          R2T[p3+1] * R1dot[q+3] +
                          R2T[p3+2] * R1dot[q+6];
      }
   }
   lastRotDotMatrix.Set(R2dotTR1[0][0] + R2TR1dot[0][0],   R2dotTR1[0][1] + R2TR1dot[0][1],   R2dotTR1[0][2] + R2TR1dot[0][2],
                        R2dotTR1[1][0] + R2TR1dot[1][0],   R2dotTR1[1][1] + R2TR1dot[1][1],   R2dotTR1[1][2] + R2TR1dot[1][2],
                        R2dotTR1[2][0] + R2TR1dot[2][0],   R2dotTR1[2][1] + R2TR1dot[2][1],   R2dotTR1[2][2] + R2TR1dot[2][2]);


	if (specifyRotMatrixDeriv)
	{
		// Specify derivative of last rotation matrix w.r.t. spacecraft state 
	   Rmatrix33 matrixR1(R1[0], R1[1], R1[2], R1[3], R1[4], R1[5], R1[6], R1[7], R1[8]);
	   Rmatrix33 matrixR2(R2[0], R2[1], R2[2], R2[3], R2[4], R2[5], R2[6], R2[7], R2[8]);
	   std::vector<Rmatrix33> dR1 = inCoord->GetDerivativeOfLastRotationMatrix();
	   std::vector<Rmatrix33> dR2 = outCoord->GetDerivativeOfLastRotationMatrix();

		// (d/dX)(R2T * R1) = [d(R2T)/dX]*R1 + R2T*[dR1/dX]
		lastRotMatrixDeriv.clear();
		for (Integer stateIndex = 0; stateIndex < dR1.size(); ++stateIndex)
		{
			Rmatrix33 val = dR2[stateIndex].Transpose()*matrixR1 
				             + matrixR2.Transpose()*dR1[stateIndex];
			lastRotMatrixDeriv.push_back(val);
		}
	}

	inCoord->SetCalculateRotMatrixDeriv(inCoordPrevFlag);
	outCoord->SetCalculateRotMatrixDeriv(outCoordPrevFlag);

#ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == GmatTimeConstants::MJD_OF_J2000))
      {
         MessageInterface::ShowMessage(
            "   internal State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            intState[0], intState[1], intState[2], intState[3], 
            intState[4], intState[5]);
         MessageInterface::ShowMessage(
            "   output State   = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[3], outState[4], 
            outState[5]);
         firstCallFired = true;
      }
   #endif
   
   return true;
}


bool CoordinateConverter::Convert(const GmatTime &epoch, const Real *inState,
   CoordinateSystem *inCoord, Real *outState,
   CoordinateSystem *outCoord,
   bool forceComputation, bool omitTranslation)
{
   if ((!inCoord) || (!outCoord))
      throw CoordinateSystemException(
      "Undefined coordinate system - conversion not performed.");

	bool inCoordPrevFlag = inCoord->SetCalculateRotMatrixDeriv(specifyRotMatrixDeriv);
	bool outCoordPrevFlag = outCoord->SetCalculateRotMatrixDeriv(specifyRotMatrixDeriv);

#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("In ConvertConvert(const GmatTime & ...) function, inCoord is %s(%p) and outCoord is %s(%p)\n",
      (inCoord->GetName()).c_str(), inCoord, (outCoord->GetName()).c_str(),
      outCoord);
   MessageInterface::ShowMessage
      ("   forceComputation=%d, omitTranslation=%d\n", forceComputation,
      omitTranslation);
#endif
   if (inCoord->GetName() == outCoord->GetName())
   {
      // assuming state is size 6 here!!!
      for (Integer i = 0; i<6; i++) outState[i] = inState[i];
      lastRotMatrix.Set(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
      return true;
   }

#ifdef DEBUG_FIRST_CALL
   if ((firstCallFired == false) || (epoch == GmatTimeConstants::MJD_OF_J2000))
   {
      MessageInterface::ShowMessage(
         "Coordinate conversion check:\n   %s --> %s\n",
         inCoord->GetName().c_str(), outCoord->GetName().c_str());
      MessageInterface::ShowMessage(
         "   Epoch: %.12lf\n", GmatTime(epoch).GetMjd());
      MessageInterface::ShowMessage(
         "   input State    = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
         inState[0], inState[1], inState[2], inState[3], inState[4],
         inState[5]);
   }
#endif

#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("   inCoord->GetOrigin=%s(%p), outCoord->GetOrigin=%s(%p)\n",
      inCoord->GetOrigin()->GetName().c_str(), inCoord->GetOrigin(),
      outCoord->GetOrigin()->GetName().c_str(), outCoord->GetOrigin());
#endif

   // call coordinate system methods to convert - allow exceptions to
   // percolate up (to be caught at a higher level)
   bool sameOrigin = (inCoord->GetOrigin() == outCoord->GetOrigin() ?
   //bool sameOrigin = (inCoord->GetOrigin()->GetName() == outCoord->GetOrigin()->GetName() ?
      true : false);

   bool coincident = sameOrigin || omitTranslation;
   Real intState[6];

#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("   sameOrigin=%d, omitTranslation=%d, coincident=%d\n",
      sameOrigin, omitTranslation, coincident);
#endif

   inCoord->ToBaseSystem(epoch, inState, intState, coincident, forceComputation);
#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage
      ("In Convert, sameOrigin is %s,and coincident is %s\n",
      (sameOrigin ? "TRUE" : "FALSE"),
      (coincident ? "TRUE" : "FALSE"));
   MessageInterface::ShowMessage("inState = %18.10f   %18.10f   %18.10f\n",
      inState[0], inState[1], inState[2]);
   MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
      inState[3], inState[4], inState[5]);
   MessageInterface::ShowMessage("intState = %18.10f   %18.10f   %18.10f\n",
      intState[0], intState[1], intState[2]);
   MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
      intState[3], intState[4], intState[5]);
#endif

   std::string inBaseName = inCoord->GetBaseSystem();
   std::string outBaseName = outCoord->GetBaseSystem();
#ifdef DEBUG_BASE_SYSTEM
   MessageInterface::ShowMessage("inBase system  = %s\n", inBaseName.c_str());
   MessageInterface::ShowMessage("outBase system = %s\n", outBaseName.c_str());
#endif
   Real baseState[6];

   if (inBaseName != outBaseName)
   {
      ConvertFromBaseToBase(epoch, inCoord->GetSolarSystem(), inBaseName, outBaseName, intState, baseState);
   }
   else
   {
      for (unsigned int ii = 0; ii < 6; ii++)
         baseState[ii] = intState[ii];
   }
#ifdef DEBUG_BASE_SYSTEM
   MessageInterface::ShowMessage("baseState = %18.10f   %18.10f   %18.10f\n",
      baseState[0], baseState[1], baseState[2]);
   MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
      baseState[3], baseState[4], baseState[5]);
   MessageInterface::ShowMessage(" ... about to call outCoord->FromBaseSystem ... outCoord is %sNULL\n",
      (outCoord ? "NOT " : ""));
   MessageInterface::ShowMessage("epoch = %le, coincident = %s, forceComputation = %s\n", epoch.GetMjd(),
      (coincident ? "true" : "false"), (forceComputation ? "true" : "false"));
#endif

   outCoord->FromBaseSystem(epoch, baseState, outState, coincident, forceComputation);
#ifdef DEBUG_TO_FROM
   MessageInterface::ShowMessage("outState = %18.10f   %18.10f   %18.10f\n",
      outState[0], outState[1], outState[2]);
   MessageInterface::ShowMessage("          %18.10f   %18.10f   %18.10f\n",
      outState[3], outState[4], outState[5]);
#endif

   // last rotation matrix
   // R1 * R2T
   Real R1[9];
   Real R2[9];
   Real lrm[3][3];
   Integer p3;

   inCoord->GetLastRotationMatrix(R1);
   outCoord->GetLastRotationMatrix(R2);
   Real  R2T[9] = { R2[0], R2[3], R2[6],
      R2[1], R2[4], R2[7],
      R2[2], R2[5], R2[8] };

   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3 * p;
      for (Integer q = 0; q < 3; ++q)
      {
         lrm[p][q] = R2T[p3] * R1[q] +
            R2T[p3 + 1] * R1[q + 3] +
            R2T[p3 + 2] * R1[q + 6];
      }
   }
   lastRotMatrix.Set(lrm[0][0], lrm[0][1], lrm[0][2],
      lrm[1][0], lrm[1][1], lrm[1][2],
      lrm[2][0], lrm[2][1], lrm[2][2]);


   // last rotation Dot matrix
   // R1*R2Dot + R1dot*R2
   Real R1dot[9];
   Real R2dot[9];

   inCoord->GetLastRotationDotMatrix(R1dot);
   outCoord->GetLastRotationDotMatrix(R2dot);

#ifdef DEBUG_ROT_DOT_MATRIX
   MessageInterface::ShowMessage("R1dot = %le %le %le %le %le %le %le %le %le\n",
      R1dot[0], R1dot[1], R1dot[2], R1dot[3], R1dot[4], R1dot[5], R1dot[6], R1dot[7], R1dot[8]);
   MessageInterface::ShowMessage("R2dot = %le %le %le %le %le %le %le %le %le\n",
      R2dot[0], R2dot[1], R2dot[2], R2dot[3], R2dot[4], R2dot[5],
      R2dot[6], R2dot[7], R2dot[8]);
#endif
   Real  R2dotTR1[3][3];
   Real  R2TR1dot[3][3];
   Real  R2dotT[9] = { R2dot[0], R2dot[3], R2dot[6],
      R2dot[1], R2dot[4], R2dot[7],
      R2dot[2], R2dot[5], R2dot[8] };
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3 * p;
      for (Integer q = 0; q < 3; ++q)
      {
         R2dotTR1[p][q] = R2dotT[p3] * R1[q] +
            R2dotT[p3 + 1] * R1[q + 3] +
            R2dotT[p3 + 2] * R1[q + 6];
      }
   }
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3 * p;
      for (Integer q = 0; q < 3; ++q)
      {
         R2TR1dot[p][q] = R2T[p3] * R1dot[q] +
            R2T[p3 + 1] * R1dot[q + 3] +
            R2T[p3 + 2] * R1dot[q + 6];
      }
   }
   lastRotDotMatrix.Set(R2dotTR1[0][0] + R2TR1dot[0][0], R2dotTR1[0][1] + R2TR1dot[0][1], R2dotTR1[0][2] + R2TR1dot[0][2],
      R2dotTR1[1][0] + R2TR1dot[1][0], R2dotTR1[1][1] + R2TR1dot[1][1], R2dotTR1[1][2] + R2TR1dot[1][2],
      R2dotTR1[2][0] + R2TR1dot[2][0], R2dotTR1[2][1] + R2TR1dot[2][1], R2dotTR1[2][2] + R2TR1dot[2][2]);


	if (specifyRotMatrixDeriv)
	{
		// Specify derivative of last rotation matrix w.r.t. spacecraft state 
		Rmatrix33 matrixR1(R1[0], R1[1], R1[2], R1[3], R1[4], R1[5], R1[6], R1[7], R1[8]);
		Rmatrix33 matrixR2(R2[0], R2[1], R2[2], R2[3], R2[4], R2[5], R2[6], R2[7], R2[8]);
		std::vector<Rmatrix33> dR1 = inCoord->GetDerivativeOfLastRotationMatrix();
		std::vector<Rmatrix33> dR2 = outCoord->GetDerivativeOfLastRotationMatrix();

      // (d/dX)(R2T * R1) = [d(R2T)/dX]*R1 + R2T*[dR1/dX] 
		lastRotMatrixDeriv.clear();
		for (Integer stateIndex = 0; stateIndex < dR1.size(); ++stateIndex)
		{
			Rmatrix33 val = dR2[stateIndex].Transpose()*matrixR1 
                         + matrixR2.Transpose()*dR1[stateIndex];
			lastRotMatrixDeriv.push_back(val);
		}
	}

	inCoord->SetCalculateRotMatrixDeriv(inCoordPrevFlag);
	outCoord->SetCalculateRotMatrixDeriv(outCoordPrevFlag);

#ifdef DEBUG_FIRST_CALL
   if ((firstCallFired == false) || (epoch.GetMjd() == GmatTimeConstants::MJD_OF_J2000))
   {
      MessageInterface::ShowMessage(
         "   internal State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
         intState[0], intState[1], intState[2], intState[3],
         intState[4], intState[5]);
      MessageInterface::ShowMessage(
         "   output State   = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
         outState[0], outState[1], outState[2], outState[3], outState[4],
         outState[5]);
      firstCallFired = true;
   }
#endif

   return true;
}


//------------------------------------------------------------------------------
// Rmatrix33 CoordinateConverter::GetLastRotationMatrix() const
//------------------------------------------------------------------------------
/**
 * This method returns the last-computed rotation matrix, the matrix from
 * the input coordinate system to its base system.
 *
 * @return last-computed rotation matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 CoordinateConverter::GetLastRotationMatrix() const
{
   return lastRotMatrix;
}

//------------------------------------------------------------------------------
// Rmatrix33 CoordinateConverter::GetLastRotationDotMatrix() const
//------------------------------------------------------------------------------
/**
 * This method returns the last-computed rotation dot matrix, the matrix from
 * the input coordinate system to its base system.
 *
 * @return last-computed rotation dot matrix
 */
//------------------------------------------------------------------------------
Rmatrix33 CoordinateConverter::GetLastRotationDotMatrix() const
{
   return lastRotDotMatrix;
}


Rmatrix33 CoordinateConverter::GetRotationMatrixFromICRFToFK5(const A1Mjd &atEpoch)
{
	RotationMatrixFromICRFToFK5(atEpoch);
	return icrfToFK5; 
}


//------------------------------------------------------------------------------
// Rmatrix33 CoordinateConverter::GetLastRotationMatrixDerivative() const
//------------------------------------------------------------------------------
/**
* This method returns the last-computed rotation matrix derivative, the matrix from
* the input coordinate system to its base system.
*
* @return last-computed rotation matrix derivative
*/
//------------------------------------------------------------------------------
std::vector<Rmatrix33> CoordinateConverter::GetLastRotationMatrixDerivative() const
{
	return lastRotMatrixDeriv;
}


//------------------------------------------------------------------------------
// bool CoordinateConverter::ConvertFromBaseToBase(const A1Mjd &epoch,
//          SolarSystem *solarSystem,
//          const std::string &inBase,  const std::string &outBase,
//          const Rvector &inBaseState, Rvector &outBaseState)
//------------------------------------------------------------------------------
/**
 * This method converts the inBaseState vector the base system of the inCoord
 * to the base system of the outCoord, and returns the result in outBaseState.
 *
 * @param epoch            time for which to do the conversion
 * @param solarSystem      pointer to the solar system to use
 * @param inBase           inCoord base system
 * @param outBase          outCoord base system
 * @param inBaseState      input state, in inCoord base system
 * @param outBaseState     output state, in outCoord base system
 *
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool CoordinateConverter::ConvertFromBaseToBase(const A1Mjd &epoch,
                                                SolarSystem *solarSystem,
		                                          const std::string &inBase,
		                                          const std::string &outBase,
		                                          const Rvector &inBaseState,
		                                          Rvector &outBaseState)
{
   // if the base types are the same, just set the output state to the input state
   if (inBase == outBase)
   {
      if (inBaseState.GetSize() != outBaseState.GetSize())
      {
         std::string errmsg = "Cannot convert from coordinate system base type ";
         errmsg += inBase + " to base type ";
         errmsg += outBase + " - state vectors passed in are of differing sizes.\n";
         throw CoordinateSystemException(errmsg);
      }
      for (Integer ii = 0; ii < inBaseState.GetSize(); ii++)
      {
         outBaseState[ii] = inBaseState[ii];
      }
      return true;
   }
   const Real *in  = inBaseState.GetDataVector();
   Real       *out = new Real[outBaseState.GetSize()];

   if (ConvertFromBaseToBase(epoch, solarSystem, inBase, outBase, in, out))
   {
      outBaseState.Set(out);
      delete [] out;
      return true;
   }

   delete [] out;
   return false;
}


bool CoordinateConverter::ConvertFromBaseToBase(const GmatTime &epoch,
   SolarSystem *solarSystem,
   const std::string &inBase,
   const std::string &outBase,
   const Rvector &inBaseState,
   Rvector &outBaseState)
{
   // if the base types are the same, just set the output state to the input state
   if (inBase == outBase)
   {
      if (inBaseState.GetSize() != outBaseState.GetSize())
      {
         std::string errmsg = "Cannot convert from coordinate system base type ";
         errmsg += inBase + " to base type ";
         errmsg += outBase + " - state vectors passed in are of differing sizes.\n";
         throw CoordinateSystemException(errmsg);
      }
      for (Integer ii = 0; ii < inBaseState.GetSize(); ii++)
      {
         outBaseState[ii] = inBaseState[ii];
      }
      return true;
   }
   const Real *in = inBaseState.GetDataVector();
   Real       *out = new Real[outBaseState.GetSize()];

   if (ConvertFromBaseToBase(epoch, solarSystem, inBase, outBase, in, out))
   {
      outBaseState.Set(out);
      delete[] out;
      return true;
   }

   delete[] out;
   return false;
}


//------------------------------------------------------------------------------
// bool CoordinateConverter::ConvertFromBaseToBase(const A1Mjd &epoch,
//          SolarSystem *solarSystem,
//          const std::string &inBase,  const std::string &outBase,
//          const Real *inBaseState,    Real *&outBaseState)
//------------------------------------------------------------------------------
/**
 * This method converts the inBaseState vector the base system of the inCoord
 * to the base system of the outCoord, and returns the result in outBaseState.
 *
 * @param epoch            time for which to do the conversion
 * @param solarSystem      pointer to solar system to use
 * @param inBase           inCoord base system
 * @param outBase          outCoord base system
 * @param inBaseState      input state, in inCoord base system
 * @param outBaseState     output state, in outCoord base system
 *
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool CoordinateConverter::ConvertFromBaseToBase(const A1Mjd &epoch,
                                                SolarSystem *solarSystem,
		                                          const std::string &inBase,
		                                          const std::string &outBase,
                                                const Real *inBaseState,
                                                Real *outBaseState)
{
   // if the base types are the same, just set the output state to the input state
   if (inBase == outBase)
   {
      for (Integer i=0;i<6;i++) outBaseState[i] = inBaseState[i];
      return true;
   }

   // Get rotation and rotation dot matrixes:
   RotationMatrixFromICRFToFK5(epoch);
   const Real* iToF = icrfToFK5.GetDataVector();
   

   // calculate outBaseState:
   if ((inBase == "ICRF") && (outBase == "FK5"))
   {
      outBaseState[0] = iToF[0]*inBaseState[0] + iToF[1]*inBaseState[1] + iToF[2]*inBaseState[2];
      outBaseState[1] = iToF[3]*inBaseState[0] + iToF[4]*inBaseState[1] + iToF[5]*inBaseState[2];
      outBaseState[2] = iToF[6]*inBaseState[0] + iToF[7]*inBaseState[1] + iToF[8]*inBaseState[2];

      outBaseState[3] = iToF[0]*inBaseState[3] + iToF[1]*inBaseState[4] + iToF[2]*inBaseState[5];
      outBaseState[4] = iToF[3]*inBaseState[3] + iToF[4]*inBaseState[4] + iToF[5]*inBaseState[5];
      outBaseState[5] = iToF[6]*inBaseState[3] + iToF[7]*inBaseState[4] + iToF[8]*inBaseState[5];
   }
   else if ((inBase == "FK5") && (outBase == "ICRF"))
   {
      // Transpose the conversion matrix and multiply
      outBaseState[0] = iToF[0]*inBaseState[0] + iToF[3]*inBaseState[1] + iToF[6]*inBaseState[2];
      outBaseState[1] = iToF[1]*inBaseState[0] + iToF[4]*inBaseState[1] + iToF[7]*inBaseState[2];
      outBaseState[2] = iToF[2]*inBaseState[0] + iToF[5]*inBaseState[1] + iToF[8]*inBaseState[2];

      outBaseState[3] = iToF[0]*inBaseState[3] + iToF[3]*inBaseState[4] + iToF[6]*inBaseState[5];
      outBaseState[4] = iToF[1]*inBaseState[3] + iToF[4]*inBaseState[4] + iToF[7]*inBaseState[5];
      outBaseState[5] = iToF[2]*inBaseState[3] + iToF[5]*inBaseState[4] + iToF[8]*inBaseState[5];
   }
   else
   {
      std::string errmsg = "Cannot convert from coordinate system base type ";
      errmsg += inBase + " to base type ";
      errmsg += outBase + " - unknown base type.\n";
      throw CoordinateSystemException(errmsg);
   }

   return true;
}


bool CoordinateConverter::ConvertFromBaseToBase(const GmatTime &epoch,
   SolarSystem *solarSystem, //SolarSystem unused, remove
   const std::string &inBase,
   const std::string &outBase,
   const Real *inBaseState,
   Real *outBaseState)
{
   // if the base types are the same, just set the output state to the input state
   if (inBase == outBase)
   {
      for (Integer i = 0; i<6; i++) outBaseState[i] = inBaseState[i];
      return true;
   }

   // Get rotation and rotation dot matrixes:
   RotationMatrixFromICRFToFK5(epoch.GetMjd());
   const Real* iToF = icrfToFK5.GetDataVector();


   // calculate outBaseState:
   if (inBase == "ICRF")
   {
      outBaseState[0] = iToF[0] * inBaseState[0] + iToF[1] * inBaseState[1] + iToF[2] * inBaseState[2];
      outBaseState[1] = iToF[3] * inBaseState[0] + iToF[4] * inBaseState[1] + iToF[5] * inBaseState[2];
      outBaseState[2] = iToF[6] * inBaseState[0] + iToF[7] * inBaseState[1] + iToF[8] * inBaseState[2];

      outBaseState[3] = iToF[0] * inBaseState[3] + iToF[1] * inBaseState[4] + iToF[2] * inBaseState[5];
      outBaseState[4] = iToF[3] * inBaseState[3] + iToF[4] * inBaseState[4] + iToF[5] * inBaseState[5];
      outBaseState[5] = iToF[6] * inBaseState[3] + iToF[7] * inBaseState[4] + iToF[8] * inBaseState[5];
   }
   else if (inBase == "FK5")
   {
      // Transpose the conversion matrix and multiply
      outBaseState[0] = iToF[0] * inBaseState[0] + iToF[3] * inBaseState[1] + iToF[6] * inBaseState[2];
      outBaseState[1] = iToF[1] * inBaseState[0] + iToF[4] * inBaseState[1] + iToF[7] * inBaseState[2];
      outBaseState[2] = iToF[2] * inBaseState[0] + iToF[5] * inBaseState[1] + iToF[8] * inBaseState[2];

      outBaseState[3] = iToF[0] * inBaseState[3] + iToF[3] * inBaseState[4] + iToF[6] * inBaseState[5];
      outBaseState[4] = iToF[1] * inBaseState[3] + iToF[4] * inBaseState[4] + iToF[7] * inBaseState[5];
      outBaseState[5] = iToF[2] * inBaseState[3] + iToF[5] * inBaseState[4] + iToF[8] * inBaseState[5];
   }
   else
   {
      std::string errmsg = "Cannot convert from coordinate system base type ";
      errmsg += inBase + " to base type ";
      errmsg += outBase + " - unknown base type.\n";
      throw CoordinateSystemException(errmsg);
   }

   return true;
}

//------------------------------------------------------------------------------
// bool CoordinateConverter::RotateFromICRFtoFK5(Real* state, const A1Mjd& atEpoch)
//------------------------------------------------------------------------------
/**
 * This public method rotates a state vector from ICRF to FK5 (MJ2000Eq) at
 * the requested epoch. 
 *
 * @param *instate        Real 6 posvel vector instate
 * @param *outstate       Real 6 posvel vector outstate
 * @param &atEpoch        time at which to create the matrix
 *
 */
 //------------------------------------------------------------------------------
void CoordinateConverter::RotateFromICRFtoFK5(const Real* inState, Real* outState, const A1Mjd& atEpoch)
{
   //Calculate matrix
   RotationMatrixFromICRFToFK5(atEpoch);

   const Real* iToF = icrfToFK5.GetDataVector();

   outState[0] = iToF[0] * inState[0] + iToF[1] * inState[1] + iToF[2] * inState[2];
   outState[1] = iToF[3] * inState[0] + iToF[4] * inState[1] + iToF[5] * inState[2];
   outState[2] = iToF[6] * inState[0] + iToF[7] * inState[1] + iToF[8] * inState[2];

   outState[3] = iToF[0] * inState[3] + iToF[1] * inState[4] + iToF[2] * inState[5];
   outState[4] = iToF[3] * inState[3] + iToF[4] * inState[4] + iToF[5] * inState[5];
   outState[5] = iToF[6] * inState[3] + iToF[7] * inState[4] + iToF[8] * inState[5];
}


//------------------------------------------------------------------------------
// bool CoordinateConverter::RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch)
//------------------------------------------------------------------------------
/**
 * This method creates and stores a rotation matrix from ICRF to FK5 (MJ2000Eq) at
 * the requested epoch.
 *
 * @param &atEpoch            time at which to create the matrix
 *
 */
 //------------------------------------------------------------------------------
void CoordinateConverter::RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch)
{
   Real theEpoch = atEpoch.Get();
   #ifdef DEBUG_ICRF_TOFK5
         MessageInterface::ShowMessage(
            "Enter CoordinateConverter::RotationMatrixFromICRFToFK5 at epoch %18.12lf; \n\n", theEpoch);
   #endif
   
   if ((fabs(theEpoch - lastIcrfToFk5Epoch) < 1.0e-10)) //Tolerance is roughly float precision
   { 
      //Matrix is already calculated for this epoch, just return what we have
      return;
   }

   // Specify Euler rotation vector for theEpoch:
   Real vec[3];
   ICRFFile* icrfFile = ICRFFile::Instance();
   icrfFile->Initialize();
   icrfFile->GetICRFRotationVector(theEpoch, &vec[0], 3, 9);

   // Calculate rotation matrix based on Euler rotation vector:
   Real angle = GmatMathUtil::Sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
   Real a[3];
   a[0] = vec[0]/angle; a[1] = vec[1]/angle; a[2] = vec[2]/angle;
   Real c = GmatMathUtil::Cos(angle);
   Real s = GmatMathUtil::Sin(angle);

   // rotation matrix from FK5 to ICRF:
   Rmatrix33 rotM;
   rotM.SetElement(0,0, c+a[0]*a[0]*(1-c));
   rotM.SetElement(0,1, a[0]*a[1]*(1-c)+a[2]*s);
   rotM.SetElement(0,2, a[0]*a[2]*(1-c)-a[1]*s);
   rotM.SetElement(1,0, a[0]*a[1]*(1-c)-a[2]*s);
   rotM.SetElement(1,1, c+a[1]*a[1]*(1-c));
   rotM.SetElement(1,2, a[1]*a[2]*(1-c)+a[0]*s);
   rotM.SetElement(2,0, a[0]*a[2]*(1-c)+a[1]*s);
   rotM.SetElement(2,1, a[1]*a[2]*(1-c)-a[0]*s);
   rotM.SetElement(2,2, c+a[2]*a[2]*(1-c));

   // rotation matrix from ICRF to FK5:
   icrfToFK5 = rotM.Transpose();

   // rotation dot matrix from ICRF to FK5:
   icrfToFK5Dot.Set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

   //Set this epoch as the last calculated epoch
   lastIcrfToFk5Epoch = theEpoch;
   #ifdef DEBUG_ICRF_TOFK5
      MessageInterface::ShowMessage("theEpoch  = %18.12lf\n",theEpoch);

      MessageInterface::ShowMessage("rotation vector = %18.12e %18.12e %18.12e\n", vec[0], vec[1], vec[2]);
      MessageInterface::ShowMessage("R(0,0)=%18.12e,  R(0,1)=%18.12e,  R(0,2)=%18.12e\n",icrfToFK5(0,0),icrfToFK5(0,1),icrfToFK5(0,2));
      MessageInterface::ShowMessage("R(1,0)=%18.12e,  R(1,1)=%18.12e,  R(1,2)=%18.12e\n",icrfToFK5(1,0),icrfToFK5(1,1),icrfToFK5(1,2));
      MessageInterface::ShowMessage("R(2,0)=%18.12e,  R(2,1)=%18.12e,  R(2,2)=%18.12e\n",icrfToFK5(2,0),icrfToFK5(2,1),icrfToFK5(2,2));

      MessageInterface::ShowMessage("Rdot(0,0)=%18.12e,  Rdot(0,1)=%18.12e,  Rdot(0,2)=%18.12e\n",icrfToFK5Dot(0,0),icrfToFK5Dot(0,1),icrfToFK5Dot(0,2));
      MessageInterface::ShowMessage("Rdot(1,0)=%18.12e,  Rdot(1,1)=%18.12e,  Rdot(1,2)=%18.12e\n",icrfToFK5Dot(1,0),icrfToFK5Dot(1,1),icrfToFK5Dot(1,2));
      MessageInterface::ShowMessage("Rdot(2,0)=%18.12e,  Rdot(2,1)=%18.12e,  Rdot(2,2)=%18.12e\n\n\n",icrfToFK5Dot(2,0),icrfToFK5Dot(2,1),icrfToFK5Dot(2,2));
   #endif

   #ifdef DEBUG_ICRF_TOFK5
      MessageInterface::ShowMessage("NOW exiting CoordinateConverter::RotationMatrixFromICRFToFK5 ...\n\n");
   #endif

}


