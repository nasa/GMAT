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
#include <stdarg.h>
#include <sstream>
#include <stdio.h>            // for sprintf()
#include "ArrayTemplate.hpp"
#include "TableTemplate.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "Rvector.hpp"
#include "Linear.hpp"         // for operator<<, operator >>

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rvector()
//------------------------------------------------------------------------------
Rvector::Rvector()
   : ArrayTemplate<Real>()
{
}

//------------------------------------------------------------------------------
//  Rvector(int size)
//------------------------------------------------------------------------------
Rvector::Rvector(int size)
   : ArrayTemplate<Real> (size)
{
    int i;
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] = 0.0;
    }
}

//------------------------------------------------------------------------------
//  Rvector(int size, Real a1, ...)
//------------------------------------------------------------------------------
/**
 * @note . is required for variable length Real values. eg) 123., 100.
 */
//------------------------------------------------------------------------------
Rvector::Rvector(int size, Real a1, ...)
  : ArrayTemplate<Real> (size)
{
    int i;
    va_list ap;

    elementD[0] = a1;
    va_start(ap, a1);
    for (i = 1; i < sizeD; i++)
    {
        elementD[i] = va_arg(ap, Real);
    }

    va_end(ap);
}

//------------------------------------------------------------------------------
//  Rvector(const RealArray &ra)
//------------------------------------------------------------------------------
Rvector::Rvector(const RealArray &ra)
   : ArrayTemplate<Real> (ra.size())
{
   int i;
   for (i = 0; i < sizeD; i++)
   {
      elementD[i] = ra.at(i);
   }
}


//------------------------------------------------------------------------------
//  Rvector(const Rvector &v)
//------------------------------------------------------------------------------
Rvector::Rvector(const Rvector &v)
   : ArrayTemplate<Real>(v)
{
}

//------------------------------------------------------------------------------
//  ~Rvector()
//------------------------------------------------------------------------------
Rvector::~Rvector()
{
}


//------------------------------------------------------------------------------
//  Set(int numElem, Real a1, ...)
//------------------------------------------------------------------------------
/**
 * @note . is required for variable length Real values. eg) 123., 100.
 */
//------------------------------------------------------------------------------
void Rvector::Set(int numElem, Real a1, ...)
{
   int i;
   va_list ap;
   
   // Set size if not sized otherwise check the size
   if (!IsSized())
      SetSize(numElem);
   else if (GetSize() < numElem)
      throw  ArrayTemplateExceptions::IllegalSize();
   
   elementD[0] = a1;
   va_start(ap, a1);
   for (i = 1; i < numElem; i++)
   {
      elementD[i] = va_arg(ap, Real);
   }
 
   va_end(ap);
}


//------------------------------------------------------------------------------
// void Set(Real *data, int size = 0)
//------------------------------------------------------------------------------
/*
 * Sets elements.
 *
 * @param  data  Real data array
 * @param  size  Number of data points to set
 *               if size is 0, it set all elements
 */
//------------------------------------------------------------------------------
void Rvector::Set(Real *data, int size)
{
   if (size > sizeD)
      throw ArrayTemplateExceptions::IllegalSize();

   int count = size;
   if (size == 0)
      count = sizeD;
   
   for (int i = 0; i < count; i++)
      elementD[i] = data[i];
   
}

//------------------------------------------------------------------------------
//  RealArray GetRealArray() const
//------------------------------------------------------------------------------
RealArray Rvector::GetRealArray() const
{
   RealArray ra;
   for (int ii = 0; ii < sizeD; ii++)
      ra.push_back((Real) elementD[ii]);
   return ra;
}


//------------------------------------------------------------------------------
//  Real GetMagnitude() const
//------------------------------------------------------------------------------
Real Rvector::GetMagnitude() const
{
    Real sum = 0.0;
    int  i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        sum += elementD[i]*elementD[i];
    }
    return(GmatMathUtil::Sqrt(sum));
}


