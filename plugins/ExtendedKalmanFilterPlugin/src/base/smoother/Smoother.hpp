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


#ifndef Smoother_hpp
#define Smoother_hpp

#include "kalman_defs.hpp"
#include "SmootherBase.hpp"

/**
 * Provides core functionality used in smoothing.
 *
 * This class provides the data structures and interfaces used when performing
 * smoothing.  It uses the settings from the specified filter
 */
class KALMAN_API Smoother: public SmootherBase
{
public:
   Smoother(const std::string &name);
   virtual ~Smoother();
   Smoother(const Smoother &sm);
   Smoother&    operator=(const Smoother &sm);

   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase*);

protected:
   virtual void           SmoothState(SmootherInfoType &smootherStat, bool includeUpdate);
   virtual UnsignedInt    FindIndex(SeqEstimator::UpdateInfoType &filterInfo,
                                    std::vector<SeqEstimator::UpdateInfoType> &filterInfoVector);
   virtual bool           ObsMatch(SeqEstimator::UpdateInfoType &filterInfo1,
                                   SeqEstimator::UpdateInfoType &filterInfo2);

   virtual bool           WriteAdditionalMatData();

   virtual void           GetSqrtCov(const SeqEstimator::UpdateInfoType &filterInfo, Rmatrix &sqrtCov);
   virtual void           GetSqrtCov(const SeqEstimator::FilterMeasurementInfoType &filterMeasInfo, Rmatrix &sqrtCov);
   virtual void           SmoothCovState(const Rmatrix &Ra, const Rmatrix &Rb, const Rvector &Xa, const Rvector &Xb, Rmatrix &cov, Rvector &state);
};

#endif /* Smoother_hpp */
