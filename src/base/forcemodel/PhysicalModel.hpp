//$Id$
//------------------------------------------------------------------------------
//                              PhysicalModel
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
// *** File Name : PhysicalModel.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  This software is subject to the Software Usage Agreement described***
// ***  by NASA Case Number GSC-14735-1.  The Software Usage Agreement    ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 2/5/2003 - D. Conway, Thinking Systems, Inc.
//                             Incorporated the Derivative class into this
//                             class and removed Derivative from the class
//                             hierarchy
//
//                           : 3/3/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated parameter strings to include units;
//                             Added code to switch between relative and 
//                             absolute error
//
//                           : 09/24/2003 - W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/15/2003 - W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - virtual char* GetParameterName(
//                                                   const int parm) const
//                                  to
//                                  virtual std::string GetParameterName(
//                                                   const int parm) const
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - ParameterCount()
//                                - GetParameter()
//                                - SetParameter()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//
//                           : 10/20/2003 - W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - Fixed format.
//                              Removals:
//                                - GetParameterName()
//
//                           : 10/23/2003 - D. Conway, Thinking Systems, Inc. &
//                                          W. Waktola, Missions Applications
//                                          Branch
//                              Changes:
//                                - Changed constructor from
//                                    PhysicalModel::PhysicalModel(void)
//                                  to
//                                    PhysicalModel(UnsignedInt typeId,
//                                          const std::string &typeStr,
//                                          const std::string &nomme = "")
//                                - Added parameterCount = 1 in constructors
//
// **************************************************************************
/** 
 * Base class used to model the physical system
 */

#ifndef PhysicalModel_hpp
#define PhysicalModel_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Spacecraft.hpp"
#include "GmatState.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

#include "ODEModelException.hpp"

#include <math.h>



class PropagationStateManager;


/** 
 * Base class used to model the physical system
 * 
 * This class is a base class used to model the physics of the system being
 * studied.  The propagators work in tandem with classes derived from this one 
 * to advance the system over time.  
 *
 * Propagators fall into two basic subclasses: Integrators and analytic 
 * solutions.  The analytic solutions typically require minimal interaction with
 * the system; for example, for two body orbit propagation, the PhysicalModel
 * supplies the gravitational constant for the central body.  Integrators
 * require more detailed information to evolve their models; see the text of the
 * PhysicalModelIntegrator class description for details of their requirements.
 */
class GMAT_API PhysicalModel : public GmatBase
{
public:
   PhysicalModel(UnsignedInt typeId, const std::string &typeStr,
                 const std::string &nomme = "");
   virtual ~PhysicalModel(void);

   PhysicalModel(const PhysicalModel&);
   PhysicalModel& operator=(const PhysicalModel&);
    
   virtual bool Initialize();

   virtual CelestialBody* GetBody();
   virtual std::string    GetBodyName();
   virtual Integer        GetDimension();
   virtual Real *         GetState();
   virtual Real *         GetStateDot(); 

   virtual Real*          GetJ2KState();
   const Real *           GetDerivativeArray();

   const Real*            GetJ2KDerivativeArray();

   virtual bool SetBody(const std::string& theBody);
   virtual void SetBodyName(const std::string& theBody);
   virtual void SetBody(CelestialBody* toBody);
   virtual void SetForceOrigin(CelestialBody* toBody);
   virtual void SetDimension(Integer);

   // Change from priblic to private
   //virtual void SetState(const Real * st);
   
   virtual void SetState(GmatState * st);

   virtual CelestialBody* GetForceOrigin();

   Real GetErrorThreshold() const;
   bool SetErrorThreshold(const Real thold = 0.10);

