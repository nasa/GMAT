// *** File Name : csaltdefs.hpp
// *** Created   : July 27, 2017
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For           :  Flight Dynamics Analysis Branch (Code 595)       ***
// **************************************************************************
//
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

#ifndef CSALTDEFS_HPP
#define CSALTDEFS_HPP

//#include "gmatdefs.hpp"
#include "utildefs.hpp"

#ifdef _WIN32  // Windows DLL import/export definitions

   #ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
      #ifdef CSALT_EXPORTS
         #define CSALT_API __declspec(dllexport)
      #else
         #define CSALT_API __declspec(dllimport)
      #endif
   #endif

#endif //  End of OS nits

#ifndef CSALT_API
   #define CSALT_API
#endif

// Deprecation messages are turned off for R2020a; uncomment to activate
//#ifdef __GNUC__
//#define DEPRECATED(func) func __attribute__ ((deprecated))
//#elif defined(_MSC_VER)
//#define DEPRECATED(func) __declspec(deprecated) func
//#else
//#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
//#define DEPRECATED(func) func
//#endif

#define DEPRECATED(func) func

#endif // CSALTDEFS_HPP
