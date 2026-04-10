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
// Created: 2003/09/15 Linda Jun, NASA/GSFC
//
/**
 * Defines Matrix operations.
 */
//------------------------------------------------------------------------------
#include "TableTemplate.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "UtilityException.hpp"
#include "Linear.hpp"         // for operator<<, operator >>
#include "StringUtil.hpp"     // for Replace()
#include <stdarg.h>
#include <sstream>
#include <stdio.h>            // Fix for header rearrangement in gcc 4.4
#include "MessageInterface.hpp"
#include "LUFactorization.hpp"

//#define DEBUG_DETERMINANT
//#define DEBUG_MULTIPLY
//#define DEBUG_DIVIDE

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  Rmatrix()
//------------------------------------------------------------------------------
Rmatrix::Rmatrix()
   : TableTemplate<Real>()
{
}


//------------------------------------------------------------------------------
//  Rmatrix(int r, int c)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(int r, int c)
   : TableTemplate<Real>(r, c) 
{
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      elementD[i] = 0.0;

}


//------------------------------------------------------------------------------
//  Rmatrix(int r, int c, Real a1,...)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(int r, int c, Real a1,...)
   : TableTemplate<Real>(r, c) 
{
   va_list ap;
   int i;
   elementD[0] = a1;
   va_start(ap, a1);
    
   for (i = 1; i < rowsD*colsD; i++)
      elementD[i] = va_arg(ap, Real);

   va_end(ap);
}


//------------------------------------------------------------------------------
//  Rmatrix(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix::Rmatrix(const Rmatrix &m)
   : TableTemplate<Real>(m) 
{
}

// ekf mod 12/16
//------------------------------------------------------------------------------
//  Rmatrix::Identity(unsigned int size)
// Return an identity matrix of the passed in size
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Identity(unsigned int size)
{
   Rmatrix tmp(size, size);
   for (unsigned int i = 0; i < size; ++i)
   {
      tmp(i,i) = 1.0;
   }
   return tmp;
}

Rmatrix Rmatrix::Diagonal(unsigned int size, Rvector data)
{
   Rmatrix tmp(size, size);
   for (unsigned int i = 0; i < size; ++i)
   {
      tmp(i,i) = data(i);
   }
   return tmp;
}
// end ekf mod

//------------------------------------------------------------------------------
//  ~Rmatrix()
//------------------------------------------------------------------------------
Rmatrix::~Rmatrix() 
{
}


//------------------------------------------------------------------------------
//  virtual bool IsOrthogonal(Real accuracyRequired)= 
//                            GmatRealConstants::REAL_EPSILON) const
//------------------------------------------------------------------------------
bool Rmatrix::IsOrthogonal(Real accuracyRequired) const 
{
   bool orthogonal = true;  // assume it's orthogonal and try to prove it's not
   int i, j;

   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   // create an array of pointers to column vectors
   ArrayTemplate< Rvector* > columnVect(colsD);

   // initialize the array
   for (i = 0; i < colsD; i++)
   {
      columnVect[i] = new Rvector(rowsD);
   }

   // copy from matrix
   for (i = 0; i < colsD; i++)
   {
      for (j = 0; j < rowsD; j++)
      {
         (*columnVect(i))(j) = elementD[j*colsD + i];
      }
   }

   // are they mutually orthogonal
   for (i = 0; i < colsD && orthogonal; i++)
   {
      for (j = i + 1; j < colsD; j++)
      {
         if (!GmatMathUtil::IsZero((*columnVect(i))*(*columnVect(j)),accuracyRequired)) 
            orthogonal = false;
      }
   }

   for (i = 0; i < colsD; i++)
   {
      delete columnVect[i];
   }

   return orthogonal;
}


//------------------------------------------------------------------------------
//  virtual bool IsOrthonormal(Real accuracyRequired) = 
//                             GmatRealConstants::REAL_EPSILON)const
//------------------------------------------------------------------------------
bool Rmatrix::IsOrthonormal (Real accuracyRequired) const 
{
   bool normal = true;  // assume it's normal, try to prove it's not
   int i, j;

   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }
    
   // create an array of pointers to column vectors
   ArrayTemplate< Rvector* > columnVect(colsD);

   // initialize the array
   for (i = 0; i < colsD; i++)
   {
      columnVect[i] = new Rvector(rowsD);
   }

   // copy from matrix
   for (i = 0; i < colsD; i++)
   {
      for (j = 0; j < rowsD; j++)
      {
         (*columnVect(i))(j) = elementD[j*colsD + i];
      }
   }

   // see if each magnitude of each columnVect is equal to one
   for (i = 0; i < colsD && normal; i++)
   {
      if (!GmatMathUtil::IsZero(columnVect(i)->GetMagnitude() - 1, accuracyRequired))
         normal = false;
   }

   for (i = 0; i < colsD; i++)
   {
      delete columnVect[i];
   }
   return ((bool) (normal && IsOrthogonal(accuracyRequired)));
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator=(const Rmatrix &m) 
{
   TableTemplate<Real>::operator=(m);
   return *this;
}


