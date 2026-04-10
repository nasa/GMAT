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
// Created: 2017.02.23
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include "OCHTrajectoryData.hpp"

//#define DEBUG_READ_OCH
//#define DEBUG_WRITE_OCH

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
OCHTrajectoryData::OCHTrajectoryData(std::string fileName) :
   TrajectoryData(),
   dataFound(false),
   extraDataFound(false),
   feasibility(0.0),
   optimality(0.0),
   cost(std::numeric_limits<Real>::infinity()),
   maxConViolation(std::numeric_limits<Real>::infinity()),
   exitStatus("N/A"),
   exitCode(0),
   solutionType("N/A")
{
   if (fileName != "")
      ReadFromFile(fileName);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
OCHTrajectoryData::OCHTrajectoryData(const OCHTrajectoryData &copy) :
   TrajectoryData(copy),
   dataFound(copy.dataFound),
   extraDataFound(copy.extraDataFound),
   feasibility(copy.feasibility),
   optimality(copy.optimality),
   cost(copy.cost),
   maxConViolation(copy.maxConViolation),
   exitStatus(copy.exitStatus),
   exitCode(copy.exitCode),
   solutionType(copy.solutionType)
{
}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
OCHTrajectoryData& OCHTrajectoryData::operator=(const OCHTrajectoryData &copy)
{
   
   TrajectoryData::operator=(copy);
   if (&copy == this)
      return *this;      
         
   dataFound = copy.dataFound;
   extraDataFound = copy.extraDataFound;
   feasibility = copy.feasibility;
   optimality = copy.optimality;
   cost = copy.cost;
   maxConViolation = copy.maxConViolation;
   exitStatus = copy.exitStatus;
   exitCode = copy.exitCode;
   solutionType = copy.solutionType;
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
OCHTrajectoryData::~OCHTrajectoryData()
{

}

//------------------------------------------------------------------------------
// void SetNumSegments(Integer num)
//------------------------------------------------------------------------------
/**
* This method sets the number of segments in the och data
*
* @param <num> the number of segments
*
* @note We need to override the base definition here since we need the
*       pointers to specifically point to OCHTrajectoryData objects
*
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetNumSegments(Integer num)
{
   bool isLarger = num > numSegments;
   if (isLarger)
   {
      for (Integer ii = 0; ii < (num - numSegments); ii++)
      {
         segments_.push_back(new OCHTrajectorySegment());
         hasSegmentHadDuplicates.push_back(false);
      }
   }
   else
   {
      segments_.resize(num);
      hasSegmentHadDuplicates.resize(num);
   }
   numSegments = num;

}

//------------------------------------------------------------------------------
// void GetCentralBodyAndRefFrame(Integer forSegment,
//                                std::string &itsCentralBody,
//                                std::string &itsRefFrame)
//------------------------------------------------------------------------------
void OCHTrajectoryData::GetCentralBodyAndRefFrame(Integer forSegment,
                                                  std::string &itsCentralBody,
                                                  std::string &itsRefFrame)
{
   if ((forSegment < 0) || (forSegment >= numSegments))
   {
      std::string errmsg = "ERROR - OCHTrajectoryData: segment number ";
      errmsg            += "out of range";
      throw LowThrustException(errmsg);
   }
   itsCentralBody = ((OCHTrajectorySegment*)segments_.at(forSegment))
                    ->GetCentralBody();
   itsRefFrame    = ((OCHTrajectorySegment*)segments_.at(forSegment))
                    ->GetRefFrame();
}

//------------------------------------------------------------------------------
// void SetCentralBodyAndRefFrame(Integer forSegment,
//                                std::string &newCentralBody,
//                                std::string &newRefFrame)
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetCentralBodyAndRefFrame(Integer forSegment,
                                                  std::string newCentralBody,
                                                  std::string newRefFrame)
{
   if ((forSegment < 0) || (forSegment >= numSegments))
   {
      std::string errmsg = "ERROR - OCHTrajectoryData: segment number ";
      errmsg += "out of range";
      throw LowThrustException(errmsg);
   }
   ((OCHTrajectorySegment*)segments_.at(forSegment))
      ->SetCentralBody(newCentralBody);
   ((OCHTrajectorySegment*)segments_.at(forSegment))
      ->SetRefFrame(newRefFrame);
}

//------------------------------------------------------------------------------
// void GetCentralBodyAndRefFrame(Real epoch,
//                                std::string &itsCentralBody,
//                                std::string &itsRefFrame)
//------------------------------------------------------------------------------
void OCHTrajectoryData::GetCentralBodyAndRefFrame(Real epoch,
                                                  std::string &itsCentralBody,
                                                  std::string &itsRefFrame)
{
   Integer relevantSegment = GetRelevantSegment(epoch);
   GetCentralBodyAndRefFrame(relevantSegment, itsCentralBody, itsRefFrame);
}

//------------------------------------------------------------------------------
// void SetTimeSystem(Integer forSegment,
//                    std::string &newTimeSystem)
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetTimeSystem(Integer forSegment,
                                      std::string newTimeSystem)
{
   if ((forSegment < 0) || (forSegment >= numSegments))
   {
      std::string errmsg = "ERROR - OCHTrajectoryData: segment number ";
      errmsg += "out of range";
      throw LowThrustException(errmsg);
   }
   ((OCHTrajectorySegment*)segments_.at(forSegment))
      ->SetTimeSystem(newTimeSystem);
}

//------------------------------------------------------------------------------
// void SetFeasibility(Real feas)
//------------------------------------------------------------------------------
/**
* Set the feasibility of the trajectory data
*
* @param feas The feasibility
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetFeasibility(Real feas)
{
   feasibility = feas;
}

//------------------------------------------------------------------------------
// Real GetFeasibility()
//------------------------------------------------------------------------------
/**
* Get the feasibility of the trajectory data
*
* @return The feasibility
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::GetFeasibility()
{
   return feasibility;
}

//------------------------------------------------------------------------------
// void SetOptimality(Real opt)
//------------------------------------------------------------------------------
/**
* Set the optimality of the trajectory data
*
* @param opt The optimality
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetOptimality(Real opt)
{
   optimality = opt;
}

//------------------------------------------------------------------------------
// Real GetOptimality()
//------------------------------------------------------------------------------
/**
* Get the optimality of the trajectory data
*
* @return The optimality
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::GetOptimality()
{
   return optimality;
}

//------------------------------------------------------------------------------
// void SetCostValue(Real costVal)
//------------------------------------------------------------------------------
/**
* Set the value of the cost function of the trajectory data
*
* @param costVal The cost value
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetCostValue(Real costVal)
{
   cost = costVal;
}

//------------------------------------------------------------------------------
// Real GetCostValue()
//------------------------------------------------------------------------------
/**
* Set the value of the cost function of the trajectory data
*
* @return The cost value
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::GetCostValue()
{
   return cost;
}

//------------------------------------------------------------------------------
// void SetMaxConstraintViolation(Real maxConViol)
//------------------------------------------------------------------------------
/**
* Set the max constraint violation of the trajectory
*
* @param maxConViol The max constraint violation
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetMaxConstraintViolation(Real maxConViol)
{
   maxConViolation = maxConViol;
}

//------------------------------------------------------------------------------
// Real GetMaxConstraintViolation()
//------------------------------------------------------------------------------
/**
* Get the max constraint violation of the trajectory
*
* @return The max constraint violation
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::GetMaxConstraintViolation()
{
   return maxConViolation;
}

//------------------------------------------------------------------------------
// void SetOptimizerExitStatus(std::string optExitStatus)
//------------------------------------------------------------------------------
/**
* Set the optimizer exit status that was achieved for the trajectory
*
* @param optExitStatus The exit status
* @param optExitCode The exit code returned from the optimizer
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetOptimizerExitStatus(std::string optExitStatus, 
                                               Integer optExitCode)
{
   exitStatus = optExitStatus;
   exitCode = optExitCode;
}

//------------------------------------------------------------------------------
// std::string GetOptimizerExitStatus()
//------------------------------------------------------------------------------
/**
* Get the optimizer exit status that was achieved for the trajectory
*
* @return The exit status
*/
//------------------------------------------------------------------------------
std::string OCHTrajectoryData::GetOptimizerExitStatus()
{
   return exitStatus;
}

//------------------------------------------------------------------------------
// void SetMaxMeshRelativeErrors(Real maxRelativeMeshErrors)
//------------------------------------------------------------------------------
/**
* Set the max relative mesh error from the trajectory.
*
* @param maxRelMeshErrors The max realative error
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetMaxMeshRelativeError(Real maxRelativeMeshError)
{
   maxRelMeshError = maxRelativeMeshError;
}

//------------------------------------------------------------------------------
// Real GetMaxMeshRelativeErrors()
//------------------------------------------------------------------------------
/**
* Get the max relative mesh error from the trajectory.
*
* @return The max error of each segment
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::GetMaxMeshRelativeError()
{
   return maxRelMeshError;
}

//------------------------------------------------------------------------------
// void SetSolutionType(std::string solType)
//------------------------------------------------------------------------------
/**
* Set the solution type that is being stored.
*
* @param <solType> The solution type
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::SetSolutionType(std::string solType)
{
   solutionType = solType;
}

//------------------------------------------------------------------------------
// std::string SetSolutionType()
//------------------------------------------------------------------------------
/**
* Get the solution type that is being stored.
*
* @return The solution type
*/
//------------------------------------------------------------------------------
std::string OCHTrajectoryData::GetSolutionType()
{
   return solutionType;
}

//------------------------------------------------------------------------------
// Real ProcessTimeString(std::string input, std::string timeSystem)
//------------------------------------------------------------------------------
/**
* This method processes a time string based on the time system specified
*
* @param <input>      the time value input
* @param <timeSystem> the time system format of the input
* @return the time value converted to A1ModJulian
*
*/
//------------------------------------------------------------------------------
Real OCHTrajectoryData::ProcessTimeString(std::string input,
                                          std::string timeSystem)
{
   // TODO: process time string and output Real time value.
   // For now just convert it to a string

   Real value;
   bool success = GmatStringUtil::ToReal(input,&value);
   if (success)
   {
      std::string system, format;
      theTimeConverter->
         GetTimeSystemAndFormat(timeSystem, system,
            format);
      Integer origTimeTypeID =
         theTimeConverter->GetTimeTypeID(system);
      std::string a1ModJulainName = "A1";
      Integer a1ModJulianTypeID =
         theTimeConverter->GetTimeTypeID(a1ModJulainName);
      value = theTimeConverter->Convert(value, origTimeTypeID,
         a1ModJulianTypeID);
      return value;
   }
   else
   {
      std::string errmsg = "ERROR - OCHTrajectoryData: error converting ";
      errmsg            += "time string to Real";
      throw LowThrustException(errmsg);
   }
}

//------------------------------------------------------------------------------
// void WriteToFile(std::string fileName)
//------------------------------------------------------------------------------
/**
* This method writes all OCH data to a text file
*
* @param <fileName> the file to write to
*
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::WriteToFile(std::string fileName)
{
   #ifdef DEBUG_WRITE_OCH
      MessageInterface::ShowMessage(" Entering WriteToFile with %s\n",
                                    fileName.c_str());
      MessageInterface::ShowMessage("  number of segments: %d\n",
                                    (Integer) segments_.size());
   #endif
   std::ofstream fOut;
   fOut.open(fileName);
 
   // Get a timestamp to put at the top of the file
   time_t rawtime;
   struct tm * timeinfo;
   time (&rawtime);
   timeinfo = localtime(&rawtime);
   char buffer[80];
   strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);

   // Open the file
   if (fOut.is_open())
   {
      fOut << "Optimal Control History file written by CSALT, "
           << buffer << std::endl;
      fOut << std::endl;

      // Write out the exit conditions of the optimization
      fOut << "EXIT_CONDITIONS_START" << std::endl;
      fOut << "\tOPTIMIZER_FEASIBILITY_METRIC\t= " << std::scientific << std::setprecision(17) << feasibility << std::endl;
      fOut << "\tOPTIMIZER_OPTIMALITY_METRIC\t\t= " << optimality << std::endl;
      fOut << "\tCOST\t\t\t\t\t\t\t= " << cost << std::endl;
      fOut << "\tMAX_CONSTRAINT_VIOLATION\t\t= " << maxConViolation << std::endl;
      fOut << "\tOPTIMIZER_EXIT_STATUS\t\t\t= Exited with code " << exitCode << ", " << exitStatus << std::endl;
      fOut << "\tMAX_RELATIVE_MESH_ERROR\t\t\t= " << maxRelMeshError << std::endl;
      fOut << "\tSOLUTION_TYPE\t\t\t\t\t= " << solutionType << std::endl;
      fOut << "EXIT_CONDITIONS_END" << std::endl;
      fOut << std::endl;
      fOut.unsetf(std::ios::scientific);

      // Loop through all segments (phases)
      for (UnsignedInt s = 0; s < segments_.size(); s++)
      {
         OCHTrajectorySegment *och = (OCHTrajectorySegment*) segments_.at(s);
         #ifdef DEBUG_WRITE_OCH
            MessageInterface::ShowMessage(
                      " Grabbed OCHTrajectorySegment at %d <%p>\n", s, och);
         #endif
        
         // Print the header items
         fOut << "META_START" << std::endl;
         if (och->GetCentralBody().compare(""))  // non-blank central body
            fOut << "\tCENTRAL_BODY\t= " << och->GetCentralBody() << std::endl;
         if (och->GetObjectId().compare(""))     // non-blank ID
            fOut << "\tOBJECT_ID\t\t= " << och->GetObjectId() << std::endl;
         if (och->GetObjectName().compare(""))   // non-blank name
            fOut << "\tOBJECT_NAME\t\t= " << och->GetObjectName() << std::endl;
         if (och->GetRefFrame().compare(""))     // non-blank reference frame
            fOut << "\tREF_FRAME\t\t= " << och->GetRefFrame() << std::endl;
         if (och->GetTimeSystem().compare(""))   // non-blank time system
            fOut << "\tTIME_SYSTEM\t\t= " << och->GetTimeSystem() << std::endl;
         fOut << "\tNUM_STATES\t\t= " << och->GetNumStates() << std::endl;
         fOut << "\tNUM_CONTROLS\t= " << och->GetNumControls() << std::endl;
         fOut << "\tNUM_INTEGRALS\t= " << och->GetNumIntegrals() << std::endl;
         fOut << "META_STOP" << std::endl;
         fOut << std::endl;

         // Loop through all data columns and print the state, then control,
         // then integral data
         fOut << "DATA_START" <<std::endl;
         Integer numData      = och->GetNumDataPoints();
         Integer numStates    = och->GetNumStates();
         Integer numControls  = och->GetNumControls();
         Integer numIntegrals = och->GetNumIntegrals();
         #ifdef DEBUG_WRITE_OCH
            MessageInterface::ShowMessage(
                              " About to loop over numDataPoints\n");
         #endif

         for (Integer idx = 0; idx < numData; idx++)
         {
            Real theTime = och->GetTime(idx);
            fOut << "\t" << std::setw(26) << std::setprecision(17) << std::left
                         << theTime;

            for (Integer stateIdx = 0; stateIdx < numStates; stateIdx++)
            {
               if (och->GetStateSize(idx))   // non-zero state size
                  fOut << " " << std::setw(26) << std::setprecision(17)
                       << std::left << och->GetState(idx,stateIdx);
               else
               {
                  Rvector neededTime;
                  neededTime.SetSize(1);
                  neededTime(0) = theTime;
                  SetAllowExtrapolation(true);
                  SetAllowInterSegmentExtrapolation(true);
                  Rmatrix interpState = GetState(neededTime);
                  fOut << " " << std::setw(26) << std::setprecision(17)
                     << std::left << interpState(0, stateIdx);
               }
            }

            for (Integer controlIdx = 0; controlIdx < numControls; controlIdx++)
            {
               if (och->GetControlSize(idx))
                  fOut << " " << std::setw(26) << std::setprecision(17)
                       << std::left << och->GetControl(idx,controlIdx);
               else
               {
                  Rvector neededTime;
                  neededTime.SetSize(1);
                  neededTime(0) = theTime;
                  SetAllowExtrapolation(true);
                  SetAllowInterSegmentExtrapolation(true);
                  Rmatrix interpControl = GetControl(neededTime);
                  fOut << " " << std::setw(26) << std::setprecision(17)
                       << std::left << interpControl(0,controlIdx);
               }
            }
            
            for (Integer integralIdx = 0; integralIdx < numIntegrals;
                 integralIdx++)
            {
               if (och->GetIntegralSize(idx))
                  fOut << " " << std::setw(26) << std::setprecision(17)
                       << std::left << och->GetIntegral(idx,integralIdx);
               else
               {
                  Rvector neededTime;
                  neededTime.SetSize(1);
                  neededTime(0) = theTime;
                  SetAllowExtrapolation(true);
                  SetAllowInterSegmentExtrapolation(true);
                  Rmatrix interpIntegral = GetIntegral(neededTime);
                  fOut << " " << std::setw(26) << std::setprecision(17)
                       << std::left << interpIntegral(0,integralIdx);
               }
            }
             
            fOut << " " << std::endl;
         } // loop over numData
         fOut << "DATA_STOP" <<std::endl;
         fOut << std::endl;
      } // loop through segments
      fOut.close();
   } // if file is open
   #ifdef DEBUG_WRITE_OCH
   else
   {
      MessageInterface::ShowMessage(" File %s did NOT open!\n",
                                    fileName.c_str());
   }
   #endif
   #ifdef DEBUG_WRITE_OCH
      MessageInterface::ShowMessage(" EXITING WriteToFile with %s\n",
                                    fileName.c_str());
   #endif
}

//------------------------------------------------------------------------------
// void ReadFromFile(std::string fileName)
//------------------------------------------------------------------------------
/**
* This method populates OCH data from an OCH file
*
* @param <fileName> the file to read
*
*/
//------------------------------------------------------------------------------
void OCHTrajectoryData::ReadFromFile(std::string fileName)
{
   #ifdef DEBUG_READ_OCH
      MessageInterface::ShowMessage(" Entering ReadFromFile with %s\n",
                                    fileName.c_str());
   #endif

   // Create and initialize the local variables
   std::ifstream fIn;
   std::string currLine;
   Integer currSegment = -1;
   Integer lineNumber = 0;
   Integer dataLine;
   Integer fileLocation = 0; // 1 = in segment header, 2 = in data, 0 = neither
   bool    metaFound = false;
   Integer nmStates    = -1;
   Integer nmControls  = -1;
   Integer nmIntegrals = -1;
   TrajectoryDataStructure localData;

   // Clear the vectors
   for (Integer ii = 0; ii < segments_.size(); ii++)
      delete segments_.at(ii);
   segments_.clear();
   hasSegmentHadDuplicates.clear();
   
   numSegments = 0;

   // Open the file
   fIn.open(fileName);
   if (fIn.is_open())
   {
      #ifdef DEBUG_READ_OCH
         MessageInterface::ShowMessage("  ReadFromFile File is open: %s\n",
                                       fileName.c_str());
      #endif
      OCHTrajectorySegment *och = NULL;
      
      // Loop through all lines in the file
      while (std::getline(fIn,currLine))
      {
         lineNumber++;
         #ifdef DEBUG_READ_OCH
            MessageInterface::ShowMessage("  currLine: %s\n", currLine.c_str());
         #endif
        
         if (currLine.find("\"COMMENT") != std::string::npos)
             currLine = currLine.substr(0,currLine.find("\"COMMENT"));            
         
         // Check if a segment header is starting
         if (currLine.find("META_START") != std::string::npos) 
         {
            if ((fileLocation == 1) || // already in META - error
                (fileLocation == 2))   // still in DATA - error
            {
               std::string errmsg = "ERROR reading data from ";
               errmsg += "this file: " + fileName;
               errmsg += ".  Unexpected META_START found in META ";
               errmsg += "or DATA block. \n";
               throw LowThrustException(errmsg);
            }
            fileLocation = 1;
            metaFound = true;
            currSegment++;
            // All segments added for this class must be of type
            // OCHTrajectorySegment
            och = new OCHTrajectorySegment();
            // push the new one onto the list at index currSegment
            segments_.push_back(och);
            hasSegmentHadDuplicates.push_back(false);
            numSegments++;
            #ifdef DEBUG_READ_OCH
               MessageInterface::ShowMessage(
                                  "  OCH [%d] <%p> added to the list!!!!!\n",
                                  currSegment, och);
            #endif

         }
         // Check if a segment header is ending
         else if (currLine.find("META_STOP") != std::string::npos)
         {
            fileLocation = 0;
         }
         // Check if a data segment is starting
         else if (currLine.find("DATA_START") != std::string::npos) 
         {
            if (fileLocation == 1)  // still in META - error
            {
               std::string errmsg = "ERROR reading data from ";
               errmsg += "this file: " + fileName;
               errmsg += ".  Missing META_STOP. \n";
               throw LowThrustException(errmsg);
            }
            if (fileLocation == 2)   // already in DATA - error
            {
               std::string errmsg = "ERROR reading data from ";
               errmsg += "this file: " + fileName;
               errmsg += ".  Unexpected DATA_START found within DATA block. \n";
               throw LowThrustException(errmsg);
            }
            if (!metaFound)
            {
               std::string errmsg = "ERROR reading data from ";
               errmsg += "this file: " + fileName;
               errmsg += ".  No META data found or missing META_START. \n";
               throw LowThrustException(errmsg);
            }
            dataFound = true;
            fileLocation = 2;
            dataLine = -1;
            
            // Make sure we knew how many data columns to expect
            if (nmStates == -1)
            {
               std::string errmsg = "ERROR - Missing NUM_STATES field ";
               errmsg            += "in meta data of ";
               errmsg            += "this file: " + fileName + "\n";
               throw LowThrustException(errmsg);
            }
            if (nmControls == -1)
            {
               std::string errmsg = "ERROR - Missing NUM_CONTROLS field ";
               errmsg            += "in meta data of ";
               errmsg            += "this file: " + fileName + "\n";
               throw LowThrustException(errmsg);
            }
            if (nmIntegrals == -1)
            {
               std::string errmsg = "ERROR - Missing NUM_INTEGRALS field ";
               errmsg            += "in meta data of ";
               errmsg            += "this file: " + fileName + "\n";
               throw LowThrustException(errmsg);
            }
            if ((nmStates == 0) && (nmControls == 0) && (nmIntegrals == 0))
            {
               std::string errmsg = "ERROR: Trajectory data could not be ";
               errmsg            += "read from this file: " + fileName + "\n";
               throw LowThrustException(errmsg);
            }
            localData.states.SetSize(nmStates);
            localData.controls.SetSize(nmControls);
            localData.integrals.SetSize(nmIntegrals);
         }
         // Check if a data segment is ending
         else if (currLine.find("DATA_STOP") != std::string::npos) 
         {
            if (fileLocation != 2)  // error
            {
               std::string errmsg = "ERROR reading data from ";
               errmsg += "this file: " + fileName;
               errmsg += ".  Unexpected DATA_STOP found outside DATA block. \n";
               throw LowThrustException(errmsg);
            }
            fileLocation = 0;
//            // reset in case there's another Meta/Data segment/section  TBD
//            nmStates    = -1;
//            nmControls  = -1;
//            nmIntegrals = -1;
         }
         else if (fileLocation == 1) // @todo simplify reading META data
         {
            // Process the header data

            if (currLine.find("CENTRAL_BODY") != std::string::npos)
            {
               std::string input = GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAllBlanks(
                                   currLine.substr(currLine.find("=") + 1,
                                   currLine.length()-currLine.find("=") - 1)),
                                   "\r"),"\n");
               och->SetCentralBody(input);
            }
            else if (currLine.find("OBJECT_ID") != std::string::npos)
            {
               std::string input = GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAllBlanks(
                                   currLine.substr(currLine.find("=") + 1,
                                   currLine.length()-currLine.find("=") - 1)),
                                   "\r"),"\n");
               och->SetObjectId(input);
            }
            else if (currLine.find("OBJECT_NAME") != std::string::npos)
            {
               std::string input = GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAllBlanks(
                                   currLine.substr(currLine.find("=") + 1,
                                   currLine.length()-currLine.find("=") - 1)),
                                   "\r"),"\n");
               och->SetObjectName(input);
            }
            else if (currLine.find("REF_FRAME") != std::string::npos)
            {
               std::string input = GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAllBlanks(
                                   currLine.substr(currLine.find("=") + 1,
                                   currLine.length()-currLine.find("=") - 1)),
                                   "\r"),"\n");
               och->SetRefFrame(input);
            }
            else if (currLine.find("TIME_SYSTEM") != std::string::npos)
            {
               std::string input = GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAll(
                                   GmatStringUtil::RemoveAllBlanks(
                                   currLine.substr(currLine.find("=") + 1,
                                   currLine.length()-currLine.find("=") - 1)),
                                   "\r"),"\n");

               if (theTimeConverter->IsValidTimeSystem(input))
                  och->SetTimeSystem(input);
               else
               {
                  std::string errmsg = "ERROR - OCHTrajectoryData: ";
                  errmsg += "error reading TIME SYSTEM from ";
                  errmsg += "this file: " + fileName + ".  Time system "; 
                  errmsg += "\"" + input + "\" is unknown.\n";
                  throw LowThrustException(errmsg);
               }
            }
            else if (currLine.find("NUM_STATES") != std::string::npos)
            {
               Integer value;
               bool success = GmatStringUtil::ToInteger(
                              GmatStringUtil::RemoveAll(
                              GmatStringUtil::RemoveAll(
                              currLine.substr(currLine.find("=") + 1,
                              currLine.length()-currLine.find("=") - 1),
                              "\r"),"\n"),&value);
               if (success)
               {
                  SetNumStateParams(currSegment,value);
                  nmStates = value;
               }
               else
               {
                  std::string errmsg = "ERROR - OCHTrajectoryData: ";
                  errmsg            += "error reading NUM STATES from ";
                  errmsg            += "this file: " + fileName + "\n";
                  throw LowThrustException(errmsg);
               }
            }
            else if (currLine.find("NUM_CONTROLS") != std::string::npos)
            {
               Integer value;
               bool success = GmatStringUtil::ToInteger(
                              GmatStringUtil::RemoveAll(
                              GmatStringUtil::RemoveAll(
                              currLine.substr(currLine.find("=") + 1,
                              currLine.length()-currLine.find("=") - 1),
                              "\r"),"\n"),&value);
               if (success)
               {
                  SetNumControlParams(currSegment,value);
                  nmControls = value;
               }
               else
               {
                  std::string errmsg = "ERROR - OCHTrajectoryData: ";
                  errmsg            += "error reading NUM CONTROLS from ";
                  errmsg            += "this file: " + fileName + "\n";
                  throw LowThrustException(errmsg);
               }
            }
            else if (currLine.find("NUM_INTEGRALS") != std::string::npos)
            {
               Integer value;
               bool success = GmatStringUtil::ToInteger(
                              GmatStringUtil::RemoveAll(
                              GmatStringUtil::RemoveAll(
                              currLine.substr(currLine.find("=") + 1,
                              currLine.length()-currLine.find("=") - 1),
                              "\r"),"\n"),&value);
               if (success)
               {
                  SetNumIntegralParams(currSegment,value);
                  nmIntegrals = value;
               }
               else
               {
                  std::string errmsg = "ERROR - OCHTrajectoryData: error ";
                  errmsg            += "reading NUM_INTEGRALS from ";
                  errmsg            += "this file: " + fileName + "\n";
                  throw LowThrustException(errmsg);
               }
            }
         }
         else if (fileLocation == 2)
         {
            // Process the data line - ignore BLANK lines
            if (GmatStringUtil::IsBlank(currLine))
               continue;

            dataLine++;
            Integer lineLocation;
            Real value;
            
            // How many data columns do we expect on each line?
            Integer numExpected = nmStates + nmControls + nmIntegrals;

            std::istringstream lineStr;
            lineStr.str(currLine);
            
            //Get the time first - read as a string to use existing
            // ProcessTimeString method
            std::string timeStr;
            lineStr >> timeStr;

            localData.time = ProcessTimeString(timeStr,och->GetTimeSystem());
            
            // larger data array to check for additional data
            Rvector dataVec(numExpected * 2);
            Real dataVal;
            // Try to get all of the data expected
            Integer counter = 0;
            while (lineStr >> dataVal)
            {
               dataVec[counter++] = dataVal;
            }
            if (counter < numExpected)
            {
               std::stringstream errmsg1;
               errmsg1.precision(15);
               errmsg1 << "Error reading this file \"" << fileName
                       << "\": expected " << numExpected << " data columns, "
                       << "but found only " << counter << ".\n";
               throw LowThrustException(errmsg1.str());
            }
            if (counter > numExpected)
            {
               std::stringstream errmsg1;
               errmsg1.precision(15);
               errmsg1 << "Error reading this file \"" << fileName
               << "\": expected only " << numExpected << " data columns, "
               << "but found " << counter << ".\n";
               throw LowThrustException(errmsg1.str());
            }
            Integer idx = 0;
            for (Integer ii = 0; ii < nmStates; ii++)
               localData.states(ii) = dataVec(idx++);
            for (Integer ii = 0; ii < nmControls; ii++)
               localData.controls(ii) = dataVec(idx++);
            for (Integer ii = 0; ii < nmIntegrals; ii++)
               localData.integrals(ii) = dataVec(idx++);
            /// Add the localData to the segment
            try
            {
               och->AddDataPoint(localData);
            }
            catch (LowThrustException &lt)
            {
               // Assume exception thrown is for non-monotonic times
               std::string errmsg = "ERROR initializing guess from ";
               errmsg += "input file \"" + fileName + "\": data points are not ";
               errmsg += "in the correct temporal order.\n";
               throw LowThrustException(errmsg);
            }
         } // process the data line (location == 2)
      } // loop through lines in the file
      if (fileLocation == 1)
      {
         std::string errmsg = "ERROR reading OCH file " + fileName;
         errmsg            += ": META_STOP line ";
         errmsg            += "not found.\n";
         throw LowThrustException(errmsg);
      }
      if (fileLocation == 2)
      {
         std::string errmsg = "ERROR reading OCH file " + fileName;
         errmsg            += ": DATA_STOP line ";
         errmsg            += "not found.\n";
         throw LowThrustException(errmsg);
      }
      if (!metaFound)
      {
         std::string errmsg = "ERROR reading OCH file " + fileName;
         errmsg            += ": META data ";
         errmsg            += "not found.\n";
         throw LowThrustException(errmsg);
      }
      if (!dataFound)
      {
         std::string errmsg = "ERROR: Trajectory data could not be ";
         errmsg            += "read from this file: " + fileName + "\n";
         throw LowThrustException(errmsg);
      }

   } // is file open
   else 
   {
      std::string errmsg = "ERROR - OCHTrajectoryData: cannot open ";
      errmsg            += "this file: " + fileName + "\n";
      throw LowThrustException(errmsg);
   }
}
