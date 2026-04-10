//$Id$
//------------------------------------------------------------------------------
//                               GetEphemStates
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.05.02
//
/**
 * Defines the GetEphemStates class.
 * [initialEpoch, initialState, finalEpoch, finalState] =
 *    GetEphemStates(ephemType, sat, epochFormat, coordSystem)
 */
//------------------------------------------------------------------------------
#ifndef _GetEphemStates_hpp
#define _GetEphemStates_hpp

#include "BuiltinGmatFunction.hpp"

class GMAT_API GetEphemStates : public BuiltinGmatFunction
{
public:
   GetEphemStates(const std::string &typeStr, const std::string &nomme);
   virtual ~GetEphemStates();
   GetEphemStates(const GetEphemStates &f);
   GetEphemStates& operator=(const GetEphemStates &f);
   
   virtual WrapperTypeArray
                        GetOutputTypes(IntegerArray &rowCounts,
                                       IntegerArray &colCounts) const;
   virtual void         SetOutputTypes(WrapperTypeArray &outputTypes,
                                       IntegerArray &rowCounts,
                                       IntegerArray &colCounts);
   virtual bool         Initialize(ObjectInitializer *objInit, bool reinitialize = false);
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize = false);
   virtual void         Finalize(bool cleanUp = false);
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
protected:
   Spacecraft*          inSat;
   CoordinateSystem*    coordSysOnFile;
   CoordinateSystem*    outCoordSys;
   std::string          inEphemType;
   std::string          centralBodyOnFile;
   std::string          axisSystemOnFile;
   std::string          outEpochFormat;
   Real                 ephemInitialA1Mjd;
   Real                 ephemFinalA1Mjd;
   Rvector6             ephemInitialState;
   Rvector6             ephemFinalState;
   
   bool                 ReadSpiceEphemerisFile();
   bool                 ReadCode500EphemerisFile();
   bool                 ReadSTKEphemerisFile();
   bool                 ReadCCSDSEphemerisFile();

   CoordinateSystem *   CreateLocalCoordSystem(const std::string &csName,
                                               const std::string &inOrigin,
                                               const std::string &inAxisType,
                                               bool &needsConversion);
   ElementWrapper*      CreateOutputEpochWrapper(Real a1MjdEpoch,
                                                 const std::string &outName);
   ElementWrapper*      CreateOutputStateWrapper(Integer outIndex,
                                                 const std::string &outName);
};


#endif // _GetEphemStates_hpp
