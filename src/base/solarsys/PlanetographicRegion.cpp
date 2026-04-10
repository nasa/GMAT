//------------------------------------------------------------------------------
//                             PlanetographicRegion.cpp
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the 
// FDSS III Contract
//
// Author: David E. Washington, Pearl River Technologies, Inc.
// Created: 2022.3.9
//
/// PlanetographicRegion: a defined region over the surface of a celestial body.
/** It is not dependent on the surface features, but merely a region over which
    spacecraft will do something.  It is defined as a collection of positions
    (Lat, long) in the fixed coordinates of the celestial body.
**/
//------------------------------------------------------------------------------
#include "PlanetographicRegion.hpp"
#include "FileUtil.hpp"
#include "StringUtil.hpp"
#include "APIFunctions.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
PlanetographicRegion::PARAMETER_TEXT[PlanetographicRegionParamCount - BodyFixedPointParamCount] =
{
   "Latitude",
   "Longitude",
   "AreaFileName"
};

const Gmat::ParameterType
PlanetographicRegion::PARAMETER_TYPE[PlanetographicRegionParamCount - BodyFixedPointParamCount] =
{
   Gmat::RVECTOR_TYPE, // LATITUDE
   Gmat::RVECTOR_TYPE, // LONGITUDE
   Gmat::STRING_TYPE   // FILENAME
};

//------------------------------------------------------------------------------
// PlanetographicRegion(const string& objectName)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
 //------------------------------------------------------------------------------
PlanetographicRegion::PlanetographicRegion(const std::string& objectName) :
   BodyFixedPoint("PlanetographicRegion", objectName, Gmat::REGION)

{
   parameterCount = PlanetographicRegionParamCount;
   objectTypes.push_back(Gmat::REGION);
   objectTypeNames.push_back("PlanetographicRegion");
   objectTypes.push_back(Gmat::SPACE_POINT);
   objectTypeNames.push_back("BodyFixedPoint");
}


//------------------------------------------------------------------------------
// PlanetographicRegion(const PlanetographicRegion& regionToCopy)
//------------------------------------------------------------------------------
/**
 * Copy Constructor
 *
 * @param regionToCopy The original being copied
 */
 //------------------------------------------------------------------------------
PlanetographicRegion::PlanetographicRegion(const PlanetographicRegion& regionToCopy) :
   BodyFixedPoint(regionToCopy)
{
   latitudeList = regionToCopy.latitudeList;
   longitudeList = regionToCopy.longitudeList;
   filename = regionToCopy.filename;
}

//------------------------------------------------------------------------------
// PlanetographicRegion::operator=(const PlanetographicRegion& regionToCopy)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param regionToCopy The PlanetographicRegion providing parameters for this one
 *
 * @return This PlanetographicRegion
 */
 //------------------------------------------------------------------------------
PlanetographicRegion & PlanetographicRegion::operator=(const PlanetographicRegion& regionToCopy)
{
   if (&regionToCopy == this)
      return *this;

   BodyFixedPoint::operator=(regionToCopy);
   latitudeList = regionToCopy.latitudeList;
   longitudeList = regionToCopy.longitudeList;
   filename = regionToCopy.filename;
   return *this;
}

//------------------------------------------------------------------------------
// ~PlanetographicRegion() 
//------------------------------------------------------------------------------
/**
 * Destructor
 */
 //------------------------------------------------------------------------------
PlanetographicRegion::~PlanetographicRegion()
{
}


//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a replica of this instance
 *
 * @return A pointer to the new instance
 */
 //------------------------------------------------------------------------------
GmatBase * PlanetographicRegion::Clone() const
{
   return (new PlanetographicRegion(*this));
}

//------------------------------------------------------------------------------
// Rvector& PlanetographicRegion::GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets a Rvector parameter
 *
 * @param label The name of the parameter
 *
 * @return the value of the requested parameter  */
 //------------------------------------------------------------------------------
