//$Id$
//------------------------------------------------------------------------------
//                               Interpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Interpolator base class
 */
//------------------------------------------------------------------------------


#include "Interpolator.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
//  Interpolator(const std::string &name, const std::string &typestr,
//               Integer dim)
//------------------------------------------------------------------------------
/**
 * Constructs the core elements of an Interpolator.
 * 
 * @param <name>    Name for this interpolator.
 * @param <typestr> Text string identifying the type of interpolator.
 * @param <dim>     Dimension of data that gets interpolated (defaults to 1).
 */
//------------------------------------------------------------------------------
Interpolator::Interpolator(const std::string &name, const std::string &typestr,
                           Integer dim) :
//   GmatBase           (Gmat::INTERPOLATOR, typestr, name),
   independent        (NULL),
   dependent          (NULL),
   previousX          (-9.9999e65),
   dimension          (dim),
   requiredPoints     (2),
   bufferSize         (2),
   pointCount         (0),
   latestPoint        (-1),
   rangeCalculated    (false),
   dataIncreases      (true),
   forceInterpolation (true),
   allowExtrapolation (false),
   instanceName       (name)
{
   range[0] = range[1] = 0.0;
}


//------------------------------------------------------------------------------
//  ~Interpolator()
//------------------------------------------------------------------------------
/**
 * Destroys the core elements of an Interpolator.
 */
//------------------------------------------------------------------------------
Interpolator::~Interpolator()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  Interpolator(const Interpolator &i)
//------------------------------------------------------------------------------
/**
 * Constructs the core elements of an Interpolator (Copy Constructor).
 * 
 * @param <i> The interpolator that is copied.
 */
//------------------------------------------------------------------------------
Interpolator::Interpolator(const Interpolator &i) :
//   GmatBase           (i),
   previousX          (i.previousX),
   dimension          (i.dimension),
   requiredPoints     (i.requiredPoints),
   bufferSize         (i.bufferSize),
   pointCount         (i.pointCount),
   latestPoint        (i.latestPoint),
   rangeCalculated    (i.rangeCalculated),
   dataIncreases      (i.dataIncreases),
   forceInterpolation (i.forceInterpolation),
   allowExtrapolation (i.allowExtrapolation),
   instanceName       (i.instanceName)
{
   if (i.independent)
      CopyArrays(i);
   else
   {
      independent = NULL;
      dependent = NULL;
   }

   if (rangeCalculated)
   {
      range[0] = i.range[0];
      range[1] = i.range[1];
   }
}


//------------------------------------------------------------------------------
//  Interpolator& operator=(const Interpolator &i)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <i> Interpolator that is used to set the values for this one.
 *
 * @return this instance, configured like the input instance.
 */
