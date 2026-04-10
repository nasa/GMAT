//$Id: ICRFFile.cpp 9513 2012-02-24 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                            ICRFFile.cpp
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
// Created: 2012/05/30
//
/**
 * Implements ICRFFile class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include "ICRFFile.hpp"
#include <stdio.h>
#include "LagrangeInterpolator.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const Integer ICRFFile::MAX_TABLE_SIZE = 128;

ICRFFile*     ICRFFile::instance       = NULL;

//------------------------------------------------------------------------------
//  public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ICRFFile* Instance()
//------------------------------------------------------------------------------
/**
 * Returns a pointer to the instance of the singleton.
 *
 * @return pointer to the instance
 */
//------------------------------------------------------------------------------
ICRFFile* ICRFFile::Instance()
{
   if (instance == NULL)
	  instance = new ICRFFile("ICRF_Table.txt",3);	// this file contains a table of Euler rotation vectors for time range from 1957 to 2100

   return instance;
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the instance by reading data from the file.
 */
//------------------------------------------------------------------------------
void ICRFFile::Initialize()
{
   if (isInitialized)
      return;

   // Allocate buffer to store ICRF rotation vector table:
   AllocateArrays();
   
   // Use FileManager::FindPath() for new file path implementation (LOJ: 2014.07.01)
   
	// Open IAU2000/2006 data file:
   // FileManager* thefile = FileManager::Instance();
   // std::string path = thefile->GetPathname(FileManager::ICRF_FILE);
   // std::string name = thefile->GetFilename(FileManager::ICRF_FILE);
   // icrfFileName = path+name;
   // FILE* fpt = fopen(icrfFileName.c_str(), "r");
   // if (fpt == NULL)
	//   throw GmatBaseException("Error: GMAT can't open '" + icrfFileName + "' file!!!\n");
   
   FileManager *fm = FileManager::Instance();
   icrfFileName = fm->GetFilename(FileManager::ICRF_FILE);
   icrfFileNameFullPath = fm->FindPath(icrfFileName, FileManager::ICRF_FILE, true, true, true);
   
   // Check full path file
   if (icrfFileNameFullPath == "")
		throw GmatBaseException("The ICRF file '" + icrfFileName + "' does not exist\n");
   
   FILE* fpt = fopen(icrfFileNameFullPath.c_str(), "r");
   if (fpt == NULL)
      throw GmatBaseException("Error: GMAT can't open '" + icrfFileName + "' file!!!\n");
   
   // Read ICRF Euler rotation vector from data file and store to buffer:
   Real t;
   Real rotationvector[3];
   int c;
   Integer i;
   for (i= 0; (c = fscanf(fpt, "%lf, %le, %le, %le\n",&t,
	      &rotationvector[0],&rotationvector[1],&rotationvector[2])) != EOF; ++i)
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
         dependences[i][j] = rotationvector[j];
   }

   pointsCount = i;
   isInitialized = true;
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/*
 * Finalizes the system by closing an opened file and deleting objects.
 */
//------------------------------------------------------------------------------
void ICRFFile::Finalize()
{
   CleanupArrays();
}

//------------------------------------------------------------------------------
// bool GetICRFRotationVector(Real ind, Real* icrfRotationVector, Integer dim,
//                            Integer order)
//------------------------------------------------------------------------------
/*
 * Get ICRF Euler rotation vector for a given epoch
 *
 * @param <ind>                  epoch at which Euler rotation vector needed
 * @param <icrfRotationVector>   the array containing the result of Euler
 *                               rotation vector
 * @param <dim>                  dimension of dependent vector
 * @param <order>                interpolation order
 */
//------------------------------------------------------------------------------
bool ICRFFile::GetICRFRotationVector(Real ind,    Real* icrfRotationVector,
                                     Integer dim, Integer order)
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
			throw GmatBaseException("The value of independent variable is out of range.\n");
		}

		if(order >= pointsCount)
		{
			throw GmatBaseException("Number of data points is not enough for interpolation.\n");
		}
	}

	// Specify beginning index and ending index in order to run interpolation:
	// The ICRF table has unequal step size. Therefore, we cannot use stepsize
	// to specify midpoint but binary search:
	Integer start = 0;
	Integer end = pointsCount-1;
	Integer midpoint = -999;
	while (start < end-1)
	{
		midpoint = (start + end)/2;
		if (ind > independence[midpoint])
			start = midpoint;
		else
			end = midpoint;
	}
   if (midpoint == -999)
		throw GmatBaseException("ICRFFile::GetICRFRotationVector - ERROR computing midpoint.\n");

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
	bool returnval = interpolator->Interpolate(ind, icrfRotationVector);
	delete interpolator;

	return returnval;
}

