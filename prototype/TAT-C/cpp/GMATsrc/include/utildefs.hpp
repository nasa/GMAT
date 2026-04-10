//$Id$
// *** File Name : gmatdefs.h
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
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
// **************************************************************************
/**
 * Types and definitions used in the GmatUtil library
 */
// **************************************************************************

#ifndef UTILDEFS_HPP
#define UTILDEFS_HPP


#include <string>               // For std::string
#include <cstring>              // Resolves issues for GCC 4.3
#include <vector>               // For std::vector
#include <map>                  // For std::map
#include <stack>                // for std::stack
#include <list>                 // To fix VS DLL import/export issues


#ifdef _WIN32  // Windows
   #ifdef _MSC_VER  // Microsoft Visual C++

      #define WIN32_LEAN_AND_MEAN  // Exclude rarely-used stuff from Windows headers
      // Windows Header File entry point:
      #include <windows.h>

      #define  _USE_MATH_DEFINES  // makes Msoft define pi

   #endif  // End Microsoft C++ specific block

   #ifdef _DYNAMICLINK  // Only used for Visual C++ Windows DLLs
      #ifdef GMATUTIL_EXPORTS
         #define GMATUTIL_API __declspec(dllexport)
      #else
         #define GMATUTIL_API __declspec(dllimport)
      #endif

      // Provide the storage class specifier (extern for an .exe file, null
      // for DLL) and the __declspec specifier (dllimport for .an .exe file,
      // dllexport for DLL).
      // You must define EXP_STL when compiling the DLL.
      // You can now use this header file in both the .exe file and DLL - a
      // much safer means of using common declarations than two different
      // header files.
      #ifdef EXP_STL
      #    define DECLSPECIFIER __declspec(dllexport)
      #    define EXPIMP_TEMPLATE
      #else
      #    define DECLSPECIFIER __declspec(dllimport)
      #    define EXPIMP_TEMPLATE extern
      #endif

      #define EXPORT_TEMPLATES
   #endif
#endif //  End of OS nits

#ifndef GMATUTIL_API
   #define GMATUTIL_API
#endif


typedef double          Real;              // 8 byte float
typedef int             Integer;           // 4 byte signed integer
typedef unsigned char   Byte;              // 1 byte
typedef unsigned int    UnsignedInt;       // 4 byte unsigned integer

typedef std::vector<Real>        RealArray;
typedef std::vector<Integer>     IntegerArray;
typedef std::vector<UnsignedInt> UnsignedIntArray;
typedef std::vector<std::string> StringArray;
typedef std::vector<bool>        BooleanArray;

class GmatBase;        // Forward reference for ObjectArray
class ElementWrapper;  // Forward reference for ElementWrapper
class A1Mjd;           // Forward reference for A1Mjd (epoch)
class Rvector6;        // Forward reference for Rvector6 (ephem state)

typedef std::vector<GmatBase*>                 ObjectArray;
typedef std::vector<ElementWrapper*>           WrapperArray;
typedef std::vector<Rvector6*>                 StateArray;
typedef std::vector<A1Mjd*>                    EpochArray;
typedef std::map<std::string, Integer>         IntegerMap;
typedef std::map<std::string, UnsignedInt>     ColorMap;
typedef std::map<std::string, GmatBase*>       ObjectMap;
typedef std::map<std::string, ElementWrapper*> WrapperMap;
typedef std::stack<ObjectMap*>                 ObjectMapStack;

/// GMAT's epoch representation; eventually a struct holding MJ day & sec of day
typedef Real GmatEpoch;


/// GMAT's Radians representation
typedef Real Radians;


namespace Gmat
{
   /**
    * The list of data types
    *
    * This list needs to be synchronized with the GmatBase::PARAM_TYPE_STRING
    * list found in base/Foundation/GmatBase.cpp
    */
   enum ParameterType
   {
      INTEGER_TYPE,
      UNSIGNED_INT_TYPE,
      UNSIGNED_INTARRAY_TYPE,
      INTARRAY_TYPE,
      REAL_TYPE,
      REAL_ELEMENT_TYPE,
      STRING_TYPE,
      STRINGARRAY_TYPE,
      BOOLEAN_TYPE,
      BOOLEANARRAY_TYPE,
      RVECTOR_TYPE,
      RMATRIX_TYPE,
      TIME_TYPE,
      OBJECT_TYPE,
      OBJECTARRAY_TYPE,
      ON_OFF_TYPE,
      ENUMERATION_TYPE,
      FILENAME_TYPE,
      COLOR_TYPE,
      GMATTIME_TYPE,
      TypeCount,
      UNKNOWN_PARAMETER_TYPE = -1,
      PARAMETER_REMOVED = -3,   // For parameters will be removed in the future
   };

   enum MessageType
   {
      ERROR_ = 10, //loj: cannot have ERROR
      WARNING_,
      INFO_,
      DEBUG_,
      GENERAL_    // Default type for exceptions
   };

   enum RunState
   {
      IDLE = 10000,
      RUNNING,
      PAUSED,
      TARGETING,
      OPTIMIZING,
      ESTIMATING,
      SOLVING,
      SOLVEDPASS,
      WAITING
   };

