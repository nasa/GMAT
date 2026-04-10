//$Id$
//------------------------------------------------------------------------------
//                             GregorianDate 
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
// Author:  Joey Gurganus
// Created: 2004/07/14
//
/**
 * Definition of the Gregorian class base
 */
//------------------------------------------------------------------------------

#include "GregorianDate.hpp"
#include "TimeTypes.hpp"
#include "DateUtil.hpp"         // for IsValidTime()
#include "MessageInterface.hpp"
#include <algorithm>

//#define DEBUG_GREGORIAN_VALIDATE 1

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  GregorianDate()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 */
//---------------------------------------------------------------------------
GregorianDate::GregorianDate()
{
   SetDate("01 Jan 2000 12:00:00.000");
}

//---------------------------------------------------------------------------
//  GregorianDate(const std::string &str)
//---------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 *
 * @param <str>   Given String of Date
 */
//---------------------------------------------------------------------------
GregorianDate::GregorianDate(const std::string &str)
{
   SetDate(str);   
}

//---------------------------------------------------------------------------
//  GregorianDate(Date *newDate, Integer format = 1)
//---------------------------------------------------------------------------
/**
 * Creates default constructor with new Date.
 */
//---------------------------------------------------------------------------
GregorianDate::GregorianDate(Date *newDate, Integer format)
{
   outFormat = format;
   Initialize("");
   SetDate(newDate, format);
}


//---------------------------------------------------------------------------
//  ~GregorianDate()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
GregorianDate::~GregorianDate()
{
}

    
//---------------------------------------------------------------------------
//  std::string GetDate() const
//---------------------------------------------------------------------------
/**
 * Get the date in string.
 *
 * @return the date in string. 
 */
//---------------------------------------------------------------------------
std::string GregorianDate::GetDate() const
{
   return stringDate;
}

//---------------------------------------------------------------------------
//  bool SetDate(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Set the date in string. 
 *
 * @return        return flag indicator (true = successful; otherwise, false) 
 */
//---------------------------------------------------------------------------
bool GregorianDate::SetDate(const std::string &str) 
{
    Initialize(str); // for now....
    ParseOut(str);

    return true;
}


//---------------------------------------------------------------------------
//  bool SetDate(Date *newDate, Integer format = 1) 
//---------------------------------------------------------------------------
/**
 * Set the new date in Date
 *
 * @param   format    1 = "01 Jan 2000 11:59:28.000"
 *                    2 = "2000-01-01T11:59:28.000"
 * @return  return flag indicator (true = successful; otherwise, false) 
 * 
 */
//---------------------------------------------------------------------------
bool GregorianDate::SetDate(Date *newDate, Integer format) 
{
   // Check validity on date first then convert to string
   if (!newDate->IsValid())
   { 
      MessageInterface::ShowMessage("Warning:  Can't set date to string\n");
      isValid = false;
      return isValid;
   }  
   
   std::string temp;
   
   // Convert the date format in string
   if (format == 2)
   {
      temp += NumToString(newDate->GetYear()) + "-";
      temp += NumToString(newDate->GetMonth()) + "-";
      temp += NumToString(newDate->GetDay()) + "T";
      temp += NumToString(newDate->GetHour()) + ":";
      temp += NumToString(newDate->GetMinute()) + ":";
      temp += NumToString(newDate->GetSecond());
      stringDate = temp;
   }
   else
   {
      temp = NumToString(newDate->GetDay());
      temp += " " + GetMonthName(newDate->GetMonth()) + " "; 
      temp += NumToString(newDate->GetYear()) + " ";
      temp += NumToString(newDate->GetHour()) + ":";
      temp += NumToString(newDate->GetMinute()) + ":";
      temp += NumToString(newDate->GetSecond());
      stringDate = temp;
   }
   
   stringYMDHMS = newDate->ToPackedCalendarString();
   type = "Gregorian";
   
   isValid = true;
   return isValid;
}

//---------------------------------------------------------------------------
//  std::string GetYMDHMS() const
//---------------------------------------------------------------------------
/**
 * Get YYYYMMDD.HHMMSSmmm from Gregorian format in string 
 *
 * @return    string in YYYYMMDD.HHMMSSmmm 
 * 
 */
//---------------------------------------------------------------------------
std::string GregorianDate::GetYMDHMS() const
{
   return stringYMDHMS;
}

//---------------------------------------------------------------------------
//  bool IsValid() const 
//---------------------------------------------------------------------------
/**
 * Determines if the date is valid or not.
 *
 * @return        return flag indicator (true = valid; otherwise, false) 
 */
