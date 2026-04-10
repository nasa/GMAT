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

#include <iostream>
#include <sstream>
#include <iomanip>
#include "AttitudeConversionUtility.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "UtilityException.hpp"

//#define DEBUG_TO_DCM

//------------------------------------------------------------------------------
//  static data
//------------------------------------------------------------------------------
const std::string AttitudeConversionUtility::VALID_EULER_SEQUENCES[12] =
{
   "123",
   "231",
   "312",
   "132",
   "321",
   "213",
   "121",
   "232",
   "313",
   "131",
   "323",
   "212"
};


//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Rvector &quat1)  [static]
//------------------------------------------------------------------------------
 /**
 * Converts the input quaternion to a direction cosine matrix.
 *
 * @param quat1  the input quaternion.
 *
 * @return the cosine direction matrix representation of the input attitude.
 */
//------------------------------------------------------------------------------
Rmatrix33 AttitudeConversionUtility::ToCosineMatrix(const Rvector &quat1)
{
   #ifdef DEBUG_TO_DCM
   MessageInterface::ShowMessage("ENTERING ToDCM(q) ... %.12f  %.12f  %.12f  %.12f\n",
   quat1[0], quat1[1], quat1[2], quat1[3]);
   #endif
   // check for proper size and magnitude
   if (quat1.GetSize() != 4)
   {
      throw UtilityException(
            "Quaternion error : the quaternion must have 4 elements.\n");
   }
   if (quat1.GetMagnitude() < GmatAttitudeConstants::QUAT_MIN_MAG)
   {
      std::ostringstream errmsg;
      errmsg << "Quaternion error : the quaternion must have a magnitude greater than ";
      errmsg << GmatAttitudeConstants::QUAT_MIN_MAG << std::endl;
      throw UtilityException(errmsg.str());
   }

   Rmatrix33 I3;  // identity matrix, by default
   Rvector3  q1_3(quat1(0), quat1(1), quat1(2));
   Rmatrix33 q_x(     0.0, -q1_3(2),  q1_3(1),
                q1_3(2),      0.0, -q1_3(0),
               -q1_3(1),  q1_3(0),     0.0);
   Real      c = 1.0 / ((quat1(0) * quat1(0)) + (quat1(1) * quat1(1)) +
                        (quat1(2) * quat1(2)) + (quat1(3) * quat1(3)));
   return Rmatrix33(c * ( (((quat1(3) * quat1(3)) - (q1_3 * q1_3)) * I3)
          + (2.0 * Outerproduct(q1_3,q1_3)) - (2.0 * quat1(3) * q_x)));
}

//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Rvector3 &eulerAngles,   [static]
//                             Integer seq1, Integer seq2,
 //                            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angles and sequence to a direction cosine matrix.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the cosine direction matrix representation of the input attitude.
 */
