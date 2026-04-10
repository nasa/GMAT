//$Id$
//------------------------------------------------------------------------------
//                                  Subscriber
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
// number S-67573-G
//
// *** File Name : Subscriber.hpp
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 10/01/2003 - L. Ruley, Missions Applications Branch
//                             Updated style using GMAT cpp style guide
// **************************************************************************
//
/**
 * Definition for the Subscriber base class.
 */
//------------------------------------------------------------------------------
#ifndef Subscriber_hpp
#define Subscriber_hpp

#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "SolarSystem.hpp"
#include "ElementWrapper.hpp"

class GMAT_API Subscriber : public GmatBase
{
public:
   Subscriber(const std::string &typeStr, const std::string &nomme);
   Subscriber(const Subscriber &);
   Subscriber& operator=(const Subscriber &);
   virtual ~Subscriber(void);
   
   virtual bool         Initialize();
   virtual bool         IsInitialized();
   virtual bool         ReceiveData(const char * datastream);
   virtual bool         ReceiveData(const char * datastream, const Integer len);
   virtual bool         ReceiveData(const Real * datastream, const Integer len = 0);
   virtual bool         FlushData(bool endOfDataBlock = true);
   virtual bool         SetEndOfRun();
   virtual void         SetRunState(Gmat::RunState rs);
   void                 SetManeuvering(GmatBase *maneuver, bool flag, Real epoch,
                                       const std::string &satName,
                                       const std::string &desc);
   void                 SetManeuvering(GmatBase *maneuver, bool flag, Real epoch,
                                       const StringArray &satNames,
                                       const std::string &desc);
   void                 SetSpacecraftPropertyChanged(GmatBase *originator, Real epoch,
                                                     const std::string &satName,
                                                     const std::string &desc);
   virtual void         SetOrbitColorChanged(GmatBase *originator,
                                             const std::string &newColor,
                                             const std::string &objName,
                                             const std::string &desc);
   virtual void         SetTargetColorChanged(GmatBase *originator,
                                              const std::string &newColor,
                                              const std::string &objName,
                                              const std::string &desc);
   virtual void         SetSegmentOrbitColor(GmatBase *originator,
                                             bool overrideColor,
                                             UnsignedInt orbitColor,
                                             const StringArray &objNames);
   
   Subscriber*          Next();
   bool                 Add(Subscriber *s);
   bool                 Remove(Subscriber *s, const bool del);
   
   virtual bool         Activate(bool state = true);
   virtual bool         IsActive();
   
   virtual void         SetProviderId(Integer id);
   virtual Integer      GetProviderId();
   
   virtual void         SetProvider(GmatBase *provider, Real epochInMjd = -999.999);
   virtual void         SetDataLabels(const StringArray& elements);
   virtual void         SetPropagationDirection(Real propDir);
   virtual void         ClearDataLabels();
   virtual void         SetInternalCoordSystem(CoordinateSystem *cs);
   virtual void         SetDataCoordSystem(CoordinateSystem *cs);
   virtual void         SetDataMJ2000EqOrigin(CelestialBody *cb);
   virtual void         SetSolarSystem(SolarSystem *ss);
   
   // methods for element wrappers
   virtual const StringArray&
                        GetWrapperObjectNameArray(bool completeSet = false);
   virtual bool         SetElementWrapper(ElementWrapper* toWrapper,
                                          const std::string &name);
   virtual void         ClearWrappers(bool clearX = false, bool clearY = true);
   
   // for rename reference objects
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   // methods for parameters
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterVisible(const Integer id) const;
   virtual bool         IsParameterVisible(const std::string &label) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const char *value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);
   
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   virtual bool         GetBooleanParameter(const std::string &label) const;
   virtual bool         SetBooleanParameter(const std::string &label,
                                            const bool value);
   virtual std::string  GetOnOffParameter(const Integer id) const;
   virtual bool         SetOnOffParameter(const Integer id, 
                                          const std::string &value);
   virtual std::string  GetOnOffParameter(const std::string &label) const;
   virtual bool         SetOnOffParameter(const std::string &label, 
                                          const std::string &value);
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual const Rvector& GetRvectorParameter(const Integer id) const;
   virtual const Rvector& SetRvectorParameter(const Integer id,
                                              const Rvector &value);
   virtual const Rvector& GetRvectorParameter(const std::string &label) const;
   virtual const Rvector& SetRvectorParameter(const std::string &label,
                                              const Rvector &value);
   
   DEFAULT_TO_NO_CLONES

   enum SolverIterOption
   {
      SI_ALL,
      SI_CURRENT,
      SI_NONE,
      SolverIterOptionCount
   };
   
