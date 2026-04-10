//$Id: IAUFile.cpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                            IAUFile.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Tuan Nguyen (NASA/GSFC)
// Created: 2012/02/24
//
/**
 * Implements IAUFile class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include "IAUFile.hpp"
#include "RealUtilities.hpp"
#include "FileManager.hpp"
#include "LagrangeInterpolator.hpp"
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer IAUFile::MAX_TABLE_SIZE = 128;

IAUFile*      IAUFile::instance       = NULL;


//------------------------------------------------------------------------------
//  public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// IAUFile* Instance()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the instance of the singleton.
 *
 * @return pointer to the instance
 */
//------------------------------------------------------------------------------
IAUFile* IAUFile::Instance()
{
   if (instance == NULL)
	  instance = new IAUFile("",3);

   return instance;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the instance by reading data from the file.
 */
//------------------------------------------------------------------------------
void IAUFile::Initialize()
{
	if (isInitialized)
		return;
   
	// Allocate buffer to store IAU2000/2006 data:
	AllocateArrays();
   
   // Use FileManager::FindPath() for new file path implementation (LOJ: 2014.07.01)
   
	// Open IAU2000/2006 data file:
   // FileManager* fm = FileManager::Instance();
   // std::string path = fm->GetPathname(FileManager::IAUSOFA_FILE);
   // std::string name = fm->GetFilename(FileManager::IAUSOFA_FILE);
   // iauFileName = path+name;
	// FILE* fpt = fopen(iauFileName.c_str(), "r");
   
   FileManager *fm = FileManager::Instance();
   iauFileName = fm->GetFilename(FileManager::IAUSOFA_FILE);
   iauFileNameFullPath = fm->FindPath(iauFileName, FileManager::IAUSOFA_FILE, true, true, true);
   
   // Check full path file
   if (iauFileNameFullPath == "")
		throw GmatBaseException("The IAU file '" + iauFileName + "' does not exist\n");
   
   FILE* fpt = fopen(iauFileNameFullPath.c_str(), "r");
   if (fpt == NULL)
      throw GmatBaseException("Error: GMAT can't open '" + iauFileName + "' file!!!\n");
   
	// Read IAU2000/2006 data from data file and store to buffer:
	Real t;
	Real XYs[3];
	int c;
	Integer i;
	for (i= 0; (c = fscanf(fpt, "%lf %lf %lf %lf\n",&t,&XYs[0],&XYs[1],&XYs[2])) != EOF; ++i)
	{
		// expend the buffer size when it has no room to contain data:
		if (i >= tableSz)
		{
			// create a new buffer with a larger size:
			Integer new_size = tableSz*2;
			Real* ind = new Real[new_size];
			Real** dep = new Real*[new_size];

			// copy contain in the current buffer to the new buffer:
			memcpy(ind, independence, tableSz*sizeof(Real));
			memcpy(dep, dependences, tableSz*sizeof(Real*));
			for (Integer k=tableSz; k < new_size; ++k)
				dep[k] = NULL;

			// delete the current buffer and use the new buffer as the current buffer:
			delete independence;
			delete dependences;
			independence = ind;
			dependences = dep;
			tableSz = new_size;
		}

		// store data to buffer:
		independence[i] = t;
		if (dependences[i] == NULL)
			dependences[i] = new Real[dimension];

		for (Integer j = 0; j < dimension; ++j)
			dependences[i][j] = XYs[j];
	}

	fclose(fpt);

	pointsCount = i;  // why "this->"?
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Finalizes the system by closing the opened file and deleting objects.
 */
//------------------------------------------------------------------------------
void IAUFile::Finalize()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
// bool GetIAUData(Real ind, Real* iau_data, Integer dim, Integer order)
//------------------------------------------------------------------------------
/*
 * Gets IAU2000 data for a given independence variable.
 *
 * @param ind          ???
 * @param iau_data     ???
 * @param dim          dimension of ???
 * @param order        ???
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool IAUFile::GetIAUData(Real ind, Real* iau_data, Integer dim, Integer order)
{
	// Verify the feasibility of interpolation:
	if ((independence == NULL)||(pointsCount == 0))
	{
		throw GmatBaseException("No data point is used for interpolation.\n");
	}
	else
	{
		if((ind < independence[0])||(ind > independence[pointsCount-1]))
		{
			throw GmatBaseException("The value of an independent variable is out of range.\n");
		}

		if(order >= pointsCount)
		{
			throw GmatBaseException("Number of data points is not enough for interpolation.\n");
		}
	}

	// Specify beginning index and ending index in order to run interpolation:
	Real stepsize = 1.0;
    Integer midpoint = (ind-independence[0])/(Integer)GmatMathUtil::NearestInt(stepsize);
	Integer beginIndex = (0 > (midpoint-order/2))? 0:(midpoint-order/2);
	Integer endIndex = ((pointsCount-1) < (beginIndex+order))? (pointsCount-1):(beginIndex+order);
	beginIndex = (0 > (endIndex-order))? 0:(endIndex-order);

	// Run interpolation:
	// create an interpolator:
	LagrangeInterpolator* interpolator = new LagrangeInterpolator("", dim, order);

	// add data points in order to run interpolator:
	for (Integer i= beginIndex; i <= endIndex; ++i)
	{
		interpolator->AddPoint(independence[i], dependences[i]);
	}

	// run interpolator and get the result of dependent variables:
	interpolator->SetForceInterpolation(true);
	bool returnval = interpolator->Interpolate(ind, iau_data);
	delete interpolator;

	return returnval;
}

//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates IAUFile buffers to contain IAU2000/2006 data read from file.
 */
//------------------------------------------------------------------------------
void IAUFile::AllocateArrays()
{
   independence = new Real[tableSz];

   dependences = new Real*[tableSz];
   Integer i;
   for (i = 0; i < tableSz; ++i)
   {
      dependences[i]  = new Real[dimension];
   }
}

//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the IAUFile buffer.
 */
//------------------------------------------------------------------------------
void IAUFile::CleanupArrays()
{
   if (independence != NULL)
   {
	  // clean up the array of independent variable
	  delete independence;
	  independence = NULL;

	  // clean up the array of dependent variables
	  Integer i= 0;
	  for (i=0; i <tableSz; ++i)
	  {
		  if (dependences[i] != NULL)
			  delete dependences[i];
	  }

     delete dependences;
	  dependences = NULL;
   }
}

//------------------------------------------------------------------------------
//  private methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  IAUFile(const std::string &fileName = "IAU_SOFA.DAT",
//          const Integer     dim       = 1);
//------------------------------------------------------------------------------
/**
 * Constructs IAUFile object (default constructor).
 *
 * @param <fileName>  Name of IAU2000/2006 data file
 * @param <dim>       dimension of dependent vector
 */
//------------------------------------------------------------------------------
IAUFile::IAUFile(const std::string &fileName, Integer dim) :
   iauFileName    (fileName),
   iauFileNameFullPath (""),
   independence   (NULL),
   dependences    (NULL),
   dimension      (dim),
   tableSz        (MAX_TABLE_SIZE),
   pointsCount    (0),
   isInitialized  (false)
{
}


//------------------------------------------------------------------------------
//  ~IAUFile()
//------------------------------------------------------------------------------
/**
 * Destroys IAUFile object (destructor).
 */
//------------------------------------------------------------------------------
IAUFile::~IAUFile()
{
   CleanupArrays();
}