//------------------------------------------------------------------------------
Rmatrix33 AttitudeConversionUtility::ToCosineMatrix(const Rvector3 &eulerAngles,
                                   Integer        seq1,
                                   Integer        seq2,
                                   Integer        seq3)
{
   #ifdef DEBUG_TO_DCM
   MessageInterface::ShowMessage("ENTERING ToDCM(eulerangles) ... %.12f  %.12f  %.12f\n",
   eulerAngles[0], eulerAngles[1], eulerAngles[2]);
   MessageInterface::ShowMessage("              (eulerSeq)    ... %d  %d  %d\n",
   seq1, seq2, seq3);
   #endif
   if ((seq1 == 0) | (seq2 == 0) | (seq3 == 0))
      throw UtilityException(
         "Euler sequence ill-defined for conversion to cosine matrix.");
   Real s1 = GmatMathUtil::Sin(eulerAngles(0));
   Real s2 = GmatMathUtil::Sin(eulerAngles(1));
   Real s3 = GmatMathUtil::Sin(eulerAngles(2));
   Real c1 = GmatMathUtil::Cos(eulerAngles(0));
   Real c2 = GmatMathUtil::Cos(eulerAngles(1));
   Real c3 = GmatMathUtil::Cos(eulerAngles(2));
   bool validSequence = true;
   if (seq1 == 1)
   {
      if (seq2 == 2)
      {
         if (seq3 == 1)      //  1-2-1
            return Rmatrix33(
                      c2,          s2*s1,           -s2*c1,
                   s3*s2, c3*c1-s3*c2*s1,   c3*s1+s3*c2*c1,
                   c3*s2, -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1);
         else if (seq3 == 3)  //  1-2-3
            return Rmatrix33(
                    c3*c2,  c3*s2*s1+s3*c1, -c3*s2*c1+s1*s3,
                   -s3*c2, -s3*s2*s1+c3*c1,  s3*s2*c1+c3*s1,
                       s2,          -c2*s1,           c2*c1);
         else validSequence = false;
      }  // seq2 == 2
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  1-3-1
            return Rmatrix33(
                       c2,            s2*c1,            s2*s1,
                   -c3*s2,   c3*c2*c1-s3*s1,   c3*c2*s1+s3*c1,
                    s3*s2,  -s3*c2*c1-c3*s1,  -s3*c2*s1+c3*c1);
         else if (seq3 == 2) //  1-3-2
            return Rmatrix33(
                   c3*c2, c3*s2*c1+s1*s3, c3*s2*s1-s3*c1,
                     -s2,          c2*c1,          c2*s1,
                   s3*c2, s3*s2*c1-c3*s1, s3*s2*s1+c3*c1);
         else  validSequence = false;
      } // seq2 == 3
      else  validSequence = false;
   }  // seq1 == 1
   else if (seq1 == 2)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  2-1-2
            return Rmatrix33(
                   c3*c1-s3*c2*s1,  s3*s2, -c3*s1-s3*c2*c1,
                            s2*s1,     c2,           s2*c1,
                   s3*c1+c3*c2*s1, -c3*s2, -s3*s1+c3*c2*c1);
         else if (seq3 == 3) //  2-1-3
            return Rmatrix33(
                    c3*c1+s3*s2*s1, s3*c2, -c3*s1+s3*s2*c1,
                   -s3*c1+c3*s2*s1, c3*c2,  s3*s1+c3*s2*c1,
                             c2*s1,   -s2,           c2*c1);

         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  2-3-1
            return Rmatrix33(
                             c2*c1,     s2,          -c2*s1,
                   -c3*s2*c1+s3*s1,  c3*c2,  c3*s2*s1+s3*c1,
                    s3*s2*c1+c3*s1, -s3*c2, -s3*s2*s1+c3*c1);
         else if (seq3 == 2) //  2-3-2
            return Rmatrix33(
                   c3*c2*c1-s3*s1, c3*s2, -c3*c2*s1-s3*c1,
                           -s2*c1,    c2,           s2*s1,
                   s3*c2*c1+c3*s1, s3*s2, -s3*c2*s1+c3*c1);
         else validSequence = false;
      } // seq2 == 3
      else validSequence = false;
   } // seq1 == 2
   else if (seq1 == 3)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  3-1-2
            return Rmatrix33(
                   c3*c1-s3*s2*s1, c3*s1+s3*s2*c1, -s3*c2,
                           -c2*s1,          c2*c1,     s2,
                   s3*c1+c3*s2*s1, s3*s1-c3*s2*c1,  c3*c2);
         else if (seq3 == 3) //  3-1-3
            return Rmatrix33(
                    c3*c1-s3*c2*s1,  c3*s1+s3*c2*c1, s3*s2,
                   -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1, c3*s2,
                             s2*s1,          -s2*c1,    c2);
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 2)
      {
         if (seq3 == 1)      //  3-2-1
            return Rmatrix33(
                             c2*c1,           c2*s1,   -s2,
                   -c3*s1+s3*s2*c1,  c3*c1+s3*s2*s1, s3*c2,
                    s3*s1+c3*s2*c1, -s3*c1+c3*s2*s1, c3*c2);
         else if (seq3 == 3) //  3-2-3
            return Rmatrix33(
                    c3*c2*c1-s3*s1,  c3*c2*s1+s3*c1,   -c3*s2,
                   -s3*c2*c1-c3*s1, -s3*c2*s1+c3*c1,    s3*s2,
                             s2*c1,           s2*s1,       c2);
         else validSequence = false;
      } // seq2 == 2
      else validSequence = false;
   }  // seq1 == 3

   if (!validSequence) throw UtilityException(
      "Invalid euler sequence - cannot convert to cosine matrix.");
   // return zero matrix - it should never reach this point, though
   return Rmatrix33(false);

}

//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Real *eulerAngles,   [static]
//                             Integer seq1, Integer seq2,
 //                            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angles and sequence to a direction cosine matrix.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the cosine direction matrix representation of the input attitude.
 */
