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

#include "TATCException.hpp"

//------------------------------------------------------------------------------
//  TATCException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs TATCException instance (default constructor).
 *
 * @param details A message providing the details of the exception.
 */
//------------------------------------------------------------------------------
TATCException::TATCException(const std::string &details) :
   BaseException           ("TAT-C Exception Thrown: ", details)
{
}

//------------------------------------------------------------------------------
//  TATCException(const TATCException &be)
//------------------------------------------------------------------------------
/**
 * Constructs TATCException instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
TATCException::TATCException(const TATCException &be) :
   BaseException       (be)
{
}
