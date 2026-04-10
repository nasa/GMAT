//------------------------------------------------------------------------------
//                           ThrustHistoryFile
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Code that models thrusts using a time ordered collection of points in a file.
 */
//------------------------------------------------------------------------------

#ifndef ThrustHistoryFile_hpp
#define ThrustHistoryFile_hpp

#include "ThrustFileDefs.hpp"
#include "FileReader.hpp"
#include "ThrustSegment.hpp"
#include "ThfDataSegment.hpp"
#include "FileThrust.hpp"

/**
 * Container for the thrust history file functionality
 */
class ThrustHistoryFile: public FileReader
{
public:
   ThrustHistoryFile(const std::string& theName = "");
   virtual ~ThrustHistoryFile();
   ThrustHistoryFile(const ThrustHistoryFile& thf);
   ThrustHistoryFile& operator=(const ThrustHistoryFile& thf);

   virtual GmatBase*    Clone() const;

   virtual std::string  GetParameterText(const Integer id) const;
//   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id,
                                                const Integer index) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label) const;
   virtual const StringArray&
                        GetStringArrayParameter(const std::string &label,
                                                const Integer index) const;

   // for Ref. objects
   virtual bool         HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const UnsignedInt type);
   virtual bool         SetRefObject(GmatBase *obj, const UnsignedInt type,
                                     const std::string &name = "");
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);


   virtual bool         ReadData();
   virtual bool         AllDataSegmentsLoaded(StringArray &segsNotLoaded);
   virtual void         ActivateSegments();
   virtual void         DeactivateSegments();

   virtual bool         Initialize();
   FileThrust*          GetForce();

   virtual bool         SetPrecisionTimeFlag(bool onOff);

   DEFAULT_TO_NO_CLONES

protected:

   /// Name for the thrust history file
   std::string thrustFileName;
   /// Data blocks from the file
   std::vector<ThrustSegment> segments;
   /// Data blocks from the script
   std::vector<ThrustSegment*> scriptSegments;
   /// Scripted block names
   StringArray segmentNames;
   /// Mass source for each block
   std::map<std::string, StringArray> massSources;

   /// Text phrases in the THF idenifyinf fields in the file
   StringArray keywords;
   /// Makes sure all required keywords are present
   bool startEpochFound;
   bool coordSystemFound;
   /// Keyworks that indicate the start of the thrust profile
   StringArray dataStartKeys;
   /// List of implemented interpolators
   StringArray interpolationTypes;

   /// The PhysicalModel used in conjunction with this THF
   FileThrust theForce;

   void SetSegmentData(ThfDataSegment seg);
   bool CheckDataStart(std::string theLine, ThfDataSegment &theSegment);
   bool SetHeaderField(std::string theLine, ThfDataSegment &theSegment);
   bool ReadThrustProfile(ThfDataSegment &theSegment);
   void MapField(const std::string &key, const std::string &datum,
         ThfDataSegment &theSegment);
   void ValidateSegment(ThfDataSegment &theSegment);

   /// Parameter IDs
   enum
   {
      FILENAME = GmatBaseParamCount,
      SEGMENTS,
      ThrustHistoryFileParamCount,
   };

   /// Interface parameter types
   static const Gmat::ParameterType PARAMETER_TYPE[ThrustHistoryFileParamCount - GmatBaseParamCount];
   /// Interface parameter labels
   static const std::string PARAMETER_LABEL[ThrustHistoryFileParamCount - GmatBaseParamCount];
};

#endif /* ThrustHistoryFile_hpp */