const Rvector& PlanetographicRegion::GetRvectorParameter(const std::string &label) const 
{
   return GetRvectorParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Rvector& PlanetographicRegion::GetRvectorParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Gets a Rvector parameter
 *
 * @param id The id of the parameter
 *
 * @return the value of the requested parameter  */
 //------------------------------------------------------------------------------
const Rvector& PlanetographicRegion::GetRvectorParameter(const Integer id) const 
{
   static Rvector data;
   bool noupcast = true;

   switch (id)
   {
   case LATITUDE:
      data.SetSize(latitudeList.size());
      for (UnsignedInt i = 0; i < latitudeList.size(); ++i)
         data[i] = latitudeList[i];
      break;

   case LONGITUDE:
      data.SetSize(longitudeList.size());
      for (UnsignedInt i = 0; i < longitudeList.size(); ++i)
         data[i] = longitudeList[i];
      break;

   default:
      noupcast = false;
      break;
   }

   if (noupcast)
      return data;

   return BodyFixedPoint::GetRvectorParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets the contents of a string parameter
 *
 * @param id The parameter's id
 * @param value The new value
 *
 * @return true on success, false on failure
 */
 //------------------------------------------------------------------------------
bool PlanetographicRegion::SetStringParameter(const Integer id, const std::string &value)
{
#ifdef DEBUG_FM_SET
   MessageInterface::ShowMessage
   ("ODEModel::SetStringParameter() entered, id=%d, value='%s'\n",
      id, value.c_str());
#endif

   switch (id)
   {
   case FILENAME:
      filename = GmatFileUtil::ConvertToOsFileName(value);

      std::string theFile = GmatFileUtil::FindFile(filename);

      if (theFile == "")
      {
         std::string message = "File: " + value + " in PlanetographicRegion: " + instanceName + " does not exist.";
         throw GmatBaseException(message);
      }
      if (latitudeList.size() > 0 || longitudeList.size() > 0)
      {
         DuplicateDefinitionError();
      }
      return true;
   }
   return BodyFixedPoint::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets the contents of a string parameter
 *
 * @param field The parameter's scripted identifier
 * @param value The new value
 *
 * @return true on success, f.alse on failure
 */
 //------------------------------------------------------------------------------
bool PlanetographicRegion::SetStringParameter(const std::string & field, const std::string & value)
{
   for (int i = BodyFixedPointParamCount; i < PlanetographicRegionParamCount; ++i)
   {
      if (field == PARAMETER_TEXT[i])
      {
         return SetStringParameter(i, value);
      }
   }
   return BodyFixedPoint::SetStringParameter(field, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param field The parameter's name
 *
 * @return The parameter data, a string
 */
 //------------------------------------------------------------------------------
std::string PlanetographicRegion::GetStringParameter(const std::string& field) const
{
   for (int i = BodyFixedPointParamCount; i < PlanetographicRegionParamCount; ++i)
   {
      if (field == PARAMETER_TEXT[i])
      {
         return GetStringParameter(i);
      }
   }
   return BodyFixedPoint::GetStringParameter(field);
}


//------------------------------------------------------------------------------
// std::string PlanetographicRegion::GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The parameter's id
 *
 * @return The parameter data, a String
 */
 //------------------------------------------------------------------------------
std::string PlanetographicRegion::GetStringParameter(const Integer id) const
{
   if (id == FILENAME)
   {
      return filename;
   }
   return BodyFixedPoint::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter
 *
 * @param id The parameter's id
 *
 * @return The parameter data, a Real
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::GetRealParameter(const Integer id) const
{
   return BodyFixedPoint::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::GetRealParameter(const std::string& field) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter
 *
 * @param field The parameter's field
 *
 * @return The parameter data, a Real
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::GetRealParameter(const std::string& field) const
{
   for (int i = BodyFixedPointParamCount; i < PlanetographicRegionParamCount; ++i)
   {
      if (field == PARAMETER_TEXT[i])
      {
         return GetRealParameter(i);
      }
   }
   return BodyFixedPoint::GetRealParameter(field);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Real parameter
 *
 * @param id The parameter's id
 * @param index The index in the requested list
 *
 * @return The parameter data, a Real
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::GetRealParameter(const Integer id, const Integer index) const
{

   switch (id)
   {
   case LATITUDE:
      if (index < 0 || index >= latitudeList.size())
      {
         throw GmatBaseException("Index out of range");
      }
      return latitudeList[index];

   case LONGITUDE:
      if (index < 0 || index >= longitudeList.size())
      {
         throw GmatBaseException("Index out of range");
      }
      return longitudeList[index];
   }
   return BodyFixedPoint::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// GmatBase * PlanetographicRegion::GetRefObject(const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Retrieves an Object parameter
 *
 * @param type The parameter's type
 * @param name The name of the parameter
 *
 * @return The parameter data, a pointer to a GmatBase object
 */
 //------------------------------------------------------------------------------
GmatBase * PlanetographicRegion::GetRefObject(const UnsignedInt type, const std::string & name)
{
   return BodyFixedPoint::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::SetRealParameter(const std::string& field, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter
 *
 * @param field The name of the parameter to be set
 * @param value The value to be set
 *
 * @return value set
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::SetRealParameter(const Integer id, const Real value)
{
   return BodyFixedPoint::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::SetRealParameter(const Integer id, const Real value,
//                                             const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter
 *
 * @param id The id of the parameter to be set
 * @param value The value to be set
 * @param index The index in a list of the parameter to be set
 *
 * @return value set
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::SetRealParameter(const std::string& field, const Real value)
{
   for (int i = BodyFixedPointParamCount; i < PlanetographicRegionParamCount; ++i)
   {
      if (field == PARAMETER_TEXT[i])
      {
         return SetRealParameter(i, value);
      }
   }
   return BodyFixedPoint::SetRealParameter(field, value);
}


//------------------------------------------------------------------------------
// bool PlanetographicRegion::SetRefObject(GmatBase * obj, const UnsignedInt type, const std::string & name)
//------------------------------------------------------------------------------
/**
 * Sets an Object parameter
 *
 * @param * obj, A pointer to object to be set
 * @param type The parameter's type
 * @param name The name of the parameter
 *
 * @return bool, true if set successfully, false if not
 */
 //------------------------------------------------------------------------------
bool PlanetographicRegion::SetRefObject(GmatBase * obj, const UnsignedInt type, const std::string & name)
{
   return BodyFixedPoint::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// Real PlanetographicRegion::SetRealParameter(const Integer id, const Real value,
//                                             const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter
 *
 * @param id The id of the parameter to be set
 * @param value The value to be set
 * @param index The index in a list of the parameter to be set
 *
 * @return value set
 */
 //------------------------------------------------------------------------------
Real PlanetographicRegion::SetRealParameter(const Integer id, const Real value,
   const Integer index)
{
   std::string indexString = std::to_string(index);

   if (id == LATITUDE)
   {
      if (filename.size() > 0)
         DuplicateDefinitionError();

      std::string name = "Latitude[" + indexString + "]";
      if (value > 90.0 || value < -90.0)
      {
         std::string message = "The latitude:  " + std::to_string(value) + " in " + instanceName + " is invalid.  The number must be between -90 and 90 Degrees.";
         throw GmatBaseException(message);
      }
      latitudeList.push_back(value);
      return value;
   }
   else if (id == LONGITUDE)
   {
      if (filename.size() > 0)
         DuplicateDefinitionError();

      std::string name = "Longitude[" + indexString + "]";

      Real compare = 0.0;
      if (longitudeList.size() > 0)
      {
         Real minimum = longitudeList[0];
         Real maximum = longitudeList[0];
         for (Integer i = 1; i < longitudeList.size(); ++i)
         {
            if (longitudeList[i] < minimum)
               minimum = longitudeList[i];

            if (longitudeList[i] > maximum)
               maximum = longitudeList[i];
         }
         if ((value - minimum) >= 360.0 || (maximum - value) >= 360.0)
         {
            std::string message = "The longitude:  " + std::to_string(value) + " in " + instanceName + " is invalid.  All longitudes must be within 360 Dg of each other.";
            throw GmatBaseException(message);
         }
         compare = longitudeList[longitudeList.size() - 1];
      }
      longitudeList.push_back(value);
      return value;
   }
   else
   {
      return BodyFixedPoint::SetRealParameter(id, value);
   }
}



//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initializes this object.
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::Initialize()
{
   BodyFixedPoint::Initialize();
   InterpolateTheCoordinates();
   FinalizeCreation();
   return true;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
 //------------------------------------------------------------------------------
bool PlanetographicRegion::HasLocalClones()
{
   return false;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference objects used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::RenameRefObject(const UnsignedInt type, const std::string & oldName, const std::string & newName)
{
   return false;
}


void PlanetographicRegion::DuplicateDefinitionError() const
{
   std::string message = "In PlanetographicRegion " + instanceName +
      ": it is not allowed to have Latitude or Longitude arrays as " +
      "well as a Filename defined!";
   throw GmatBaseException(message);
}

std::vector<Rvector3> PlanetographicRegion::EliminateHiddenVertices(const std::vector<Rvector3> & region)
{
   std::vector<Rvector3> resultant;

   for (Integer index = 0; index < region.size(); ++index)
   {
      Integer nextIndex = (index + 1) % region.size();
      Rvector3 p0 = region[index];
      Rvector3 p1 = region[nextIndex];
      if (p0[0] > 0.0)
         resultant.push_back(p0);

      if (p0[0] * p1[0] < 0.0)
      {
         // estimate the point at which x equals zero
         Real t = (0.0 - p0[0]) / (p1[0] - p0[0]);

         Real x = p0[0] + t * (p1[0] - p0[0]);
         Real y = p0[1] + t * (p1[1] - p0[1]);
         Real z = p0[2] + t * (p1[2] - p0[2]);
         Rvector3 newPoint(x, y, z);
         newPoint = meanEquatorialRadius * newPoint.Normalize();
         resultant.push_back(newPoint);
      }
   }
   return resultant;
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::GetCrossings(const SpacePoint& spaceObject, 
//                                          Real startTime, Real endTime, Real interval, 
//                                          RealArray starts, RealArray ends) const
//---------------------------------------------------------------------------
/**
 *  Populates the latitudes and longitudes vectors with interpolated values along a great circle
 * between the [potentially] widely spaced points in latitudeList and longitudeList
 *
 */
void PlanetographicRegion::InterpolateTheCoordinates()
{
   if (vertices.size() > 0)
      return;

   // If there is not list of latitudes and longitudes, check to 
   // see if there is a file listed.
   if (latitudeList.size() == 0 && filename.size() > 0)
   {
      ReadLatitudeLongitudeFile();
   }

   if (latitudeList.size() != longitudeList.size())
   {
      const std::string message = "Planetographic Region: " + instanceName +
         ".  The number of entries for Latitude must equal the number of entries for Longitude";
      throw GmatBaseException(message);
   }
   if (latitudeList.size() < 3)
   {
      const std::string message = "Planetographic Region: " + instanceName +
         ".  The number of entries for Latitude must be three or greater";
      throw GmatBaseException(message);
   }

   CheckLatitudeAndLongitudeList();

   Rvector3 previous(0, 0, 0);
   for (Integer i = 0; i < latitudeList.size(); ++i)
   {
      AddExtraPointIfMovingAlongSegmentPoleToPole(i);

      Real lat = latitudeList[i] * GmatMathConstants::RAD_PER_DEG;
      Real lon = longitudeList[i] * GmatMathConstants::RAD_PER_DEG;

      Rvector3 latLongHeight(lat, lon, 0.0);
      Rvector3 cartesian = PlanetographicRegion::EllipsoidToCartesian(latLongHeight, flattening, meanEquatorialRadius);
      if (cartesian != previous)
         vertices.push_back(cartesian);

      previous = cartesian;
   }
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::RotateYX(const Real angle, Rvector3 & theVector)
//---------------------------------------------------------------------------
/**
 *  Rotates a vector by an angle in the YX plane.
 *  The plane of rotation is defined as +X = 0; +Y = PI/2
 *  @param angle     The angle to rotate in radians
 *  @param theVector The vector to rotate
 *  @return  void, but the vector is rotated
 */
void PlanetographicRegion::RotateYX(const Real angle, Rvector3 & theVector)
{
   Real temp = theVector[1] * cos(angle) - theVector[0] * sin(angle);
   theVector[0] = theVector[1] * sin(angle) + theVector[0] * cos(angle);
   theVector[1] = temp;
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::RotateZX(const Real angle, Rvector3 & theVector)
//---------------------------------------------------------------------------
/**
 *  Rotates a vector by an angle in the ZX plane.
 *  The plane of rotation is defined as +Z = 0; +X = PI/2
 *  @param angle     The angle to rotate in radians
 *  @param theVector The vector to rotate
 *  @return  void, but the vector is rotated
 */
void PlanetographicRegion::RotateZX(const Real angle, Rvector3 & theVector)
{
   Real temp = theVector[2] * cos(angle) - theVector[0] * sin(angle);
   theVector[0] = theVector[2] * sin(angle) + theVector[0] * cos(angle);
   theVector[2] = temp;
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::RotateYZ(const Real angle, Rvector3 & theVector)
//---------------------------------------------------------------------------
/**
 *  Rotates a vector by an angle in the YZ plane.
 *  The plane of rotation is defined as +Y = 0; +Z = PI/2
 *  @param angle     The angle to rotate in radians
 *  @param theVector The vector to rotate
 *  @return  void, but the vector is rotated
 */
void PlanetographicRegion::RotateZY(const Real angle, Rvector3 & theVector)
{
   Real temp = theVector[2] * cos(angle) - theVector[1] * sin(angle);
   theVector[1] = theVector[2] * sin(angle) + theVector[1] * cos(angle);
   theVector[2] = temp;
}

//------------------------------------------------------------------------------
// StringArray Parse(const std::string &input, const char delimeter)
//------------------------------------------------------------------------------
/**
 * Returns the input string broken up by any of the delimiters
 * e.g) "cd c:/my test directory" returns if delimiter is 'c'.
 *   "d "
 *   ":/my test dire"
 *   "tory"
 *
 * @param  str  Input string
 * @param  delimiters string of delimeters
 * @return  the input string, but divided into parts by the delimeter
 */
 //------------------------------------------------------------------------------
StringArray PlanetographicRegion::Parse(const std::string & input, const char delimeter)
{
   StringArray parts;
   if (input.length() == 0)
      return parts;

   std::string remaining = input;
   std::string::size_type index = remaining.npos;

   while ((index = remaining.find(delimeter)) != remaining.npos)
   {
      if (index > 0)
         parts.push_back(remaining.substr(0, index));
      remaining = remaining.substr(index + 1);
   }
   if (remaining.length() > 0)
   {
      parts.push_back(remaining);
   }
   return parts;
}


//------------------------------------------------------------------------------
// StringArray Parse(const std::string & input, const std::string & delimeters)
//------------------------------------------------------------------------------
/**
 * Returns the input string broken up by any of the delimiters
 * e.g) "cd c:/my test directory" returns if delimiter are " /".
 *   cd
 *   c:
 *   my 
 *   test 
 *   directory
 *
 * @param  str  Input string
 * @param  delimiters string of delimeters
 * @return  the input string, but divided into parts by the delimeters
 */
 //------------------------------------------------------------------------------
StringArray PlanetographicRegion::Parse(const std::string & input, const std::string & delimeters)
{
   StringArray parts = { input };
   if (delimeters.length() == 0)
   {
      return parts;
   }
   else if (delimeters.length() == 1)
   {
      return Parse(input, delimeters[0]);
   }
   else
   {
      // divide the delimeters into two substrings
      int i = delimeters.size() / 2;
      std::string firstDelimeters = delimeters.substr(0, i);
      std::string secondDelimeters = delimeters.substr(i);

      // Process with the first set of delimeters
      StringArray subStrings = Parse(input, firstDelimeters);

      // Then procoss the result of that with the second set.
      parts.clear();
      for (StringArray::iterator stringIter = subStrings.begin(); stringIter != subStrings.end(); ++stringIter)
      {
         StringArray subSubStrings = Parse(*stringIter, secondDelimeters);

         // Combine the results
         for (StringArray::iterator subStringIter = subSubStrings.begin(); subStringIter != subSubStrings.end(); ++subStringIter)
         {
            std::string thisString = *subStringIter;
            if (thisString.length() > 0)
               parts.push_back(thisString);
         }
      }
      return parts;
   }
}

void PlanetographicRegion::AddExtraPointIfMovingAlongSegmentPoleToPole(Integer currentIndex)
{
   // Return if the point is not located at the pole
   if (latitudeList[currentIndex] != 90.0 && latitudeList[currentIndex] != -90.0)
      return;

   // Return if not traversing pole-to-pole.
   Integer previousIndex = (currentIndex > 0) ? currentIndex - 1 : latitudeList.size() - 1;
   if ((latitudeList[currentIndex] + latitudeList[previousIndex]) != 0.0)
      return;

   // Error if the longitude is changing.
   if (longitudeList[currentIndex] != longitudeList[previousIndex])
   {
      throw GmatBaseException("The longitude of pole-to-pole region boundary cannot change");
   }

   // Add extra point at the equator.
   Real lon = longitudeList[currentIndex] * GmatMathConstants::RAD_PER_DEG;
   Rvector3 latLongHeight(0.0, lon, 0.0);
   Rvector3 cartesian = EllipsoidToCartesian(latLongHeight, flattening, meanEquatorialRadius);
   vertices.push_back(cartesian);
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::CheckLatitudeAndLongitudeList() const
//---------------------------------------------------------------------------
/**
 *  Checks to latitude and longitude lists for consistency
 *  Errors if conditions aren't met
 *
 */
void PlanetographicRegion::CheckLatitudeAndLongitudeList() const
{
   if (latitudeList.size() != longitudeList.size())
   {
      const std::string message = "Planetographic Region: " + instanceName +
         ".  The number of entries for Latitude must equal the number of entries for Longitude";
      throw GmatBaseException(message);
   }
   if (latitudeList.size() < 3)
   {
      const std::string message = "Planetographic Region: " + instanceName +
         ".  The number of entries for Latitude must be three or greater";
      throw GmatBaseException(message);
   }

   for (Integer i = 0; i < latitudeList.size(); ++i)
   {
      Real currentLatitude = latitudeList[i];
      Real currentLongitude = longitudeList[i];
      Integer nextIndex = (i + 1) % latitudeList.size();
      Real nextLatitude = latitudeList[nextIndex];
      Real nextLongitude = longitudeList[nextIndex];

      if (currentLatitude == -nextLatitude)
      {
         if (currentLatitude == 90.0 || currentLatitude == -90.0)
         {
            // Pole-to-pole slice
            if (currentLongitude != nextLongitude)
            {
               throw GmatBaseException("The longitude of pole-to-pole region boundary cannot change");
            }
         }
         Real changeInLongitude = fabs(nextLongitude - currentLongitude);
         if (changeInLongitude == 180.0)
         {
            throw GmatBaseException("The path from a point to its antipode is undefined. \
Please enter an intermediate point");
         }
      }
   }
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------


//------------------------------------------------------------------------------
// std::string PhysicalModel::GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
std::string PlanetographicRegion::GetParameterText(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < PlanetographicRegionParamCount)
      return PARAMETER_TEXT[id - BodyFixedPointParamCount];
   else
      return BodyFixedPoint::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer PhysicalModel::GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
Integer PlanetographicRegion::GetParameterID(const std::string &str) const
{
   for (int i = BodyFixedPointParamCount; i < PlanetographicRegionParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - BodyFixedPointParamCount])
         return i;
   }
   return BodyFixedPoint::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType PhysicalModel::GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
Gmat::ParameterType PlanetographicRegion::GetParameterType(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < PlanetographicRegionParamCount)
      return PARAMETER_TYPE[id - BodyFixedPointParamCount];
   else
      return BodyFixedPoint::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string PhysicalModel::GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
 //------------------------------------------------------------------------------
std::string PlanetographicRegion::GetParameterTypeString(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < PlanetographicRegionParamCount)
      return PlanetographicRegion::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return BodyFixedPoint::GetParameterTypeString(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::IsParameterReadOnly(const Integer id) const
{
   if ((id == LOCATION_1) || (id == LOCATION_2) || (id == LOCATION_3) || 
      (id == HORIZON_REFERENCE) || (id == STATE_TYPE) || (id == ORBIT_COLOR) || (id == TARGET_COLOR))
      return true;

   return BodyFixedPoint::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param label Text label for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::IsParameterReadOnly(const std::string &label) const
{
   if ((label == PARAMETER_TEXT[LATITUDE - BodyFixedPointParamCount]) ||
      (label == PARAMETER_TEXT[LONGITUDE - BodyFixedPointParamCount]))
      return false;

   return BodyFixedPoint::IsParameterReadOnly(label);
}

bool PlanetographicRegion::IsValidID(const std::string & id)
{
   Integer i = atoi(&id[0]);
   return (i >= 0 && i < PlanetographicRegionParamCount);
}

//---------------------------------------------------------------------------
//  The following methods get the State or Position vector in MJ2000
//---------------------------------------------------------------------------
const Rvector6 PlanetographicRegion::GetMJ2000State(const A1Mjd & atTime)
{
   return BodyFixedPoint::GetMJ2000State(atTime);
}

const Rvector6 PlanetographicRegion::GetMJ2000State(const GmatTime & atTime)
{
   return BodyFixedPoint::GetMJ2000State(atTime);
}

const Rvector3 PlanetographicRegion::GetMJ2000Position(const A1Mjd & atTime)
{
   return BodyFixedPoint::GetMJ2000Position(atTime);
}

const Rvector3 PlanetographicRegion::GetMJ2000Position(const GmatTime & atTime)
{
   return BodyFixedPoint::GetMJ2000Position(atTime);
}

const Rvector3 PlanetographicRegion::GetMJ2000Velocity(const A1Mjd & atTime)
{
   return BodyFixedPoint::GetMJ2000Velocity(atTime);
}

const Rvector3 PlanetographicRegion::GetMJ2000Velocity(const GmatTime & atTime)
{
   return BodyFixedPoint::GetMJ2000Velocity(atTime);
}


//---------------------------------------------------------------------------
//  bool PlanetographicRegion::isWithin(Real lat, Real lon, Real height)
//---------------------------------------------------------------------------
/**
 * Takes a latitude, longitude, height point and returns if it is
 * within the region.
 *
 *
 * Notes: The region is currently defined as going from one point to the next
 *        in order with no logic to make sure it is a defined region.
 *        This will create a valid region but the user needs to understand how
 *        the region is drawn so they can create the region they intend to.
 *
 *
 * @param lat Latitude position Dg
 * @param lon Longitude position Dg
 * @param height The distance of the point from the bodies center. THIS PARAMETER IS NOT CURRENTLY USED
 * @param minDistance The minimum distance to the region (negative if within)
 *
 * @return true if the point is within the region, false if not
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::isWithin(Real lat, Real lon, Real height, Real & minDistance)
{
   //Takes a lat lon pos of an object and says whether or not it is within the region.
   //Draws a line segment along the same latitude to 180 longitude and checks the number of times 
   //that line segment intersects the regions line segments.
   //Cases to consider: .
   //         Intersecting a vertex, currently if you hit a vertex, you will be counted out of the region
   //         as it will hit both segments. This would be VERY rare in real situations but shoul         
   //   
   //         Need to explore how to set up a region that goes over a pole and how that is handled 

//DJC : One option for the pole : if the pole is inside the region, anything above the region's highest latitude vertex is in the region.


//Real sphereOfInfluenceRadius = 929000;

//Current height check is only sphere of influence. A user defined height will be better.
//if (height > sphereOfInfluenceRadius)
   //return false;y

/* DEW .. I have decided to completely change this.  Instead of working with
latitudes and longitudes, the cartesian vertices of the region will be rotated
so that the spacecraft is directly overhead.  This avoids all of the issues
regarding polar regions and international dateline since the region is now a
series of points on the surface of the planet.

This also eliminates the need for great-circle interpolation.*/


   int intersections = 0;

   // Create a copy of the vertices rotated so that below satellite position
   // is on the primeMeridian at the equator.
   std::vector<Rvector3> rotatedRegion;
   Real longitude = lon * GmatMathConstants::RAD_PER_DEG;
   Real latitude = lat * GmatMathConstants::RAD_PER_DEG;

   Rvector3 latLongHeight(latitude, longitude, 0.0);
   Rvector3 subSatellitePoint = EllipsoidToCartesian(latLongHeight, flattening, meanEquatorialRadius);

   // Rotate for longitude
   RotateYX(longitude, subSatellitePoint);

   // Rotation to get subsatellitePoint to equator
   Real sphericalLatitude = atan2(subSatellitePoint[2], subSatellitePoint[0]);
   RotateZX(sphericalLatitude, subSatellitePoint);

   // Rotate the region to match
   Integer sumSatelliteCanSee = 0;
   for (std::vector<Rvector3>::const_iterator i = vertices.cbegin(); i != vertices.cend(); ++i)
   {
      Rvector3 copy(*i);

      // rotate for longitude
      RotateYX(longitude, copy);

      // rotate for latitude
      RotateZX(sphericalLatitude, copy);

      rotatedRegion.push_back(copy);
      if (copy[0] > 0.0)
         ++sumSatelliteCanSee;
   }

   minDistance = meanEquatorialRadius;

   // If the entire region is on the opposite side of the planet.
   if (sumSatelliteCanSee == 0)
      return false;

   rotatedRegion = EliminateHiddenVertices(rotatedRegion);

   //Position of the point we are checking
   Real position[2] = { 0.0, 0.0 }; // y,z
   //The other side of the line segment
   Real endOfSegment[2] = { 0.0, meanEquatorialRadius + 100.0 };


   //Loop through region edges
   for (int i = 0; i < rotatedRegion.size(); i++)
   {
      Integer nextIndex = (i + 1) % rotatedRegion.size();
      // 0->1->...->n->0
      Real y1 = rotatedRegion[i][1];
      Real y2 = rotatedRegion[nextIndex][1];

      Real z1 = rotatedRegion[i][2];
      Real z2 = rotatedRegion[nextIndex][2];

      Real a[2] = { y1, z1 };
      Real b[2] = { y2, z2 };

      //Check the edge for intersection
      Real distance;
      if (DoLinesIntersect(a, b, position, endOfSegment, distance))
      {
         intersections++;
      }
      if (distance < minDistance)
      {
         minDistance = distance;
      }
   }
   if (intersections % 2 == 1)
   {
      minDistance *= -1;
      return true;
   }
   return false;
}


//---------------------------------------------------------------------------
//  void PlanetographicRegion::ReadLatitudeLongitudeFile()
//---------------------------------------------------------------------------
/**
 * Reads in a file that has space separated values for the region's latitude and
 * longitude.  Each line has a human-readable latitude and longitude value
 *
 *
 */
 //---------------------------------------------------------------------------
void PlanetographicRegion::ReadLatitudeLongitudeFile()
{
   // Attempt to open the file
   std::string theFile = GmatFileUtil::FindFile(filename);
   std::ifstream latLongFile(theFile);
   if (latLongFile.fail())
   {
      std::string message = "PlanetographicRegion: " + instanceName + ": Unable to open file: " + filename;
      throw GmatBaseException(message);
   }

   std::string line;
   Integer lineCount(0);

   // Read the first line
   if (GmatFileUtil::GetLine(&latLongFile, line))
   {
      lineCount = 1;
      line = GmatStringUtil::ToLower(line);
      std::string::size_type idx;
      idx = line.find("planetographicregion");
      if (idx == std::string::npos)
      {
         std::string message = "\"PlanetographicRegion\" not found on the first line of file:" + filename;
         throw GmatBaseException(message);
      }
   }

   // Read in the columnar data
   while (!latLongFile.eof())
   {
      GmatFileUtil::GetLine(&latLongFile, line);

      ++lineCount;
      //line = GmatStringUtil::Trim(line);
      line = GmatStringUtil::Trim(line, GmatStringUtil::StripType::BOTH, true, true);
      if (line.size() == 0)
         continue;

      if (line[0] == '%')
         continue;

      StringArray stringValues = Parse(line, "\t ");
      if (stringValues.size() == 0)
         continue;

      if (stringValues.size() < 2)
      {
         std::string message = "In file: " + filename + ": line: " + 
            std::to_string(lineCount) + " there is an unpaired lat/long value";
         throw GmatBaseException(message);
      }

      Integer errorCode;
      Real number;
      if (!GmatStringUtil::IsValidReal(stringValues[0], number, errorCode))
      {
         std::string message = "In file: " + filename + ": line: " + 
            std::to_string(lineCount) + ": the Latitude parameter ";
         HandleRealParseError(errorCode, message);
      }
      latitudeList.push_back(number);

      if (!GmatStringUtil::IsValidReal(stringValues[1], number, errorCode))
      {
         std::string message = "In file: " + filename + ": line: " +
            std::to_string(lineCount) + ": the Longitude parameter ";
         HandleRealParseError(errorCode, message);
      }
      longitudeList.push_back(number);
   }
   latLongFile.close();
}

//---------------------------------------------------------------------------
//  void PlanetographicRegion::HandleRealParseError(const Integer & errorCode, std::string & message)
//---------------------------------------------------------------------------
/**
 * Provides error handling after a real parameter conversion goes wrong
 *
 * @param errorCode  the errorcode returned from GmatStringUtil::IsValidReal
 * @param message    a message revealing the context of the error
 *
 */
 //---------------------------------------------------------------------------
void PlanetographicRegion::HandleRealParseError(const Integer &errorCode, std::string &message)
{
   switch (errorCode)
   {
   case -1:
   case -2:
      message = message + "is blank";
      break;
   case -3:
      message = message + "contains numbers with multiple dots or invalid scientific notation";
      break;
   case -4:
      message = message + "doesn't begin with  + , -, ., or a digit";
      break;
   case -5:
      message = message + "has multiple E or e in scientific notation";
      break;
   case -6:
      message = message + "has multiple + or - in scientific notation";
      break;
   case -7:
      message = message + "has multiple dots";
      break;
   case -8:
      message = message + "contains non-numeric characters other than E or e";
      break;
   case -9:
      message = message + "has missing numbers after + or -sign";
      break;
   }
   throw GmatBaseException(message);
}



//---------------------------------------------------------------------------
//  bool PlanetographicRegion::DoLinesIntersect(Real a[2], Real b[2], Real c[2], Real d[2], Real & distance)
//---------------------------------------------------------------------------
/**
 * Checks to see if line segment ab intersects segment cd
 *
 * @param a Rvector3 a
 * @param b Rvector3 b
 * @param c Rvector3 c
 * @param d Rvector3 d
 * @param distance returned distance between c and segment ab
 *
 * @return true if the segments intersect, false if not
 */
 //---------------------------------------------------------------------------
bool PlanetographicRegion::DoLinesIntersect(Real a[2], Real b[2], Real c[2], Real d[2], Real & distance)
{
   // The calculation of the intersection is basically thus:
   // 1) looking along ab line, points c and d are not on the same side and
   // 2) looking along cd line, points a and b are not on the same side.
   // 3) to prevent duplicates at segment ends, b is not in line with cd

   // vector subtraction
   Real ab[2] = { b[0] - a[0], b[1] - a[1] };
   Real ac[2] = { c[0] - a[0], c[1] - a[1] };
   Real ad[2] = { d[0] - a[0], d[1] - a[1] };

   Real ca[2] = { a[0] - c[0], a[1] - c[1] };
   Real cb[2] = { b[0] - c[0], b[1] - c[1] };
   Real cd[2] = { d[0] - c[0], d[1] - c[1] };

   // cross products
   Real acab = ac[0] * ab[1] - ab[0] * ac[1];
   Real adab = ad[0] * ab[1] - ab[0] * ad[1];
   Real cacd = ca[0] * cd[1] - cd[0] * ca[1];
   Real cbcd = cb[0] * cd[1] - cd[0] * cb[1];

   bool doesIntersect = (acab * adab <= 0.0 && cacd * cbcd <= 0.0);
   if (cbcd == 0.0)
      doesIntersect = false;

   // Calculate the distance from point C to line segment ab
   distance = fabs(acab / sqrt(ab[0] * ab[0] + ab[1] * ab[1]));

   return doesIntersect;
}




//---------------------------------------------------------------------------
//  Rvector3 EllipsoidToCartesian(const Rvector3 & latLongHeight, Real flattening, Real equatorialRadius)
//---------------------------------------------------------------------------
/**
 * Converts from Latitude, Longitude, and Height values to cartesian coordinates
 *
 * @param latLongHeight a vector of the latitude, longitude, and height of a point in space
 * @param flattening  The flattening ratio of the body
 * @param equatorialRadius The radius of the body at the equator
 *
 * @return a cartesian vector [X, Y, Z] of the point in space
 */
 //---------------------------------------------------------------------------
Rvector3 PlanetographicRegion::EllipsoidToCartesian(const Rvector3 & latLongHeight, Real flattening, Real equatorialRadius)
{
   Real latitude = latLongHeight[0];
   Real longitude = latLongHeight[1];
   Real height = latLongHeight[2];

   // modeling as if a circle is rotated a bit to get the flattening.
   // phi is the rotation out of the plane
   Real phi = acos(1.0 - flattening);

   // theta is the angle along the circle to achieve the latitude
   Real theta = atan(tan(latitude) * cos(phi));

   // position on ellipse
   Real r = equatorialRadius * cos(theta);
   Real z = equatorialRadius * sin(theta) * cos(phi);

   // position in space
   r += height * cos(latitude);
   z += height * sin(latitude);

   // Rotate for longitude
   Real y = r * sin(longitude);
   Real x = r * cos(longitude);

   return Rvector3(x, y, z);
}

//---------------------------------------------------------------------------
//  Rvector3 CartesianToEllipsoid(const Rvector3 & cartesian, Real flattening, Real equatorialRadius)
//---------------------------------------------------------------------------
/**
 * Converts from cartesian coordinates to Latitude, Longitude, and Height values
 *
 * @param cartesian The position of the space point (usually a satellite or spacecraft)
 * @param flattening  The flattening ratio of the body
 * @param equatorialRadius The radius of the body at the equator
 *
 * @return a vector [latitude, longitude, height] of the space point
 */
 //---------------------------------------------------------------------------
Rvector3 PlanetographicRegion::CartesianToEllipsoid(const Rvector3 & cartesian, Real flattening, Real equatorialRadius)
{

   // Calculate the longitude.
   Real longitude = atan2(cartesian[1], cartesian[0]);
   if (longitude < 0.0)
   {
      longitude += GmatMathConstants::TWO_PI;
   }

   // Distance from rotation axis to cartesian coordinate
   const Real targetRadius = sqrt(cartesian[0] * cartesian[0] + cartesian[1] * cartesian[1]);
   const Real cartesianDistance = sqrt(cartesian[2] * cartesian[2] + targetRadius * targetRadius);

   // Prepare for iteration
   Integer loopCount(0); // To ensure exit from loop
   Real latitude;
   Real height;
   Real changeInLatitude;
   Real phi = acos(1 - flattening); // angle to create the ellipse
   Real theta = atan2(cartesian[2], targetRadius);  // initial guess

   do
   {
      // Position on surface
      Real r = equatorialRadius * cos(theta);
      Real z = equatorialRadius * sin(theta) * cos(phi);

      // Height is the distance between point on surface and target.
      Real dr = targetRadius - r;
      Real dz = cartesian[2] - z;
      height = sqrt(dr * dr + dz * dz);

      //Calculate latitude
      latitude = atan(tan(theta) / cos(phi));

      // Position in space
      r += height * cos(latitude);
      z += height * sin(latitude);

      // adjust and iterate if required

      //changeInLatitude = atan2(cartesian[2], targetRadius) - atan2(z, r);// replaced 
      // asin(delta) = theta = sin(delta) for small angles
      changeInLatitude = (r * cartesian[2] - z * targetRadius) /
         (cartesianDistance * sqrt(r * r + z * z));

      theta += changeInLatitude;
   } while (changeInLatitude != 0.0 && ++loopCount <= 10);

   return Rvector3(latitude, longitude, height);
}


