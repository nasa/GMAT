//$Id: GmatPluginFunctions.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/07/03
//
/**
 * Implementation for library code interfaces.
 * 
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "GmatPluginFunctions.hpp"
#include "MessageInterface.hpp"

#include "SailFactory.hpp"

extern "C"
{
   //---------------------------------------------------------------------------
   // Integer GetFactoryCount()
   //---------------------------------------------------------------------------
   /**
    * Method used to determine how many factories the plug-in library supports.
    * 
    * @return The number of factories in the plug-in library.
    */
   //---------------------------------------------------------------------------
   Integer GetFactoryCount()
   {
      return 1;
   }
   
   //---------------------------------------------------------------------------
   // Factory* GetFactoryPointer(Integer index)
   //---------------------------------------------------------------------------
   /**
    * Retrieves an instance of the factory that corresponds to the input integer
    * 
    * This function is used to build all of the Factory instances needed by the 
    * plug-in library.
    * 
    * @param index Zero-based index identifying which factory is requested
    * 
    * @return A pointer to the factory, or NULL if there is no factory with the 
    *         specified index 
    */
   //---------------------------------------------------------------------------
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
         case 0:
            factory = new SailFactory;
            break;
            
         default:
            break;
      }

      return factory;
   }
   
   //---------------------------------------------------------------------------
   // void SetMessageReceiver(MessageReceiver* mr)
   //---------------------------------------------------------------------------
   /**
    * Method used to pass the active MessageReceiver to the plug-in.
    * 
    * If the code in your plug-in writes to GMAT's message interfaces, implement
    * this method so that GMAT can set the output streams so that your messages 
    * display correctly.
    * 
    * @param mr The MessageReceiver.
    * 
    * @note This method is marked to be deprecated, and may be removed in future 
    * builds.
    */
   //---------------------------------------------------------------------------
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }
};