//------------------------------------------------------------------------------
Rmatrix33 AttitudeConversionUtility::ToCosineMatrix(const Real *eulerAngles,
                                   Integer    seq1,
                                   Integer    seq2,
                                   Integer    seq3)
{
   if ((seq1 == 0) | (seq2 == 0) | (seq3 == 0))
      throw UtilityException(
         "Euler sequence ill-defined for conversion to cosine matrix.");
   Real s1 = GmatMathUtil::Sin(eulerAngles[0]);
   Real s2 = GmatMathUtil::Sin(eulerAngles[1]);
   Real s3 = GmatMathUtil::Sin(eulerAngles[2]);
   Real c1 = GmatMathUtil::Cos(eulerAngles[0]);
   Real c2 = GmatMathUtil::Cos(eulerAngles[1]);
   Real c3 = GmatMathUtil::Cos(eulerAngles[2]);
   bool validSequence = true;
   if (seq1 == 1)
   {
      if (seq2 == 2)
      {
         if (seq3 == 1)      //  1-2-1
            return Rmatrix33(
                      c2,          s2*s1,           -s2*c1,
                   s3*s2, c3*c1-s3*c2*s1,   c3*s1+s3*c2*c1,
                   c3*s2, -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1);
         else if (seq3 == 3)  //  1-2-3
            return Rmatrix33(
                    c3*c2,  c3*s2*s1+s3*c1, -c3*s2*c1+s1*s3,
                   -s3*c2, -s3*s2*s1+c3*c1,  s3*s2*c1+c3*s1,
                       s2,          -c2*s1,           c2*c1);
         else validSequence = false;
      }  // seq2 == 2
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  1-3-1
            return Rmatrix33(
                       c2,            s2*c1,            s2*s1,
                   -c3*s2,   c3*c2*c1-s3*s1,   c3*c2*s1+s3*c1,
                    s3*s2,  -s3*c2*c1-c3*s1,  -s3*c2*s1+c3*c1);
         else if (seq3 == 2) //  1-3-2
            return Rmatrix33(
                   c3*c2, c3*s2*c1+s1*s3, c3*s2*s1-s3*c1,
                     -s2,          c2*c1,          c2*s1,
                   s3*c2, s3*s2*c1-c3*s1, s3*s2*s1+c3*c1);
         else  validSequence = false;
      } // seq2 == 3
      else  validSequence = false;
   }  // seq1 == 1
   else if (seq1 == 2)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  2-1-2
            return Rmatrix33(
                   c3*c1-s3*c2*s1,  s3*s2, -c3*s1-s3*c2*c1,
                            s2*s1,     c2,           s2*c1,
                   s3*c1+c3*c2*s1, -c3*s2, -s3*s1+c3*c2*c1);
         else if (seq3 == 3) //  2-1-3
            return Rmatrix33(
                    c3*c1+s3*s2*s1, s3*c2, -c3*s1+s3*s2*c1,
                   -s3*c1+c3*s2*s1, c3*c2,  s3*s1+c3*s2*c1,
                             c2*s1,   -s2,           c2*c1);

         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  2-3-1
            return Rmatrix33(
                             c2*c1,     s2,          -c2*s1,
                   -c3*s2*c1+s3*s1,  c3*c2,  c3*s2*s1+s3*c1,
                    s3*s2*c1+c3*s1, -s3*c2, -s3*s2*s1+c3*c1);
         else if (seq3 == 2) //  2-3-2
            return Rmatrix33(
                   c3*c2*c1-s3*s1, c3*s2, -c3*c2*s1-s3*c1,
                           -s2*c1,    c2,           s2*s1,
                   s3*c2*c1+c3*s1, s3*s2, -s3*c2*s1+c3*c1);
         else validSequence = false;
      } // seq2 == 3
      else validSequence = false;
   } // seq1 == 2
   else if (seq1 == 3)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  3-1-2
            return Rmatrix33(
                   c3*c1-s3*s2*s1, c3*s1+s3*s2*c1, -s3*c2,
                           -c2*s1,          c2*c1,     s2,
                   s3*c1+c3*s2*s1, s3*s1-c3*s2*c1,  c3*c2);
         else if (seq3 == 3) //  3-1-3
            return Rmatrix33(
                    c3*c1-s3*c2*s1,  c3*s1+s3*c2*c1, s3*s2,
                   -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1, c3*s2,
                             s2*s1,          -s2*c1,    c2);
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 2)
      {
         if (seq3 == 1)      //  3-2-1
            return Rmatrix33(
                             c2*c1,           c2*s1,   -s2,
                   -c3*s1+s3*s2*c1,  c3*c1+s3*s2*s1, s3*c2,
                    s3*s1+c3*s2*c1, -s3*c1+c3*s2*s1, c3*c2);
         else if (seq3 == 3) //  3-2-3
            return Rmatrix33(
                    c3*c2*c1-s3*s1,  c3*c2*s1+s3*c1,   -c3*s2,
                   -s3*c2*c1-c3*s1, -s3*c2*s1+c3*c1,    s3*s2,
                             s2*c1,           s2*s1,       c2);
         else validSequence = false;
      } // seq2 == 2
      else validSequence = false;
   }  // seq1 == 3

   if (!validSequence) throw UtilityException(
      "Invalid euler sequence - cannot convert to cosine matrix.");
   // return zero matrix - it should never reach this point, though
   return Rmatrix33(false);

}