//---------------------------------------------------------------------------
bool GregorianDate::IsValid() const
{
    return isValid;
}

//---------------------------------------------------------------------------
//  static bool IsValid(const std::string &greg)
//---------------------------------------------------------------------------
/**
 * Determines if input date is valid or not.
 *   Valid format is dd mmm yyyy hh:mm:ss.mmm.
 *   For example, 01 Jan 2000 12:00:00.000
 *
 * @param  greg  input gregorian string
 *
 * @return true if time is in valid Gregorian format; otherwise, false
 */
//---------------------------------------------------------------------------
bool GregorianDate::IsValid(const std::string &greg)
{
   return DateUtil::IsValidGregorian(greg);
}

//-------------------------------------
// private methods
//-------------------------------------

//---------------------------------------------------------------------------
//  void Initialize(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Initializes values of data method
 */
//---------------------------------------------------------------------------
void GregorianDate::Initialize(const std::string &str)
{
   stringDate = str;
   stringYMDHMS = "";
   type = "Gregorian";
   isValid = false;
}

//---------------------------------------------------------------------------
//  void ParseOut(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Parse out the string and check validity
 */
//---------------------------------------------------------------------------
void GregorianDate::ParseOut(const std::string &str) 
{
   #if DEBUG_GREGORIAN_VALIDATE
   MessageInterface::ShowMessage("GregorianDate::ParseOut() str=%s\n", str.c_str());
   #endif
   
   // Check if non-empty string then parse out; otherwise, nothing. 
   if (str != "")
   {
      StringTokenizer dateToken(str, " ");

      if (dateToken.CountTokens() == 4)
      {
         std::istringstream iss;
         Integer dayNum, yearNum;

         // Get the number
         iss.str(dateToken.GetToken(0)); 
         iss >> dayNum;

         // Get the year
         yearNum = ToInteger(dateToken.GetToken(2)); 

         // Check validity for year
         if (dateToken.GetToken(2).length() != 4 || yearNum < 1950)
            return;
//            throw GregorianDateException();

         // Check validity for month 
//          std::vector<std::string>::iterator pos;
//          pos = find(monthName.begin(),monthName.end(),
//                     dateToken.GetToken(1));

//          if (pos == monthName.end())
//             return;
// //            throw GregorianDateException();

         bool monthFound = false;
         Integer monthNum = 0;
         for (int i=0; i<12; i++)
         {
            if (GmatTimeConstants::MONTH_NAME_TEXT[i] == dateToken.GetToken(1))
            {
               monthFound = true;
               monthNum = i+1;
               break;
            }
         }
         
         if (!monthFound)
            return;

//          Integer monthNum;
//          monthNum = (Integer) distance(monthName.begin(),pos) + 1;

         std::string tempYMD;
         tempYMD = dateToken.GetToken(2) + NumToString(monthNum); 
         if (dateToken.GetToken(0).length() == 1)
            tempYMD += "0";
         tempYMD += dateToken.GetToken(0) + "."; 

         // Start with time
         StringTokenizer timeToken(dateToken.GetToken(3),":");  

         if (timeToken.CountTokens() == 3)
         {
//            // Check length of time format
//            if (timeToken.GetToken(0).length() != 2 ||
//                timeToken.GetToken(1).length() != 2 ||
//                timeToken.GetToken(2).length() != 6)
//            {
//               MessageInterface::ShowMessage(
//                  "\nWarning: invalid Gregorian format with time"); 
//               return;
//            }

            // Check length of the hour format
            if (timeToken.GetToken(0).length() != 2)
            {
               MessageInterface::ShowMessage(
                  "\nWarning: invalid Gregorian time for hours format(HH)\n"); 
               return;
            }
            // Check length of the minute format
            if (timeToken.GetToken(1).length() != 2)
            {
               MessageInterface::ShowMessage(
                  "\nWarning: invalid Gregorian time for minutes format(MM)\n"); 
               return;
            }
            if ((timeToken.GetToken(2).length() < 2) ||
               (timeToken.GetToken(2).length() > 6))
            {
               MessageInterface::ShowMessage(
                  "\nWarning: invalid Gregorian time for seconds format(SS.mmm)\n"); 
               return;
            }
            
            // Get hour and minute
            Integer hour, minute;
            hour = ToInteger(timeToken.GetToken(0)); 
            minute = ToInteger(timeToken.GetToken(1)); 
            
            tempYMD += timeToken.GetToken(0) + timeToken.GetToken(1);
            
            // Finally start with seconds
            std::string strSeconds = timeToken.GetToken(2);
            timeToken.Set(strSeconds,"."); 

            // Check time format in second
            if ((timeToken.CountTokens() > 2) || 
               ((timeToken.CountTokens() == 1) &&
                (timeToken.GetToken(0).length() != 2)) ||
               ((timeToken.CountTokens() == 2) &&
                 ((timeToken.GetToken(0).length() != 2) ||
                  (timeToken.GetToken(1).length() > 3))))
            {
               MessageInterface::ShowMessage(
                  "\nWarning: invalid Gregorian format with seconds"); 
               return;
            }
            
            if (timeToken.CountTokens() == 1)
               tempYMD += timeToken.GetToken(0) + "000";
            else if (timeToken.GetToken(1).length() == 1)
               tempYMD += timeToken.GetToken(0) + timeToken.GetToken(1) + "00";
            else if (timeToken.GetToken(1).length() == 2)
               tempYMD += timeToken.GetToken(0) + timeToken.GetToken(1) + "0";
            else 
               tempYMD += timeToken.GetToken(0) + timeToken.GetToken(1);

            // Get real number in seconds
            Real second = ToReal(strSeconds); 
            #if DEBUG_GREGORIAN_VALIDATE
            MessageInterface::ShowMessage
               ("GregorianDate::ParseOut() second=%.10f\n", second);
            #endif
            
            // Finally check validity for the date  
            if (!IsValidTime(yearNum,monthNum,dayNum,hour,minute,second))
            {
               MessageInterface::ShowMessage(
                  "\nWarning: invalid Gregorian format from DateUtil"); 
               return;
            } 
            
            stringYMDHMS = tempYMD;
            //MessageInterface::ShowMessage
            //   ("==> GregorianDate::ParseOut() stringYMDHMS=%s\n",
            //    stringYMDHMS.c_str());
         }                    
         isValid = true;
      }
      else
      {
         MessageInterface::ShowMessage(
             "\nWarning: invalid Gregorian format: %s",str.c_str());  
      }
   }
   else
   {
       MessageInterface::ShowMessage(
         "\nWarning: invalid Gregorian format since there is no value.");  
   } 

}