//------------------------------------------------------------------------------
// void ICRFFile::RotationMatrixFromICRFToFK5(const A1Mjd &atEpoch)
//------------------------------------------------------------------------------
/**
 * This method creates and stores a rotation matrix from ICRF to FK5 (MJ2000Eq) at
 * the requested epoch. This method only performs the rotation in that direction.
 * This assumes the same origin. 
 *
 * @param &atEpoch            time at which to create the matrix
 *
 */
 //------------------------------------------------------------------------------
void ICRFFile::RotationMatrixFromICRFToFK5(const Real theEpoch)
{
#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage(
      "Enter CoordinateConverter::RotationMatrixFromICRFToFK5 at epoch %18.12lf; \n\n", theEpoch);
#endif

   if ((fabs(theEpoch - lastIcrfToFk5Epoch) < 1.0e-10)) //Tolerance is roughly float precision
   {
      //Matrix is already calculated for this epoch, just return what we have
      return;
   }

   // Specify Euler rotation vector for theEpoch:
   Real vec[3];
   ICRFFile* icrfFile = ICRFFile::Instance();
   icrfFile->Initialize();
   icrfFile->GetICRFRotationVector(theEpoch, &vec[0], 3, 9);

   // Calculate rotation matrix based on Euler rotation vector:
   //Angle is equal to magnitude between ICRF and FK5 and direction of rotation axis
   Real angle = GmatMathUtil::Sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
   Real a[3];
   a[0] = vec[0] / angle; a[1] = vec[1] / angle; a[2] = vec[2] / angle;
   Real c = GmatMathUtil::Cos(angle);
   Real s = GmatMathUtil::Sin(angle);

   // rotation matrix from FK5 to ICRF:
   Rmatrix33 rotM;
   rotM.SetElement(0, 0, c + a[0] * a[0] * (1 - c));
   rotM.SetElement(0, 1, a[0] * a[1] * (1 - c) + a[2] * s);
   rotM.SetElement(0, 2, a[0] * a[2] * (1 - c) - a[1] * s);
   rotM.SetElement(1, 0, a[0] * a[1] * (1 - c) - a[2] * s);
   rotM.SetElement(1, 1, c + a[1] * a[1] * (1 - c));
   rotM.SetElement(1, 2, a[1] * a[2] * (1 - c) + a[0] * s);
   rotM.SetElement(2, 0, a[0] * a[2] * (1 - c) + a[1] * s);
   rotM.SetElement(2, 1, a[1] * a[2] * (1 - c) - a[0] * s);
   rotM.SetElement(2, 2, c + a[2] * a[2] * (1 - c));

   // rotation matrix from ICRF to FK5:
   icrfToFK5 = rotM.Transpose();

   //Set this epoch as the last calculated epoch
   lastIcrfToFk5Epoch = theEpoch;
#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage("theEpoch  = %18.12lf\n", theEpoch);

   MessageInterface::ShowMessage("rotation vector = %18.12e %18.12e %18.12e\n", vec[0], vec[1], vec[2]);
   MessageInterface::ShowMessage("R(0,0)=%18.12e,  R(0,1)=%18.12e,  R(0,2)=%18.12e\n", icrfToFK5(0, 0), icrfToFK5(0, 1), icrfToFK5(0, 2));
   MessageInterface::ShowMessage("R(1,0)=%18.12e,  R(1,1)=%18.12e,  R(1,2)=%18.12e\n", icrfToFK5(1, 0), icrfToFK5(1, 1), icrfToFK5(1, 2));
   MessageInterface::ShowMessage("R(2,0)=%18.12e,  R(2,1)=%18.12e,  R(2,2)=%18.12e\n", icrfToFK5(2, 0), icrfToFK5(2, 1), icrfToFK5(2, 2));

   MessageInterface::ShowMessage("Rdot(0,0)=%18.12e,  Rdot(0,1)=%18.12e,  Rdot(0,2)=%18.12e\n", icrfToFK5Dot(0, 0), icrfToFK5Dot(0, 1), icrfToFK5Dot(0, 2));
   MessageInterface::ShowMessage("Rdot(1,0)=%18.12e,  Rdot(1,1)=%18.12e,  Rdot(1,2)=%18.12e\n", icrfToFK5Dot(1, 0), icrfToFK5Dot(1, 1), icrfToFK5Dot(1, 2));
   MessageInterface::ShowMessage("Rdot(2,0)=%18.12e,  Rdot(2,1)=%18.12e,  Rdot(2,2)=%18.12e\n\n\n", icrfToFK5Dot(2, 0), icrfToFK5Dot(2, 1), icrfToFK5Dot(2, 2));