//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngles(const Rvector &quat1, Integer seq1,  [static]
//                           Integer seq2,         Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input quaternion to a set of euler angles, using the euler
 * sequence provided.
 *
 * @param quat1  the input quaternion.
 * @param seq1   first entry of the euler sequence
 * @param seq2   second entry of the euler sequence
 * @param seq3   third entry of the euler sequence
 *
 * @return the euler angles representation of the input attitude (radians)
 */
//------------------------------------------------------------------------------
Rvector3 AttitudeConversionUtility::ToEulerAngles(const Rvector &quat1, Integer seq1,
                                 Integer seq2,         Integer seq3)
{
   return ToEulerAngles(ToCosineMatrix(quat1),seq1, seq2, seq3);
}

//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,  [static]
//                           Integer seq2,            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input cosine matrix to a set of euler angles, using the euler
 * sequence provided.
 *
 * @param cosMat  the input cosine matrix.
 * @param seq1    first entry of the euler sequence
 * @param seq2    second entry of the euler sequence
 * @param seq3    third entry of the euler sequence
 *
 * @return the euler angles representation (radians) of the input attitude.
 */
//------------------------------------------------------------------------------
Rvector3 AttitudeConversionUtility::ToEulerAngles(const Rmatrix33 &cosMat,
                                 Integer         seq1,
                                 Integer         seq2,
                                 Integer         seq3)
{
   Real theta1, theta2, sin1, cos1;

   Real R11  = cosMat(0,0);
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R22  = cosMat(1,1);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);
   Real R33  = cosMat(2,2);

   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))          // 1-2-3
   {
      theta1 = GmatMathUtil::ATan(-R32, R33);
      theta2 = GmatMathUtil::ASin(R31);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R13*sin1 + R12*cos1),(R23*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))     // 1-3-2
   {
      theta1 = GmatMathUtil::ATan(R23, R22);
      theta2 = GmatMathUtil::ASin(-R21);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R12*sin1 - R13*cos1),(-R32*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))     // 2-3-1
   {
      theta1 = GmatMathUtil::ATan(-R13, R11);
      theta2 = GmatMathUtil::ASin(R12);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R21*sin1 + R23*cos1),(R31*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))     // 2-1-3
   {
      theta1 = GmatMathUtil::ATan(R31, R33);
      theta2 = GmatMathUtil::ASin(-R32);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R23*sin1 - R21*cos1),(-R13*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))     // 3-1-2
   {
      theta1 = GmatMathUtil::ATan(-R21, R22);
      theta2 = GmatMathUtil::ASin(R23);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R32*sin1 + R31*cos1),(R12*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))     // 3-2-1
   {
      theta1 = GmatMathUtil::ATan(R12, R11);
      theta2 = GmatMathUtil::ASin(-R13);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((R31*sin1 - R32*cos1),(-R21*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))     // 1-2-1
   {
      theta1 = GmatMathUtil::ATan(R12, -R13);
      theta2 = GmatMathUtil::ACos(R11);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R33*sin1 - R32*cos1),(R23*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))     // 1-3-1
   {
      theta1 = GmatMathUtil::ATan(R13, R12);
      theta2 = GmatMathUtil::ACos(R11);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R22*sin1 + R23*cos1),(-R32*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))     // 2-1-2
   {
      theta1 = GmatMathUtil::ATan(R21, R23);
      theta2 = GmatMathUtil::ACos(R22);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R33*sin1 + R31*cos1),(-R13*sin1 + R11*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))     // 2-3-2
   {
      theta1 = GmatMathUtil::ATan(R23, -R21);
      theta2 = GmatMathUtil::ACos(R22);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R11*sin1 - R13*cos1),(R31*sin1 + R33*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))     // 3-1-3
   {
      theta1 = GmatMathUtil::ATan(R31, -R32);
      theta2 = GmatMathUtil::ACos(R33);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R22*sin1 - R21*cos1),(R12*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))     // 3-2-3
   {
      theta1 = GmatMathUtil::ATan(R32, R31);
      theta2 = GmatMathUtil::ACos(R33);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);
      return Rvector3(theta1, theta2,
            GmatMathUtil::ATan((-R11*sin1 + R12*cos1),(-R21*sin1 + R22*cos1)));
   }
   else
      throw UtilityException(
      "Invalid Euler sequence - cannot convert cosine matrix to euler angles.");
}

