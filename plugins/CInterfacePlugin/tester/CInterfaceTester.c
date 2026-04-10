/*
 * CInterfaceTester.c
 *
 *  Created on: Jul 13, 2011
 *      Author: djc
 */

#include <stdio.h>
#include <string.h>
#include "CInterfaceTester.h"
#include <dlfcn.h>


void (*GetFunction(char* funName, void *libHandle))()
{
   if (libHandle == NULL)
      printf("The library has not been opened successfully; cannot search "
            "for function \"%s\"\n", funName);

   void (*func)() = NULL;

   #ifdef __WIN32__
      func = (void(*)())GetProcAddress((HINSTANCE)libHandle, funName);
   #else
      func = (void(*)())dlsym(libHandle, funName);
   #endif

   if (func == NULL)
      printf(" !!! Cannot locate the function \"%s\" !!!\n", funName);

   return func;
}

char *GetLastMessage(void *libHandle)
{
   char* (*LastMessage)() = NULL;
   LastMessage = (char* (*)())GetFunction("getLastMessage", libHandle);
   if (LastMessage == NULL)
      return "";

   return LastMessage();
}

int StartGmat(void *libHandle)
{
   int (*StartGmat)() = NULL;
   StartGmat = (int (*)())GetFunction("StartGmat", libHandle);
   if (StartGmat == NULL)
      return -1;
   return StartGmat();
}

int LoadScript(char *scriptName, int sandboxNumber, void *libHandle)
{
   int retval = -1;

   int (*LoadScript)(const char* script) = NULL;
   LoadScript = (int (*)(const char*))GetFunction("LoadScript", libHandle);
   if (LoadScript != NULL)
      retval = LoadScript(scriptName);

   return retval;
}

int RunScript(int sandboxNumber, void *libHandle)
{
   int retval = -1;

   int (*RunScript)() = NULL;
   RunScript = (int (*)())GetFunction("RunScript", libHandle);
   if (RunScript != NULL)
      retval = RunScript();

   return retval;
}

int FindOdeModel(char *odeName, int sandboxNumber, void *libHandle)
{
   int retval = -1;

   int (*FindOdeModel)(const char* name) = NULL;
   FindOdeModel = (int (*)(const char*))GetFunction("FindOdeModel", libHandle);
   if (FindOdeModel != NULL)
   {
      retval = FindOdeModel(odeName);
      if (retval != 0)
         printf("   !!! Did not find ODE Model !!!\n");
   }
   return retval;
}

int GetState(char *odeName, int sandboxNumber, void *libHandle)
{
   int retval = -1;

//   if (FindOdeModel(odeName, sandboxNumber, libHandle) == 0)
   {
      int (*GetStateSize)() = NULL;
      GetStateSize = (int (*)())GetFunction("GetStateSize", libHandle);
      if (GetStateSize == NULL)
         return -2;
      int size = GetStateSize();
      double* (*GetState)() = NULL;
      GetState = (double* (*)())GetFunction("GetState", libHandle);
      if (GetState != NULL)
      {
         double *state = GetState();
         printf("State data:\n   ");
         int i;
         for (i = 0; i < size; ++i)
            printf(" %lf ", state[i]);
         printf("\n");
         retval = 0;
      }
   }
   return retval;
}

int SetState(double *state, int size, char *odeName, int sandboxNumber, void *libHandle)
{
   int retval = -1;
   return retval;
}

int GetDerivatives(char *odeName, int sandboxNumber, void *libHandle)
{
   int retval = -1;

//   if (FindOdeModel(odeName, sandboxNumber, libHandle) == 0)
   {
      int (*GetStateSize)() = NULL;
      GetStateSize = (int (*)())GetFunction("GetStateSize", libHandle);
      if (GetStateSize == NULL)
         return -2;
      int size = GetStateSize();
      double* (*GetDerivatives)(double, int, int*) = NULL;
      GetDerivatives = (double* (*)(double, int, int*))GetFunction("GetDerivatives", libHandle);
      if (GetDerivatives != NULL)
      {
         int dim;
         double *dv = GetDerivatives(0.0, 1, &dim);
//         double *dv = GetDerivatives(0.0, 1);
//         dim = size;
         printf("State size = %d, derivative size = %d\n", size, dim);
         printf("Derivative data:\n   ");
         int i;
         for (i = 0; i < dim; ++i)
            printf(" %lf ", dv[i]);
         printf("\n");
         retval = 0;
      }
   }
   return retval;
}


int main(int argc, char *argv[])
{
   char *msg;
   printf("************************************************************\n"
          "*** C Interface Test Program\n"
          "************************************************************\n\n");

   printf("Loading the library\n");

   // Load the dynamic library on Mac or Linux
   /// todo: Add Windows configuration for testing the interface
   #ifdef __linux
      void *libHandle = dlopen("libCInterface.so", RTLD_LAZY);
   #else
      void *libHandle = dlopen("libCInterface.dylib", RTLD_LAZY);
   #endif

   if (libHandle == NULL)
      printf("\n%s\n", dlerror());

   printf("Looking for a function that does not exist...\n");
   void (*NoFun)() = NULL;
   NoFun = (void (*)())GetFunction("AintNoSuchFunction", libHandle);

   printf("\nStarting GMAT\n");
   if (StartGmat(libHandle) < 0)
   {
      msg = GetLastMessage(libHandle);
      printf("%s\n", msg);
      printf("GMAT failed to start; exiting...\n");
      return -1;
   }
   msg = GetLastMessage(libHandle);
   printf("%s\n", msg);

   char scriptName[256];
   char odeName[256];
   odeName[0] = '\0';

   scriptName[0] = '\0';

   printf("Finding an ODE Model 20 times\n");
   int i;
   for (i = 0; i < 20; ++i)
   {
      if (strcmp(scriptName, "../samples/Ex_ForceModels.script") != 0)
         sprintf(scriptName, "../samples/Ex_ForceModels.script");
      else
         sprintf(scriptName, "../samples/Ex_HohmannTransfer.script");
      printf("Loading the script %s\n", scriptName);
      if(LoadScript(scriptName, 0, libHandle) < 0)
      {
         msg = GetLastMessage(libHandle);
         printf("%s\n", msg);
         printf("GMAT failed load %s; exiting...\n", scriptName);
         return -1;
      }
      msg = GetLastMessage(libHandle);
      printf("%s\n", msg);

      printf("Running the script\n");
      if(RunScript(0, libHandle) < 0)
      {
         msg = GetLastMessage(libHandle);
         printf("%s\n", msg);
         printf("GMAT failed run %s; exiting...\n", scriptName);
         return -1;
      }
      msg = GetLastMessage(libHandle);
      printf("%s\n", msg);

      FindOdeModel(odeName, 0, libHandle);

      if (FindOdeModel(odeName, 0, libHandle) < 0)
      {
         msg = GetLastMessage(libHandle);
         printf("%s\n", msg);
         printf("GMAT failed find the ODE Model %s; exiting...\n", odeName);
         return -1;
      }
      msg = GetLastMessage(libHandle);
      printf("%s\n", msg);

      GetState(odeName, 0, libHandle);
      GetDerivatives(odeName, 0, libHandle);
   }

   printf("Closing the library\n");
   dlclose(libHandle);
   printf("Testing complete!\n\n");
   return 0;
}
