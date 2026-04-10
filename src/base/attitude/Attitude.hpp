//$Id$
//------------------------------------------------------------------------------
//                               Attitude
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
// Author: Wendy C. Shoan/GSFC
// Created: 2006.02.16
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Attitude base class.
 * This class is the base class for classes representing spacecraft attitude
 * for GMAT.
 *
 * @note The current builds of GMAT only contain kinematic attitude modeling.
 * 
 * @note The time passed in for the epoch or passed into the methods 
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods 
 *       assume/expect radians and radians/sec.
 * @note Due to the fact that Attitude objects are not YET configured objects,
 *       and because we want Attitude model changes to work properly in
 *       Command Mode, all data used by any child class of this base Attitude
 *       class must be declared and handled here.
 */
//------------------------------------------------------------------------------

#ifndef Attitude_hpp
#define Attitude_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"

//------------------------------------------------------------------------------
// namespace to define attitude state types
//------------------------------------------------------------------------------
 namespace GmatAttitude
 {
   enum AttitudeStateType
   {
      QUATERNION_TYPE = 0,
      DIRECTION_COSINE_MATRIX_TYPE,
      EULER_ANGLES_AND_SEQUENCE_TYPE,
      MODIFIED_RODRIGUES_PARAMETERS_TYPE
   };
   
   enum AttitudeRateStateType
   {
      ANGULAR_VELOCITY_TYPE = 0,
      EULER_ANGLE_RATES_TYPE
   };
 };
 
 //------------------------------------------------------------------------------
 // class Attitude definition
 //------------------------------------------------------------------------------
class GMAT_API Attitude : public GmatBase 
{
public:

   // --------------------------------------------------------------------------
   // BEGIN static methods
   // --------------------------------------------------------------------------
                                      
   static StringArray       GetEulerSequenceStrings();
   static UnsignedIntArray  ExtractEulerSequence(const std::string &seqStr);

   static StringArray       GetAttitudeConstraintTypes();

   // --------------------------------------------------------------------------
   // END static methods
   // --------------------------------------------------------------------------

   // constructor
   Attitude(const std::string &typeStr, const std::string &itsName = "");
   // copy constructor
   Attitude(const Attitude& att);
   // operator =
   Attitude& operator=(const Attitude& att);
   // destructor
   virtual ~Attitude();

   /// Method to Validate the input at the end of the script parsing process
   virtual bool       Validate();
   /// Method to initialize the Attitude
   virtual bool       Initialize();

   /// Methods to get/set the epoch and the reference coordinate system name
//private:
   Real               GetEpoch() const;
   void               SetEpoch(Real toEpoch); // A1Mjd time
//public:
   GmatTime           GetEpochGT() const;
   void               SetEpochGT(GmatTime toEpoch); // A1Mjd time

   void               NeedsReinitialization();

   virtual void       SetOwningSpacecraft(GmatBase *theSC);

//   virtual bool       SetReferenceCoordinateSystemName(
//                         const std::string &refName);

   /// Method to get the Euler sequence list (as strings)
   const StringArray&         GetEulerSequenceList() const;
   /// Method to get the attitude as a Quaternion
   virtual const Rvector&     GetQuaternion(Real atTime);
   /// Method to get the attitude as a set of Euler angles, using the Euler
   /// sequence provided by the user (an exception will be thrown if the
   /// sequence has not been set
   virtual const Rvector3&    GetEulerAngles(Real atTime);
   /// Method to get the attitude as a set of Euler angles,
   /// using the input Euler sequence
   virtual const Rvector3&    GetEulerAngles(Real atTime,  Integer seq1, 
                                     Integer seq2, Integer seq3);
   /// Method to get the attitude as a Direction Cosine Matrix
   virtual const Rmatrix33&   GetCosineMatrix(Real atTime);
   
   virtual const Rvector3&    GetAngularVelocity(Real atTime);
   virtual const Rvector3&    GetEulerAngleRates(Real atTime);
   