//------------------------------------------------------------------------------
//  Rvector   ToQuaternion(const Rvector3 &eulerAngles, Integer seq1,  [static]
//                         Integer seq2,                Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input set of euler angles to a quaternion, using the euler
 * sequence provided.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the quaternion representation of the input attitude.
 */
//------------------------------------------------------------------------------
Rvector AttitudeConversionUtility::ToQuaternion(const Rvector3 &eulerAngles,
                               Integer seq1,
                               Integer seq2,
                               Integer seq3)
{
   return ToQuaternion(ToCosineMatrix(eulerAngles, seq1, seq2, seq3));
}

//------------------------------------------------------------------------------
//  Rvector   ToQuaternion(const Rmatrix33 &cosMat)             [static]
//------------------------------------------------------------------------------
 /**
 * Converts the input cosine matrix to a quaternion.
 *
 * @param cosMat  the input cosine matrix.
 *
 * @return the quaternion representation of the input attitude.
 */
//------------------------------------------------------------------------------
Rvector AttitudeConversionUtility::ToQuaternion(const Rmatrix33 &cosMat)
{
   Real R11  = cosMat(0,0);
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R22  = cosMat(1,1);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);
   Real R33  = cosMat(2,2);

   Real    q1, q2, q3, q4;
   Real    matT   = cosMat.Trace();
   Real    v[4]   = {R11, R22, R33, matT};
   // find index of maximum component of v
   Integer maxI   = 0;
   Real    maxNum = v[0];
   for (Integer i = 1; i < 4; i++)
      if (v[i] > maxNum)
      {
         maxNum = v[i];
         maxI   = i;
      }
   if (maxI == 0)
   {
      q1 = 2.0 * v[0] + 1.0 - matT;
      q2 = R12 + R21;
      q3 = R13 + R31;
      q4 = R23 - R32;
   }
   else if (maxI == 1)
   {
      q1 = R21 + R12;
      q2 = 2.0 * v[1] + 1.0 - matT;
      q3 = R23 + R32;
      q4 = R31 - R13;
   }
   else if (maxI == 2)
   {
      q1 = R31 + R13;
      q2 = R32 + R23;
      q3 = 2.0 * v[2] + 1.0 - matT;
      q4 = R12 - R21;
   }
   else // maxI == 4
   {
      q1 = R23 - R32;
      q2 = R31 - R13;
      q3 = R12 - R21;
      q4 = 1.0 + matT;
   }

   return (Rvector(4,q1,q2,q3,q4)).Normalize();
}


//------------------------------------------------------------------------------
//  Rvector   ToQuaternion(const Rvector3 &MRPs)             [static]
//------------------------------------------------------------------------------
//
// New method written by Dunn
//
/**
* Converts the input Modified Rodriguez Parameters to a quaternion vector.  Note
* that we are now using the CCSDS definition of quaternions where qc = q4.
*
* @param MRPs  the input MRP vector.
*
* @return the quaternion representation of the input attitude.
*/
//------------------------------------------------------------------------------
Rvector AttitudeConversionUtility::ToQuaternion(const Rvector3 &MRPs)
{
   Real MRP1  = MRPs(0);
   Real MRP2  = MRPs(1);
   Real MRP3  = MRPs(2);

   Real PTP   = MRP1*MRP1 + MRP2*MRP2 + MRP3*MRP3;

   Real q1;
   Real q2;
   Real q3;
   Real qc;

   // Convert MRPs to Quats Following Formula from Math Spec
   q1 = 2.0 * MRP1  / ( 1 + PTP );
   q2 = 2.0 * MRP2  / ( 1 + PTP );
   q3 = 2.0 * MRP3  / ( 1 + PTP );
   qc = ( 1 - PTP ) / ( 1 + PTP );

   return (Rvector(4,q1,q2,q3,qc)).Normalize();
}

//------------------------------------------------------------------------------
//  Rvector   ToMRPs(const Rvector &quat1)             [static]
//------------------------------------------------------------------------------
//
// New method written by Dunn
//
/**
* Converts the input quaternion vector into the Modified Rodriguez Parameters.
* Note that we are now using the CCSDS definition of quaternions where qc = q4.
*
* @param quat1  the input quaternion
*
* @return the MRP representation of the input attitude.
*/
//------------------------------------------------------------------------------
Rvector3 AttitudeConversionUtility::ToMRPs(const Rvector &quat1)
{
   Real q1  = quat1(0);
   Real q2  = quat1(1);
   Real q3  = quat1(2);
   Real qc  = quat1(3);

   Real MRP1,MRP2,MRP3;

   // Convert Quats to MRPs Following Formula from Math Spec
   MRP1 = q1 / ( 1.0 + qc );
   MRP2 = q2 / ( 1.0 + qc );
   MRP3 = q3 / ( 1.0 + qc );

   return Rvector3( MRP1, MRP2, MRP3 );
}

