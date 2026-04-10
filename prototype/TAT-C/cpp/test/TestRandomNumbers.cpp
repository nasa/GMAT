//
//  TestRandomNumbers.cpp
//  
//
//  Created by Stark, Michael E. (GSFC-5850) on 5/11/17.
//
//

#include <stdio.h>
#include <iostream>
#include "RandomNumber.hpp"
#include "RealUtilities.hpp"

using namespace GmatMathUtil;
using namespace std;

int main()
{
   int n;
   char choice;
   
   cout << "Enter N >";
   cin >> n;
   cout << "enter 'y' to compute via Real Utilities, or RandomNumber is run >";
   cin >> choice;
   
   if (choice=='y') // run real utilities
   {
      Real realValue, realSum=0.0;
      SetSeed(2231958);
      printf ("Random Values from RealUtilities\n");
      for (int i=0; i<n; i++)
      {
         realValue = Rand();
         realSum += realValue;
         printf ("   %8.6f\n", realValue);
      }
      printf("\n");
      printf("Average from Real Utilities = %8.6f\n",realSum/Real(n));
   }
   
   else // direct use of Random number
   {
      Real randomValue,randomSum=0.0;
      RandomNumber *rn = RandomNumber::Instance();
      rn->SetSeed(2231958);
      printf ("Random Values from RandomNumber\n");
      for (int i=0; i < n; i++)
      {
         // RealUtilities
         randomValue=rn->Uniform();
         randomSum += randomValue;
         printf ("   %8.6f\n", randomValue);
      }
      printf("\n");
      printf("Average from RandomNumber   = %8.6f\n",randomSum/Real(n));
   }
}