protected:
   
   std::string          mSolverIterations;
   SolverIterOption     mSolverIterOption;
   
   /// Arrays used to track elements for published data
   std::vector<StringArray> theDataLabels;
   
   const char           *data;
   Subscriber           *next;
   CoordinateSystem     *theInternalCoordSystem;
   CoordinateSystem     *theDataCoordSystem;
   CelestialBody        *theDataMJ2000EqOrigin;
   SolarSystem          *theSolarSystem;
   GmatBase             *currentProvider;
   
   bool                 active;
   bool                 isManeuvering;
   bool                 isEndOfReceive;
   bool                 isEndOfDataBlock;
   bool                 isEndOfRun;
   bool                 isInitialized;
   bool                 isFinalized;
   bool                 isDataOn;
   bool                 isDataStateChanged;
   
   // arrays for holding position and size
   Rvector              mPlotUpperLeft;
   Rvector              mPlotSize;
   Integer              relativeZOrder;
   bool                 isMaximized;
   bool                 isMinimized;


   /// The current run state, so actions based on state can be taken
   Gmat::RunState       runstate;
   Gmat::RunState       prevRunState;
   Integer              currProviderId;
   Real                 propDirection;
   
   /// The list of names of Wrapper objects
   StringArray          xWrapperObjectNames;
   StringArray          yWrapperObjectNames;
   StringArray          allWrapperObjectNames;
   /// ElementWrapper array for the X parameter in XYPlot
   WrapperArray         xParamWrappers;
   /// ElemenyWrapper array for the Y parameter in XYPlot and parameters in ReportFile
   WrapperArray         yParamWrappers;
   /// Old wrappers to delete after setting new wrappers
   WrapperArray         oldWrappersToDelete;
   
   // For ElementWrapper
   bool                 SetActualElementWrapper(const StringArray &wrapperNames,
                                                WrapperArray &paramWrappers,
                                                ElementWrapper* toWrapper,
                                                const std::string &name);
   bool                 CloneWrappers(WrapperArray &toWrappers,
                                      const WrapperArray &fromWrappers);
   bool                 SetWrapperReference(GmatBase *obj, const std::string &name);
   bool                 SetActualWrapperReference(const WrapperArray &wrappers,
                                                  GmatBase *obj, const std::string &name);
   void                 WriteWrappers();
   Integer              FindIndexOfElement(StringArray &labelArray,
                                           const std::string &label);
   // For checking if data should be skipped
   bool                 ShouldDataBeSkipped(Integer whichWrapper);
   
   // Methods that derived classes can override
   virtual bool         Distribute(Integer len);
   virtual bool         Distribute(const Real *dat, Integer len);
   virtual void         HandleManeuvering(GmatBase *originator,
                                          bool maneuvering, Real epoch,
                                          const StringArray &satNames,
                                          const std::string &desc);
   virtual void         HandleSpacecraftPropertyChange(GmatBase *originator,
                                                       Real epoch,
                                                       const std::string &satName,
                                                       const std::string &desc);
   
   /// Parses string value such as "[0 127 255]" and converts to unsigned int array
   /// for color, position, size, etc.
   virtual void         PutUnsignedIntValue(Integer id, const std::string &sval);

public:
   enum
   {
      SOLVER_ITERATIONS = GmatBaseParamCount,
      TARGET_STATUS,
      UPPER_LEFT,
      SIZE,
      RELATIVE_Z_ORDER,
      MINIMIZED,
      MAXIMIZED,
      SubscriberParamCount,
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[SubscriberParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[SubscriberParamCount - GmatBaseParamCount];
   
   // for GUI population
   static StringArray solverIterOptions;
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;
   static Integer GetSolverIterOptionCount() { return SolverIterOptionCount; }
   static const std::string* GetSolverIterOptionList();
      
private:
   
   static const std::string SOLVER_ITER_OPTION_TEXT[SolverIterOptionCount];
   bool    wrappersCopied;
};
#endif // Subscribe_hpp