//---------------------------------------------------------------------------
//  std::string NumToString(const Integer num) 
//---------------------------------------------------------------------------
/**
 * Return the string from number.
 * 
 * @param <num>    Given number of date including day and time
 * 
 * @return     Return the string from the number.
 */
//---------------------------------------------------------------------------
std::string GregorianDate::NumToString(const Integer num)
{
   std::string temp("");

   if (num < 10)
      temp = "0";

   std::ostringstream ss;
   ss << num;

   return (temp += ss.str());
}

//---------------------------------------------------------------------------
//  std::string NumToString(const Real num) 
//---------------------------------------------------------------------------
/**
 * Return the string from real number.
 * 
 * @param <num>    Given real number 
 * 
 * @return     Return the string from the real number.
 */
//---------------------------------------------------------------------------
std::string GregorianDate::NumToString(const Real num)
{
   std::string temp("");

   if (num < 10.0)
      temp = "0";

   std::ostringstream ss;
   ss.precision(3);
   ss.setf(std::ios::fixed);
   ss << num;

   return (temp += ss.str());
}

//---------------------------------------------------------------------------
//  Integer ToInteger(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Return the integer number from string.
 * 
 * @param <str>    Given string
 * 
 * @return     Return the integer number from string.
 */
//---------------------------------------------------------------------------
Integer GregorianDate::ToInteger(const std::string &str)
{
   Integer num;
   std::istringstream iss;
   iss.str("");
   iss.str(str); 
   iss >> num;

   return num;
}

//---------------------------------------------------------------------------
//  Real ToReal(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Return the integer number from string.
 * 
 * @param <str>    Given string
 * 
 * @return     Return the real number from string.
 */
//---------------------------------------------------------------------------
Real GregorianDate::ToReal(const std::string &str)
{
   Real num;
   std::istringstream iss("");

   iss.precision(3);
   iss.setf(std::ios::fixed);
   iss.str(str); 
   iss >> num;

   return num;
}

//---------------------------------------------------------------------------
//  std::string GetMonthName(const Integer month) 
//---------------------------------------------------------------------------
/**
 * Return the string from number.
 * 
 * @param <month> Given number of and month
 * 
 * @return        Return the string from the number.
 */
//---------------------------------------------------------------------------
std::string GregorianDate::GetMonthName(const Integer month)
{
   if (month < 1 || month > 12) 
   {
      MessageInterface::ShowMessage("Warning:  bad month!");
      isValid = false;
      // @todo - will add new exception for throwing
      return "";
   }

   return GmatTimeConstants::MONTH_NAME_TEXT[month-1];
}
