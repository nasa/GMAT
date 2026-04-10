//------------------------------------------------------------------------------
//                           TFSMagicNumbers
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 11, 2014
/**
 * Magic number generator for signal based tracking data types
 */
//------------------------------------------------------------------------------

#ifndef TFSMagicNumbers_hpp
#define TFSMagicNumbers_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"

//#include "ObsData.hpp"
#include "ObservationData.hpp"

/**
 * Magic numbers for the TrackingFileSet measurements and .gmd files
 *
 * This class provides the list of magic numbers used to identify adapters in
 * the TrackingFileSet code.  These numbers are nost visible in the GMAT .gmd
 * files, where they provide an integer reference to the measurement model type
 * of the tracking data reported on a given line.  The class builds a small
 * container mapping signal nodes (by count) and measurement types to tracking
 * data type IDs used in the .gmd file, and in GMAT's estimation subsystem.
 *
 * The TFSMagicNumbers class is a singleton class.  That feature ensures that
 * the magic number for a given configuration is identical throughout all of
 * a running instance of GMAT.
 *
 * Long term, we should rework the class implemented here so that the magic
 * numbers are parsed from a configuration file rather than being implemented
 * in code.  But that is a task for another week.
 */
class ESTIMATION_API TFSMagicNumbers
{
public:
   static TFSMagicNumbers *Instance();

   StringArray GetKnownTypes();
   StringArray GetAvailableTypes();

   /////@todo: this function will be removed when users no longer use
   /////       type names DSNRange, Doppler in their script and data files
   //std::map<std::string,std::string> GetDeprecatedTypeMap();

   Integer     GetMagicNumber(const std::vector<StringArray> &nodelist,
                              const std::string &type);
   Integer     FillMagicNumber(ObservationData* theObs);
   Real        GetMNMultiplier(Integer magicNumber);
   void        SetType(ObservationData* forData);

private:
   /// The singleton instance of the class
   static TFSMagicNumbers* instance;
   /// Structure defining the magic number lookup
   struct LookupEntry
   {
      /// Flag indicating if node count is arbitrary
      bool arbitraryCount;
      /// Number of signal paths in the entry
      Integer signalPathCount;
      /// Number of unique nodes (separate participants)
      Integer nodeCount;
      /// Descriptors for the signal paths
      std::vector<StringArray> nodes;
      /// Measurement type
      std::string type;
      /// Constant multiplicative factor used to scale some measurements
      Real multFactor;
      /// The magic number
      Integer magicNumber;
   };

   /// Counter for the magic numbers
   Integer lastNumber;
   /// The table of lookup entries
   std::vector<LookupEntry*> lookupTable;
   /// The matching magic numbers
   std::vector<Integer> magicNumbers;
   /// Mapping from magic number to multiplier
   std::map<Integer,Real> factorMap;
   /// List of known measurement types (might be temporary)
   StringArray knownTypes;
   /// List of available types
   StringArray availableTypes;
   ///// Map of deprecated types
   //std::map<std::string, std::string> depTypeMap;

   TFSMagicNumbers();
   ~TFSMagicNumbers();
};

#endif // TFSMagicNumbers_hpp
