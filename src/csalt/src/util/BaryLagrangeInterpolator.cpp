//------------------------------------------------------------------------------
//                      BaryLagrange Interpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Youngkwang Kim
// Created: 2015/06/23
//
/**
 * Implementation of the Barycentric Lagrange Interpolator class
 * (it does not follow the Interpolator base class)
 */
//------------------------------------------------------------------------------
//#include <iostream>
#include "BaryLagrangeInterpolator.hpp"
#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
//  BaryLagrangeInterpolator()
//------------------------------------------------------------------------------
/**
 * Constructs a default barycentric Lagrange Interpolator.
 * 
 */
//------------------------------------------------------------------------------
BaryLagrangeInterpolator::BaryLagrangeInterpolator() :
   numIndVarVec              (0),
   numInterpPointVec         (0),
   indVarLB                  (0.0),
   indVarUB                  (0.0),
   isInterpPointVecDefined   (false),
   isIndVarVecDefined        (false)
{
}

//------------------------------------------------------------------------------
//  BaryLagrangeInterpolator(const BarycentricLagrnageInterpolator &bli)
//------------------------------------------------------------------------------
/**
 * Copy Constructor of barycentric Lagrange Interpolator
 * 
 */
//------------------------------------------------------------------------------
BaryLagrangeInterpolator::BaryLagrangeInterpolator(
                                 const BaryLagrangeInterpolator &bli) :
   numIndVarVec              (bli.numIndVarVec),
   numInterpPointVec         (bli.numInterpPointVec),
   indVarLB                  (bli.indVarLB),
   indVarUB                  (bli.indVarUB),
   isInterpPointVecDefined   (bli.isInterpPointVecDefined),
   isIndVarVecDefined        (bli.isIndVarVecDefined)
{
   if (isIndVarVecDefined == true)
   {
      indVar       = bli.indVar;
      weigthVec    = bli.weigthVec;
//      indVarLB     = bli.indVarLB;
//      indVarUB     = bli.indVarUB;
//      numIndVarVec = bli.numIndVarVec;
   }
   if (isInterpPointVecDefined == true)
   {      
//      numInterpPointVec = bli.numInterpPointVec;
      barycentricMatrix = bli.barycentricMatrix;
   }   
}

//------------------------------------------------------------------------------
//  BaryLagrangeInterpolator&   operator=
//                                  (const BaryLagrangeInterpolator &bli)
//------------------------------------------------------------------------------
/**
 * Assignment operator of barycentric Lagrange Interpolator.
 * 
 */
//------------------------------------------------------------------------------
BaryLagrangeInterpolator&   BaryLagrangeInterpolator::operator=
                                    (const BaryLagrangeInterpolator &bli)
{
   if (&bli == this)
      return *this;

   numIndVarVec              = bli.numIndVarVec;
   numInterpPointVec         = bli.numInterpPointVec;
   indVarLB                  = bli.indVarLB;
   indVarUB                  = bli.indVarUB;
   isInterpPointVecDefined   = bli.isInterpPointVecDefined;
   isIndVarVecDefined        = bli.isIndVarVecDefined;

   if (isIndVarVecDefined == true)
   {
      Integer sz = bli.indVar.GetSize();
      indVar.SetSize(sz);
      indVar       = bli.indVar;
      
      sz = bli.weigthVec.GetSize();
      weigthVec.SetSize(sz);
      weigthVec    = bli.weigthVec;
//      indVarLB     = bli.indVarLB;
//      indVarUB     = bli.indVarUB;
//      numIndVarVec = bli.numIndVarVec;
   }
   if (isInterpPointVecDefined == true)
   {      
//      numInterpPointVec = bli.numInterpPointVec;
      Integer r, c;
      bli.barycentricMatrix.GetSize(r, c);
      barycentricMatrix.SetSize(r, c);
      barycentricMatrix = bli.barycentricMatrix;
   }   
   return *this;
}

