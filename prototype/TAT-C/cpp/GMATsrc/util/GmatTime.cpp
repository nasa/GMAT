//$Id$
//------------------------------------------------------------------------------
//                                  GmatTime
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Tuan Dang Nguyen
// Created: 2014/07/15   GSFC-NASA
//
/**
 * This class is used to define GMAT time with a high precision. It has 2 parts:
 * The first part stores number of seconds. The second part stores fraction of 
 * seconds.
 */
//------------------------------------------------------------------------------
#include "GmatTime.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  GmatTime()
//------------------------------------------------------------------------------
GmatTime::GmatTime() :
   Days        (21545),
   Sec			(0),
   FracSec		(0.0)
{
}


//------------------------------------------------------------------------------
//  ~GmatTime()
//------------------------------------------------------------------------------
GmatTime::~GmatTime()
{
}


//------------------------------------------------------------------------------
//  GmatTime(const GmatTime& gt)
//------------------------------------------------------------------------------
GmatTime::GmatTime(const GmatTime& gt):
   Days        (gt.Days),
   Sec			(gt.Sec),
   FracSec		(gt.FracSec)
{
}


//------------------------------------------------------------------------------
//  GmatTime(const Real mjd)
//------------------------------------------------------------------------------
GmatTime::GmatTime(const Real mjd)
{
   // Take out sign of input value mjd
   Real time = mjd;
   Integer sign = 1;
   if (time < 0.0)
   {
      sign = -1;
      time = -time;
   }

   // Specify number of days, seconds of day, and fraction of second
   Days = (long)time;
   Real dayFrac = time - Days;
   Sec = (long)(dayFrac*86400);
   FracSec = dayFrac*86400 - Sec;
   if (sign == -1)
   {
      Days = -Days;
      Sec = -Sec;
      FracSec = -FracSec;
   }

   // Adjust value
   // Set GmatTime.FracSec in range [0,1)
   //if (FracSec >= 1.0)
   //{
   //   FracSec -= 1.0;
   //   ++Sec;
   //}
   //else if (FracSec < 0.0)
   //{
   //   FracSec += 1.0;
   //   --Sec;
   //}

   //long d1 = (long)(abs(FracSec) / 86400);
   long d1 = (long)(((FracSec < 0.0)? -FracSec: FracSec) / 86400);
   if (FracSec >= 0.0)
   {
      if (d1 != 0)
      {
         FracSec -= d1;
         Sec += d1;
      }
   }
   else
   {
      FracSec += (d1 + 1);
      Sec -= (d1 + 1);
   }

   // Set GmatTime.Sec in range [0,86400)
   //long d = (long)(abs(Sec) / 86400);
   long d = (long)(((Sec < 0)?-Sec:Sec) / 86400);

   if (Sec >= 0)
   {
      if (d != 0)
      {
         Sec  -= (d * 86400);
         Days += d;
      }
   }
   else
   {
      Sec  += ((d + 1) * 86400);
      Days -= (d + 1);
   }

}


