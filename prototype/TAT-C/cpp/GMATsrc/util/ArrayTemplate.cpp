//$Id$
//------------------------------------------------------------------------------
//                            ArrayTemplate
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
// Created: 1995/10/10 for GSS project
// Modified: 2003/09/15 Linda Jun - See ArrayTemplate.hpp
//
/**
 * Ccontains the declarations for the ArrayTemplate array container class
 * (see Notes below for assumptions about parameter T)
 */
//
//------------------------------------------------------------------------------
//#include <stdarg.h>  // for variable length argument list
#include "ArrayTemplate.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  <constructor>
//  ArrayTemplate()
//------------------------------------------------------------------------------
template <class T>
ArrayTemplate<T>::ArrayTemplate()
  :
  elementD((T*) 0), sizeD(0), isSizedD(false)
{
}

//------------------------------------------------------------------------------
//  <constructor>
//  ArrayTemplate(int sizeOfArray)
//------------------------------------------------------------------------------
template <class T>
ArrayTemplate<T>::ArrayTemplate(int sizeOfArray)
{
   if (sizeOfArray < 0)
   {
       throw ArrayTemplateExceptions::IllegalSize();
   }
   init(sizeOfArray);
}

//------------------------------------------------------------------------------
//  <constructor>
//  ArrayTemplate(int sizeOfArray, const T& a1, ...)
//
//  Notes: THIS CONSTUCTOR IS CURRENTLY NOT ALLOWED BY THE HP COMPILER
//------------------------------------------------------------------------------
//template <class T>
//ArrayTemplate<T>::ArrayTemplate(int sizeOfArray, const T& a1, ...);
//{
//   int i;
//
//   va_list ap;             // argument list
//    
//   if (sizeOfArray < 0)
//   {
//       throw ArrayTemplateExceptions::IllegalSize();
//   }
//
//   init(sizeOfArray);      // alloc memory and set size
//
//   va_start(ap, a1);       // make ap point to first argument in list
//   elementD[0] = a1;       // first element is array
//
//                           // remaining elements are in arg list
//   for (i = 1; i < sizeD; i++)
//   {
//      elementD[i] = va_arg(ap, const T*); // This may be a problem if
//                                          // T is a class type
//   }
//}

//------------------------------------------------------------------------------
//  <constructor>
//  ArrayTemplate(int sizeOfArray, const T* array)
//------------------------------------------------------------------------------
template <class T>
ArrayTemplate<T>::ArrayTemplate(int sizeOfArray, const T* array)
{
   int i;
   if (sizeOfArray < 0)
   {
       throw ArrayTemplateExceptions::IllegalSize();
   }
   init(sizeOfArray);
   for (i = 0; i < sizeD; i++)
   {
      elementD[i] = array[i];
   }
}

//------------------------------------------------------------------------------
//  <copy constructor>
//  ArrayTemplate(const ArrayTemplate<T> &array)
//------------------------------------------------------------------------------
template <class T>
ArrayTemplate<T>::ArrayTemplate(const ArrayTemplate<T> &array)
{
   int i;
   if (array.IsSized() == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   init(array.sizeD);
   for (i = 0; i < sizeD; i++)
   {
      elementD[i] = array.elementD[i];
   }
}

//------------------------------------------------------------------------------
//  <destructor>
//  ~ArrayTemplate()
//------------------------------------------------------------------------------
template <class T>
ArrayTemplate<T>::~ArrayTemplate()
{
   delete [] elementD;
}

//------------------------------------------------------------------------------
//  const ArrayTemplate<T>& operator=(const ArrayTemplate<T>& array)
//------------------------------------------------------------------------------
template <class T>
const ArrayTemplate<T>& ArrayTemplate<T>::operator=(
                        const ArrayTemplate<T>& array)
{
   if (array.IsSized() == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   int i;
   if (isSizedD == false)
   {
       init(array.sizeD);
   }
   else if (sizeD != array.sizeD)
   {
      throw ArrayTemplateExceptions::DimensionError();
   }

   if (*this != array)
   {
      for (i = 0; i < sizeD; i++)
      {
         elementD[i] = array.elementD[i];
      }
   } 
   return *this;
}

//------------------------------------------------------------------------------
//  bool operator==(const ArrayTemplate<T> &array) const
//------------------------------------------------------------------------------
template <class T>
bool
ArrayTemplate<T>::operator==(const ArrayTemplate<T> &array) const
{
   if ((isSizedD == false) || (array.IsSized() == false))
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   int i;
   if(this!= &array)
   {
      if (sizeD == array.sizeD)
      {
         for(i = 0; i<sizeD; i++)
         {
            if(elementD[i] != array[i])
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
//  bool operator!=(const ArrayTemplate<T> &array) const
//------------------------------------------------------------------------------
template <class T>
bool
ArrayTemplate<T>::operator!=(const ArrayTemplate<T> &array) const
{
   return (bool)!(*this == array);
}

//------------------------------------------------------------------------------
//  T& operator()(int index)
//------------------------------------------------------------------------------
template <class T>
T&
ArrayTemplate<T>::operator()(int index)
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }

   if (index < 0 || index >= sizeD)
   {
      throw ArrayTemplateExceptions::OutOfBounds();
   }
   return elementD[index];
}

//------------------------------------------------------------------------------
//  const T& operator() (int index) const
//------------------------------------------------------------------------------
template <class T>
const T&
ArrayTemplate<T>::operator() (int index) const
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }

   if (index < 0 || index >= sizeD)
   {
      throw ArrayTemplateExceptions::OutOfBounds();
   }
   return elementD[index];
}

//------------------------------------------------------------------------------
//  T& operator[] (int index)
//------------------------------------------------------------------------------
template <class T>
T&
ArrayTemplate<T>::operator[] (int index)
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   return (*this)(index);
}

//------------------------------------------------------------------------------
//  const T& operator[] (int index) const
//------------------------------------------------------------------------------
template <class T>
const T&
ArrayTemplate<T>::operator[] (int index) const
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   return (*this)(index);
}

