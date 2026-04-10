%module gmat_py
%include "std_string.i"
%{
#include "../src/gmatutil/include/utildefs.hpp"
#include "../src/gmatutil/util/A1Date.hpp"
#include "../src/gmatutil/util/A1Mjd.hpp"
#include "../src/gmatutil/util/BaseException.hpp"
#include "../src/gmatutil/util/Date.hpp"
#include "../src/gmatutil/util/EopFile.hpp"
#include "../src/gmatutil/util/ElapsedTime.hpp"
#include "../src/gmatutil/util/GmatDefaults.hpp"
#include "../src/gmatutil/util/GmatGlobal.hpp"
#include "../src/gmatutil/util/FileUtil.hpp"
#include "../src/gmatutil/util/GregorianDate.hpp"
#include "../src/gmatutil/util/MemoryTracker.hpp"
#include "../src/gmatutil/util/MessageInterface.hpp"
#include "../src/gmatutil/util/MessageReceiver.hpp"
#include "../src/gmatutil/util/RealUtilities.hpp"
#include "../src/gmatutil/util/Rmatrix.hpp"
#include "../src/gmatutil/util/Rmatrix33.hpp"
#include "../src/gmatutil/util/Rmatrix66.hpp"
#include "../src/gmatutil/util/ArrayTemplate.hpp"
#include "../src/gmatutil/util/Rvector.hpp"
#include "../src/gmatutil/util/Rvector3.hpp"
#include "../src/gmatutil/util/Rvector6.hpp"
#include "../src/gmatutil/util/TimeSystemConverter.hpp"
#include "../src/gmatutil/util/TimeTypes.hpp"
#include "../src/gmatutil/util/UtcDate.hpp"
#include "../src/gmatutil/util/UtilityException.hpp"
%}

//Does not link on Windows
%ignore GmatRealUtil::ToString;
%ignore Rmatrix::ToString;

%include "../src/gmatutil/include/utildefs.hpp"
%include "../src/gmatutil/util/A1Date.hpp"

//Does not link on Windows
%ignore A1Mjd::J2000;
%include "../src/gmatutil/util/A1Mjd.hpp"

%include "../src/gmatutil/util/BaseException.hpp"
%include "../src/gmatutil/util/Date.hpp"
%include "../src/gmatutil/util/EopFile.hpp"
%include "../src/gmatutil/util/ElapsedTime.hpp"
%include "../src/gmatutil/util/GmatDefaults.hpp"
%include "../src/gmatutil/util/GmatGlobal.hpp"

//Does not link on Windows
%ignore GmatFileUtil::GetGmatPath;
%ignore GmatFileUtil::PrepareCompare;
%rename(IsFileNameValid) GmatFileUtil::IsValidFileName;
%include "../src/gmatutil/util/FileUtil.hpp"

%ignore GregorianDate::GetType;
%ignore GregorianDate::SetType;
%include "../src/gmatutil/util/GregorianDate.hpp"

//Does not link on Windows
%ignore MemoryTracker::Instance;
%ignore MemoryTracker::SetScript;
%ignore MemoryTracker::SetShowTrace;
%ignore MemoryTracker::Add;
%ignore MemoryTracker::Remove;
%ignore MemoryTracker::GetNumberOfTracks;
%ignore MemoryTracker::GetTracks;
%include "../src/gmatutil/util/MemoryTracker.hpp"

//Does not link on Windows
%ignore MessageInterface::MAX_MESSAGE_LENGTH;
%include "../src/gmatutil/util/MessageInterface.hpp"

%include "../src/gmatutil/util/MessageReceiver.hpp"
 //%include "../src/gmatutil/util/RealUtilities.hpp"

// friend functions cause link error so ignored
%ignore SkewSymmetric4by4(const Rvector3 &v);
%ignore TransposeTimesMatrix(const Rmatrix &m1, const Rmatrix &m2);
%ignore MatrixTimesTranspose(const Rmatrix &m1, const Rmatrix &m2);
%ignore TransposeTimesTranspose(const Rmatrix &m1, const Rmatrix &m2);
%include "../src/gmatutil/util/Rmatrix.hpp"

// friend functions cause link error so ignored
%ignore SkewSymmetric(const Rvector3& v);
%ignore TransposeTimesMatrix(const Rmatrix33& m1, const Rmatrix33& m2);
%ignore MatrixTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2); 
%ignore TransposeTimesTranspose(const Rmatrix33& m1, const Rmatrix33& m2); 
%include "../src/gmatutil/util/Rmatrix33.hpp"

// friend functions cause link error so ignored
%ignore SkewSymmetric(const Rvector6& v);
%ignore TransposeTimesMatrix(const Rmatrix66& m1, const Rmatrix66& m2);
%ignore MatrixTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2); 
%ignore TransposeTimesTranspose(const Rmatrix66& m1, const Rmatrix66& m2); 
%include "../src/gmatutil/util/Rmatrix66.hpp"

%include "../src/gmatutil/util/ArrayTemplate.hpp"

// friend functions cause link error so ignored
%ignore Outerproduct(const Rvector &v1, const Rvector &v2);
%include "../src/gmatutil/util/Rvector.hpp"
%include "../src/gmatutil/util/Rvector3.hpp"

// Does not link on Windows
%ignore Rvector6::UTIL_REAL_UNDEFINED;
%ignore Rvector6::RVECTOR6_UNDEFINED;
%include "../src/gmatutil/util/Rvector6.hpp"

%include "../src/gmatutil/util/TimeSystemConverter.hpp"
%include "../src/gmatutil/util/TimeTypes.hpp"
%include "../src/gmatutil/util/UtcDate.hpp"
%include "../src/gmatutil/util/UtilityException.hpp"