   virtual void IncrementTime(Real dt);
   virtual Real GetTime();
   virtual void SetTime(Real t);
   virtual void SetDirection(Real dir);
   virtual Real GetStepPrecision(Real stepSize);

   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1, 
         const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);
   virtual Real EstimateError(Real * diffs, Real * answer) const;
   virtual bool GetComponentMap(Integer * map, Integer order = 1, 
         Integer id = -1) const;
   virtual void SetStmIndex(Integer id, Integer paramID);

   // Support for extra derivative calcs -- the STM contribution, for example
   virtual const IntegerArray& GetSupportedDerivativeIds();
   virtual const StringArray&  GetSupportedDerivativeNames();

   
   virtual void SetSolarSystem(SolarSystem *ss);
   virtual void SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm,
                                      const Integer parmID = -1);
   virtual void SetSatelliteParameter(const Integer i,
                                      Integer parmID,
                                      const Real parm);

   virtual void SetSatelliteParameter(const Integer i,
                                      const std::string parmName, 
                                      const std::string parm);
   virtual void SetSpaceObject(const Integer i, GmatBase *obj);

   virtual bool UsesSpacecraftOrigin();
   virtual void ClearSatelliteParameters(const std::string parmName = "");
   virtual bool StateChanged(bool reset = true);
   
   virtual bool IsTransient();
   virtual bool DepletesMass();
   virtual bool AttitudeAffectsDynamics();
   virtual bool IsUserForce();
   virtual bool IsUnique(const std::string &forBody = "");
   virtual bool SetAllowODEDelete(bool deleteFlag);
   virtual bool AllowODEDelete() const;
   virtual void SetPropList(ObjectArray *soList);
   virtual bool CheckQualifier(const std::string &qualifier,
         const std::string &forType = "");
   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity, Integer totalSize);
   ////virtual bool SetStmRowId(Integer rowNumber, Integer rowId); 
   

   // Parameter accessor methods -- inherited from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool IsParameterReadOnly(const Integer id) const;
   virtual bool IsParameterReadOnly(const std::string &label) const;

   virtual GmatTime     GetGmatTimeParameter(const Integer id) const;
   virtual GmatTime     SetGmatTimeParameter(const Integer id, 
                                             const GmatTime value);
   virtual GmatTime     GetGmatTimeParameter(const std::string &label) const;
   virtual GmatTime     SetGmatTimeParameter(const std::string &label, 
                                             const GmatTime value);

   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const Integer id,
                                         const Integer index) const;
   virtual Real         GetRealParameter(const Integer id, const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value,
                                         const Integer index);
   virtual Real         SetRealParameter(const Integer id, const Real value,
                                         const Integer row, const Integer col);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer index) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value,
                                         const Integer index);
   virtual Real         GetRealParameter(const std::string &label,
                                         const Integer row,
                                         const Integer col) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value, const Integer row,
                                         const Integer col);

   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value); 


   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string& value,
                                          const Integer index);
   virtual std::string GetStringParameter(const std::string& label,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const std::string& label,
                                          const std::string& value,
                                          const Integer index);

   virtual const StringArray& GetStringArrayParameter(const Integer id) const;
   virtual const StringArray& GetStringArrayParameter(const std::string& label) const;


   virtual GmatBase*   GetRefObject(const UnsignedInt type,
                                    const std::string &name);
   const StringArray&  GetRefObjectNameArray(const UnsignedInt type);
   virtual bool        SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name = "");
   virtual bool        SetRefObject(GmatBase *obj, const UnsignedInt type,
                              const std::string &name, const Integer index);
   virtual GmatBase*   GetRefObject(const UnsignedInt type,
                              const std::string &name, const Integer index);

   virtual void        SetPropStateManager(PropagationStateManager *sm);

   // Methods used for PM based Parameters
   virtual bool        BuildModelState(GmatEpoch now, Real *state,
                                       Real *j2kState, Integer dimension = 6);
   virtual bool        BuildModelStateGT(GmatTime now, Real *state,
                                       Real *j2kState, Integer dimension = 6);

   // Methods for getting stop epochs for force models
   virtual Real                     GetForceMaxStep(bool forward = true);
   virtual Real                     GetForceMaxStep(Real theEpoch, bool forward = true);
   virtual Real                     GetForceMaxStep(const GmatTime& theEpochGT, bool forward = true);

   virtual bool                     IsEndOfSegment(bool forward);
   virtual bool                     IsEndOfThrust(bool forward);

   // Used for mass Jacobian computation
   /// Test to see if the force is conservative
   bool                IsConservative();
   /// Toggle mass Jacobian computation
   void                ComputeMassJacobian(bool truefalse);
   /// Toggle time Jacobian computation
   void                ComputeTimeJacobian(bool truefalse);
   /// Test to see if the mass Jacobian is implemented
   bool                HasMassJacobian();
   /// Test to see if the time Jacobian is implemented
   bool                HasTimeJacobian();
   /// Retrieve the mass Jacobian
   const Real          *GetMassJacobian();
   /// Retrieve the time Jacobian
   const Real          *GetTimeJacobian();
   const ObjectArray&          GetSpaceObjectList();

   virtual bool IsSolvingForThrustEventsWithSpinningAttitude();