//------------------------------------------------------------------------------
Interpolator& Interpolator::operator=(const Interpolator &i)
{
   if (&i == this)
      return *this;
   
//   GmatBase::operator=(i);
   
   // Free any allocated memory
   if (independent)
      CleanupArrays();
   
   // Now set the member data to the new values
   dimension      = i.dimension;
   requiredPoints = i.requiredPoints;
   bufferSize     = i.bufferSize;
   pointCount     = i.pointCount;
   latestPoint    = i.latestPoint;
   
   // Copy over the memory from i
   if (i.independent)
      CopyArrays(i);
   
   rangeCalculated = i.rangeCalculated;
   if (rangeCalculated)
   {
      range[0] = i.range[0];
      range[1] = i.range[1];
   }
   
   dataIncreases      = i.dataIncreases;
   forceInterpolation = i.forceInterpolation;
   allowExtrapolation = i.allowExtrapolation;
   instanceName       = i.instanceName;
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool AddPoint(const Real ind, const Real *data)
//------------------------------------------------------------------------------
/**
 * Add a data point to the ring buffer used in the Interpolator.
 * 
 * This method is the core method used to fill the buffer prior to 
 * interpolation.  It manages a ring buffer of data points used by the 
 * interpolation routine (Interpolate(const Real, Real*)) to generate the 
 * estimated parameter values.
 * 
 * @param <ind>  Value of the independent (domain) variable.
 * @param <data> Array of dependent data associated with the independent value
 *               in the first variable.  This array must not be smaller than
 *               the dimension of the interpolator instance, or a memory access
 *               violation will occur.
 * 
 * @return  True if the data was added to the buffer, false if a problem was 
 *          encountered.
 *
 * @todo Handle memory access violations when the input array is too small.
 */
//------------------------------------------------------------------------------
bool Interpolator::AddPoint(const Real ind, const Real *data)
{
   Integer i;
   
   if (!independent)
      AllocateArrays();
   
   // Reset the index into the array if at the end
   if (latestPoint == bufferSize-1)
      latestPoint = -1;
   
   dataIncreases = (ind > previousX ? true : false);
   previousX = ind;
   
   independent[++latestPoint] = ind;
   for (i = 0; i < dimension; ++i)
      dependent[latestPoint][i] = data[i];
   
   ++pointCount;
   rangeCalculated = false;
   
//    MessageInterface::ShowMessage
//       ("===> Interpolator::AddPoint() ind=%f, data[0]=%f added, latestPoint=%d, "
//        "pointCount=%d\n", ind, data[0], latestPoint, pointCount);
   
   return true;
}


//------------------------------------------------------------------------------
//  Integer IsInterpolationFeasible(Real ind)
//------------------------------------------------------------------------------
/**
 * Checks if interpolation is feasible. Derived class should implement
 * this method if any checking is done.
 *
 * @param ind The value of the independent parameter.
 * @return  1 if feasible
 *         -1 if there is not enough data to interpolate
 *         -2 if requested data is not within the interpolation range
 */
//------------------------------------------------------------------------------
Integer Interpolator::IsInterpolationFeasible(Real ind)
{
   return 1;
}


//------------------------------------------------------------------------------
// void SetForceInterpolation(bool flag)
//------------------------------------------------------------------------------
/*
 * Sets force interpolation flag.
 */
//------------------------------------------------------------------------------
void Interpolator::SetForceInterpolation(bool flag)
{
   forceInterpolation = flag;
}


//------------------------------------------------------------------------------
// bool GetForceInterpolation();
//------------------------------------------------------------------------------
/*
 * Retrieves force interpolation flag.
 */
//------------------------------------------------------------------------------
bool Interpolator::GetForceInterpolation()
{
   return forceInterpolation;
}

//------------------------------------------------------------------------------
// void SetExtrapolation(bool flag)
//------------------------------------------------------------------------------
/*
 * Sets extrapolation flag.
 */
//------------------------------------------------------------------------------
void Interpolator::SetExtrapolation(bool flag)
{
   allowExtrapolation = flag;
}

//------------------------------------------------------------------------------
//  void Clear()
//------------------------------------------------------------------------------
/**
 * Resets the buffer for the interpolator.  
 * 
 * This method does not reallocate the buffer; it just resets the pointers and
 * data counts so that the interpolation can be restarted.
 */
//------------------------------------------------------------------------------
void Interpolator::Clear()
{
   latestPoint = -1;
   pointCount = 0;
   previousX = -9.9999e65;
}


//------------------------------------------------------------------------------
//  Integer GetBufferSize()
//------------------------------------------------------------------------------
/**
 * Access method for the bufferSize parameter.
 *
 * @return  The buffer size.
 */
//------------------------------------------------------------------------------
Integer Interpolator::GetBufferSize()
{
   return bufferSize;
}


//------------------------------------------------------------------------------
//  Integer GetPointCount()
//------------------------------------------------------------------------------
/**
 * Access method for the pointCount parameter.
 *
 * @return  The buffer size.
 */
//------------------------------------------------------------------------------
Integer Interpolator::GetPointCount()
{
   return pointCount;
}


//------------------------------------------------------------------------------
// std::string GetName()
//------------------------------------------------------------------------------
/**
 * Retrieves the name assigned to this interpolator
 *
 * @return The instance name
 */
//------------------------------------------------------------------------------
std::string Interpolator::GetName()
{
   return instanceName;
}

//------------------------------------------------------------------------------
// bool GetRange(Real &lower, Real &upper)
//------------------------------------------------------------------------------
/**
 * Retrieves the lower and upper range of the independent variable
 *
 * @param   lower  [output] lower end of independent variable range
 * @param   upper  [output] upper end of independent variable range
 *
 * @return true if range obtained; false otherwise
 */
//------------------------------------------------------------------------------
bool Interpolator::GetRange(Real &lower, Real &upper)
{
   if (!rangeCalculated)
      SetRange();
   
   lower = range[0];
   upper = range[1];
   return true;
}


//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates the data structures used by the ring buffer.  
 */
//------------------------------------------------------------------------------
void Interpolator::AllocateArrays()
{
   Integer i;

   CleanupArrays();
    
   independent = new Real[bufferSize];
   dependent = new Real*[bufferSize];
   for (i = 0; i < bufferSize; ++i)
      dependent[i] = new Real[dimension];
   latestPoint = -1;
}


//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the data arrays in the ring buffer, and resets the
 * counters used to manage the buffer.
 */
//------------------------------------------------------------------------------
void Interpolator::CleanupArrays()
{
   Integer i;
    
   if (independent)
      delete [] independent;
        
   if (dependent) {
      for (i = 0; i < bufferSize; ++i)
         delete [] dependent[i];
      delete [] dependent;
   }
    
   independent = NULL;
   dependent = NULL;
   latestPoint = -1;
   pointCount = 0;
}


//------------------------------------------------------------------------------
//  void CopyArrays(const Interpolator &i)
//------------------------------------------------------------------------------
/**
 * Copies the ring buffer from one Interpolator to this one.  
 * 
 * @param i The Interpolator that supplies the data copied to this one.
 */
//------------------------------------------------------------------------------
void Interpolator::CopyArrays(const Interpolator &i)
{
   Integer j;
    
   AllocateArrays();
   memcpy(independent, i.independent, bufferSize*sizeof(Real));
   for (j = 0; j < bufferSize; ++j)
      memcpy(dependent[j], i.dependent[j], dimension*sizeof(Real));
}


//------------------------------------------------------------------------------
//  void SetRange()
//------------------------------------------------------------------------------
/**
 * Finds the minimum and maximum values of the independent variable.
 */
//------------------------------------------------------------------------------
void Interpolator::SetRange()
{
   if (rangeCalculated)
      return;
   
   // Not enough points -- just continue
   if (requiredPoints > pointCount)
      return;
   
   range[0] = range[1] = independent[0];
   
   Integer i, max = (pointCount > bufferSize ? bufferSize : pointCount);
   
   for (i = 1; i < max; ++i) {
      if (independent[i] < range[0])
         range[0] = independent[i];
      if (independent[i] > range[1])
         range[1] = independent[i];
   }
   
   rangeCalculated = true;
}
