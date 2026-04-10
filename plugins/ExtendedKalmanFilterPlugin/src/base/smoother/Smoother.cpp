//$Id$
//------------------------------------------------------------------------------
//                         Smoother
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 * Class for smoother
 */
//------------------------------------------------------------------------------

#include "Smoother.hpp"

#include "GmatConstants.hpp"
#include <sstream>
#include "MessageInterface.hpp"
#include "EstimatorException.hpp"
#include "Solver.hpp"
#include "ODEModel.hpp"

// EKF mod 12/16
#include "GroundstationInterface.hpp"
#include "Spacecraft.hpp" 
#include "SpaceObject.hpp"
#include "StringUtil.hpp"
#include "Rmatrix66.hpp"
#include "StateConversionUtil.hpp"

#include <algorithm>
#include <sstream>
#include <iostream>
#include <string>


//------------------------------------------------------------------------------
// Smoother(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the derived class
 * @param name The name of the instance
 */
//------------------------------------------------------------------------------
Smoother::Smoother(const std::string &name) :
   SmootherBase            ("Smoother", name)
{
   objectTypeNames.push_back("Smoother");
}


//------------------------------------------------------------------------------
// ~Smoother()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Smoother::~Smoother()
{
}


//------------------------------------------------------------------------------
// Smoother::Smoother(const Smoother & sm) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sm The smoother that provides configuration information for this one
 */
//------------------------------------------------------------------------------
Smoother::Smoother(const Smoother & sm) :
   SmootherBase            (sm)
{
}


//------------------------------------------------------------------------------
// Smoother& operator=(const Smoother &sm)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
 *
 * @param sm The smoother that provides configuration information for this one
 *
 * @return This smoother, configured to match sm
 */
