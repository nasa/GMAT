//------------------------------------------------------------------------------
//                             OCH TrajectoryData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Jeremy Knittel / NASA
// Created: 2017.02.22
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------
    
#ifndef OCHTrajectoryData_hpp
#define OCHTrajectoryData_hpp

#include "csaltdefs.hpp"
#include "OCHTrajectorySegment.hpp"
#include "LowThrustException.hpp"
#include "StringUtil.hpp"
#include "ArrayTrajectoryData.hpp"
#include "TrajectoryData.hpp"
#include <fstream>
#include <iostream>
#include <ctime>
#include <iomanip>

class CSALT_API OCHTrajectoryData : public TrajectoryData
{
public:
   /// default constructor
   OCHTrajectoryData(std::string fileName = "");
   /// copy constructor
   OCHTrajectoryData(const OCHTrajectoryData &copy);
   /// operator=
   OCHTrajectoryData& operator=(const OCHTrajectoryData &copy);
   
   /// destructor
   virtual ~OCHTrajectoryData();
   
  
   virtual void WriteToFile(std::string fileName);
   virtual void ReadFromFile(std::string fileName);

   virtual void SetNumSegments(Integer num);
   
   /// Access functions for meta data
   virtual void GetCentralBodyAndRefFrame(Integer forSegment,
                                          std::string &itsCentralBody,
                                          std::string &itsRefFrame);
   virtual void SetCentralBodyAndRefFrame(Integer forSegment,
                                          std::string newCentralBody,
                                          std::string newRefFrame);
   virtual void GetCentralBodyAndRefFrame(Real epoch,
                                          std::string &itsCentralBody,
                                          std::string &itsRefFrame);
   virtual void SetTimeSystem(Integer forSegment, std::string newTimeSys);

   /// Access functions for exit conditions
   virtual void SetFeasibility(Real feasTol);
   virtual Real GetFeasibility();
   virtual void SetOptimality(Real optTol);
   virtual Real GetOptimality();
   virtual void SetCostValue(Real costVal);
   virtual Real GetCostValue();
   virtual void SetMaxConstraintViolation(Real maxConViol);
   virtual Real GetMaxConstraintViolation();
   virtual void SetOptimizerExitStatus(std::string optExitStatus,
                                       Integer optExitCode);
   virtual std::string GetOptimizerExitStatus();
   virtual void SetMaxMeshRelativeError(Real maxRelativeMeshError);
   virtual Real GetMaxMeshRelativeError();
   virtual void SetSolutionType(std::string);
   virtual std::string GetSolutionType();

protected:

   virtual Real ProcessTimeString(std::string input, std::string timeSystem);

   /// Boolean determining whether or not the DATA section of the och file 
   /// was found
   bool dataFound;
   /// Boolean of whether or not extra data was detected in the och file that
   /// is ignored
   bool extraDataFound;

   /// Values for the exit conditions of the solution
   /// The feasibility of the trajectory
   Real feasibility;
   /// The optimality of the trajectory
   Real optimality;
   /// The value of the cost function
   Real cost;
   /// The max constraint violation
   Real maxConViolation;
   /// The exit status of the optimizer
   std::string exitStatus;
   /// The exit code from the optimizer
   Integer exitCode;
   /// The max relative error of each phase's mesh
   Real maxRelMeshError;
   /// The type of solution that has been saved to the trajectory data
   std::string solutionType;
};

#endif
