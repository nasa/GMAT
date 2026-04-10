//$Id$
//------------------------------------------------------------------------------
//                               TATCException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.05.05
//
/**
 * Exception class used by the TAT-C code to generate visibilty reports.
 */
//------------------------------------------------------------------------------


#ifndef TATCException_hpp
#define TATCException_hpp

#include "BaseException.hpp"
#include "gmatdefs.hpp"          // For GMAT_API

/**
 * Exception class used to report issues with event location.
 */
class GMAT_API TATCException : public BaseException
{
public:
   /// class construction/destruction
   TATCException(const std::string &details = "");
   TATCException(const TATCException &be);
   //   virtual ~TATCException(); // unimplemented destructor
};

#endif // TATCException_hpp
