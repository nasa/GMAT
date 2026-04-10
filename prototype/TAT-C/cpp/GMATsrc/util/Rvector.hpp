//$Id$
//------------------------------------------------------------------------------
//                                Rvector
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
// Author: M. Weippert, T. McRoberts, L. Jun, E. Corderman
// Created: 1995/10/10 for GSS project (originally Vector)
// Modified:
//   2003/09/15 Linda Jun - Replaced GSSString with std::string
//
/**
 * Declarations for the Rvector class, providing linear algebra operations
 * for the general n-element Real vector.
 */
//------------------------------------------------------------------------------
#ifndef Rvector_hpp
#define Rvector_hpp

#include "utildefs.hpp"
#include "ArrayTemplate.hpp"
#include "BaseException.hpp"
#include "RealUtilities.hpp"
#include "GmatGlobal.hpp"      // for DATA_PRECISION

//forward declaration for matrix operations
class Rmatrix;

class GMATUTIL_API Rvector : public ArrayTemplate<Real>
{
public:
   
   // exceptions
   class ZeroVector : public BaseException
   { public:  ZeroVector(const std::string &details = "")
        : BaseException("Rvector error : vector is a zero vector", details) {}; };
   
   Rvector();
   Rvector(int size);
   Rvector(int size, Real a1, ... );  //Note: . is required for Real value. eg) 123., 100.
   Rvector(const RealArray &ra);
   Rvector(const Rvector &v);
   virtual ~Rvector();
   
   void Set(int numElem, Real a1, ...);
   void Set(Real *data, int size = 0);
   
   virtual RealArray GetRealArray() const;
   
   virtual Real GetMagnitude() const;          
   Rvector GetUnitRvector() const; 
   const Rvector& Normalize();
   const Rvector& operator=(const Rvector &v); 
   bool operator==(const Rvector &v)const;
   bool operator!=(const Rvector &v)const;
   Rvector operator-() const;                     // negation 
   Rvector operator+(const Rvector & v) const;
   const Rvector& operator+=(const Rvector &v);
   Rvector operator-(const Rvector &v) const; 
   const Rvector& operator-=(const Rvector &v);
   Rvector operator*(Real s) const;
   const Rvector& operator*=(Real s);
   Real operator*(const Rvector &v) const;        // dot product
   Rvector operator/(Real s) const;
   const Rvector& operator/=(Real s);
   
   // select vector elements, may add things like find() here as well
   Real Max();
   Real Min();
   
   // sorting
   void Sort(bool ascending = true);   // standard sort
   void Sort(IntegerArray &indices,    // sort including indices
             bool ascending=true);
   
   friend class Rmatrix;
   Rvector operator*(const Rmatrix &m) const; 
   const Rvector& operator*=(const Rmatrix &m);
   Rvector operator/(const Rmatrix &m) const;
   const Rvector& operator/=(const Rmatrix &m);
   
   virtual bool MakeZeroVector();
   virtual bool IsZeroVector() const;
   Real Norm();
   
   virtual std::string ToString(const std::string &format, Integer col) const;
   virtual std::string ToString(Integer precision, bool horizontal = true,
                                const std::string &prefix = "") const;
   virtual std::string ToString(bool useCurrentFormat = true,
                                bool scientific = false, bool showPoint = false,
                                Integer precision = GmatGlobal::DATA_PRECISION,
                                Integer width = GmatGlobal::DATA_WIDTH,
                                bool horizontal = true, Integer spacing = 1,
                                const std::string &prefix = "",
                                bool appendEol = false) const;
   
   friend Rvector GMATUTIL_API operator*(Real s, const Rvector &v);
   friend Rmatrix GMATUTIL_API Outerproduct(const Rvector &v1, const Rvector &v2);
   
   friend std::istream& operator>> (std::istream &input, Rvector &a);
   friend std::ostream& operator<< (std::ostream &output, const Rvector &a);
   
protected:
private:
};

// structure for Matlab-type sort which move original indices as well as values
struct Element
{
   Real value;
   Integer index;
};
// comparison operator used in Matlab-type index sort
bool operator<(const Element &e1, const Element &e2);
#endif // Rvector_hpp