//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngleRates(const Rvector3 &angVel,         [static]
//                               const Rvector3 &eulerAngles,
//                               Integer seq1, Integer seq2,
//                               Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input angular velocity to a set of euler angle rates,
 * using the euler sequence provided.
 *
 * @param angVel       the input angular velocity (radians/sec)
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the euler angle rates representation (radians/second).
 * @note Obviously, the angular velocity and euler angles passed in must have
 *       been computed at the same time.
 */
//------------------------------------------------------------------------------
Rvector3 AttitudeConversionUtility::ToEulerAngleRates(const Rvector3 &angularVel,
                                     const Rvector3 &eulerAngles,
                                     Integer        seq1,
                                     Integer        seq2,
                                     Integer        seq3)
{
   #ifdef DEBUG_EULER_ANGLE_RATES
      MessageInterface::ShowMessage(
            "Computing Euler Angle Rates from angular velocity %12.10f  %12.10f  %12.10f\n",
            angularVel[0], angularVel[1], angularVel[2]);
      MessageInterface::ShowMessage("and Euler Angles %12.10f  %12.10f  %12.10f\n",
            eulerAngles[0] * GmatMathConstants::DEG_PER_RAD, eulerAngles[1] * GmatMathConstants::DEG_PER_RAD,
            eulerAngles[2] * GmatMathConstants::DEG_PER_RAD);
      MessageInterface::ShowMessage("with sequence %d  %d  %d\n", seq1, seq2, seq3);
   #endif
   bool      singularity = false;
   Real      s2          = GmatMathUtil::Sin(eulerAngles(1));
   Real      c2          = GmatMathUtil::Cos(eulerAngles(1));
   Real      s3          = GmatMathUtil::Sin(eulerAngles(2));
   Real      c3          = GmatMathUtil::Cos(eulerAngles(2));
   Rmatrix33 Si;
   #ifdef DEBUG_EULER_ANGLE_RATES
      MessageInterface::ShowMessage(
            "s2 = %12.10f,   c2 = %12.10f,   s3 = %12.10f,   c3 = %12.10f\n", s2, c2, s3, c3);
   #endif

   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))        // 1-2-3
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    c3/c2,   -s3/c2, 0.0,
                       s3,       c3, 0.0,
                -c3*s2/c2, s3*s2/c2, 1.0);
      }
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))  // 1-3-2
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   c3/c2, 0.0,    s3/c2,
                     -s3, 0.0,       c3,
                c3*s2/c2, 1.0, s3*s2/c2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))  // 2-3-1
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,     c3/c2,   -s3/c2,
                0.0,        s3,       c3,
                1.0, -c3*s2/c2, s3*s2/c2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))  // 2-1-3
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   s3/c2,    c3/c2, 0.0,
                      c3,      -s3, 0.0,
                s3*s2/c2, c3*s2/c2, 1.0);
      }
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))  // 3-1-2
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(  -s3/c2, 0.0,     c3/c2,
                      c3, 0.0,        s3,
                s3*s2/c2, 1.0, -c3*s2/c2);
      }
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))  // 3-2-1
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,    s3/c2,    c3/c2,
                0.0,       c3,      -s3,
                1.0, s3*s2/c2, c3*s2/c2);
      }
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))  // 1-2-1
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,     s3/s2,     c3/s2,
                0.0,        c3,       -s3,
                1.0, -s3*c2/s2, -c3*c2/s2);
      }
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))  // 1-3-1
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,   -c3/s2,     s3/s2,
                0.0,       s3,        c3,
                1.0, c3*c2/s2, -s3*c2/s2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))  // 2-1-2
   {
      #ifdef DEBUG_EULER_ANGLE_RATES
         MessageInterface::ShowMessage(
               "in 2-1-2 calculations\n");
      #endif
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    s3/s2, 0.0,   -c3/s2,
                       c3, 0.0,       s3,
                -s3*c2/s2, 1.0, c3*c2/s2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))  // 2-3-2
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    c3/s2, 0.0,     s3/s2,
                      -s3, 0.0,        c3,
                -c3*c2/s2, 1.0, -s3*c2/s2);
      }
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))  // 3-1-3
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    s3/s2,     c3/s2, 0.0,
                       c3,       -s3, 0.0,
                -s3*c2/s2, -c3*c2/s2, 1.0);
      }
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))  // 3-2-3
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   -c3/s2,     s3/s2, 0.0,
                       s3,        c3, 0.0,
                 c3*c2/s2, -s3*c2/s2, 1.0);
      }
   }
   else
       throw UtilityException(
      "Invalid Euler sequence - cannot compute euler angle rates.");
   if (singularity)
   {
      #ifdef DEBUG_EULER_ANGLE_RATES
         MessageInterface::ShowMessage(
               "...... singularity found!!!\n");
      #endif
      std::stringstream errmsg;
      errmsg << "The attitude defined by the input euler angles (";
      errmsg << (eulerAngles(0) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eulerAngles(1) * GmatMathConstants::DEG_PER_RAD) << ", "
             << (eulerAngles(2) * GmatMathConstants::DEG_PER_RAD);
      errmsg << ") is near a singularity.  The allowed values are:\n";
      errmsg << "For a symmetric sequence EulerAngle2 != 0. ";
      errmsg << "For a non-symmetric sequence EulerAngle2 != 90.  ";
      errmsg << "The tolerance on EulerAngle2 singularity is ";
      errmsg << GmatAttitudeConstants::EULER_ANGLE_TOLERANCE << ".\n";
      throw UtilityException(errmsg.str());
   }
   #ifdef DEBUG_EULER_ANGLE_RATES
      MessageInterface::ShowMessage(
            "After computation, Si =  %12.10f  %12.10f  %12.10f\n"
            "                         %12.10f  %12.10f  %12.10f\n"
            "                         %12.10f  %12.10f  %12.10f\n",
            Si(0,0), Si(0,1), Si(0,2), Si(1,0), Si(1,1), Si(1,2), Si(2,0), Si(2,1), Si(2,2));
      Rvector3 earates = Si * angularVel;
      MessageInterface::ShowMessage(
            "After computation, euler angle rates =  %12.10f  %12.10f  %12.10f\n",
            earates[0] * GmatMathConstants::DEG_PER_RAD,
            earates[1] * GmatMathConstants::DEG_PER_RAD,
            earates[2] * GmatMathConstants::DEG_PER_RAD);
   #endif

   return Si * angularVel;
 }