//------------------------------------------------------------------------------
//  Rvector GetUnitRvector() const
//------------------------------------------------------------------------------
Rvector Rvector::GetUnitRvector() const
{
   int i;

   if (isSizedD == false)
      throw ArrayTemplateExceptions::UnsizedArray();

   Real mag = GetMagnitude();

   if (GmatMathUtil::IsZero(mag))
      throw ZeroVector(" from Rvector::GetUnitRvector()\n");
   
   Rvector vect(*this);
   
   for (i = 0; i < sizeD; i++)
      vect.elementD[i] /= mag;

   return vect;
}


//------------------------------------------------------------------------------
//  const Rvector& Normalize()
//------------------------------------------------------------------------------
const Rvector& Rvector::Normalize()
{
   int i;
   
   if (isSizedD == false)
      throw ArrayTemplateExceptions::UnsizedArray();
   
   Real mag = GetMagnitude();
   if (GmatMathUtil::IsZero(mag))
      throw ZeroVector(" from Rvector::Normalize()\n");
   
   for (i = 0; i < sizeD; i++)
      elementD[i] /=  mag;
   
   return *this;
}


//------------------------------------------------------------------------------
//  const Rvector& operator=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator=(const Rvector &v)
{
    ArrayTemplate<Real>::operator=(v);
    return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const Rvector &v) const
//------------------------------------------------------------------------------
bool Rvector::operator==(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    if(this!= &v)
    {
        if (sizeD == v.sizeD)
        {
            for(i = 0; i<sizeD; i++)
            {
                if(elementD[i] != v[i])
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------
//  bool operator!=(const Rvector &v) const
//------------------------------------------------------------------------------
bool Rvector::operator!=(const Rvector &v) const
{
    return (bool)!(*this == v);
}

//------------------------------------------------------------------------------
//  Rvector operator-() const                      // negation
//------------------------------------------------------------------------------
Rvector Rvector::operator-() const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    Rvector vector(sizeD);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] = -elementD[i];
    }
    return vector;
}

//------------------------------------------------------------------------------
//  Rvector operator+(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rvector::operator+(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
    
    Rvector vect(*this);
    for (i = 0; i < sizeD; i++)
    {
        vect.elementD[i] += v.elementD[i];
    }
    return vect;
}

//------------------------------------------------------------------------------
//  const Rvector& operator+=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator+=(const Rvector &v)
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }

    for (i = 0; i < sizeD; i++)
    {
        elementD[i] += v.elementD[i];
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator-(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rvector::operator-(const Rvector &v) const
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
    
    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] -= v.elementD[i];
    }
    return vector;
} 

//------------------------------------------------------------------------------
//  const Rvector& operator-=(const Rvector &v)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator-=(const Rvector &v)
{
    int i;
   
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] -= v.elementD[i];
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator*(Real s) const
//------------------------------------------------------------------------------
Rvector Rvector::operator*(Real s) const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }

    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] *= s;
    }
    return vector;
}

//------------------------------------------------------------------------------
//  const Rvector& operator*=(Real s)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator*=(Real s)
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
   
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] *= s;
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Real operator*(const Rvector &v) const         // dot product
//------------------------------------------------------------------------------
Real Rvector::operator*(const Rvector &v) const
{
    if ((isSizedD == false) || (v.IsSized() == false))
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (sizeD != v.sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }
   
    Real sum = 0.0;
    int  i;
    for (i = 0; i < sizeD; i++)
    {
        sum += elementD[i] * v.elementD[i];
    }
    return sum;    
}

//------------------------------------------------------------------------------
//  Rvector operator/(Real s) const
//------------------------------------------------------------------------------
Rvector Rvector::operator/(Real s) const
{
    int i;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
   
    Rvector vector(*this);
    for (i = 0; i < sizeD; i++)
    {
        vector.elementD[i] /= s;
    }
    return vector;
}

