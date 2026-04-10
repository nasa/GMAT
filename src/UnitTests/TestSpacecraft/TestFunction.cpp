//------------------------------------------------------------------------------
//                              TestFunction
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
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/04/29
//
/**
 * Unit test program for callin functions  
 */
//------------------------------------------------------------------------------

#include "TestFunction.hpp"
#include "MessageInterface.hpp"

void  printEpoch(const Spacecraft *sc)
{
   
   std::string format = sc->GetStringParameter("Epoch");

   MessageInterface::ShowMessage("\n========================================="
                                 "=========================================");

   MessageInterface::ShowMessage("\nEpoch Information....\n"
                   "ID(\"Epoch\"): %d\nText: %s\nFormat: %s\nValue: %s\n",
                   sc->GetParameterID("Epoch"),
                   sc->GetParameterText(sc->GetParameterID("Epoch")).c_str(),
                   format.c_str(), sc->GetStringParameter(format).c_str()) ;

   MessageInterface::ShowMessage(
                   "\nsc->GetStringParameter(\"TAIModJulian\"): %s",
                   sc->GetStringParameter("TAIModJulian").c_str());

   MessageInterface::ShowMessage(
                   "\nsc->GetStringParameter(\"UTCModJulian\"): %s",
                   sc->GetStringParameter("UTCModJulian").c_str());

   MessageInterface::ShowMessage(
                   "\nsc->GetStringParameter(\"TAIGregorian\"): %s",
                   sc->GetStringParameter("TAIGregorian").c_str());

   MessageInterface::ShowMessage(
                   "\nsc->GetStringParameter(\"Epoch.UTCGregorian\"): %s",
                   sc->GetStringParameter("Epoch.UTCGregorian").c_str());

   MessageInterface::ShowMessage("\nsc->GetStringParameter(\"Epoch\"): %s",
                   sc->GetStringParameter("Epoch").c_str());

   Integer id = sc->GetParameterID("Epoch");
   MessageInterface::ShowMessage("\nsc->GetStringParameter(\"%d\"): %s",
                   id, sc->GetStringParameter(id).c_str());

   MessageInterface::ShowMessage("\nsc->GetEpoch(): %f", sc->GetEpoch());

//     MessageInterface::ShowMessage("\nsc->GetRealParameter(\"Epoch\"): %f\n",
//                                    sc->GetRealParameter("Epoch"));

   MessageInterface::ShowMessage("\n========================================="
                                 "=========================================\n");


}

void printState(const std::string &title, const Real *state)
{
    std::cout << "\n--------- " << title << " ---------\n";
    for (int i=0; i < 6; i++)
        std::cout << "[" << i << "]: " << state[i] << std::endl;
}

void printState(const std::string &title, const Rvector6 state)
{
    std::cout << "\n--------- " << title << " ---------\n";
    for (int i=0; i < 6; i++)
        std::cout << "[" << i << "]: " << state.Get(i) << std::endl;
}

void  printState(const Spacecraft *s)
{
    printState("Spacecraft", s);
}

void  printState(const std::string &title, const Spacecraft *s)
{
    std::cout << "\n--------- " << title 
              << " (" << s->GetStringParameter(s->GetParameterID("StateType")) 
              << ") ---------\n";
    for (int i=1; i <= 7; i++)
        std::cout << s->GetParameterText(i) << ": " 
                  << "\t" << s->GetRealParameter(i) << std::endl;
}

void printTime(const Real time)
{
    std::ostringstream timeBuffer;
    timeBuffer.precision(9);
    timeBuffer.setf(std::ios::fixed);
    timeBuffer.str("");
    timeBuffer << time;
    std::cout << "StringTime: " << timeBuffer.str() << std::endl;
}

void printTime(const Real time,const std::string &fromTime,
               const std::string &toTime)
{
#if 0
    TimeConverter timeConverter;
    std::ostringstream timeBuffer;
    timeBuffer.precision(9);
    timeBuffer.setf(std::ios::fixed);
    timeBuffer.str("");

    Real newTime = timeConverter.Convert(time,fromTime,toTime); 
    std::cout << "\n---- Time: " << time << ", Converting from " << fromTime 
              << " to " << toTime << "---\n";
    std::cout << "RealTime:   " << newTime << std::endl;
    timeBuffer << newTime;
    std::cout << "StringTime: " << timeBuffer.str() << std::endl;
#endif
}