//------------------------------------------------------------------------------
//  Rvector3   ToAngularVelocity(const Rvector3 &eulerRates,       [static]
//                               const Rvector3 &eulerAngles,
//                               Integer seq1, Integer seq2,
//                               Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angle rates to an angular velocity,
 * using the euler sequence provided.
 *
 * @param eulerRates   the input euler angle rates (radians/sec)
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the angular velocity representation (radians/second).
 * @note Obviously, the euler rates and euler angles passed in must have been
 *       computed at the same time.
 */
//------------------------------------------------------------------------------
Rvector3 AttitudeConversionUtility::ToAngularVelocity(const Rvector3 &eulerRates,
                                     const Rvector3 &eulerAngles,
                                     Integer        seq1,
                                     Integer        seq2,
                                     Integer        seq3)
{
   #ifdef DEBUG_EULER_ANGLE_RATES
      MessageInterface::ShowMessage("Entering ToAngVel with EulerRates  = %lf  %lf  %lf\n",
            eulerRates[0] * GmatMathConstants::DEG_PER_RAD,
            eulerRates[1] * GmatMathConstants::DEG_PER_RAD,
            eulerRates[2] * GmatMathConstants::DEG_PER_RAD);
      MessageInterface::ShowMessage("                   and EulerAngles = %lf  %lf  %lf\n",
            eulerAngles[0] * GmatMathConstants::DEG_PER_RAD,
            eulerAngles[1] * GmatMathConstants::DEG_PER_RAD,
            eulerAngles[2] * GmatMathConstants::DEG_PER_RAD);
      MessageInterface::ShowMessage("                   and Euler Seq   = %d  %d  %d\n",
            seq1, seq2, seq3);
   #endif
   Real s2 = GmatMathUtil::Sin(eulerAngles(1));
   Real c2 = GmatMathUtil::Cos(eulerAngles(1));
   Real s3 = GmatMathUtil::Sin(eulerAngles(2));
   Real c3 = GmatMathUtil::Cos(eulerAngles(2));
   Rmatrix33 S;

   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))        // 1-2-3
   {
      S.Set( c3*c2,  s3, 0.0,
            -s3*c2,  c3, 0.0,
                s2, 0.0, 1.0);
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))  // 1-3-2
   {
      S.Set(c3*c2, -s3, 0.0,
              -s2, 0.0, 1.0,
            s3*c2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))  // 2-3-1
   {
      S.Set(    s2, 0.0, 1.0,
             c3*c2,  s3, 0.0,
            -s3*c2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))  // 2-1-3
   {
      S.Set(s3*c2,  c3, 0.0,
            c3*c2, -s3, 0.0,
              -s2, 0.0, 1.0);
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))  // 3-1-2
   {
      S.Set(-s3*c2,  c3, 0.0,
                s2, 0.0, 1.0,
             c3*c2,  s3, 0.0);
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))  // 3-2-1
   {
      S.Set(  -s2, 0.0, 1.0,
            s3*c2,  c3, 0.0,
            c3*c2, -s3, 0.0);
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))  // 1-2-1
   {
      S.Set(   c2, 0.0, 1.0,
            s3*s2,  c3, 0.0,
            c3*s2, -s3, 0.0);
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))  // 1-3-1
   {
      S.Set(    c2, 0.0, 1.0,
            -c3*s2,  s3, 0.0,
             s3*s2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))  // 2-1-2
   {
      S.Set( s3*s2,  c3, 0.0,
                c2, 0.0, 1.0,
            -c3*s2,  s3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))  // 2-3-2
   {
      S.Set(c3*s2, -s3, 0.0,
               c2, 0.0, 1.0,
            s3*s2,  c3, 0.0);
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))  // 3-1-3
   {
      S.Set(s3*s2,  c3, 0.0,
            c3*s2, -s3, 0.0,
               c2, 0.0, 1.0);
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))  // 3-2-3
   {
      S.Set(-c3*s2,  s3, 0.0,
             s3*s2,  c3, 0.0,
                 c2, 0.0, 1.0);
   }
   else
       throw UtilityException(
      "Invalid Euler sequence - cannot compute euler angle rates.");

   #ifdef DEBUG_EULER_ANGLE_RATES
      Rvector3 av = S * eulerRates;
      MessageInterface::ShowMessage("Exiting ToAngVel with ang vel  = %lf  %lf  %lf\n",
            av[0], av[1], av[2]);
   #endif
   return S * eulerRates;
}

