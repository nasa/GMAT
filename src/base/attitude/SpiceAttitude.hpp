//$Id:$
//------------------------------------------------------------------------------
//                                 SpiceAttitude
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under
// FDSS Task order 28.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2010.04.16
//
/**
 * Class definition for the SpiceAttitude attitude class.
 * This class calls the spice utility(ies) to get the attitude (pointing) data
 * for an object (currently, for spacecraft only).
 *
 * @note The time passed in for the epoch or passed into the methods
 *       should be an A1Mjd (though its type is currently Real).
 * @note Methods called to set/get parameter values will require/return angle
 *       values in degrees and rate values in degrees/sec.  All other methods
 *       assume/expect radians and radians/sec.
 */
//------------------------------------------------------------------------------

#ifndef SpiceAttitude_hpp
#define SpiceAttitude_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Attitude.hpp"
#include "Rmatrix33.hpp"
#ifdef __USE_SPICE__
   #include "SpiceAttitudeKernelReader.hpp"
#endif


class GMAT_API SpiceAttitude : public Attitude
{
public:
   /// Constructor
   SpiceAttitude(const std::string &attName = "");
   /// copy constructor
   SpiceAttitude(const SpiceAttitude& att);
   /// operator =
   SpiceAttitude& operator=(const SpiceAttitude& att);
   /// destructor
   virtual ~SpiceAttitude();

   /// Initialize the SpiceAttitude attitude
   virtual bool Initialize();

   /// inherited from GmatBase
   virtual GmatBase* Clone() const;

   /// Set the object's identifying information
   virtual void SetObjectID(const std::string &objName, Integer objNaifId, Integer objRefFrameNaifId);

   // override these from Attitude class to ensure the value is obtained
   // regardless of the delta time since the last attitude update

   /// get the attitude as a Quaternion
   virtual const Rvector&     GetQuaternion(Real atTime);
   /// get the attitude as a set of Euler angles, using the Euler
   /// sequence provided by the user (an exception will be thrown if the
   /// sequence has not been set
   virtual const Rvector3&    GetEulerAngles(Real atTime);
   /// get the attitude as a set of Euler angles, using the input Euler sequence
   virtual const Rvector3&    GetEulerAngles(Real atTime,  Integer seq1,
                                     Integer seq2, Integer seq3);
   /// get the attitude as a Direction Cosine Matrix
   virtual const Rmatrix33&   GetCosineMatrix(Real atTime);

   virtual const Rvector3&    GetAngularVelocity(Real atTime);
   virtual const Rvector3&    GetEulerAngleRates(Real atTime);

   // inherited from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         SetStringParameter(const std::string label,
                                           const std::string &value,
                                           const Integer index);
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const;

protected:
   enum
   {
       ATTITUDE_KERNEL_NAME = AttitudeParamCount,
       SC_CLOCK_KERNEL_NAME,
       FRAME_KERNEL_NAME,
       SpiceAttitudeParamCount
   };

   #ifdef __USE_SPICE__
      SpiceAttitudeKernelReader *reader;
   #endif

   /// name of the object whose attitude is to be retrieved from the SPICE kernel(s)
   std::string scName;
   /// NAIF Id for the object
   Integer     naifId;
   /// NAIF Id for the object's reference frame
   Integer     refFrameNaifId;
   // array of CK kernel names
   StringArray ck;
   // array of SCLK kernel names
   StringArray sclk;
   // array of Frame kernel names
   StringArray fk;

   // array of full path kernel names
   StringArray ckFullPath;
   StringArray sclkFullPath;
   StringArray fkFullPath;

   // static text and types for parameters
   static const std::string PARAMETER_TEXT[SpiceAttitudeParamCount - AttitudeParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[SpiceAttitudeParamCount - AttitudeParamCount];

   static const Integer UNDEFINED_NAIF_ID;
   static const Integer UNDEFINED_NAIF_ID_REF_FRAME;

   virtual void ComputeCosineMatrixAndAngularVelocity(Real atTime);
   virtual void ComputeCosineMatrixAndAngularVelocity(GmatTime &atTime);

private:
   // Default constructor - not implemented
   //SpiceAttitude(); // MSVC compiler gives warning: multiple default constructors specified

};
#endif /*SpiceAttitude_hpp*/