Real julianDate(const Integer yr, const Integer mon, const Integer day, 
                const Integer hour, const Integer min, const Real sec)
{
     Integer computeYearMon = ( 7*(yr + (Integer)(mon + 9)/12 ) )/4;
     Integer computeMonth = (275 * mon)/9;
     Real    fractionalDay = ((sec/60.0 + min)/60 + hour)/24.0; 
   
     std::cout << "julianDate's factionalDay = " << fractionalDay << std::endl;
     return ( 367*yr - computeYearMon + computeMonth + day + 
              1721013.5 + fractionalDay);
}


std::string jdToGregorian(const Real jd)
{
 
    Integer dayofMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    Real t1900 = (jd - 2415019.5)/365.25; 
    Integer year = 1900 + (Integer)t1900;

    Integer leapYears = (Integer)((year - 1900 - 1)*0.25);
   
    Real days = (jd - 2415019.5) - ((year - 1900)*365.0 + leapYears); 

    std::cout << "\nConverting to Gregorigan...\nYear: " << year 
              << ", leapsyear : " << leapYears << ", days: " << days
              << ",  Integer (days): " << Integer(days) << std::endl;
    
    if (days < 1.0)
    {
       --year;
       leapYears = (Integer)((year - 1900 -1)*0.25);
       days = (jd - 2415019.5) - ((year - 1900)*365.0 + leapYears); 
    }

    if (year%4 == 0)
       dayofMonth[1] = 29;

    Integer dayOfYear = (Integer)days;

    Integer lMonthSum = 0;
    Integer temp = 0;
    Integer month ; 

    for (month=0; month < 12; month++)
    {
        temp += dayofMonth[month]; 
        if (temp > dayOfYear) break;

        lMonthSum = temp; 
    }

    Integer day = dayOfYear - lMonthSum;
 
    std::cout << "\nmonth: " << ++month << ", dayOfYear: " << dayOfYear 
              << ", temp: " << temp << ", day = " << day << std::endl; 

    Real ut = (days - dayOfYear)*24.0;

    // TimeToHMS
    Integer hour = (Integer)ut;
    Integer min = (Integer)((ut - hour)*60);
    Real sec = (ut - hour - (min/60.0)) * 3600;

    std::cout << "\nut: " << ut << ", hour: " << hour 
              << ", min: " << min << ", sec = " << sec << std::endl; 

    UtcDate *utcDate = new UtcDate(year,month,day,hour,min,sec);
    std::cout << "\nutcDate in string: " << utcDate->ToPackedCalendarString() 
              << " , and real: " << utcDate->ToPackedCalendarReal() 
              << std::endl;

    std::cout << "\nutcDate->GetXXX, year: " << utcDate->GetYear() 
              << " , month: " << utcDate->GetMonth() << ", day:  "
              << utcDate->GetDay() << ", hour: " << utcDate->GetHour()
              << ", min: " << utcDate->GetMinute() << ", second: " 
              << utcDate->GetSecond() << std::endl;
              
    Real ymd = utcDate->GetYear() * 1.0e+04 + utcDate->GetMonth() * 1.0e+02 
               + utcDate->GetDay();
    Real hms = utcDate->GetHour() * 1.0e+07 + utcDate->GetMinute()*1.0e+05 
               + utcDate->GetSecond() * 1.0e+03;

#if 0
    Real hms = utcDate->GetHour() * 1.0e-02 + utcDate->GetMinute()  * 1.0e-04 
               + utcDate->GetSecond() * 1.0e-06;
    Real ymd_hms = ymd + hms;
    std::cout << "\nutcDate(ymd): " << ymd << " and hms: " << hms 
              << "...Final: " << ymd_hms << std::endl;
#endif

    std::ostringstream timeBuffer;
    timeBuffer.precision(0);
    timeBuffer.setf(std::ios::fixed);
    timeBuffer << ymd << "." << hms;            

    std::string stringTime= timeBuffer.str(); 
    Real realTime = atof(stringTime.c_str()); 
    std::cout << "StringTime: " << stringTime <<  " and, RealTime: "
              << realTime << std::endl;

    delete utcDate;

    return("Just testing");
}

std::string mjdToGregorian(const Real mjd)
{
    Real jd = mjd + 2430000;
    std::string newDate = jdToGregorian(jd);
    return newDate;
}