   enum WrapperDataType
   {
      NUMBER_WT,          // Real, Integer
      MATRIX_WT,          // Rmatrix
      STRING_WT,          // a raw text string
      STRING_OBJECT_WT,   // name of a String Object
      OBJECT_PROPERTY_WT,
      VARIABLE_WT,
      ARRAY_WT,
      ARRAY_ELEMENT_WT,
      PARAMETER_WT,
      OBJECT_WT,
      BOOLEAN_WT,
      INTEGER_WT,
      ON_OFF_WT,
      UNKNOWN_WRAPPER_TYPE = -2
   };

}

typedef std::vector<UnsignedInt>           ObjectTypeArray;
typedef std::vector<Gmat::WrapperDataType> WrapperTypeArray;
typedef std::map<std::string, UnsignedInt> ObjectTypeMap;
typedef std::map<UnsignedInt, StringArray> ObjectTypeArrayMap;

#ifdef EXPORT_TEMPLATES

   /**
    * The following code cleans up template import/export issues between GMAT and 
    * Visual Studio 2010.  These blocks of code might result in multiply defined
    * structures when using plugin libraries, or when refactoring the base code 
    * into separate libraries.  If that happens, they need to be moved into a
    * .cpp file that is only built in the base library.
    */

    // Instantiate STL template classes used in GMAT  
    // This does not create an object. It only forces the generation of all
    // of the members of the listed classes. It exports them from the DLL 
    // and imports them into the .exe file.

    // This fixes std::string:
    // Only do this if the export is not already in a different module (wx, I'm looking at you!)
    #ifdef IMPEXP_STDSTRING
        EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<char>;
        EXPIMP_TEMPLATE template class DECLSPECIFIER std::basic_string<char, std::char_traits<char>, std::allocator<char>>;
    #endif

//    const std::basic_string::size_type std::basic_string<char, std::char_traits<char>, std::allocator<char>>::npos = size_t(-1); 
//    EXPIMP_TEMPLATE template const typename DECLSPECIFIER std::basic_string<char, std::char_traits<char>, std::allocator<char>>::npos;


 //   template<class _Elem,
	//class _Traits,
	//class _Alloc>
	//_PGLOBAL const typename basic_string<_Elem, _Traits, _Alloc>::size_type
	//	basic_string<_Elem, _Traits, _Alloc>::npos =
	//		(typename basic_string<_Elem, _Traits, _Alloc>::size_type)(-1);

    // Fix StringArray:
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::string>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<std::string>;

    // Fix vector of StringArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<StringArray>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<StringArray>;

    // Fix ObjectArray
    class GmatBase;     // forward reference
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<GmatBase*>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<GmatBase*>;

    // Fix IntegerArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Integer>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Integer>;

    // Fix RealArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Real>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Real>;

    // Fix UnsignedIntArray
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<UnsignedInt>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<UnsignedInt>;

    // Fix vector of bools
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<bool>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<bool>;

    // Fix ObjectType vector
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<UnsignedInt>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<UnsignedInt>;

    // Fix WrapperDataType
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<Gmat::WrapperDataType>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<Gmat::WrapperDataType>;

    // Fix vector of IntegerArrays
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<IntegerArray>;
    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<IntegerArray>;


    // Maps -- still need to be addressed
    //EXPIMP_TEMPLATE template class DECLSPECIFIER std::allocator<std::string>;
    //EXPIMP_TEMPLATE template struct DECLSPECIFIER std::less< std::string >;
    //EXPIMP_TEMPLATE template struct DECLSPECIFIER std::pair< std::string,std::string >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::allocator<std::pair<const std::string,std::string> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false>;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tree_nod<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::allocator<std::_Tree_nod<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> > >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::_Tree_val<std::_Tmap_traits<std::string,std::string,std::less<std::string>, std::allocator<std::pair<const std::string,std::string> >,false> >;
    //EXPIMP_TEMPLATE template class  DECLSPECIFIER std::map<std::string, std::string, std::less< std::string >, std::allocator<std::pair<const std::string,std::string> > >;

/*
   #define EXPORT_STL_MAP( mapkey, mapvalue ) \
     template struct DECLSPECIFIER std::pair< mapkey,mapvalue >; \
     template class DECLSPECIFIER std::allocator< \
       std::pair<const mapkey,mapvalue> >; \
     template struct DECLSPECIFIER std::less< mapkey >; \
     template class DECLSPECIFIER std::allocator< \
       std::_Tree_ptr<std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> > >; \
     template class DECLSPECIFIER std::allocator< \
       std::_Tree_nod<std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> > >; \
     template class DECLSPECIFIER std::_Tree_nod< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::_Tree_ptr< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
       std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::_Tree_val< \
       std::_Tmap_traits<mapkey,mapvalue,std::less<mapkey>, \
   	std::allocator<std::pair<const mapkey,mapvalue> >,false> >; \
     template class DECLSPECIFIER std::map< \
       mapkey, mapvalue, std::less< mapkey >, \
       std::allocator<std::pair<const mapkey,mapvalue> > >;

   EXPORT_STL_MAP(std::string, std::string)
*/

    // Here are lists of strings:
    //EXPIMP_TEMPLATE template class DECLSPECIFIER std::list<std::string>;


    // other examples:
//    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<int>;
//    EXPIMP_TEMPLATE template class DECLSPECIFIER std::vector<char>;

#endif

#endif //UTILDEFS_HPP