//------------------------------------------------------------------------------
//  GmatTime& operator=(const GmatTime& gt)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator=(const GmatTime& gt)
{
   if (this != &gt)
   {
      Days     = gt.Days;
	   Sec		= gt.Sec;
	   FracSec	= gt.FracSec;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatTime& operator=(const Real mjd)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator=(const Real mjd)
{
   GmatTime gt(mjd);
   FracSec = gt.GetFracSec();
   Sec     = gt.GetSec();
   Days    = gt.GetDays();

   return *this;
}


//------------------------------------------------------------------------------
//  GmatTime operator+(const GmatTime& gt) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator+(const GmatTime& gt) const
{
   GmatTime gt1(*this);

   // Adding 2 number
   gt1.FracSec += gt.FracSec;
   gt1.Sec     += gt.Sec;
   gt1.Days    += gt.Days;

   // Adjust value
   // Set GmatTime.FracSec in range [0,1)
   if (gt1.FracSec >= 1.0)
   {
      gt1.FracSec -= 1.0;
      ++gt1.Sec;
   }
   else if (gt1.FracSec < 0.0)
   {
      gt1.FracSec += 1.0;
      --gt1.Sec;
   }

   // Set GmatTime.Sec in range [0,86400)
   //long d = (long)(abs(gt1.Sec) / 86400);
   long d = (long)(((gt1.Sec < 0) ? -gt1.Sec : gt1.Sec) / 86400);
   if (gt1.Sec >= 0)
   {
      if (d != 0)
      {
         gt1.Sec  -= (d * 86400);
         gt1.Days += d;
      }
   }
   else
   {
      gt1.Sec  += ((d + 1) * 86400);
      gt1.Days -= (d + 1);
   }

   return gt1;
}


//------------------------------------------------------------------------------
// GmatTime operator+(const Real mjd) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator+(const Real mjd) const
{
   GmatTime gt(mjd);
   GmatTime gt1(*this);
   GmatTime result = (gt1 + gt);
   return result;
}


//------------------------------------------------------------------------------
//  const GmatTime& operator+=(const GmatTime& gt)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator+=(const GmatTime& gt)
{
   (*this) = (*this) + gt;

   return *this;
}


//------------------------------------------------------------------------------
// const GmatTime& ::operator+=(const Real mjd)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator+=(const Real mjd)
{
   (*this) = ((*this) + GmatTime(mjd));

   return *this;
}


//------------------------------------------------------------------------------
//  GmatTime operator-(const GmatTime& gt) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator-(const GmatTime& gt) const
{
   GmatTime gt1(*this);

   // Subtract
   gt1.FracSec -= gt.FracSec;
   gt1.Sec     -= gt.Sec;
   gt1.Days    -= gt.Days;

   // Adjust value
   // Set GmatTime.FracSec in range [0,1)
   //if (gt1.FracSec >= 1.0)
   //{
   //   gt1.FracSec -= 1.0;
   //   ++gt1.Sec;
   //}
   //else if (gt1.FracSec < 0.0)
   //{
   //   gt1.FracSec += 1.0;
   //   --gt1.Sec;
   //}

   //long d = (long)(abs(gt1.FracSec));
   long d = (long)((gt1.FracSec < 0.0) ? -gt1.FracSec : gt1.FracSec);
   if (gt1.FracSec >= 0.0)
   {
      if (d != 0)
      {
         gt1.FracSec -= d;
         gt1.Sec += d;
      }
   }
   else
   {
      gt1.FracSec += (d + 1);
      gt1.Sec -= (d + 1);
   }

   // Set GmatTime.Sec in range [0,86400)
   //d = (long)(abs(gt1.Sec) / 86400);
   d = (long)(((gt1.Sec < 0) ? -gt1.Sec : gt1.Sec) / 86400);
   if (gt1.Sec >= 0)
   {
      if (d != 0)
      {
         gt1.Sec  -= (d * 86400);
         gt1.Days += d;
      }
   }
   else
   {
      gt1.Sec  += ((d + 1) * 86400);
      gt1.Days -= (d + 1);
   }

   return gt1;
}


//------------------------------------------------------------------------------
// GmatTime operator-(const Real mjd) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator-(const Real mjd) const
{
   GmatTime gt(mjd);
   GmatTime gt1(*this);
   GmatTime result = gt1 - gt;

   return result;
}


//------------------------------------------------------------------------------
//  const GmatTime& operator-=(const GmatTime& gt)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator-=(const GmatTime& gt)
{
   (*this) = ((*this) - gt);

   return *this;
}


//------------------------------------------------------------------------------
// const GmatTime& ::operator-=(const Real mjd)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator-=(const Real mjd)
{
   (*this) = ((*this) - mjd);

   return *this;
}


//------------------------------------------------------------------------------
// GmatTime GmatTime::operator*(const Real num) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator*(const Real num) const
{
   GmatTime gt1(Sec*num);
   GmatTime gt2(FracSec*num);
   GmatTime result = gt1 + gt2;

   return result;
}


//------------------------------------------------------------------------------
// GmatTime operator/(const Real num) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator/(const Real num) const
{
   GmatTime result;
   GmatTime gt(*this);

   // Specify Days
   result.Days = (long)(gt.Days / num);

   // Specify Sec
   Real remainDays = gt.Days - result.Days * num;
   result.Sec = (long)((gt.Sec + remainDays * 86400)/ num); 

   // Specify FracSec
   Real remainSec = gt.Sec + remainDays * 86400 - result.Sec * num;
   result.FracSec = gt.FracSec + remainSec;


   // Adjust value
   // Set GmatTime.FracSec in range [0,1)
   //long d1 = (long)(abs(result.FracSec));
   long d1 = (long)((result.FracSec < 0.0) ? -result.FracSec : result.FracSec);
   if (result.FracSec >= 0.0)
   {
      if (d1 != 0)
      {
         result.FracSec -= d1;
         result.Sec += d1;
      }
   }
   else 
   {
      result.FracSec += (d1+1);
      result.Sec -= (d1+1);
   }

   // Set GmatTime.Sec in range [0,86400)
   //long d = (long)(abs(result.Sec) / 86400);
   long d = (long)(((result.Sec < 0) ? -result.Sec : result.Sec) / 86400);
   if (result.Sec >= 0)
   {
      if (d != 0)
      {
         result.Sec -= (d * 86400);
         result.Days += d;
      }
   }
   else
   {
      result.Sec += ((d + 1) * 86400);
      result.Days -= (d + 1);
   }

   return result;
}



//------------------------------------------------------------------------------
// bool GmatTime::operator==(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator==(const GmatTime& gt) const
{
   GmatTime time = gt;
   if ((Days == time.Days) && (Sec == time.Sec) && (FracSec == time.FracSec))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool GmatTime::operator!=(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator!=(const GmatTime& gt) const
{
   return !((*this) == gt);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator<(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator<(const GmatTime& gt) const
{
   GmatTime time = gt - (*this);
   if (time.GetTimeInSec() > 0.0)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool GmatTime::operator>(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator>(const GmatTime& gt) const
{
   GmatTime time = (*this) - gt;
   if (time.GetTimeInSec() > 0.0)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool GmatTime::operator<=(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator<=(const GmatTime& gt) const
{
   return !((*this) > gt);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator>=(const GmatTime& gt) const
//------------------------------------------------------------------------------
bool GmatTime::operator>=(const GmatTime& gt) const
{
   return !((*this) < gt);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator==(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator==(const Real mjd) const
{
   GmatTime time(mjd);
   return ((*this) == time);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator!=(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator!=(const Real mjd) const
{
   return !((*this) == mjd);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator<(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator<(const Real mjd) const
{
   GmatTime time(mjd);
   return (*this) < time;
}


//------------------------------------------------------------------------------
// bool GmatTime::operator>(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator>(const Real mjd) const
{
   GmatTime time(mjd);
   return (*this) > time;
}


//------------------------------------------------------------------------------
// bool GmatTime::operator<=(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator<=(const Real mjd) const
{
   GmatTime time(mjd);
   return ((*this) <= time);
}


//------------------------------------------------------------------------------
// bool GmatTime::operator>=(const Real mjd) const
//------------------------------------------------------------------------------
bool GmatTime::operator>=(const Real mjd) const
{
   GmatTime time(mjd);
   return ((*this) >= time);
}


GmatTime* GmatTime::Clone()
{
   return new GmatTime(*this); 
}


Real GmatTime::GetMjd() const
{
   return (Days + (Sec+FracSec)/GmatTimeConstants::SECS_PER_DAY);  
}


void GmatTime::SetTimeInSec(const Real sec)
{
   // Take out sign of input value sec
   Integer sign = 1;
   Real iVal = sec;
   if (sec < 0.0)
   {
      iVal = -iVal;
      sign = -1;
   }

   // Specify second and fraction of second
   long numdays = (long)(iVal / 86400);                  // unit: day
   Real seconds = iVal - numdays*86400;                  // unit: sec
   long s       = (long)seconds;                         // unit: sec
   Real fs      = seconds - s;                           // unit: sec

   // Add sign to it
   if (sign == 1)
   {
      Days = numdays;
      Sec = s;
      FracSec = fs;
   }
   else
   {
      FracSec = -fs;
      Sec = -s;
      Days = -numdays;
   }

}


Real GmatTime::GetTimeInSec() const
{ 
   return (Days*GmatTimeConstants::SECS_PER_DAY + Sec +FracSec);
}


// John version
std::string GmatTime::ToString() const
{
   char epochbuffer[40];

   Real timeInSecs = Sec + FracSec;

   // Create a constant in seconds equivalent to 1e-7 day
   // This is used to split up floating point calculations
   // to maintain precision at the FracSecond level
   const Real DAY_FRAC = GmatTimeConstants::SECS_PER_DAY / 1.0e7;

   // Split up seconds of day into even multiples of 1e-7 days and the remainder
   Real remainderSec     = GmatMathUtil::Mod(Sec, DAY_FRAC);
   Real remainderFracSec = GmatMathUtil::Mod(FracSec, DAY_FRAC);
   
   // Get rid of noise in mod calculation for Sec
   remainderSec = GmatMathUtil::Round(GmatMathUtil::Mod(remainderSec, 1.0)*1e5)/1e5;

   Real remainder = remainderSec + remainderFracSec;
   if (remainder >= DAY_FRAC)
      remainder -= DAY_FRAC;

   // firstValue is the time in seconds that can be represented in
   // the first 7 digits to the right of the decimal when converted to days
   Real firstValue = timeInSecs - remainder;
   Real firstValueFrac = GmatMathUtil::Round(GmatMathUtil::Mod(firstValue, 1.0)*1e5)/1e5; // Remove precision noise
   
   // secondValue is the time in seconds that cannot be represented in
   // the first 7 digits to the right of the decimal when converted to days
   Real secondValue = FracSec - firstValueFrac;
   
   if (FracSec < firstValueFrac)
       secondValue = FracSec + (1 - firstValueFrac);

   // Get first 7 digits in fraction of day
   sprintf(epochbuffer, "%.7lf", firstValue / GmatTimeConstants::SECS_PER_DAY);
   Integer index = 0;
   while (epochbuffer[index] != '.')
      ++index;
   std::string s1(&epochbuffer[index + 1]);

   // Get remaining digits in fraction of day
   sprintf(epochbuffer, "%.22lf", secondValue / GmatTimeConstants::SECS_PER_DAY);
   index = 0;
   while (epochbuffer[index] != '.')
      ++index;
   std::string s2(&epochbuffer[index + 8]);

   std::stringstream ss;
   ss << Days << "." << s1 << s2;

   return ss.str();
}


bool GmatTime::SetMjdString(std::string sMjd)
{
   std::string::size_type pos = sMjd.find_first_of('.');

   // Convert integer to floating point
   if (pos == sMjd.npos)
   {
      sMjd = sMjd + ".";
      pos = sMjd.find_first_of('.'); // update pos value
   }

   //Add trailing zeros to avoid an error when using fracDayStr.substr(0, 9)
   sMjd = sMjd + "000000000";

   std::string dayStr = sMjd.substr(0, pos);
   std::string fracDayStr = "0" + sMjd.substr(pos);
   std::string secPart1Str = fracDayStr.substr(0, 9);
   std::string secPart2Str = "0.0000000" + fracDayStr.substr(9);
   
   Days = (long)(atoi(dayStr.c_str()));
   Sec = 0;
   FracSec = 0.0;

   Real secPart1 = atof(secPart1Str.c_str()) * GmatTimeConstants::SECS_PER_DAY;
   Real secPart2 = atof(secPart2Str.c_str()) * GmatTimeConstants::SECS_PER_DAY;

   Sec = GmatMathUtil::Floor(secPart1);
   FracSec = GmatMathUtil::Mod(secPart1, 1);
   FracSec = GmatMathUtil::Round(FracSec*1e5)/1e5;
   
   if ((FracSec + secPart2) >= 1)
   {
      FracSec = (FracSec - 1) + secPart2;
      Sec++;
   }
   else
   {
      FracSec = FracSec + secPart2;
   }

   if (Sec > GmatTimeConstants::SECS_PER_DAY)
   {
      Sec -= GmatTimeConstants::SECS_PER_DAY;
      Days++;
   }

   return true;
}


bool GmatTime::IsNearlyEqual(GmatTime gt, Real tolerance)
{
   bool retVal = false;
   Real time = ((*this) - gt).GetTimeInSec();
   if (time < 0.0)
      time = -time;
   if (time < tolerance)
      retVal = true;

   return retVal;
}


const GmatTime& GmatTime::AddSeconds(const Real sec)
{
   GmatTime gt;
   gt.SetTimeInSec(sec);
   (*this) = ((*this) + gt);
   return *this;
}


const GmatTime& GmatTime::SubtractSeconds(const Real sec)
{
   GmatTime gt;
   gt.SetTimeInSec(sec);
   (*this) = ((*this) - gt);
   return *this;
}
