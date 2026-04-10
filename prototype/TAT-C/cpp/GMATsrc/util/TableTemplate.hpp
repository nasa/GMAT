//$Id$
//------------------------------------------------------------------------------
//                              TableTemplate
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: M. Weippert, T. McRoberts, E. Corderman
// Created: 1995/10/10 for GSS project
// Modified: 2003/09/15 Linda Jun - Replaced GSSString with std::string.
//
/**
 * Provides declarations for the TableTemplate template class, representing
 * a 2-dimensional table of any type T (see assumptions about type T, below)
 */
//------------------------------------------------------------------------------
#ifndef TableTemplate_hpp
#define TableTemplate_hpp

#include "utildefs.hpp"
#include "BaseException.hpp"
#include <iterator>           // For back_inserter() with VC++ 2010

//  exceptions
class GMATUTIL_API TableTemplateExceptions
{
public:

    class DimensionError : public BaseException 
          {public : DimensionError(const std::string &message =
          "TableTemplate error : dimension error or mismatch.\n")
          : BaseException(message) {};  };
    class OutOfBounds : public BaseException
          {public : OutOfBounds(const std::string &message =
          "TableTemplate error : index out-of-bounds.\n")
          : BaseException(message) {};  };
    class UnsizedTable : public BaseException 
          {public : UnsizedTable(const std::string &message =
          "TableTemplate error : unsized table.\n")
          : BaseException(message) {};  };
    class TableAlreadySized : public BaseException 
          {public : TableAlreadySized(const std::string &message =
          "TableTemplate error : table already sized.\n")
          : BaseException(message) {};  };
    class IllegalSize : public BaseException 
          {public : IllegalSize(const std::string &message =
          "TableTemplate error : illegal (negative) size.\n")
          : BaseException(message) {};  };

private:
};

template <class T>
class TableTemplate
{

/**
 *  Tables are stored in row major order.
 *  Assumptions about template parameter types:
 *      type has appropriate initializers (constructors and "=" operator)
 *  The exceptions are declared in a separate class because the current HP
 *  compiler cannot properly handle exceptions declared a template class 
 *  and thrown in another template class.
 */

public:
    TableTemplate();
    TableTemplate(Integer r, Integer c);
    // HP won't compile this 
    // TableTemplate(Integer r, Integer c, const T &a11,...);
    TableTemplate(Integer r, Integer c, const T* array);
    TableTemplate(const TableTemplate<T> &table);
    virtual ~TableTemplate();

    T& operator()(Integer r, Integer c);
    const T& operator()(Integer r, Integer c) const;
    TableTemplate<T>& operator=(const TableTemplate<T> &table);
    bool operator==(const TableTemplate<T> &table) const;
    bool operator!=(const TableTemplate<T> &table) const;
    
    virtual T    GetElement(Integer r, Integer c) const;
    virtual void SetElement(Integer r, Integer c, const T &value);
    virtual bool IsSized() const;
    virtual void SetSize(Integer r, Integer c, bool zeroElements = true);
    virtual void ChangeSize(int r, int c, bool zeroElements);

    virtual void GetSize(Integer &r, Integer &c) const;
    virtual Integer  GetNumColumns() const;
    virtual Integer  GetNumRows() const;
    
    const T* GetDataVector() {return elementD;}
   
protected:
    T   *elementD;
    Integer rowsD, colsD;
    bool isSizedD;
    void init(Integer r, Integer c);

private:
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#include "TableTemplate.cpp"
#endif

#endif // TableTemplate_hpp