protected:
      
   /// pointer to the body for which this force is computed
   CelestialBody *body;
   /// pointer to the origin used in propagation
   CelestialBody *forceOrigin;
   /// name of the body
   std::string bodyName;
   /// Number of parameters being modeled
   Integer dimension;
   /// Flag that is set when SetState() or SetTime() is called
   bool stateChanged;
   /// Flag that indicates if this force is to be deleted from memory when removed from an ODEModel
   bool allowODEDelete;
   
   /// Prop State Manager
   PropagationStateManager *psm;
   /// GMAT state that the physical model uses
   GmatState *theState;
   /// Array of data parameters containing the model data
   Real *modelState;
   /// Array of data parameters containing derivative w.r.t. time of the model data 
   Real *modelStateDot;
   
   /// The state vector in J2000BodyMJ2000Eq coordinates.
   Real *rawState;
   /// Derivative w.r.t. time of the state vector in J2000BodyMJ2000Eq coordinates. 
   Real *rawStateDot;

   /// The base epoch
   Real epoch;
   GmatTime epochGT;

   /// Number of seconds elapsed from the base epoch
   Real elapsedTime;
   /// Number of seconds previously elapsed from the base epoch
   Real prevElapsedTime;
   /// The direction of the step
   Real direction;

   /// Array containing the most recent derivative calculation, when needed (in force model coordinate system)
   Real * deriv;
   /// Array containing the most recent derivative calculation, when needed (in j2kbody MJ2000 coordinate system)
   Real * rawDeriv;

   /// Mass Jacobian vector (a single column for the Jacobian data)
   Real * massJacobian;
   /// Time Jacobian vector
   Real * timeJacobian;

   /// IDs for each element of the derivative vector 
   IntegerArray derivativeIds;
   /// Text names for each element of the derivative vector 
   StringArray derivativeNames;
   /// Threshold for switching between relative and absolute error control
   Real relativeErrorThreshold;
   /// Pointer to the solar system model used as a data provider for the forces
   SolarSystem *solarSystem;
   
   /// Flag indicating that the Cartesian state derivatives should be filled
   bool                      fillCartesian;
   /// Starting index for Cartesian state data (defaults to 0)
   Integer                   cartesianStart;
   /// Number of C. state derivatives that are filled (Usually matches satCount)
   Integer                   cartesianCount;

   /// Flag indicating that the orbital State Transition Matrix derivatives should be filled
   bool                      fillSTM;
   /// Starting index for STM data
   Integer                   stmStart;
   /// Number of STM matrices that need to be filled
	Integer                   stmCount;
   /////// Number of rows/columns in the STM 
   Integer                   stmRowCount; 
	/// Number of rows/columns in the STM 
	Integer                   totalSTMSize;
	
   /////////// Mapping for the STM entries 
   ////////IntegerArray              stmRowId; 

   /// Flag indicating that the orbital A-matrix should be filled
   bool                      fillAMatrix;
   /// Starting index for A-matrix data
   Integer                   aMatrixStart;
   /// Number of A-matrices that need to be filled
   Integer                   aMatrixCount;

   // Separate mass derivative modeling
   /// Flag indicating if the force is conservative
   bool                      isConservative;
   /// Flag indicating if the force is conservative
   bool                      hasMassJacobian;
   /// Trigger to build the mass Jacobian data
   bool                      fillMassJacobian;

   // Separate time derivatve modeling
   /// Flag indicating if the force model has an analytic time Jacobian
   bool                      hasTimeJacobian;
   /// Trigger to build the time Jacobian data
   bool                      fillTimeJacobian;

      /// Time converter singleton
   TimeSystemConverter *theTimeConverter;


	/// Pointers to the spacecraft 
	std::vector<GmatBase*> scObjs;

	//// Methods used for PM based Parameters
   //virtual bool              BuildModelState(GmatEpoch now, Real *state,
   //                                Real *j2kState, Integer dimension = 6);

   /// Parameter IDs
   enum
   {
      EPOCH = GmatBaseParamCount, 
      ELAPSED_SECS,
      BODY_NAME,
      DERIVATIVE_ID,
      PhysicalModelParamCount
   };

   static const std::string 
      PARAMETER_TEXT[PhysicalModelParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType 
      PARAMETER_TYPE[PhysicalModelParamCount - GmatBaseParamCount];


   enum ShapeModel 
   {
      SPHERICAL_MODEL,
      SPAD_FILE_MODEL,
      NPLATE_MODEL 
   };

private:
   // Change fro prublic to private
   void SetState(const Real * st); 
   void SetStateDot(const Real * stDot); 

};

#endif // PhysicalModel_hpp