//------------------------------------------------------------------------------
//  bool IsSized() const
//------------------------------------------------------------------------------
template <class T>
bool
ArrayTemplate<T>::IsSized() const
{
   return isSizedD;
}

//------------------------------------------------------------------------------
//  void SetSize(int size)
//------------------------------------------------------------------------------
template <class T>
void
ArrayTemplate<T>::SetSize(int size)
{
   if (isSizedD == true)
   {
       //throw ArrayTemplateExceptions::ArrayAlreadySized();
      // wcs - 2005.02.01 - need to be able to resize
      delete [] elementD;
   }

   if (size < 0)
   {
       throw ArrayTemplateExceptions::IllegalSize();
   }
   init(size);   
}

// ekf mod 12/16
//------------------------------------------------------------------------------
//  void Resize(Integer size)
//------------------------------------------------------------------------------
template <class T>
void ArrayTemplate<T>::Resize(Integer size)
{
   if (size < 0)
   {
       throw ArrayTemplateExceptions::IllegalSize();
   }
   // if already sized, then resize
   if (isSizedD)
   {
      // copy from existing storage into temp storage
	  Integer originalSize = sizeD;
      T* tmp = new T[size];
      T* myElementD = &elementD[0];
      for (Integer i = 0; i < originalSize; ++i)
      {
         tmp[i] = myElementD[i];
      }
	  
	  // Reset the size (deletes original data)
      SetSize(size);

      // copy from temp storage into new storage
      for (Integer i = 0; i < originalSize; ++i)
      {
         elementD[i] = tmp[i];
      }

      // delete temp storage
      delete [] tmp;

      // all done
      return;
   }
   // else size as if new
   SetSize(size);
}
// end ekf mod

//------------------------------------------------------------------------------
//  int GetSize() const
//------------------------------------------------------------------------------
template <class T>
int
ArrayTemplate<T>::GetSize() const
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }
   return sizeD;
}

//------------------------------------------------------------------------------
//  T GetElement(int index) const
//------------------------------------------------------------------------------
template <class T>
T
ArrayTemplate<T>::GetElement(int index) const
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }

   if (index >= sizeD || index < 0)
   {
      throw ArrayTemplateExceptions::OutOfBounds();
   }
   return elementD[index];
}

//------------------------------------------------------------------------------
//  void SetElement(int index, const T& value)
//------------------------------------------------------------------------------
template <class T>
void
ArrayTemplate<T>::SetElement(int index, const T& value)
{
   if (isSizedD == false)
   {
       throw ArrayTemplateExceptions::UnsizedArray();
   }

   if (index >= sizeD || index < 0)
   {
      throw ArrayTemplateExceptions::OutOfBounds();
   }
   elementD[index] = value;
}

//---------------------------------
//  protected
//---------------------------------

//------------------------------------------------------------------------------
//  void init(int s)
//------------------------------------------------------------------------------
template <class T>
void
ArrayTemplate<T>::init(int s)
{
   sizeD = s;
   if (sizeD == 0)
   {
       elementD = (T *) 0;
   }
   else
   {
       elementD = new T[sizeD];
   }
   isSizedD = true;
}
