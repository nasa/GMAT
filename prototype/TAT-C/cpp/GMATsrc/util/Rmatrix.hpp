//$Id$
//------------------------------------------------------------------------------
//                                Rmatrix
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Created: 1996/06/30 for GSS project (originally Matrix)
// Modified:
//   2003/09/15 Linda Jun - Replaced GSSString with std::string.
//
/**
 * Declares Matrix operations.
 */
//------------------------------------------------------------------------------
#ifndef h_Rmatrix_hpp
#define h_Rmatrix_hpp

#include "RealUtilities.hpp"
#include "TableTemplate.hpp"
#include "BaseException.hpp"
#include "utildefs.hpp"
#include "GmatGlobal.hpp"      // for DATA_PRECISION

// Forward declarations
class Rvector;
class Rvector3;

class GMATUTIL_API Rmatrix :  public TableTemplate<Real>
{
public:
   
   // exceptions
   class NotSquare : public BaseException 
   { public:  NotSquare(const std::string &message =
                        "Rmatrix error: matrix not square\n")
        : BaseException(message) {}; };
   class IsSingular : public BaseException
   { public:  IsSingular(const std::string &message =
                         "Rmatrix error: matrix is singular\n")
        : BaseException(message) {}; };
   class DivideByZero : public BaseException
   { public:  DivideByZero(const std::string &message =
                           "Rmatrix error: attempt to divide by zero\n")
        : BaseException(message) {}; };
   
   Rmatrix();
   Rmatrix(int r, int c);
   Rmatrix(int r, int c, Real a1, ...);
   Rmatrix(const Rmatrix &m);
   virtual ~Rmatrix();
   
// ekf mod 12/16
   static Rmatrix Identity(unsigned int size);
   static Rmatrix Diagonal(unsigned int size, Rvector data);
// end ekf mod

   virtual bool 
   IsOrthogonal(Real accuracyRequired = GmatRealConstants::REAL_EPSILON) const;
   virtual bool 
   IsOrthonormal(Real accuracyRequired = GmatRealConstants::REAL_EPSILON) const;
   
   const Rmatrix& operator=(const Rmatrix &m);
   bool operator==(const Rmatrix &m)const;
   bool operator!=(const Rmatrix &m)const;
   
   //  operations with other matrices
   Rmatrix operator+(const Rmatrix& RHSRmatrix) const;
   const Rmatrix& operator+=(const Rmatrix &RHSRmatrix);
   
   Rmatrix operator-(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator-=(const Rmatrix &RHSRmatrix);
   
   Rmatrix operator*(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator*=(const Rmatrix &RHSRmatrix);
   
   Rmatrix operator/(const Rmatrix &RHSRmatrix) const;
   const Rmatrix& operator/=(const Rmatrix &RHSRmatrix);
   
   Rmatrix ElementWiseMultiply(const Rmatrix &m);
   Rmatrix ElementWiseDivide(const Rmatrix &m);

   // operations with scalars
   Rmatrix operator+(Real scalar) const;
   const Rmatrix& operator+=(Real scalar);
   
   Rmatrix operator-(Real scalar) const;
   const Rmatrix& operator-=(Real scalar);
   
   Rmatrix operator*(Real scalar) const;
   const Rmatrix& operator*=(Real scalar);
   
   Rmatrix operator/(Real scalar) const;
   const Rmatrix& operator/=(Real scalar);
   
   
   // negate
   Rmatrix operator-() const;
   
   friend class Rvector;
   friend class Rvector3;
   Rvector operator*(const Rvector &v) const;
   
   GMATUTIL_API friend Rmatrix operator+(Real scalar, const Rmatrix &m);
   GMATUTIL_API friend Rmatrix operator-(Real scalar, const Rmatrix &m);
   GMATUTIL_API friend Rmatrix operator*(Real scalar, const Rmatrix &m);
   GMATUTIL_API friend Rmatrix operator/(Real scalar, const Rmatrix &m);
   
   virtual Real   Trace() const ;                            
   virtual Real   Determinant() const;            
   virtual Real   Cofactor(int r, int c) const;            
   Rmatrix Transpose() const;                        
   Rmatrix Inverse() const;
   Rmatrix Inverse(Real zeroValue) const;    // ekf mod 12/16
   virtual Rmatrix Pseudoinverse(Real zeroValue = 1e-12) const;            
   Rmatrix Symmetric() const;            
   Rmatrix AntiSymmetric() const;    
   
   // friends
   friend Rmatrix SkewSymmetric4by4(const Rvector3 &v);
   friend Rmatrix TransposeTimesMatrix(const Rmatrix &m1, const Rmatrix &m2);
   friend Rmatrix MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2);
   friend Rmatrix TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2);
   
   friend std::istream& operator>> (std::istream &input, Rmatrix &a);
   friend std::ostream& operator<< (std::ostream &output, const Rmatrix &a);
   
   Rvector GetRow(int r) const;
   Rvector GetColumn(int c) const;
   Rvector GetRowOrColumn() const;
   
   void    MakeOneRowMatrix(const Rvector &vec);
   void    MakeOneColumnMatrix(const Rvector &vec);
   
   const StringArray& GetStringVals(Integer p = GmatGlobal::DATA_PRECISION,
                                    Integer w = GmatGlobal::DATA_WIDTH);
   
   virtual std::string ToString(Integer precision, Integer width = 1,
                                bool horizontal = false,
                                const std::string &prefix = "",
                                bool appendEol = true) const;
   
   virtual std::string ToString(bool useCurrentFormat = true,
                                bool scientific = false, bool showPoint = false,
                                Integer precision = GmatGlobal::DATA_PRECISION,
                                Integer width = GmatGlobal::DATA_WIDTH,
                                bool horizontal = true, Integer spacing = 1,
                                const std::string &prefix = "",
                                bool appendEol = true) const;
   
   virtual std::string ToRowString(Integer row, Integer precision,
                                   Integer width = 1, bool zeroFill = false) const;
   
protected:   
   StringArray stringVals;
   
private:
};
#endif // Rmatrix_hpp