//------------------------------------------------------------------------------
//  const Rvector& operator/=(Real s)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator/=(Real s)
{
    int i;
   
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (GmatMathUtil::IsZero(s))
    {
        throw RealUtilitiesExceptions::ArgumentError();
    }
    for (i = 0; i < sizeD; i++)
    {
        elementD[i] /= s;
    }
    return *this;
}

//------------------------------------------------------------------------------
//  Real Max()
//------------------------------------------------------------------------------
/**
 * 
 * returns maximum value in vector
 * 
 * @return  maximum value contained in this vector
 */
//------------------------------------------------------------------------------
Real Rvector::Max()
{
   if (!isSizedD) throw ArrayTemplateExceptions::UnsizedArray();
   
   Real maxval = elementD[0];
   for (int i = 0; i < sizeD; i++)
      if (elementD[i] > maxval)
         maxval=elementD[i];
   return maxval;
}

//------------------------------------------------------------------------------
//  Real Min()
//------------------------------------------------------------------------------
/**
 *
 * returns minimum value in vector
 *
 * @return  minimum value contained in this vector
 */
//------------------------------------------------------------------------------
Real Rvector::Min()
{
   if (!isSizedD) throw ArrayTemplateExceptions::UnsizedArray();
   
   Real minval = elementD[0];
   for (int i = 0; i < sizeD; i++)
      if (elementD[i] < minval)
         minval=elementD[i];
   return minval;
}

//------------------------------------------------------------------------------
//  void Sort(bool ascending = true)
//------------------------------------------------------------------------------
/**
 *
 * sorts an Rvector in ascending or descending order
 *
 * @param  ascending input of true => ascending sort, false => descending sort
 */
//------------------------------------------------------------------------------
void Rvector::Sort(bool ascending)
{
   if (!isSizedD) throw ArrayTemplateExceptions::UnsizedArray();
   
   std::list<Real> rlist;
   rlist.clear();
   
   // load & sort rlist
   for (int i = 0; i < sizeD; i++)
      rlist.push_back(elementD[i]);
   rlist.sort();
   
   // pull sorted values off list in ascending or descending order
   if (ascending)
      for (int i = 0; i < sizeD; i++)
      {
         elementD[i] = rlist.front();
         rlist.pop_front();
      }
   else // not ascending is descending
      for (int i = 0; i < sizeD; i++)
      {
         elementD[i] = rlist.back();
         rlist.pop_back();
      }
}

//------------------------------------------------------------------------------
//  void Sort(IntegerArray &indices, bool ascending = true)
//------------------------------------------------------------------------------
/**
 *
 * sorts an Rvector in ascending or descending order, returning an array
 * containing the original indices of the values in the updated Rvector
 *
 * @note  this sort matches the Matlab sort that returns an index array
 *
 * @param  indices   array containing original indices after sort is done
 * @param  ascending input of true => ascending sort, false => descending sort
 */
//------------------------------------------------------------------------------
void Rvector::Sort(IntegerArray &indices, bool ascending)
{
   if (!isSizedD) throw ArrayTemplateExceptions::UnsizedArray();
   
   indices.resize(sizeD);
   std::list<Element> elist;
   elist.clear();
   
   // load list with element containing values from Rvector and
   // the corresponding indices
   Element e;
   for (int i = 0; i < sizeD; i++)
   {
      e.value = elementD[i];
      e.index = i;
      elist.push_back(e);
   }
   // sort uses operator< as defined for struct Element
   elist.sort();
   
   // elist is in ascending order, load vector and indices from appropriate end
   if (ascending)
      for (int i = 0; i < sizeD; i++)
      {
         e = elist.front();
         elementD[i] = e.value;
         indices[i] = e.index;
         elist.pop_front();
      }
   else // !ascending is descending
      for (int i = 0; i < sizeD; i++)
      {
         e = elist.back();
         elementD[i] = e.value;
         indices[i] = e.index;
         elist.pop_back();
      }
}

