//------------------------------------------------------------------------------
//                       SparseMatrixLibraryHeader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Youngkwang Kim
// Created: 2015
//
/**
 *  This file is used to set up use of Boost libraries
 */
//------------------------------------------------------------------------------

#include "SparseMatrixUtil.hpp"

// note that BOOST compressed matrix has a better performance
//           than coordinate matrix type.
#define _use_BOOST_Compressed_Matrix 

//using SparseMatrixUtil;

// using Iterators are not recommended
// because they are BOOST dependent!
#ifdef _use_BOOST_Compressed_Matrix 
////using BOOST_Compressed_Matrix::RSMatrix;
//using BOOST_Compressed_Matrix::ConstIterator1;
//using BOOST_Compressed_Matrix::ConstIterator2;
//using BOOST_Compressed_Matrix::Iterator1;
//using BOOST_Compressed_Matrix::Iterator2;
////using BOOST_Compressed_Matrix::SparseMatrixUtil;
#endif