//------------------------------------------------------------------------------
//  Rmatrix33  EulerAxisAndAngleToDCM(const Rvector3 &eAxis, Real eAngle)
//                                                                      [static]
//------------------------------------------------------------------------------
/**
 * This method computes the direction cosine matrix given the input
 * euler axis and angle.
 *
 * @param <eAxis>  euler axis.
 * @param <eAngle> euler angle.
 *
 * @return  Cosine matrix representation of the attitude.
 *
 */
//------------------------------------------------------------------------------
Rmatrix33 AttitudeConversionUtility::EulerAxisAndAngleToDCM(const Rvector3 &eAxis, Real eAngle)
{
   Rmatrix33 a_x(      0.0, -eAxis(2),  eAxis(1),
                  eAxis(2),       0.0, -eAxis(0),
                 -eAxis(1),  eAxis(0),      0.0);
   Rmatrix33 I33(true);
   Real c = GmatMathUtil::Cos(eAngle);
   Real s = GmatMathUtil::Sin(eAngle);
   return Rmatrix33((c*I33) + ((1.0 - c)*Outerproduct(eAxis, eAxis))
                    - s*a_x);
}

//------------------------------------------------------------------------------
//  void  DCMToEulerAxisAndAngle(const Rmatrix33 &cosmat,              [static]
//                               Rvector3 &eAxis, Real &eAngle)
//------------------------------------------------------------------------------
/**
 * This method computes the euler axis and angle given the input cosine matrix.
 *
 * @param <cosmat>  cosine matrix.
 * @param <eAxis>   euler axis (output).
 * @param <eAngle>  euler angle (output).
 *
 * @return  Euler Axis/Angle representation of the attitude.
 *
 */
//------------------------------------------------------------------------------
void AttitudeConversionUtility::DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat,
                                      Rvector3 &eAxis, Real &eAngle)
{
   static Real TOL = 1.0E-14;
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);

   eAngle      = GmatMathUtil::ACos(0.5 * (cosMat.Trace() - 1.0));
   Real s      = GmatMathUtil::Sin(eAngle);
   if (GmatMathUtil::Abs(s)  < TOL)
   {
      eAxis.Set(1.0, 0.0, 0.0);
      return;
   }
   Real mult   = 1.0 / (2.0 * s);
   eAxis.Set(mult*(R23-R32), mult*(R31-R13), mult*(R12-R21));
   return;
}

//------------------------------------------------------------------------------
//  bool  IsValidEulerSequence(const std::string &theSeq)             [static]
//------------------------------------------------------------------------------
/**
 * This method determines of the input string represents a valid
 * Euler Rotation Sequence.
 *
 * @param <theSeq>  euler sequence string
 *
 * @return  true if input is a valid euler sequence; false otherwise
 *
 */
//------------------------------------------------------------------------------
bool AttitudeConversionUtility::IsValidEulerSequence(const std::string &theSeq)
{
   for (Integer i=0; i<12; i++)
      if (theSeq == VALID_EULER_SEQUENCES[i])  return true;
   return false;
}