//------------------------------------------------------------------------------
Smoother& Smoother::operator=(
      const Smoother &sm)
{
   if (this != &sm)
   {
      SmootherBase::operator=(sm);
   }
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Object cloner
 *
 * @return Pointer to a new Smoother configured to match this one.
 */
 //------------------------------------------------------------------------------
GmatBase* Smoother::Clone() const
{
   return new Smoother(*this);
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
void  Smoother::Copy(const GmatBase* orig)
{
   operator=(*((Smoother*)(orig)));
}


//------------------------------------------------------------------------------
// void SmoothState(SmootherInfoType &smootherStat, bool includeUpdate)
//------------------------------------------------------------------------------
/**
 * Smooths the state based on the forward and backward filter data
 *
 * @param smootherStat The smoother structure to store the resultant smoothed state to.
 * @param includeUpdate Flag to indicate if the post-update (in the forward sense)
 *        data should be used.
 */
 //------------------------------------------------------------------------------
void Smoother::SmoothState(SmootherInfoType &smootherStat, bool includeUpdate)
{
   UnsignedInt backFilterIndex = FindIndex(forwardFilterInfo[filterIndex],
                                           backwardFilterInfo);

   Rvector forwardState = forwardFilterInfo[filterIndex].state;
   Rvector backwardState = backwardFilterInfo[backFilterIndex].state;

   Rmatrix forwardSqrtCov(stateSize, stateSize);
   Rmatrix backwardSqrtCov(stateSize, stateSize);

   GetSqrtCov(forwardFilterInfo[filterIndex], forwardSqrtCov);
   GetSqrtCov(backwardFilterInfo[backFilterIndex], backwardSqrtCov);

   smootherStat.epoch = forwardFilterInfo[filterIndex].epoch;
   smootherStat.isObs = forwardFilterInfo[filterIndex].isObs && includeUpdate;

   if (smootherStat.isObs)
   {
      Rvector forwardAprioriState = forwardFilterInfo[filterIndex].measStat.state;
      Rvector backwardAprioriState = backwardFilterInfo[backFilterIndex].measStat.state;

      Rmatrix forwardAprioriSqrtCov(stateSize, stateSize);
      Rmatrix backwardAprioriSqrtCov(stateSize, stateSize);

      GetSqrtCov(forwardFilterInfo[filterIndex].measStat, forwardAprioriSqrtCov);
      GetSqrtCov(backwardFilterInfo[backFilterIndex].measStat, backwardAprioriSqrtCov);

      Rmatrix smoothCov;
      Rvector smoothState;
      SmoothCovState(forwardSqrtCov, backwardAprioriSqrtCov,
         forwardState, backwardAprioriState, smoothCov, smoothState);

      smootherStat.cov.SetSize(smoothCov.GetNumRows(), smoothCov.GetNumColumns());
      smootherStat.cov = smoothCov;
      smootherStat.state = smoothState.GetRealArray();

      SmoothCovState(forwardAprioriSqrtCov, backwardSqrtCov,
         forwardAprioriState, backwardState, smoothCov, smoothState);

      smootherStat.measStat.cov.SetSize(smoothCov.GetNumRows(), smoothCov.GetNumColumns());
      smootherStat.measStat.cov = smoothCov;
      smootherStat.measStat.state = smoothState.GetRealArray();

      Rmatrix covVNB = GetCovarianceVNB(smootherStat.measStat.cov);
      smootherStat.measStat.covVNB.SetSize(covVNB.GetNumRows(), covVNB.GetNumColumns());
      smootherStat.measStat.covVNB = covVNB;
   }
   else
   {
      if (forwardFilterInfo[filterIndex].isObs)
      {
         // Replace forward state & cov with pre-update values
         forwardState = forwardFilterInfo[filterIndex].measStat.state;
         GetSqrtCov(forwardFilterInfo[filterIndex].measStat, forwardSqrtCov);
      }

      Rmatrix smoothCov;
      Rvector smoothState;
      SmoothCovState(forwardSqrtCov, backwardSqrtCov,
         forwardState, backwardState, smoothCov, smoothState);

      smootherStat.cov.SetSize(smoothCov.GetNumRows(), smoothCov.GetNumColumns());
      smootherStat.cov = smoothCov;
      smootherStat.state = smoothState.GetRealArray();
   }

   Rmatrix covVNB = GetCovarianceVNB(smootherStat.cov);
   smootherStat.covVNB.SetSize(covVNB.GetNumRows(), covVNB.GetNumColumns());
   smootherStat.covVNB = covVNB;
}


//------------------------------------------------------------------------------
// void GetSqrtCov(const SeqEstimator::UpdateInfoType &filterInfo, Rmatrix &sqrtCov)
//------------------------------------------------------------------------------
/**
 * Gets the matrix square root of the covariance matrix from a filter info struct.
 * This function uses the covariance square root in the struct, if available.
 * If the square root covariance is not available, this function calculates it
 * from the full covariance.
 *
 * @param filterInfo The filter update to obtain the covariance from.
 * @param sqrtCov The matrix to store the resultant matrix square root into.
 */
 //------------------------------------------------------------------------------
void Smoother::GetSqrtCov(const SeqEstimator::UpdateInfoType &filterInfo, Rmatrix &sqrtCov)
{
   if (filterInfo.sqrtCov.GetNumRows() == stateSize && filterInfo.sqrtCov.GetNumColumns() == stateSize)
   {
      sqrtCov = filterInfo.sqrtCov;
   }
   else
   {
      cf.Factor(filterInfo.cov, sqrtCov);
      sqrtCov = sqrtCov.Transpose();
   }
}


//------------------------------------------------------------------------------
// void GetSqrtCov(const SeqEstimator::FilterMeasurementInfoType &filterMeasInfo, Rmatrix &sqrtCov)
//------------------------------------------------------------------------------
/**
 * Gets the matrix square root of the covariance matrix from a filter measurement info struct.
 * This function uses the covariance square root in the struct, if available.
 * If the square root covariance is not available, this function calculates it
 * from the full covariance.
 *
 * @param filterMeasInfo The filter measurement info to obtain the covariance from.
 * @param sqrtCov The matrix to store the resultant matrix square root into.
 */
 //------------------------------------------------------------------------------
void Smoother::GetSqrtCov(const SeqEstimator::FilterMeasurementInfoType &filterMeasInfo, Rmatrix &sqrtCov)
{
   if (filterMeasInfo.sqrtCov.GetNumRows() == stateSize && filterMeasInfo.sqrtCov.GetNumColumns() == stateSize)
   {
      sqrtCov = filterMeasInfo.sqrtCov;
   }
   else
   {
      cf.Factor(filterMeasInfo.cov, sqrtCov);
      sqrtCov = sqrtCov.Transpose();
   }
}


//------------------------------------------------------------------------------
// void SmoothCovState(const Rmatrix &Ra, const Rmatrix &Rb, const Rvector &Xa, const Rvector &Xb, Rmatrix &cov, Rvector &state)
//------------------------------------------------------------------------------
/**
 * Smooths the provided covariance and state from the forward and backward filter.
 *
 * @param Ra The covariance square root from the forward filter.
 * @param Rb The covariance square root from the backward filter.
 * @param Xa The state from the forward filter.
 * @param Xb The state from the backward filter.
 * @param cov The output smoothed covariance (this is a full covariance, not a square root).
 * @param state The output smoothed state).
 */
 //------------------------------------------------------------------------------
void Smoother::SmoothCovState(const Rmatrix &Ra, const Rmatrix &Rb, const Rvector &Xa, const Rvector &Xb, Rmatrix &cov, Rvector &state)
{
   Rmatrix Rd = thinQR(Ra, Rb);
   Rmatrix Sd = Rd.Inverse();
   Rmatrix Ua = Sd * Ra;
   Rmatrix Ub = Sd * Rb;
   Rmatrix Mc = thinQR(Rb*Ub.Transpose()*Ua, Ra*Ua.Transpose()*Ub);

   cov.SetSize(Mc.GetNumRows(), Mc.GetNumColumns());
   cov = Mc * Mc.Transpose();
   Symmetrize(cov);

   Rvector r = Sd * (Xb - Xa);
   state.SetSize(stateSize);
   state = Xa + Ra * Ua.Transpose()*r;
}


//------------------------------------------------------------------------------
// UnsignedInt FindIndex(SeqEstimator::UpdateInfoType &filterInfo,
//                       std::vector<SeqEstimator::UpdateInfoType> &filterInfoVector)
//------------------------------------------------------------------------------
/**
 * Find the index of a filter info struct that matches the provied struct
 */
 //------------------------------------------------------------------------------
UnsignedInt Smoother::FindIndex(SeqEstimator::UpdateInfoType &filterInfo,
                                std::vector<SeqEstimator::UpdateInfoType> &filterInfoVector)
{
   bool found = false;
   UnsignedInt searchIndex = 0;


   int high = filterInfoVector.size() - 1;
   int low = 0;
   int index = 0;

   while (high >= low)
   {
	   index = low + (high - low) / 2;

	   if (GmatMathUtil::IsEqual(filterInfo.epoch, filterInfoVector[index].epoch, ESTTIME_ROUNDOFF))
	   {
		   if (ObsMatch(filterInfo, filterInfoVector[index]))
			   return index;
	   }

	   if (filterInfoVector[index].epoch < filterInfo.epoch)
		   high = index - 1;
	   else
		   low = index + 1; 
   }

   // Some corner cases are not covered with the binary search above so we search again in those cases. Very very rare.

   for (UnsignedInt ii = 0U; ii < filterInfoVector.size(); ii++)
   {
      if (GmatMathUtil::IsEqual(filterInfo.epoch, filterInfoVector[ii].epoch, ESTTIME_ROUNDOFF))
      {
         found = true;
         searchIndex = ii;

         if (ObsMatch(filterInfo, filterInfoVector[ii]))
            break;
      }
   }

   
   if(!found)
      throw EstimatorException("Unable to find a matching epoch between the forward and backward filter data while smoothing");

   return searchIndex;
}


//------------------------------------------------------------------------------
// bool ObsMatch(SeqEstimator::UpdateInfoType &filterInfo1,
//               SeqEstimator::UpdateInfoType &filterInfo2)
//------------------------------------------------------------------------------
/**
 * Find if the two filter info struct correspond to the same measurement(s)
 */
 //------------------------------------------------------------------------------
bool Smoother::ObsMatch(SeqEstimator::UpdateInfoType &filterInfo1,
                               SeqEstimator::UpdateInfoType &filterInfo2)
{
   // See if one has a measurement while the other doesn't
   if (filterInfo1.isObs != filterInfo2.isObs)
      return false;

   // If both are not measurements, we're done
   if (!filterInfo1.isObs)
      return true;

   // If both are measurements, need to compare record numbers
   // TODO: When batch update is implemented, will need to check that each element
   //       of recNum match
   if (filterInfo1.measStat.recNum == filterInfo2.measStat.recNum)
      return true;

   // Otherwise, false
   return false;
}


//------------------------------------------------------------------------------
// bool WriteAdditionalMatData()
//------------------------------------------------------------------------------
/**
 * Method used to write additional Smoother data to the MATLAB .mat file
 *
 * @return true on success
 */
 //------------------------------------------------------------------------------
bool Smoother::WriteAdditionalMatData()
{
   bool retval = true;

   StringArray dataDesc;

   DataBucket matBackComputed, matBackFilter;
   IntegerMap matBackComputedIndex, matBackFilterIndex;
   matBackComputed.SetInitialRealValue(NAN);
   matBackFilter.SetInitialRealValue(NAN);

   // populate backwards filter mat data
   for (UnsignedInt ii = 0U; ii < backwardFilterInfo.size(); ii++)
   {
      AddMatlabFilterData(backwardFilterInfo[ii], matBackFilter, matBackFilterIndex);

      if (backwardFilterInfo[ii].isObs)
         AddMatlabData(backwardFilterInfo[ii].measStat, matBackComputed, matBackComputedIndex);
   }

   // Add backward filter computed data
   matWriter->ClearData();
   AddMatData(matBackComputed);

   dataDesc = GetMatDataDescription(matBackComputed);
   matWriter->DescribeData(dataDesc);
   matWriter->WriteData("BackwardComputed");

   // Add backward filter data
   matWriter->ClearData();
   AddMatData(matBackFilter);

   dataDesc = GetMatDataDescription(matBackFilter);
   matWriter->DescribeData(dataDesc, iterationsTaken);
   matWriter->WriteData("BackwardFilter");

   return retval;
}
