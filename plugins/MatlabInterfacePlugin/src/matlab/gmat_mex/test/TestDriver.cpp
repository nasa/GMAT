#include <iostream>
#include "MatlabClient.hpp"

int main(int argc, char **argv)
{
   std::cout << "\n######### Test Driver #########\n"; 

   if (argc < 2 || argc > 3 ) 
   {
      std::cout << "\nUsage: TestDriver [-preh] <string>, argc = " 
                << argc << std::endl;
      return -1;
   }

   std::string sendMsg;
   std::string option("-p");

   if (argc == 2 && *argv[1] != '-')
   {
      sendMsg = argv[1];
   }
   else
   {
      option = argv[1];
      sendMsg = argv[2];   
   }


   MatlabClient *matlabClient = new MatlabClient();
   if (matlabClient->Connect())
   {
      std::cout << "\nConnected successful\n"; 

      if (option == "-p")
         matlabClient->GetConnection()->Poke(_T("script"), 
                                             (wxChar *)sendMsg.c_str());

      else if (option == "-r")
      {
         std::cout << "\nTBD for requesting data\n"; 
         wxChar *data = matlabClient->GetConnection()->Request((
                                                    wxChar *)sendMsg.c_str());
         std::cout << "\nSeems successful for requesting data = (" 
                   << data << ")\n";
      }

    
      else if (option == "-e")
         std::cout << "\nTBD for executing\n"; 

      else
         std::cout << "\nSorry no option found...\n";

      if (!matlabClient->Disconnect())
         std::cout << "\nDisconnected unsuccessful\n"; 
   }
   else 
      std::cout << "\nNot able to connect GMAT\n"; 
      
   return 0;
}