//------------------------------------------------------------------------------
//  ~BaryLagrangeInterpolatorInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys the core elements of an Interpolator.
 */
//------------------------------------------------------------------------------
BaryLagrangeInterpolator::~BaryLagrangeInterpolator()
{
   // do nothing
}


//------------------------------------------------------------------------------
// void SetIndVarVec(const Rvector *indVarVec)
//------------------------------------------------------------------------------
/**
 * Set indVarVec which is independent data points where the dependent variable
 * data is given.
 * As setting indVarVec, weigthVec is also calculated.
 *
 * @param <indVarVec>    data points of the independent variable
 *
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::SetIndVarVec(const Rvector *indVarVec)
{
   // Given a time vector, insert it in the decision vector  
   
   if (ChkIndVarVecFeasibility(indVarVec) == true)
   {
      numIndVarVec = indVarVec->GetSize();
      indVar.SetSize(numIndVarVec);

      indVarLB = (*indVarVec)(0);
      indVarUB = (*indVarVec)(numIndVarVec - 1);
      indVar   = (*indVarVec);
      CalWeightVec();

      isIndVarVecDefined = true;
   }
   else
      isIndVarVecDefined = false;

   return;
}

//------------------------------------------------------------------------------
// bool SetInterpPointVec(Rvector interpPointVec)
//------------------------------------------------------------------------------
/**
 * Set interpPointVec which is independent data points where interpolation
 * will be made. Before this method is called, indVarVec must be defined.
 * As setting interpPointVec, barycentricMatrix is also calculated.
 *
 * @param <interpPoints> the independent data points where the interpolation
 *                       needs to be done
 *
 * @return if setting interpPointVec is successful, return true;
 *         otherwise return false.
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::SetInterpPointVec
                                      (const Rvector *interpPointVec)
{
   if (isIndVarVecDefined == false)
   {
      std::string errmsg=
                  "For BaryLagrangeInterpolator::SetInterpPointVec,";
      errmsg += "indVarVec must be defined, ";
      errmsg += "before setting interpPointVec!";
      throw LowThrustException(errmsg);
      return;
   }
   if ( ChkInterpPointVecFeasibility(interpPointVec) == true)
   {
      CalBarycentricMatrix(interpPointVec);
      numInterpPointVec = interpPointVec->GetSize();

      isInterpPointVecDefined = true;
   }
   else
      isInterpPointVecDefined = false;

   return;
}

//------------------------------------------------------------------------------
//  Integer  GetNumIndVarVec()
//------------------------------------------------------------------------------
/**
 * Returns the number of indVar elements.
 * 
 * @return  The number of elements of indVarVec. 
 *          If it is not defined yet, return 0.
 */
//------------------------------------------------------------------------------
Integer  BaryLagrangeInterpolator::GetNumIndVarVec()
{
   if (isIndVarVecDefined == true)
      return numIndVarVec;
   else
      return 0; 
}

//------------------------------------------------------------------------------
//  Integer  GetNumInterpPointVec()
//------------------------------------------------------------------------------
/**
 * Returns the number of indVar elements.
 * 
 * @return  The number of elements of interpPointVec. 
 *          If it is not defined yet, return 0.
 */
//------------------------------------------------------------------------------
Integer  BaryLagrangeInterpolator::GetNumInterpPointVec()
{
   if (isInterpPointVecDefined == true)
      return numInterpPointVec;
   else
      return 0; 
}

