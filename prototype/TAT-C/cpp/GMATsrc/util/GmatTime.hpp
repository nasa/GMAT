//$Id$
//------------------------------------------------------------------------------
//                                    GmatTime
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Tuan Dang Nguyen
// Created: 2014/07/15
/**
 * This class is used to define GMAT time with a high precision. It has 2 parts:
 * The first part stores number of seconds. The second part stores fraction of 
 * seconds.
 */
//------------------------------------------------------------------------------
#ifndef GmatTime_hpp
#define GmatTime_hpp

#include "utildefs.hpp"

class GMATUTIL_API GmatTime
{
public:
   GmatTime();
   virtual ~GmatTime();
   GmatTime(const GmatTime& gt);
   GmatTime(const Real mjd);

   // Arithmatic Operators: + and -
   GmatTime operator+(const GmatTime& gt) const;
   GmatTime operator-(const GmatTime& gt) const;
   GmatTime operator+(const Real mjd) const;
   GmatTime operator-(const Real mjd) const;
   GmatTime operator*(const Real num) const;
   GmatTime operator/(const Real num) const;

   // Assign operator
   const GmatTime& operator=(const GmatTime& gt);
   const GmatTime& operator+=(const GmatTime& gt);
   const GmatTime& operator-=(const GmatTime& gt);

   const GmatTime& operator=(const Real mjd);
   const GmatTime& operator+=(const Real mjd);
   const GmatTime& operator-=(const Real mjd);

   // Logic operators
   bool operator==(const GmatTime& gt) const;
   bool operator!=(const GmatTime& gt) const;
   bool operator<(const GmatTime& gt) const;
   bool operator>(const GmatTime& gt) const;
   bool operator<=(const GmatTime& gt) const;
   bool operator>=(const GmatTime& gt) const;

   bool operator==(const Real mjd) const;
   bool operator!=(const Real mjd) const;
   bool operator<(const Real mdj) const;
   bool operator>(const Real mjd) const;
   bool operator<=(const Real mjd) const;
   bool operator>=(const Real mjd) const;

   virtual GmatTime* Clone();

   virtual void SetTimeInSec(const Real sec);
   GmatEpoch GetMjd() const;
   Real GetTimeInSec() const;


   long GetDays() const {return Days;};
   long GetSec() const {return Sec;};
   Real GetFracSec() const {return FracSec;};
   void SetDays(long days){ Days = days; };
   void SetSec(long sec){ Sec = sec; };
   void SetFracSec(Real fsec){ FracSec = fsec; };
   bool SetMjdString(std::string sMjd);

   std::string ToString() const;
   bool IsNearlyEqual(GmatTime gt, Real tolerance);
   const GmatTime& AddSeconds(const Real sec);
   const GmatTime& SubtractSeconds(const Real sec);

protected:
   long Days;     // number of days
   long Sec;		// time in seconds
   Real FracSec;	// time in fraction of seconds   
};

#endif //GmatTime_hpp