//------------------------------------------------------------------------------
//  Rvector operator*(const Rmatrix &m) const
//------------------------------------------------------------------------------
/**
 * @note Here vector is treated as a row matrix 1 x N.
 */
//------------------------------------------------------------------------------
Rvector Rvector::operator*(const Rmatrix &m) const
{
    int i,j;

    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }
    if (m.rowsD != sizeD)
    {
        throw ArrayTemplateExceptions::DimensionError();
    }

    Rvector v(m.colsD);
    for (i = 0; i < m.colsD; i++)
    {
        v(i) = 0;
        for (j = 0; j < m.rowsD; j++)  
        {
            v(i) = v(i) + m(j,i)*elementD[j];
        }
    }
    return v;
}

//------------------------------------------------------------------------------
//  const Rvector& operator*=(const Rmatrix &m)
//------------------------------------------------------------------------------
/**
 * @note Here vector is treated as a row matrix 1 x N.
 */
//------------------------------------------------------------------------------
const Rvector& Rvector::operator*=(const Rmatrix &m)
{
   //VC++ compiler gives this warning:
   //505 [Warning C4717] 'Rvector::operator*=' : recursive on all control paths,
   //function will cause runtime stack overflow
   //*this *= m;

   *this = *this * m;
   return *this;
}

//------------------------------------------------------------------------------
//  Rvector operator/(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rvector Rvector::operator/(const Rmatrix &m) const
{
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }

    Rmatrix invM(m);
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    return (*this) * invM;
}

//------------------------------------------------------------------------------
//  const Rvector& operator/=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rvector& Rvector::operator/=(const Rmatrix &m)
{
    if (isSizedD == false)
    {
        throw ArrayTemplateExceptions::UnsizedArray();
    }
    if (m.IsSized() == false)
    {
        throw TableTemplateExceptions::UnsizedTable();
    }

    Rmatrix invM(m);
    try
    {
        invM = m.Inverse();
    }
    catch (Rmatrix::IsSingular)
    {
        throw;
    }
    *this *= invM;
    return *this;
}


//------------------------------------------------------------------------------
// bool MakeZeroVector()
//------------------------------------------------------------------------------
bool Rvector::MakeZeroVector()
{
   for (Integer i=0; i<sizeD; i++)
      elementD[i] = 0.0;
   return true;
}

