//------------------------------------------------------------------------------
//                         AttitudeConversionUtility
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, GSFC/GSSB
// Created: 2013.11.25
//
/**
 * Definition of the static class containing methods to convert between
 * attitude state representations.
 * This is a static class: No instances of this class may be declared.
 */
//------------------------------------------------------------------------------

#ifndef AttitudeConversionUtil_hpp
#define AttitudeConversionUtil_hpp

#include "utildefs.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"


class GMATUTIL_API AttitudeConversionUtility
{
public:

   static Rmatrix33 ToCosineMatrix(const Rvector &quat1);
   static Rmatrix33 ToCosineMatrix(const Rvector3 &eulerAngles,
                                   Integer seq1, Integer seq2,
                                   Integer seq3);
   static Rmatrix33 ToCosineMatrix(const Real *eulerAngles,
                                   Integer seq1, Integer seq2,
                                   Integer seq3);
   static Rvector3  ToEulerAngles(const Rvector &quat1, Integer seq1,
                                  Integer seq2,         Integer seq3);
   static Rvector3  ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                  Integer seq2,            Integer seq3);
   static Rvector   ToQuaternion(const Rvector3 &eulerAngles,
                                 Integer seq1, Integer seq2, Integer seq3);
   static Rvector   ToQuaternion(const Rmatrix33 &cosMat);
   static Rvector   ToQuaternion(const Rvector3 &MRPs);
   static Rvector3  ToMRPs(const Rvector &quat1);

   static Rvector3  ToEulerAngleRates(const Rvector3 &angularVel,
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2,
                                      Integer seq3);
   static Rvector3  ToAngularVelocity(const Rvector3 &eulerRates,
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2,
                                      Integer seq3);

   // method to convert an euler axis and angle to a cosine matrix
   static Rmatrix33 EulerAxisAndAngleToDCM(
                        const Rvector3 &eAxis, Real eAngle);
   // method to convert a cosine matrix to an euler axis and angle
   static void      DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat,
                                            Rvector3 &eAxis, Real &eAngle);

   static bool      IsValidEulerSequence(const std::string &theSeq);

private:
   //------------------------------------------------------------------------------
   // private static data
   //------------------------------------------------------------------------------
   static const std::string VALID_EULER_SEQUENCES[12];

   //------------------------------------------------------------------------------
   // private constructors, destructor, operator=
   //------------------------------------------------------------------------------
   AttitudeConversionUtility();
   AttitudeConversionUtility(const AttitudeConversionUtility &copy);
   ~AttitudeConversionUtility();
   AttitudeConversionUtility& operator=(const AttitudeConversionUtility &copy);

};
#endif // AttitudeConversionUtil_hpp