//------------------------------------------------------------------------------
//  bool operator==(const Rmatrix &m)const
//------------------------------------------------------------------------------
bool Rmatrix::operator==(const Rmatrix &m)const
{
   int ii,jj;

   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if ( this != &m)
   {
      if ((rowsD == m.rowsD) && (colsD == m.colsD))
      {
         for (ii=0; ii<rowsD; ii++)
         {
            for (jj=0; jj<colsD; jj++)
            {
               //loj: 5/5/06 used epsilon
               //if (elementD[ii*colsD+jj] != m(ii,jj))
               if (GmatMathUtil::Abs(elementD[ii*colsD+jj] - m(ii,jj)) >
                   GmatRealConstants::REAL_TOL)
               {
                  return false;
               }
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
//  bool operator!=(const Rmatrix &m)const
//------------------------------------------------------------------------------
bool Rmatrix::operator!=(const Rmatrix &m)const
{
   return (bool) (!( *this== m));
}


//------------------------------------------------------------------------------
//  Rmatrix operator+(const Rmatrix &m)const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator+(const Rmatrix &m) const 
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   // Added handling of 1x1 - MxN or MxN - 1x1 (LOJ: 2010.10.29)
   bool oneByOnePlusMatrix = false;
   bool matrixPlusOneByOne = false;
   
   if (rowsD != m.rowsD || colsD != m.colsD)
   {
      if (rowsD == 1 && colsD == 1)
         oneByOnePlusMatrix = true;
      else if (m.rowsD == 1 && m.colsD == 1)
         matrixPlusOneByOne = true;
      else
         throw TableTemplateExceptions::DimensionError();
   }
   
   Rmatrix sum(rowsD, colsD);
   
   if (oneByOnePlusMatrix)
   {
      sum.SetSize(m.rowsD, m.colsD);
      Real oneByOne = GetElement(0, 0);
      for (int i = 0; i < m.rowsD*m.colsD; i++)
         sum.elementD[i] = oneByOne + m.elementD[i];
   }
   else if (matrixPlusOneByOne)
   {
      Real oneByOne = m.GetElement(0, 0);
      for (int i = 0; i < rowsD*colsD; i++)
         sum.elementD[i] = elementD[i] + oneByOne;
   }
   else
   {
      for (int i = 0; i < rowsD*colsD; i++)
         sum.elementD[i] = elementD[i] + m.elementD[i];
   }
   
   return sum;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator+=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator+=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   *this = *this + m;
   
   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator-(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator-(const Rmatrix &m) const 
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   // Added handling of 1x1 - MxN or MxN - 1x1 (LOJ: 2010.10.29)
   bool oneByOneMinusMatrix = false;
   bool matrixMinusOneByOne = false;
   
   if (rowsD != m.rowsD || colsD != m.colsD)
   {
      if (rowsD == 1 && colsD == 1)
         oneByOneMinusMatrix = true;
      else if (m.rowsD == 1 && m.colsD == 1)
         matrixMinusOneByOne = true;
      else
         throw TableTemplateExceptions::DimensionError();
   }
   
   Rmatrix diff(rowsD, colsD);
   
   if (oneByOneMinusMatrix)
   {
      diff.SetSize(m.rowsD, m.colsD);
      Real oneByOne = GetElement(0, 0);
      for (int i = 0; i < m.rowsD*m.colsD; i++)
         diff.elementD[i] = oneByOne - m.elementD[i];
   }
   else if (matrixMinusOneByOne)
   {
      Real oneByOne = m.GetElement(0, 0);
      for (int i = 0; i < rowsD*colsD; i++)
         diff.elementD[i] = elementD[i] - oneByOne;
   }
   else
   {
      for (int i = 0; i < rowsD*colsD; i++)
         diff.elementD[i] = elementD[i] - m.elementD[i];
   }
   
   return diff;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator-=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator-=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   *this = *this - m;
   
   return *this;
}


//------------------------------------------------------------------------------
//  const Rmatrix operator*(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator*(const Rmatrix &m) const 
{
   #ifdef DEBUG_MULTIPLY
   MessageInterface::ShowMessage
      ("Rmatrix::operator*() entered this=%s, m=%s\n", this->ToString().c_str(),
       m.ToString().c_str());
   #endif
   
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   // Added handling of 1x1 * MxN or MxN * 1x1 (LOJ: 2010.10.29)
   bool oneByOneTimesMatrix = false;
   bool matrixTimesOneByOne = false;
   
   #ifdef DEBUG_MULTIPLY
   MessageInterface::ShowMessage
      ("   rowsD=%d, colsD=%d, m.rowsD=%d, m.colsD=%d\n", rowsD, colsD, m.rowsD, m.colsD);
   #endif
   
   if (colsD != m.rowsD)
   {
      if (rowsD == 1 && colsD == 1)
         oneByOneTimesMatrix = true;
      else if (m.rowsD == 1 && m.colsD == 1)
         matrixTimesOneByOne = true;
      else
         throw TableTemplateExceptions::DimensionError();
   }
   
   #ifdef DEBUG_MULTIPLY
   MessageInterface::ShowMessage
      ("   oneByOneTimesMatrix=%d, matrixTimesOneByOne=%d\n",
       oneByOneTimesMatrix, matrixTimesOneByOne);
   #endif
   
   if (oneByOneTimesMatrix)
   {
      Rmatrix prod(m.rowsD, m.colsD);  // declare a zero matrix
      Real oneByOne = GetElement(0, 0);
      
      for (int i = 0; i < m.rowsD; i++)
         for (int j = 0; j < m.colsD; j++)
            prod(i, j) = m.GetElement(i, j) * oneByOne;
      
      #ifdef DEBUG_MULTIPLY
      MessageInterface::ShowMessage
         ("Rmatrix::operator*() returning OneByOne*Matrix %s\n", prod.ToString().c_str());
      #endif
      return prod;
   }
   else if ( matrixTimesOneByOne)
   {
      Rmatrix prod(rowsD, colsD);  // declare a zero matrix
      Real oneByOne = m.GetElement(0, 0);
      
      for (int i = 0; i < rowsD; i++)
         for (int j = 0; j < colsD; j++)
            prod(i, j) = GetElement(i, j) * oneByOne;
      
      #ifdef DEBUG_MULTIPLY
      MessageInterface::ShowMessage
         ("Rmatrix::operator*() returning Matrix*OneByOne %s\n", prod.ToString().c_str());
      #endif
      return prod;
   }
   else
   {
      Rmatrix prod(rowsD, m.colsD);  // declare a zero matrix
      
      for (int i = 0; i < rowsD; i++)
         for (int j = 0; j < m.colsD; j++)
            for (int k = 0; k < colsD; k++)
               prod(i, j) += elementD[i*colsD + k] * m(k, j);
      
      #ifdef DEBUG_MULTIPLY
      MessageInterface::ShowMessage
         ("Rmatrix::operator*() returning %s\n", prod.ToString().c_str());
      #endif
      return prod;
   }
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator*=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator*=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   *this = *this * m;  
   return *this;
}


//------------------------------------------------------------------------------
//  const Rmatrix operator/(const Rmatrix &m) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator/( const Rmatrix &m) const
{ 
   #ifdef DEBUG_DIVIDE
   MessageInterface::ShowMessage
      ("Rmatrix::operator/() entered this=%s, m=%s\n", this->ToString().c_str(),
       m.ToString().c_str());
   #endif
   
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   
   bool oneByOneDivideMatrix = false;
   bool matrixDivideOneByOne = false;
   
   #ifdef DEBUG_DIVIDE
   MessageInterface::ShowMessage
      ("   rowsD=%d, colsD=%d, m.rowsD=%d, m.colsD=%d\n", rowsD, colsD, m.rowsD, m.colsD);
   #endif
   
   if (rowsD == 1 && colsD == 1)
      oneByOneDivideMatrix = true;
   else if (m.rowsD == 1 && m.colsD == 1)
      matrixDivideOneByOne = true;
   
   #ifdef DEBUG_DIVIDE
   MessageInterface::ShowMessage
      ("   oneByOneDivideMatrix=%d, matrixDivideOneByOne=%d\n",
       oneByOneDivideMatrix, matrixDivideOneByOne);
   #endif
   
   if (oneByOneDivideMatrix)
   {
      Rmatrix div(m.rowsD, m.colsD);
      Real oneByOne = GetElement(0, 0);
      
      for (int i = 0; i < m.rowsD; i++)
         for (int j = 0; j < m.colsD; j++)
            div(i, j) = oneByOne / m.GetElement(i, j);
      
      #ifdef DEBUG_DIVIDE
      MessageInterface::ShowMessage
         ("Rmatrix::operator/() returning OneByOne/Matrix %s\n", div.ToString().c_str());
      #endif
      return div;
   }
   else if (matrixDivideOneByOne)
   {
      Rmatrix div(rowsD, colsD);
      Real oneByOne = m.GetElement(0, 0);
      
      for (int i = 0; i < rowsD; i++)
         for (int j = 0; j < colsD; j++)
            div(i, j) = GetElement(i, j) / oneByOne;
      
      #ifdef DEBUG_DIVIDE
      MessageInterface::ShowMessage
         ("Rmatrix::operator/() returning Matrix/OneByOne %s\n", div.ToString().c_str());
      #endif
      return div;
   }
   else
      return (*this)*m.Inverse();
}


//------------------------------------------------------------------------------
//  const Rmatrix & operator/=(const Rmatrix &m)
//------------------------------------------------------------------------------
const Rmatrix& 
Rmatrix::operator/=(const Rmatrix &m) 
{
   if ((isSizedD == false) || (m.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   return (*this) *= m.Inverse();
}

//------------------------------------------------------------------------------
//  Rmatrix ElementWiseMultiply(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix Rmatrix::ElementWiseMultiply(const Rmatrix &m)
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   Integer mr, mc;
   m.GetSize(mr, mc);
   if ((mr != rowsD) || (mc != colsD))
      throw TableTemplateExceptions::DimensionError();
   
   Rmatrix result(*this);
   for (int i = 0; i < rowsD*colsD; i++)
      result.elementD[i] *= m.elementD[i];
   return result;
}

//------------------------------------------------------------------------------
//  Rmatrix ElementWiseDivide(const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix Rmatrix::ElementWiseDivide(const Rmatrix &m)
{
   if ((isSizedD == false) || (m.IsSized() == false))
      throw TableTemplateExceptions::UnsizedTable();
   Integer mr, mc;
   m.GetSize(mr, mc);
   if ((mr != rowsD) || (mc != colsD))
      throw TableTemplateExceptions::DimensionError();
   
   Rmatrix result(*this);
   for (int i = 0; i < rowsD*colsD; i++)
      result.elementD[i] /= m.elementD[i];
   return result;
}

//------------------------------------------------------------------------------
//  Rmatrix operator+(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator+(Real scalar) const 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   Rmatrix result(rowsD, colsD);
   
   for (int i = 0; i < rowsD*colsD; i++)
      result.elementD[i] = elementD[i] + scalar;
   
   return result;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator+=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator+=(Real scalar) 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i] + scalar;
   
   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator-(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator-(Real scalar) const 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   Rmatrix result(rowsD, colsD);
   
   for (int i = 0; i < rowsD*colsD; i++)
      result.elementD[i] = elementD[i] - scalar;
   
   return result;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator-=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator-=(Real scalar) 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   for (int i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i] - scalar;
   
   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator*(Real scalar) const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator*(Real scalar) const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix prod(rowsD, colsD);
    
   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      prod.elementD[i] = elementD[i]*scalar;
   }
   return prod;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator*=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator*=(Real scalar) 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      elementD[i] = elementD[i]*scalar;
   }
   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator/(Real scalar)const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator/(Real scalar) const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix quot(rowsD, colsD);
   
   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      quot.elementD[i] = elementD[i]/scalar;

   return quot;
}


//------------------------------------------------------------------------------
//  const Rmatrix& operator/=(Real scalar)
//------------------------------------------------------------------------------
const Rmatrix& Rmatrix::operator/=(Real scalar) 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();

   if (GmatMathUtil::IsZero(scalar))
      throw Rmatrix::DivideByZero();
   
   int i;
   for (i = 0; i < rowsD*colsD; i++)
      elementD[i] = elementD[i]/scalar;

   return *this;
}


//------------------------------------------------------------------------------
//  Rmatrix operator-() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::operator-() const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix neg(rowsD, colsD);
    
   int i;
   for (i = 0; i < rowsD*colsD; i++)
   {
      neg.elementD[i] = -elementD[i];
   }
   return neg;
}


//------------------------------------------------------------------------------
//  Rvector operator*(const Rvector &v) const
//------------------------------------------------------------------------------
Rvector Rmatrix::operator*(const Rvector &v) const 
{
   if (isSizedD == false)
      throw TableTemplateExceptions::UnsizedTable();

   if (v.IsSized() == false)
      throw ArrayTemplateExceptions::UnsizedArray();

   if (colsD != v.sizeD)
      throw TableTemplateExceptions::DimensionError();
   
   Rvector prod(rowsD);

   int i, j;
   Real var;
   for (i = 0; i < rowsD; i++)
   { 
      var = 0.0;
      for (j = 0; j < colsD; j++)
      {
         prod.elementD[i] = elementD[i*colsD + j]*v.elementD[j];
         var = var + prod.elementD[i];
      }
      prod.elementD[i] = var;
   }
   
   return prod;
}


//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix operator+(Real scalar, const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix operator+(Real scalar, const Rmatrix &m) 
{
   if (m.IsSized() == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   Rmatrix result(m);
   
   for (int i = 0; i < m.rowsD*m.colsD; i++)
      result.elementD[i] += scalar;
   
   return result;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix operator-(Real scalar, const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix operator-(Real scalar, const Rmatrix &m) 
{
   if (m.IsSized() == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   Rmatrix result(m);
   
   for (int i = 0; i < m.rowsD*m.colsD; i++)
      result.elementD[i] = scalar - result.elementD[i];
   
   return result;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix operator*(Real scalar, const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix operator*(Real scalar, const Rmatrix &m) 
{
   if (m.IsSized() == false)
      throw TableTemplateExceptions::UnsizedTable();

   Rmatrix prod(m);
    
   int i;
   for (i = 0; i < m.rowsD*m.colsD; i++)
      prod.elementD[i] *= scalar;

   return prod;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix operator/(Real scalar, const Rmatrix &m)
//------------------------------------------------------------------------------
Rmatrix operator/(Real scalar, const Rmatrix &m) 
{
   if (m.IsSized() == false)
      throw TableTemplateExceptions::UnsizedTable();
   
   Rmatrix div(m);
   
   for (int i = 0; i < m.rowsD*m.colsD; i++)
      div.elementD[i] = scalar / m.elementD[i];
   
   return div;
}


//------------------------------------------------------------------------------
//  virtual real Trace() const
//------------------------------------------------------------------------------
Real Rmatrix::Trace() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   Real sum = 0;

   int i;
   for (i = 0; i < rowsD; i++)
   {
      sum += elementD[i*colsD + i];
   }

   return sum;
}


//------------------------------------------------------------------------------
//  virtual Real Determinant() const
//------------------------------------------------------------------------------
Real Rmatrix::Determinant() const
{
   #ifdef DEBUG_DETERMINANT
      MessageInterface::ShowMessage("Entering Determinant with rowsD = %d and colsD = %d\n", rowsD, colsD);
   #endif
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   Real D;

   if (rowsD == 1)
   {
      #ifdef DEBUG_DETERMINANT
         MessageInterface::ShowMessage("Entering Determinant rowsD == 1 clause\n");
      #endif
      D = elementD[0];
   }
   else if (rowsD == 2)
   {
      #ifdef DEBUG_DETERMINANT
         MessageInterface::ShowMessage("Entering Determinant rowsD == 2 clause\n");
      #endif
      D = elementD[0]*elementD[3] - elementD[1]*elementD[2];
   }
   else if (rowsD == 3)
   {
      #ifdef DEBUG_DETERMINANT
         MessageInterface::ShowMessage("Entering Determinant rowsD == 3 clause\n");
      #endif
      D = elementD[0]*elementD[4]*elementD[8] +
         elementD[1]*elementD[5]*elementD[6] +
         elementD[2]*elementD[3]*elementD[7] -
         elementD[0]*elementD[5]*elementD[7] -
         elementD[1]*elementD[3]*elementD[8] -
         elementD[2]*elementD[4]*elementD[6];
   }
   else
   {
      // Currently limited by inefficiencies in the algorithm
      if (rowsD > 9)
      {
         LUFactorization lu;
         D = lu.Determinant(*this);

         // std::string errmsg = "GMAT Determinant method not yet optimized.  ";
         // errmsg += "Currently limited to matrices of size 9x9 or smaller.";
         // throw UtilityException(errmsg);
      }
      #ifdef DEBUG_DETERMINANT
         MessageInterface::ShowMessage("Entering Determinant else clause\n");
      #endif
      D = 0.0;
      int i;
      for (i = 0; i < colsD; i++)
      {
         Real c = Cofactor(0,i);
         #ifdef DEBUG_DETERMINANT
            MessageInterface::ShowMessage("Cofactor(0,%d) = %12.10f\n", (Integer) i, c);
            MessageInterface::ShowMessage("   now multiplying by element[%d] (%12.10f) to get %12.10f\n",
                  (Integer) i, elementD[i], (elementD[i] * c));
         #endif
         D += elementD[i] * c;
//         D += elementD[i]*Cofactor(0,i);
      }
      #ifdef DEBUG_DETERMINANT
         MessageInterface::ShowMessage("... at end of summation, D = %12.10f\n", D);
      #endif
   }
   
   return D;
}


//------------------------------------------------------------------------------
//  virtual Real Cofactor(int r, int c) const
//------------------------------------------------------------------------------
Real Rmatrix::Cofactor(int r, int c) const
{
   #ifdef DEBUG_DETERMINANT
      MessageInterface::ShowMessage("Entering Cofactor with r     = %d and c     = %d\n", r, c);
      MessageInterface::ShowMessage("                   and rowsD = %d and colsD = %d\n", rowsD, colsD);
   #endif
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();
   if (rowsD > 9)
   {
      std::string errmsg = "GMAT Cofactor method not yet optimized.  ";
      errmsg += "Currently limited to matrices of size 9x9 or smaller.";
      throw UtilityException(errmsg);
   }
   Rmatrix Minor(rowsD - 1, colsD - 1);
   Real Cof;
  
   // build the minor matrix
   int i, j, minorI, minorJ;
   for (i = 0, minorI = -1; i < rowsD; i++) 
   {
      if (i != r) 
      {
         minorI++;
         for ( j = 0, minorJ = -1; j < colsD; j++) 
         { 
            if (j != c) 
            {
               minorJ++;
               Minor(minorI, minorJ) = elementD[i*colsD + j]; 
            } // if (j != c) 
         } // for (j = ...
      } // if (i != r)
   } // for (i = ...
   #ifdef DEBUG_DETERMINANT
      MessageInterface::ShowMessage("about to call Determinant on minor: \n%s\n", (Minor.ToString()).c_str());
   #endif

   Cof = Minor.Determinant();

   // if r+c is odd Cof is negative
   if ((r+c)%2 == 1)
      Cof = - Cof;
   return Cof;
}


//------------------------------------------------------------------------------
//  Rmatrix Transpose() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Transpose() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   Rmatrix tran(colsD, rowsD);
    
   int i, j;
   for (i = 0; i < rowsD; i++)
   {
      for (j = 0; j < colsD; j++)
      {
         tran(j, i) = elementD[i*colsD + j]; 
      }
   }

   return tran;
}


//------------------------------------------------------------------------------
//  Rmatrix Inverse() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Inverse(Real zeroValue) const  //ekf mod 12/16 added Real zeroValue
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();

   Rmatrix A = *this;

   // Verify information matrix is a diagonal matrix
   bool isDiagonal = true;
   for (Integer i = 0; i < A.GetNumRows(); ++i)
   {
      for (Integer j = i; j < A.GetNumColumns(); ++j)
      {
         if ((i != j) && (A.GetElement(i, j) != 0.0))
         {
            isDiagonal = false;
            break;
         }
      }
      if (!isDiagonal)
         break;
   }

   if (isDiagonal)
   {
      // Verify all diagonal elements not zero
      for (Integer i = 0; i < A.GetNumRows(); ++i)
         if (A.GetElement(i, i) == 0.0)
            throw Rmatrix::IsSingular();

      // Take inverse all elements on diagonal
      Real val;
      for (Integer i = 0; i < this->GetNumRows(); ++i)
      {
         val = 1.0 / A.GetElement(i, i);
         A.SetElement(i, i, val);
      }

      return A;
   }


   //int dummy_marker = 4;
   int IndexRange = rowsD;
   
   ArrayTemplate<bool> PivotAllowed(IndexRange);
   ArrayTemplate<int> PivotRowList(IndexRange), PivotColumnList(IndexRange);
   Real PivotElement;
   int PivotRow = -1, PivotColumn = -1;
   Real tmp;
   int k;
   for (k = 0; k < IndexRange; k++)
   {
      PivotAllowed(k) = true;
   }
   // Outer Loop
   int i, n, j;
   for (n = 0; n < IndexRange; n++) 
   {
      PivotElement = GmatRealConstants::REAL_EPSILON;

      // find pivot element
      for (i = 0; i < IndexRange; i++) 
      {
         if (PivotAllowed(i))          // row I not yet used to pivot
         {
            for (j = 0; j < IndexRange; j++) 
            {
               if (PivotAllowed(j))  // column J not yet used to pivot
               {
                  if (GmatMathUtil::Abs(PivotElement) < GmatMathUtil::Abs(A(i,j))) 
                  {
                     PivotRow = i;
                     PivotColumn = j;
                     PivotElement = A(i,j);
                  }
               }
            }
         } 
      }
        
      if (GmatMathUtil::IsZero(PivotElement,zeroValue)){   // ekf mod 12/16
         throw Rmatrix::IsSingular(); }

      PivotRowList(n) = PivotRow;
      PivotColumnList(n) = PivotColumn;
      PivotAllowed(PivotColumn) = false;

      A(PivotRow, PivotColumn) = 1.0;

    
      // normalize and exchange rows
      for (j = 0; j < IndexRange; j++) 
      {
         tmp = A(PivotRow, j);
         A(PivotRow, j) = A(PivotColumn, j);
         A(PivotColumn, j) = tmp / PivotElement;
      }

      // Perform elimination
      for (i = 0; i < IndexRange; i++) 
      {
         if (i != PivotColumn)         // except for pivoted row.
         {
            tmp = A(i, PivotColumn);
            A(i, PivotColumn) = 0.0;
            for (j = 0; j < IndexRange; j++) 
            {
               A(i, j) = A(i, j) - A(PivotColumn, j)*tmp;
            }
         } // if (i != PivotColumn)
      } // for (i = 0...
   } // for (n = 0...

   // Reorder matrix by exchanging columns
   for (n = (IndexRange - 1); n > -1; n--) 
   {
      // exchange two columns
      for (i = 0; i < IndexRange; i++) 
      {
         tmp = A(i, PivotRowList(n));
         A(i, PivotRowList(n)) = A(i, PivotColumnList(n));
         A(i, PivotColumnList(n)) = tmp;
      }
   }
   return A;
}


//------------------------------------------------------------------------------
//  Rmatrix Inverse() const      ekf mod 12/16
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Inverse() const
{
   return Inverse(0.000000000001);
}


//----------------------------------------------------------------------------- 
//  virtual Rmatrix Pseudoinverse() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Pseudoinverse(Real zeroValue) const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   LUFactorization lu;

   Rmatrix InverseM(colsD, rowsD), TransposeM(colsD, rowsD);
   Rmatrix m1(rowsD, rowsD), m2(colsD, colsD);
   Real accuracyRequired = 0.005;
   if (rowsD < colsD) 
   {
      m1 = MatrixTimesTranspose(*this, *this);
      if (!GmatMathUtil::IsZero(lu.Determinant(m1),accuracyRequired))
         InverseM = Transpose()*m1.Inverse(zeroValue);
      else
         InverseM = Transpose()*m1.Inverse(zeroValue);
         //throw Rmatrix::IsSingular();
   } 
   else if (rowsD > colsD) 
   {
      m2 = TransposeTimesMatrix(*this, *this);
      if (!GmatMathUtil::IsZero(lu.Determinant(m2), accuracyRequired))
         InverseM = m2.Inverse(zeroValue)*Transpose();
      else 
         InverseM = m2.Inverse(zeroValue)*Transpose();
         //throw Rmatrix::IsSingular();
   }
   else
      InverseM = Inverse(zeroValue);
   return InverseM;
}


//------------------------------------------------------------------------------
//  Rmatrix Symmetric() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::Symmetric() const 
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();

   return (*this + Transpose())/2;
}


//------------------------------------------------------------------------------
//  Rmatrix AntiSymmetric() const
//------------------------------------------------------------------------------
Rmatrix Rmatrix::AntiSymmetric() const
{
   if (isSizedD == false)
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (rowsD != colsD)
      throw Rmatrix::NotSquare();

   return (*this - Transpose())/2;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix SkewSymmetric4x4(const Rvector3 &v)
//----------------------------------------------------------------------------- 
Rmatrix SkewSymmetric4by4(const Rvector3 &v)
{
   Rmatrix skew(4,4);
   skew(0,0) = 0.0;
   skew(0,1) = v[2];
   skew(0,2) = -v[1];
   skew(0,3) = v[0];

   skew(1,0) = -v[2];
   skew(1,1) = -0.0;
   skew(1,2) = v[0];
   skew(1,3) = v[1];

   skew(2,0) = v[1];
   skew(2,1) = -v[0];
   skew(2,2) = 0.0;
   skew(2,3) = v[2];

   skew(3,0) = -v[0];
   skew(3,1) = -v[1];
   skew(3,2) = -v[2];
   skew(3,3) = 0.0;
   return skew;
}


//------------------------------------------------------------------------------
//  <friend>
//  Rmatrix TransposeTimesMatrix(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix TransposeTimesMatrix(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.colsD, m2.colsD);

   int i, j, k;
   for (i = 0; i < m1.colsD; i++)
   {
      for (j = 0; j < m2.colsD; j++)
      {
         for (k = 0; k < m1.rowsD; k++)
         {
            m(i, j) += m1(k, i)*m2(k, j);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.rowsD, m2.rowsD);

   int i, j, k;
   for (i = 0; i < m1.rowsD; i++)
   {
      for (j = 0; j < m2.rowsD; j++)
      {
         for (k = 0; k < m1.colsD; k++)
         {
            m(i, j) += m1(i, k)*m2(j, k);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
//  <friend>
//  TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2)
//------------------------------------------------------------------------------
Rmatrix TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2) 
{
   if ((m1.IsSized() == false) || (m2.IsSized() == false))
   {
      throw TableTemplateExceptions::UnsizedTable();
   }

   if (m1.rowsD != m2.rowsD)
      throw TableTemplateExceptions::DimensionError();
    
   Rmatrix m(m1.colsD, m2.rowsD);

   int i, j, k;
   for (i = 0; i < m1.colsD; i++)
   {
      for (j = 0; j < m2.rowsD; j++)
      {
         for (k = 0; k < m1.rowsD; k++)
         {
            m(i, j) += m1(k, i)*m2(j, k);
         }
      }
   }

   return m;
}


//------------------------------------------------------------------------------
// friend std::istream& operator>> (std::istream &input, Rmatrix &a)
//------------------------------------------------------------------------------
std::istream& operator>> (std::istream &input, Rmatrix &a)
{
   return GmatRealUtil::operator>> (input, a);
}


//------------------------------------------------------------------------------
// friend std::ostream& operator<< (std::ostream &output, const Rmatrix &a)
//------------------------------------------------------------------------------
std::ostream& operator<< (std::ostream &output, const Rmatrix &a)
{
   return GmatRealUtil::operator<< (output, a);
}

//------------------------------------------------------------------------------
// Rvector GetRow(int r) const
//------------------------------------------------------------------------------
Rvector Rmatrix::GetRow(int r) const
{
   Rvector rvec(colsD);
   
   for (int i=0; i<colsD; i++)
      rvec.SetElement(i, GetElement(r, i));
   
   return rvec;
}


//------------------------------------------------------------------------------
// Rvector GetColumn(int c) const
//------------------------------------------------------------------------------
Rvector Rmatrix::GetColumn(int c) const
{
   Rvector rvec(rowsD);

   for (int i=0; i<rowsD; i++)
      rvec.SetElement(i, GetElement(i, c));

   return rvec;
}

//------------------------------------------------------------------------------
// Rvector GetRowOrColumn() const
//------------------------------------------------------------------------------
/**
 * Returns row or column vector if matrix is one dimensional array (1xN or Mx1 matrix)
 */
//------------------------------------------------------------------------------
Rvector Rmatrix::GetRowOrColumn() const
{
   Rvector rvec;
   if (GetNumRows() == 1 || GetNumColumns() == 1)
   {
      if (GetNumRows() == 1)
         rvec = GetRow(0);
      else
         rvec = GetColumn(0);
   }
   else
      throw UtilityException
         ("Rmatrix::GetRowOrColumn() The matrix has more than one row and column");
   
   return rvec;
}

//------------------------------------------------------------------------------
// void MakeOneRowMatrix(const Rvector &vec)
//------------------------------------------------------------------------------
/*
 * Makes one row matrix by resizing the matrix to the size of input
 * vector and copying the value
 *
 */
//------------------------------------------------------------------------------
void Rmatrix::MakeOneRowMatrix(const Rvector &vec)
{
   // Reset size to match input vector
   Integer vecSize = vec.GetSize();
   SetSize(1, vecSize);
   for (Integer j = 0; j < vecSize; j++)
      SetElement(0, j, vec(j));
}


//------------------------------------------------------------------------------
// void MakeOneColumnMatrix(const Rvector &vec)
//------------------------------------------------------------------------------
/*
 * Makes one column matrix by resizing the matrix to the size of input
 * vector and copying the value
 *
 */
//------------------------------------------------------------------------------
void Rmatrix::MakeOneColumnMatrix(const Rvector &vec)
{
   // Reset size to match input vector
   Integer vecSize = vec.GetSize();
   SetSize(vecSize, 1);
   for (Integer i = 0; i < vecSize; i++)
      SetElement(i, 0, vec(i));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringVals(Integer p = GmatGlobal::DATA_PRECISION,
//                                  Integer w = GmatGlobal::DATA_WIDTH)
//------------------------------------------------------------------------------
const StringArray& Rmatrix::GetStringVals(Integer p, Integer w)
{
   stringVals.clear();
   std::stringstream ss("");
   
   ss.setf(std::ios::right);
   
   for (int i=0; i<rowsD*colsD; i++)
   {
      ss.str("");
      ss << std::setw(w) << std::setprecision(p) << elementD[i];
      stringVals.push_back(ss.str());
   }

   return stringVals;
}


//------------------------------------------------------------------------------
// std::string ToString(Integer precision, Integer width, bool horizontal,
//                      const std::string &prefix, bool appendEol) const
//------------------------------------------------------------------------------
/*
 * Formats Rmatrix value to String.
 *
 * @param  precision   Precision to be used in formatting
 * @param  width       Width to be used in formatting (1)
 * @param  horizontal  Format horizontally if true (false)
 * @param  prefix      Prefix to be used in vertical formatting ("")
 * @param  appendEol   Appends eol if true (true)
 *
 * @return Formatted Rmatrix value string
 */
//------------------------------------------------------------------------------
std::string Rmatrix::ToString(Integer precision, Integer width, bool horizontal,
                              const std::string &prefix, bool appendEol) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, precision, width, horizontal, 1, prefix,
                           appendEol);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToString(bool useCurrentFormat, bool scientific,
//                      bool showPoint, Integer precision, Integer width,
//                      bool horizontal, Integer spacing,
//                      const std::string &prefix, bool appendEol) const
//------------------------------------------------------------------------------
/*
 * Formats Rmatrix value to String.
 *
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal (true)
 * @param  scientific  Formats using scientific notation if true (false)
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  width  Width to be used in formatting (GmatGlobal::DATA_WIDTH)
 * @param  horizontal  Format horizontally if true (false)
 * @param  spacing  Spacing to be used in formatting (1)
 * @param  appendEol  Appends eol if true (true)
 *
 * @return Formatted Rmatrix value string
 */
//------------------------------------------------------------------------------
std::string Rmatrix::ToString(bool useCurrentFormat, bool scientific,
                              bool showPoint, Integer precision, Integer width,
                              bool horizontal, Integer spacing,
                              const std::string &prefix, bool appendEol) const
{
   GmatGlobal *global = GmatGlobal::Instance();
   
   if (!useCurrentFormat)
      global->SetActualFormat(scientific, showPoint, precision, width, horizontal,
                              spacing, prefix, appendEol);
   
   std::stringstream ss("");
   ss << *this;
   return ss.str();
}


//------------------------------------------------------------------------------
// std::string ToRowString(Integer row, Integer precision, Integer width,
//                         bool showPoint)
//------------------------------------------------------------------------------
/*
 * Formats Rmatrix row value to String.
 *
 * @param  row         Row values to format
 * @param  precision   Precision to be used in formatting
 * @param  width       Width to be used in formatting (1)
 * @param  showPoint   True if showing point (false)
 *
 * @return Formatted Rmatrix value string
 */
//------------------------------------------------------------------------------
std::string Rmatrix::ToRowString(Integer row, Integer precision, Integer width,
                                 bool showPoint) const
{
   #ifdef DEBUG_TO_ROW_STRING
   MessageInterface::ShowMessage
      ("Rmatrix::ToRowString() row=%d, prec=%d, width=%d, showPoint=%d\n",
       row, precision, width, showPoint);
   #endif
   
   // Use c-style formatting (LOJ: This works better with alignment)
   //-----------------------------------------------------------------
   #if 1
   //-----------------------------------------------------------------
   
   Integer w = width;
   char format[50], buffer[200];
   
   if (showPoint)
      sprintf(format, "%s%d.%de", "%", w, precision);
   else
      sprintf(format, "%s%d.%dg", "%", w, precision);
   
   Rvector rowVec = GetRow(row);
   Integer size = rowVec.GetSize();
   std::stringstream ss("");
   
   for (int i=0; i<size; i++)
   {
      sprintf(buffer, format, rowVec[i]);
      
      // How do I specify 2 digints of the exponent? (LOJ: 2010.05.03)
      // Manually remove extra 0 in the exponent of scientific notation.
      // ex) 1.23456e-015 to 1.23456e-15
      //ss << buffer;
      
      std::string sval = buffer;
      if ((sval.find("e-0") != sval.npos) && (sval.size() - sval.find("e-0")) == 5)
         sval = GmatStringUtil::Replace(sval, "e-0", "e-");
      if ((sval.find("e+0") != sval.npos) && (sval.size() - sval.find("e+0")) == 5)
         sval = GmatStringUtil::Replace(sval, "e+0", "e+");
      
      ss << sval;
      ss << " ";
   }

   return ss.str();
   
   //-----------------------------------------------------------------
   #else
   //-----------------------------------------------------------------
   
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, showPoint, precision, width, true, 1, "", false);
   Rvector rowVec = GetRow(row);
   std::stringstream ss("");
   ss << rowVec << " ";
   
   return ss.str();
   
   //-----------------------------------------------------------------
   #endif
   //-----------------------------------------------------------------
}