//------------------------------------------------------------------------------
// bool IsZeroVector() const
//------------------------------------------------------------------------------
bool Rvector::IsZeroVector() const
{
   if (GmatMathUtil::IsZero(GetMagnitude()))
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
//  Real Norm()
//------------------------------------------------------------------------------
/**
 * This method calls returns the square root of the sum of the squares.
 */
//------------------------------------------------------------------------------
Real Rvector::Norm()
{
   Real sum = 0;
       
   for (Integer i=0; i<sizeD; i++)
      sum += GmatMathUtil::Pow(elementD[i], 2);
      
   return GmatMathUtil::Sqrt(sum);
}


//------------------------------------------------------------------------------
// std::string ToString(const std::string &format, Iteger col) const
//------------------------------------------------------------------------------
/*
 * Formats Rvector value to String using c-style formatting.
 *
 * @param  format  c-style format, such as "%16.9f  "
 * @param  col  Number of columns
 *
 * @return Formatted Rvector value string
 */
//------------------------------------------------------------------------------
std::string Rvector::ToString(const std::string &format, Integer col) const
{
   char buffer[100];
   std::stringstream ss("");
   for (int i = 0; i < sizeD; i++)
   {
      sprintf(buffer, format.c_str(), elementD[i]);
      ss << buffer;
      if (i != 0 && (i%col) == 0)
         ss << std::endl;
   }
   
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(Integer precision, bool horizontal,
//                      const std::string &prefix) const
//------------------------------------------------------------------------------
/*
 * Formats Rvector value to String.
 *
 * @param  precision  Precision to be used in formatting
 * @param  horizontal  Format horizontally if true (true)
 * @param  prefix  Prefix to be used in vertical formatting ("")
 *
 * @return Formatted Rvector value string
 */
//------------------------------------------------------------------------------
std::string Rvector::ToString(Integer precision, bool horizontal,
                              const std::string &prefix) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, precision, 0, horizontal, 1, prefix, false);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(bool useCurrentFormat, bool scientific, bool showPoint,
//                      Integer precision, Integer width, bool horizontal,
//                      Integer spacing, const std::string &prefix,
//                      bool appendEol) const
//------------------------------------------------------------------------------
/*
 * Formats Rvector value to String.
 *
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal (true)
 * @param  scientific  Formats using scientific notation if true (false)
 * @param  showPoint  Shows decimal point (false)
 * @param  precision  Precision to be used in formatting (global default)
 * @param  width  Width to be used in formatting (global default)
 * @param  horizontal  Format horizontally if true (true)
 * @param  spacing  Spacing to be used in horozontal formatting (1)
 * @param  prefix  Prefix to be used in vertical formatting ("")
 * @param  appendEol  Appends eol if true (true)
 *
 * @return Formatted Rvector value
 */
//------------------------------------------------------------------------------
std::string Rvector::ToString(bool useCurrentFormat, bool scientific,
                              bool showPoint, Integer precision, Integer width,
                              bool horizontal, Integer spacing,
                              const std::string &prefix, bool appendEol) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   
   if (!useCurrentFormat)
      global->SetActualFormat(scientific, showPoint, precision, width, horizontal,
                              spacing, prefix, appendEol);
   // Set append EOL flag
   global->SetAppendEol(appendEol);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//---------------------------------
// friend function
//---------------------------------

//------------------------------------------------------------------------------
//  <friend>
//  Rvector operator*(Real s, const Rvector &v)
//------------------------------------------------------------------------------
Rvector operator*(Real s, const Rvector &v)
{
    int i;

    if (v.isSizedD == false)
        throw ArrayTemplateExceptions::UnsizedArray();

    Rvector returnRvector(v.sizeD);
    
    for (i = 0; i < v.sizeD; i++)
        returnRvector.elementD[i] = s * v.elementD[i];
    
    return returnRvector;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix Outerproduct(const Rvector &v1, const Rvector &v2)
//------------------------------------------------------------------------------
/**
 * @note Rmatrix multiplication, with first vector considered a Nx1
 *       matrix and a second vector considered a 1xM matrix.
 */
//------------------------------------------------------------------------------
Rmatrix Outerproduct(const Rvector &v1, const Rvector &v2)
{
    int i,j;

    if ((v1.isSizedD == false) || (v2.isSizedD == false))
        throw ArrayTemplateExceptions::UnsizedArray();

    Rmatrix prod(v1.sizeD, v2.sizeD);
    for (i = 0; i < v1.sizeD; i++)
        for (j = 0; j < v2.sizeD;j++)
            prod(i,j) = v1.elementD[i] * v2.elementD[j];
    
    return prod;
}


//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rvector &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rvector &a)
{
   return GmatRealUtil::operator>> (input, a);
}


//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rvector &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rvector &a)
{
   return GmatRealUtil::operator<< (output, a);
}


//------------------------------------------------------------------------------
//  bool operator<(const Element &e1, const Element &e2);
//------------------------------------------------------------------------------
/**
 *
 * provides ordering function for struct Element, which is used by STL list
 * sort function when sorting list of values and original indices
 *
 * @oaram  e1 left  hand Element in test
 * @param  e2 right hand Element in test
 * @return value is true if e1.value < e2.value
 */
//------------------------------------------------------------------------------
bool operator<(const Element &e1, const Element &e2)
{
   return e1.value < e2.value;
}