   /// Method returning the type of the attitude model
   std::string         GetAttitudeModelName() const;
   /// Method returning a flag indicating whether or not modification of
   /// the reference coordinate system is allowed for this attitude model
   bool                CSModifyAllowed() const;
   /// Method returning a flag indicating whether or not setting of
   /// the initial attitude conditions is allowed for this attitude model
   bool                SetInitialAttitudeAllowed() const;
   /// Does this model compute attitude rates?
   bool                ModelComputesRates() const;
   
   /// Has this attitude been initialized?
   bool                IsInitialized()
                       { return isInitialized; };

   /// methods to access object parameters
   virtual std::string GetRefObjectName(const UnsignedInt type) const;
   virtual const StringArray&
                       GetRefObjectNameArray(const UnsignedInt type);
   virtual bool        SetRefObjectName(const UnsignedInt type,
                                        const std::string &name);
   virtual bool        RenameRefObject(const UnsignedInt type,
                                       const std::string &oldName,
                                       const std::string &newName);
   virtual GmatBase*   GetRefObject(const UnsignedInt type,
                                    const std::string &name);
   virtual bool        SetRefObject(GmatBase *obj, const UnsignedInt type,
                                    const std::string &name = "");
   virtual bool        HasRefObjectTypeArray();
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
                                    
   /// methods to get/set parameter values
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual UnsignedInt
                        GetPropertyObjectType(const Integer id) const;
   virtual bool         CanAssignStringToObjectProperty(const Integer id) const;

   virtual bool        IsParameterReadOnly(const Integer id) const;
   virtual bool        IsParameterReadOnly(const std::string &label) const;

   virtual GmatTime     GetGmatTimeParameter(const Integer id) const;
   virtual GmatTime     SetGmatTimeParameter(const Integer id, 
                                             const GmatTime value);
   virtual GmatTime     GetGmatTimeParameter(const std::string &label) const;
   virtual GmatTime     SetGmatTimeParameter(const std::string &label, 
                                             const GmatTime value);
   
   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        GetRealParameter(const std::string &label) const;
   
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value,
                                        const Integer index);
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const Integer id) const; 
   virtual const UnsignedIntArray& 
                       GetUnsignedIntArrayParameter(
                       const std::string &label) const;
                                               
   virtual const Rvector&    GetRvectorParameter(const Integer id) const;
   virtual const Rvector&    GetRvectorParameter(const std::string &label) const;
   virtual const Rvector&    SetRvectorParameter(const Integer id,
                                                 const Rvector &value);
   virtual const Rvector&    SetRvectorParameter(const std::string &label,
                                                 const Rvector &value);
                                    
   virtual const Rmatrix&    GetRmatrixParameter(const Integer id) const;
   virtual const Rmatrix&    GetRmatrixParameter(const std::string &label) const;
   virtual const Rmatrix&    SetRmatrixParameter(const Integer id,
                                                 const Rmatrix &value);
   virtual const Rmatrix&    SetRmatrixParameter(const std::string &label,
                                                 const Rmatrix &value);
                                                 
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        SetStringParameter(const std::string &label, 
                                          const std::string &value);
   virtual std::string GetStringParameter(const Integer id,
                                          const Integer index) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value,
                                          const Integer index);
   virtual const StringArray& 
                       GetStringArrayParameter(const Integer id) const; 
   virtual const StringArray& 
                       GetStringArrayParameter(const std::string &label) const;
   virtual const std::string&  
                       GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                           const std::string &prefix = "",
                                           const std::string &useName = "");

	virtual Rmatrix33   GetRotationMatrix(const GmatTime &epochGT);
	virtual std::vector<Rmatrix33>
                       GetRotationMatrixDerivative(GmatTime &epochGT, CoordinateSystem *j2kCS);

   DEFAULT_TO_NO_CLONES