//------------------------------------------------------------------------------
//  bool IsInterpPointVecDefined()
//------------------------------------------------------------------------------
/**
 * Returns flag indicating whether or not the interpolation point vector has
 * been defined
 * 
 * @return  if interpPointVec is defined, return true; otherwise
 *          return false
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::IsInterpPointVecDefined()
{
   return isInterpPointVecDefined;
}

//------------------------------------------------------------------------------
//  bool IsIndVarVecDefined()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not the independent variable vector
 * has been defined
 * 
 * @return  if indVarVec is defined, return true; otherwise
 *          return false
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::IsIndVarVecDefined()
{
   return isIndVarVecDefined;
}

//------------------------------------------------------------------------------
// void Interpolate(Rvector        indVarVec,
//                  Rvector        funcValueVec,
//                  Rvector        interpPointVec,
//				        Real           *resultVec)
//------------------------------------------------------------------------------
/**
 * Perform barycentric Lagrange interpolation, interpolation data will be
 * written at *resultVec.
 *
 * Interpolate with the input indVarVec and interpPointVec.
 * Note that whenever indVarVec or interpPointVec is delivered to
 * interpolator, additional calculation of weigthVec or barycentricMatrix
 * will be made, respectively.
 * So,
 * it is important to choose a proper 'interpolate' method based on situation.
 *
 * @param <indVarVec>      data points of the independent variable
 * @param <funcValueVec>   data points of the dependent variable
 * @param <interpPointVec> the independent data points where the interpolation
 *                         needs to be done
 * @param <resultVec>      the dependent data obtained by interpolation
 *
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::Interpolate(const Rvector *indVarVec,
	                                        const Rvector *funcValueVec,
                                           const Rvector *interpPointVec,
					                            Rvector       &resultVec)
{
   SetIndVarVec(indVarVec);
   SetInterpPointVec(interpPointVec);
   ChkFuncValueVecFeasibility(funcValueVec);

   CalBarycentricMatrix(interpPointVec);
   
   resultVec.SetSize(interpPointVec->GetSize());
   resultVec = barycentricMatrix*(*funcValueVec);
   return;
}

//------------------------------------------------------------------------------
// void Interpolate(Rvector    funcValueVec,
//                  Rvector    interpPointVec,
//				        Real       *resultVec)
//------------------------------------------------------------------------------
/**
 * Perform barycentric Lagrange interpolation, interpolation data will be
 * written at *resultVec.
 *
 * Interpolate with the pre-defined indVarVec and the input interpPointVec.
 * Note that whenever indVarVec or interpPointVec is delivered to
 * interpolator, additional calculation of weigthVec or barycentricMatrix
 * will be made, respectively.
 * So,
 * it is important to choose a proper 'interpolate' method based on situation.
 *
 * @param <funcValueVec>   data points of the dependent variable
 * @param <interpPointVec> the independent data points where the interpolation
 *                         needs to be done
 * @param <resultVec>      the dependent data obtained by interpolation
 *
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::Interpolate(const Rvector *funcValueVec,
	                                               const Rvector *interpPointVec,
					                                   Rvector       &resultVec)
{
   // check whether interpolation is possible
   if (isIndVarVecDefined== false)
   {
      std::string errmsg="For BaryLagrangeInterpolator::Interpolate,";
      errmsg += "Because indVarVec is not defined, ";
      errmsg += "it must be provided as an input!;";
      throw LowThrustException(errmsg);
   }

   SetInterpPointVec(interpPointVec);
   ChkFuncValueVecFeasibility(funcValueVec);
   
   CalBarycentricMatrix(interpPointVec);
   
   resultVec.SetSize(interpPointVec->GetSize());
   resultVec = barycentricMatrix*(*funcValueVec);
   return;
}

//------------------------------------------------------------------------------
// void Interpolate(Rvector    funcValueVec,
//				        Real       *resultVec)
//------------------------------------------------------------------------------
/**
 * Perform barycentric Lagrange interpolation, interpolation data will be
 * written at *resultVec.
 *
 * Interpolate with the pre-defined indVarVec and interpPointVec.
 * Note that whenever indVarVec or interpPointVec is delivered to
 * interpolator, additional calculation of weigthVec or barycentricMatrix
 * will be made, respectively.
 * So,
 * it is important to choose a proper 'interpolate' method based on situation.
 *
 * @param <funcValueVec>   data points of the dependent variable
 * @param <resultVec>      the denpendent data obtained by interpolation
 * @param <numResultVec>   the size of resultVec used for checking validity
 *
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::Interpolate(const Rvector *funcValueVec,
	                                               Rvector       &resultVec)
{
   // check whether interpolation is possible
   if (isIndVarVecDefined== false)
   {
      std::string errmsg="For BaryLagrangeInterpolator::Interpolate,";
      errmsg += "Because indVarVec is not defined, ";
      errmsg += "it must be provided as an input!;";
      throw LowThrustException(errmsg);
   }
   if (isInterpPointVecDefined== false)
   {
      std::string errmsg="For BaryLagrangeInterpolator::Interpolate,";
      errmsg += "Because interpPointVec is not defined, ";
      errmsg += "it must be provided as an input!;";
      throw LowThrustException(errmsg);
   }
   ChkFuncValueVecFeasibility(funcValueVec);

   resultVec.SetSize(barycentricMatrix.GetNumRows());
   resultVec = barycentricMatrix*(*funcValueVec);
   return;
}

//------------------------------------------------------------------------------
//  bool IsStrictlyIncreasing(const Rvector *testVec)
//------------------------------------------------------------------------------
/**
 * Checks whether the test vector is strictly increasing or not.
 *
 * @param <testVec>  the subject of 'strictly-increasing-ness' test.
 *
 * @return  if testVec is strictly increasing, return true; 
 *          otherwise return false.
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::IsStrictlyIncreasing(
                                      const Rvector *testVec)
{
   Integer  sizeOfTestVec = testVec->GetSize();
   if (sizeOfTestVec != 1)
   {
      for (int idx = 1; idx < sizeOfTestVec; ++idx)
      {
         if (((*testVec)(idx) - (*testVec)(idx-1)) <= 0.0)
             return false;
      }
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool CheckIndVarVecFeasibility(Rvector indVarVec)
//------------------------------------------------------------------------------
/**
 * Checks whether indVarVec is feasible or not.
 *
 * @param <indVarVec>    data points of the independent variable
 *
 * @return  if indVarVec is feasible, return true; otherwise, return false
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::
	 ChkIndVarVecFeasibility(const Rvector *indVarVec)
{
   if (indVarVec->GetSize() < 2)
   {
	   std::string errmsg="For BaryLagrangeInterpolator";
      errmsg += "::ChkIndVarVecFeasibility, an indVarVec must have ";
      errmsg += "at least two elements!";
      throw LowThrustException(errmsg);
      return false;
   }
   else
   {
      bool isStrictlyIncreasing = IsStrictlyIncreasing(indVarVec);
      if (isStrictlyIncreasing == false)
      {
	      std::string errmsg="For BaryLagrangeInterpolator";
         errmsg += "::ChkIndVarVecFeasibility, an indVarVec must be ";
         errmsg += "a strictly increasing vector!";
         throw LowThrustException(errmsg);
         return false;
      }      
   }
   return true;
}

//------------------------------------------------------------------------------
//  bool CheckInterpPointVecFeasibility(Rvector interpPointVec)
//------------------------------------------------------------------------------
/**
 * Checks whether interpPoint is feasible or not.
 *
 * @param <interpPointVec> the independent data points where the interpolation
 *                         needs to be done
 *
 * @return  if interpPointVec is feasible, return true; otherwise, return false
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::
	 ChkInterpPointVecFeasibility(const Rvector *interpPointVec)
{

   bool isStrictlyIncreasing = IsStrictlyIncreasing(interpPointVec);

   // check whether it is strictly increasing or not

   if (isStrictlyIncreasing == false)
   {
	   std::string errmsg="For BaryLagrangeInterpolator";
      errmsg += "::ChkInterpPointVecFeasibility, an interpPointVec must be ";
      errmsg += "a strictly increasing vector!";
      throw LowThrustException(errmsg);
      return false;
   }
   // get the number of interpPoints for futher tests
   // Integer numInterpPointVec = (Integer) interpPointVec->GetSize();
   // mute above line (see below) to remove compiler warning


   // the following block is for checking 
   // whether any interp point is identical to any of indVarVec element
   for (int idx1 = 0; idx1 < numInterpPointVec; ++idx1)
   {
      for (int idx2 =0; idx2 < numIndVarVec; ++idx2)
      {
         // check any interpPoint is identical to 
		 // any indVar point.

         if ((*interpPointVec)(idx1) == indVar(idx2))
		   {
	         std::string errmsg="For BaryLagrangeInterpolator";
            errmsg += "::ChkInterpPointVecFeasibility, an interpPoint is ";
            errmsg += "identical to an indVar point!";
            throw LowThrustException(errmsg);
            return false;
		   }
      }
   }
   
   // all tests have been passed. 
   return true;              
}

//------------------------------------------------------------------------------
//  bool CheckFuncValueVecFeasibility(Rvector funcValueVec)
//------------------------------------------------------------------------------
/**
 * Checks whether FuncValueVec is feasible or not.
 *
 * @param <funcValueVec>   data points of the dependent variable
 *
 * @return  if FuncValueVec is feasible, return true; otherwise, return false
 */