#endif

#ifdef DEBUG_ICRF_TOFK5
   MessageInterface::ShowMessage("NOW exiting CoordinateConverter::RotationMatrixFromICRFToFK5 ...\n\n");
#endif

}

//------------------------------------------------------------------------------
// void ICRFFile::RotateFromICRFtoFK5(Real* state, const A1Mjd& atEpoch)
//------------------------------------------------------------------------------
/**
 * This public method rotates a state vector from ICRF to FK5 (MJ2000Eq) at
 * the requested epoch.
 *
 * @param *instate        Real 6 posvel vector instate
 * @param *outstate       Real 6 posvel vector outstate
 * @param &atEpoch        time at which to create the matrix
 *
 */
 //------------------------------------------------------------------------------
void ICRFFile::RotateFromICRFtoFK5(const Real* inState, Real* outState, const Real theEpoch)
{
   //Calculate matrix
   RotationMatrixFromICRFToFK5(theEpoch);

   const Real* iToF = icrfToFK5.GetDataVector();

   outState[0] = iToF[0] * inState[0] + iToF[1] * inState[1] + iToF[2] * inState[2];
   outState[1] = iToF[3] * inState[0] + iToF[4] * inState[1] + iToF[5] * inState[2];
   outState[2] = iToF[6] * inState[0] + iToF[7] * inState[1] + iToF[8] * inState[2];

   outState[3] = iToF[0] * inState[3] + iToF[1] * inState[4] + iToF[2] * inState[5];
   outState[4] = iToF[3] * inState[3] + iToF[4] * inState[4] + iToF[5] * inState[5];
   outState[5] = iToF[6] * inState[3] + iToF[7] * inState[4] + iToF[8] * inState[5];
}

//------------------------------------------------------------------------------
// void ICRFFile::RotateFromFK5toICRF(Real* state, const A1Mjd& atEpoch)
//------------------------------------------------------------------------------
/**
 * This public method rotates a state vector from FK5 (MJ2000Eq) to ICRF at
 * the requested epoch.
 *
 * @param *instate        Real 6 posvel vector instate
 * @param *outstate       Real 6 posvel vector outstate
 * @param &atEpoch        time at which to create the matrix
 *
 */
 //------------------------------------------------------------------------------
void ICRFFile::RotateFromFK5toICRF(const Real* inState, Real* outState, const Real theEpoch)
{
   //Calculate matrix
   RotationMatrixFromICRFToFK5(theEpoch);

   const Real* fToI = icrfToFK5.GetDataVector();

   outState[0] = fToI[0] * inState[0] + fToI[3] * inState[1] + fToI[6] * inState[2];
   outState[1] = fToI[1] * inState[0] + fToI[4] * inState[1] + fToI[7] * inState[2];
   outState[2] = fToI[2] * inState[0] + fToI[5] * inState[1] + fToI[8] * inState[2];

   outState[3] = fToI[0] * inState[3] + fToI[3] * inState[4] + fToI[6] * inState[5];
   outState[4] = fToI[1] * inState[3] + fToI[4] * inState[4] + fToI[7] * inState[5];
   outState[5] = fToI[2] * inState[3] + fToI[5] * inState[4] + fToI[8] * inState[5];
}

//------------------------------------------------------------------------------
//  protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates ICRFFile buffers to contain ICRF Euler rotation vector read from file.
 */
//------------------------------------------------------------------------------
void ICRFFile::AllocateArrays()
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
void ICRFFile::CleanupArrays()
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
//  ICRFFile(const std::string &fileName = "ICRF_Table.txt",
//           const Integer dim = 3);
//------------------------------------------------------------------------------
/**
 * Constructs ICRFFile object (default constructor).
 *
 * @param <fileName>  Name of ICRF data file
 * @param <dim>       dimension of dependent vector
 */
//------------------------------------------------------------------------------
ICRFFile::ICRFFile(const std::string &fileName, Integer dim) :
   icrfFileName      (fileName),
   icrfFileNameFullPath (""),
   independence      (NULL),
   dependences       (NULL),
   dimension         (dim),
   tableSz           (MAX_TABLE_SIZE),
   pointsCount       (0),
   lastIcrfToFk5Epoch (0.0),
   isInitialized     (false)
{
}

//------------------------------------------------------------------------------
//  ~ICRFFile()
//------------------------------------------------------------------------------
/**
 * Destroys ICRFFile object (destructor).
 */
//------------------------------------------------------------------------------
ICRFFile::~ICRFFile()
{
   CleanupArrays();
}

