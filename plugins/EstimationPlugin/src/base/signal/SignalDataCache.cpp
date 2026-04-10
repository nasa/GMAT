//------------------------------------------------------------------------------
//                           SignalDataCache
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: 
// Created: July 10, 2019
/**
 * Data Cache used in the measurement modeling subsystem
 */
 //------------------------------------------------------------------------------

#include "SignalDataCache.hpp"
#include "SignalData.hpp"

//------------------------------------------------------------------------------
// CacheKey::CacheKey(unsigned long strandId, Real aFreq, Real time1, Real time2)
//------------------------------------------------------------------------------
/**
 * Constructor
 * 
 * @param strandId The unique strand Id calculatd from the strand
 * @param afreq    The frequency of signal   (unit: MHz)
 * @param aEpoch1  The time at which signal is transmitted from or received at ground station
 * @param aEpoch2  The time at which signal is received from or transmitted at spacecraft
 */
 //------------------------------------------------------------------------------
SignalDataCache::CacheKey::CacheKey(unsigned long strandId, Real aFreq, Real time1, Real time2) :
   strand(strandId),
   freq(trunc(aFreq * 1000000)),
   epoch1(trunc(time1 * 1000000000)),
   epoch2(trunc(time2 * 1000000000))
{ }

//------------------------------------------------------------------------------
// CacheKey::operator==(const CacheKey& k) const
//------------------------------------------------------------------------------
/**
 * Is equals operator
 * 
 * @param k The cache key to compare against the current instance
 */
 //------------------------------------------------------------------------------
bool SignalDataCache::CacheKey::operator==(const CacheKey& k) const
{
   return strand == k.strand
      && freq == k.freq
      && epoch1 == k.epoch1
      && epoch2 == k.epoch2;
}


//------------------------------------------------------------------------------
// CacheValue::CacheValue(const SignalData & sd)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param sd The signal data co build the cache value from
 * @param ic The iono correction for the signal data
 */
 //------------------------------------------------------------------------------
SignalDataCache::CacheValue::CacheValue(const SignalData & sd, const RealArray & ic) :
   tPrecTime(sd.tPrecTime),
   rPrecTime(sd.rPrecTime),
   tLoc(sd.tLoc),
   rLoc(sd.rLoc),
   tVel(sd.tVel),
   rVel(sd.rVel),
   tSTM(sd.tSTM),
   rSTM(sd.rSTM),
   tOStateSSB(sd.tOStateSSB),
   rOStateSSB(sd.rOStateSSB),
   ionoCorrection(ic)
{
}

// 
//------------------------------------------------------------------------------
// CacheKeyHasher::operator()(const CacheKey & k) const
//------------------------------------------------------------------------------
/**
 * Hashing operator for the CacheKey, uses the djb2 hashing implementation
 *
 * @param k The CacheKey to produce the hash from
 */
 //------------------------------------------------------------------------------
size_t SignalDataCache::CacheKeyHasher::operator()(const CacheKey & k) const
{  
   size_t hash = 17;
   hash = (hash * 31) + std::hash<double>()(k.epoch1);
   hash = (hash * 31) + std::hash<double>()(k.epoch2);
   hash = (hash * 31) + std::hash<double>()(k.freq);
   return (hash * 31) + k.strand;
}

// djb2 
//------------------------------------------------------------------------------
// StrandToHash(StringArray* participants)
//------------------------------------------------------------------------------
/**
 * Helper method to generate a unique Hash from a strand participant list
 *
 * @param participants an ordered StringArray of the participants in the strand
 */
 //------------------------------------------------------------------------------
unsigned long SignalDataCache::StrandToHash(const StringArray* participants)
{
   unsigned long hash = 17;
   for (auto participant : *participants) {
      hash = (hash * 31) + (std::hash<std::string>()(participant));
   }
   return hash;
}