protected:
   enum
   {
       ATTITUDE_DISPLAY_STATE_TYPE = GmatBaseParamCount,   
       ATTITUDE_RATE_DISPLAY_STATE_TYPE,   
       REFERENCE_COORDINATE_SYSTEM,
       EPOCH,                      // A1Mjd (Real)
       Q_1,
       Q_2,
       Q_3,
       Q_4,
       EULER_ANGLE_SEQUENCE,
       EULER_ANGLE_1,
       EULER_ANGLE_2,
       EULER_ANGLE_3,
       DCM_11,
       DCM_12,
       DCM_13,
       DCM_21,
       DCM_22,
       DCM_23,
       DCM_31,
       DCM_32,
       DCM_33,
       MRP_1,    // Dunn Added
       MRP_2,
       MRP_3,
       EULER_ANGLE_RATE_1,
       EULER_ANGLE_RATE_2,
       EULER_ANGLE_RATE_3,
       ANGULAR_VELOCITY_X,
       ANGULAR_VELOCITY_Y,
       ANGULAR_VELOCITY_Z, 
       // Add additional CSFixed fields here
       // none at this time
       // Add additional Spinner fields here
       // none at this time
       // Add additional SpiceAttitude fields here
       // none at this time
       // Add additional PrecessingSpinner fields here
       NUTATION_REFERENCE_VECTOR_X,
       NUTATION_REFERENCE_VECTOR_Y,
       NUTATION_REFERENCE_VECTOR_Z,
       BODY_SPIN_AXIS_X,
       BODY_SPIN_AXIS_Y,
       BODY_SPIN_AXIS_Z,
       INITIAL_PRECESSION_ANGLE,
       PRECESSION_RATE,
       NUTATION_ANGLE,
       INITIAL_SPIN_ANGLE,
       SPIN_RATE,
       // Add additional NadirPointing fields here
       ATTITUDE_REFERENCE_BODY,
       ATTITUDE_CONSTRAINT_TYPE,
       BODY_ALIGNMENT_VECTOR_X,
       BODY_ALIGNMENT_VECTOR_Y,
       BODY_ALIGNMENT_VECTOR_Z,
       BODY_CONSTRAINT_VECTOR_X,
       BODY_CONSTRAINT_VECTOR_Y,
       BODY_CONSTRAINT_VECTOR_Z,
       // Add additional CCSDS-AEM fields here
       AEM_FILE_NAME,
       AttitudeParamCount
   };
   
   enum OtherReps
   {
       EULER_SEQUENCE_LIST = 7000,
       EULER_SEQUENCE,
       EULER_ANGLES,               // degrees
       EULER_ANGLE_RATES,          // degrees/second
       QUATERNION,
       MRPS,					   // Dunn Added
       DIRECTION_COSINE_MATRIX,
       ANGULAR_VELOCITY,           // degrees/second
       BODY_SPIN_AXIS,             // PrecessingSpinner
       NUTATION_REFERENCE_VECTOR,  // PrecessingSpinner
       EndOtherReps
   };
   
   static const std::string
                        PARAMETER_TEXT[AttitudeParamCount - GmatBaseParamCount];
   
   static const Gmat::ParameterType
                        PARAMETER_TYPE[AttitudeParamCount - GmatBaseParamCount];
 
   static const std::string
                        OTHER_REP_TEXT[EndOtherReps - 7000]; // OTHER_REPS_OFFSET
   
   static const Gmat::ParameterType
                        OTHER_REP_TYPE[EndOtherReps - 7000]; // OTHER_REPS_OFFSET
 
   static const std::string EULER_SEQ_LIST[12];
   static const Real        TESTACCURACY;
   static const Real        ATTITUDE_TIME_TOLERANCE;
   static const Integer     OTHER_REPS_OFFSET;

   static const std::string ATTITUDE_CONSTRAINT_TYPE_LIST[2];

   
   GmatAttitude::AttitudeStateType     inputAttitudeType;
   GmatAttitude::AttitudeRateStateType inputAttitudeRateType;
                         
   std::string           attitudeDisplayType; 
   std::string           attitudeRateDisplayType;
                         
   bool                  isInitialized;
   bool                  needsReinit;
   /// the list of possible euler sequences
   StringArray           eulerSequenceList;
   
   /// initial user-supplied epoch as an A1Mjd time (as Real)
   Real                  epoch;
   GmatTime              epochGT;

   // pointer to the owning spacecraft
   GmatBase              *owningSC;
    
   /// the reference coordinate system name
   std::string           refCSName;
   /// pointer to the reference coordinate system
   CoordinateSystem      *refCS;
   std::string           eulerSequence;
   /// initial user-supplied euler sequence
   UnsignedIntArray      eulerSequenceArray;
   
   /// the input rotation matrix (from Fi to Fb) computed, on
   /// initialization (from quaternion or euler angles/sequence 
   /// if necessary)
   Rmatrix33             RBi;  // Identity Matrix by default
   /// the input angular velocity computed, on initialization 
   /// (from euler angle rates if necessary)
   Rvector3              wIBi;
   
   /// the current rotation matrix (from inertial to body)
   Rmatrix33             dcm;
   /// the current angular velocity, with respect to the inertial frame
   /// (radians/second)
   Rvector3              angVel;
   /// last time that the CosineMatrix and angular velocity
   /// were computed                     
   Real                  attitudeTime;
   GmatTime              attitudeTimeGT;

   /// the last computed quaternion
   Rvector               quaternion;
   /// the last computed MRPs - Dunn Added
   Rvector3              mrps;
   /// the last computed euler angles (radians)
   Rvector3              eulerAngles;
   /// the last computed euler angle rates  (radians/second)
   Rvector3              eulerAngleRates;
   /// the name of the attitude model
   std::string           attitudeModelName;
   /// flag indicating whether or not the user can modify the reference
   /// coordinate system for this attitude model
   bool                  modifyCoordSysAllowed;
   /// flag indicating whether or not the user can modify/set the initial
   /// attitude for this attitude model
   bool                  setInitialAttitudeAllowed;
   /// Has the warning about setting the coordinate system having no effect
   /// been written?
   bool                  warnNoCSWritten;
   /// Has the warning about setting the initial attitude having no effect
   /// been written?
   bool                  warnNoAttitudeWritten;
   /// does the current model compute attitude rates?
   bool                  modelComputesRates;
   /// Has warning about attitude rates not being computed been written?
   bool                  warnNoRatesWritten;

   /// Additional data for CSFixed
   /// none at this time
   /// Additional data for Spinner
   /// none at this time
   /// Additional data for SpiceAttitude
   /// none at this time

   /// Additional data for PrecessingSpinner
   Rvector3              nutationReferenceVector;
   Rvector3              bodySpinAxis;
   Real                  initialPrecessionAngle;
   Real                  precessionRate;
   Real                  nutationAngle;
   Real                  initialSpinAngle;
   Real                  spinRate;

   /// Additional data for NadirPointing
   std::string           refBodyName;
   CelestialBody         *refBody;
   std::string           attitudeConstraintType;
   Rvector3              bodyAlignmentVector;
   Rvector3              bodyConstraintVector;

   /// Additional data for CCSDS-AEM
   std::string           aemFile;
   std::string           aemFileFullPath;
   
   //------------------------------------------------------------------------------
   //  virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime)
   //------------------------------------------------------------------------------
   /**
    * This method computes the current CosineMatrix at the input time atTime.
    *
    * @param atTime the A1Mjd time at which to compute the attitude.
    *
    * @note This method will update the currentRBI and currentwIBB members
    *       in the class.
    * @note This method is pure virtual and will need to be implemented in
    *       the 'leaf' classes.
    */
   //------------------------------------------------------------------------------
   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime) = 0;
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime) = 0;

private:
   // default constructor - not implemented
   Attitude();

   bool        ValidateCosineMatrix(const Rmatrix33 &mat);
   bool        ValidateEulerAngles(const Rvector &eAngles, const UnsignedIntArray &eulSeq);
   bool        ValidateEulerSequence(const std::string &seq);
   bool        ValidateEulerSequence(const UnsignedIntArray &eulAng);
   bool        ValidateQuaternion(const Rvector &quat);
   bool        ValidateMRPs(const Rvector &mrps);		// Dunn Added
   void        UpdateState(const std::string &rep);
   void        SetRealArrayFromString(Integer id, const std::string &sval);
   Real        GetPrecessingSpinnerRealParameter(const Integer id) const;
   Real        SetPrecessingSpinnerRealParameter(const Integer id, const Real value);
   Real        GetNadirPointingRealParameter(const Integer id) const;
   Real        SetNadirPointingRealParameter(const Integer id, const Real value);

   bool        IsInitialAttitudeParameter(Integer id, std::string ofType = "Any") const;
};
#endif /*Attitude_hpp*/