//------------------------------------------------------------------------------
bool BaryLagrangeInterpolator::ChkFuncValueVecFeasibility(
                                      const Rvector *funcValueVec)
{
   if (funcValueVec->GetSize() != numIndVarVec)
   {
	   std::string errmsg = "For BaryLagrangeInterpolator";
      errmsg += "::ChkFuncValueVecFeasibility, the length of func value vector";
      errmsg += " must be same as that of indVarVec!";
      throw LowThrustException(errmsg);
      return false;
   }
   else
      return true;
}

//------------------------------------------------------------------------------
//  void CalBarycentricMatrix(Rvector interpPointVec)
//------------------------------------------------------------------------------
/**
 * Calculates barycentricMatrix for Lagrange interpolation.
 *
 * @param <interpPointVec> the independent data points where the interpolation
 *                         needs to be done
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::CalBarycentricMatrix(
                                      const Rvector *interpPointVec)
{
   Integer numInterpPoints = (Integer) interpPointVec->GetSize();
   
   // allocate new memory for barycentricMatrix
   barycentricMatrix.SetSize(numInterpPoints,numIndVarVec);

   Rvector normalizationVec(numInterpPoints);

   Integer  colIdx, rowIdx;

   for (colIdx = 0; colIdx < numIndVarVec; colIdx++)
   {
      for (rowIdx = 0; rowIdx < numInterpPoints; rowIdx++)
	  {
         barycentricMatrix(rowIdx,colIdx) = 
            weigthVec(colIdx)/((*interpPointVec)(rowIdx) - indVar(colIdx));
		 normalizationVec(rowIdx) += barycentricMatrix(rowIdx,colIdx); 
	  }                                      
   }
   for (rowIdx = 0; rowIdx < numInterpPoints; rowIdx++)
   {
      for (colIdx = 0; colIdx < numIndVarVec; colIdx++)
	  {
         barycentricMatrix(rowIdx,colIdx) /= normalizationVec(rowIdx);
	  }                                      
   }
}

//------------------------------------------------------------------------------
//  void CalWeightVec()
//------------------------------------------------------------------------------
/**
 * Calculates weightVec for barycentric Lagrange interpolation.
 */
//------------------------------------------------------------------------------
void BaryLagrangeInterpolator::CalWeightVec()
{
   // allocate new memory for weigthVec
   weigthVec.SetSize(numIndVarVec);

   for (int idx1 = 0; idx1 < numIndVarVec; idx1++)
   {
      weigthVec(idx1) = 1;
      for (int idx2 = 0; idx2 < numIndVarVec; idx2++)
	  {
         if (idx1 != idx2)
			 weigthVec(idx1) /= (indVar(idx1) - indVar(idx2));
	  }                                      
   }
}
