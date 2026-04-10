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



// #include <iostream>
// #include <boost/filesystem/operations.hpp>
// #include <boost/filesystem/fstream.hpp>
// using namespace boost::filesystem; 
// using namespace std;

// void show_files( const path & directory, bool recurse_into_subdirs = true )
// {
//   if( exists( directory ) )
//   {
//     directory_iterator end ;
//     for( directory_iterator iter(directory) ; iter != end ; ++iter )
//       if ( is_directory( *iter ) )
//       {
//         cout << iter->native_directory_string() << " (directory)\n" ;
//         if( recurse_into_subdirs ) show_files(*iter) ;
//       }
//       else 
//         cout << iter->native_file_string() << " (file)\n" ;
//   }
// }

// int main()
// {
//     show_files( "/usr/share/doc/bind9" ) ;
// }



#include <windows.h>
#include <iostream>
using namespace std;

int main()
{
    HANDLE hFind;
    WIN32_FIND_DATA FindData;
    int ErrorCode;
    BOOL Continue = TRUE;
    
    cout << "A decent FindFirst/Next demo." << endl << endl;
    
    hFind = FindFirstFile("C:\\Projects\\gmat\\files\\GmatFunctions\\*.gmf", &FindData);
    
    if(hFind == INVALID_HANDLE_VALUE)
    {
        ErrorCode = GetLastError();
        if (ErrorCode == ERROR_FILE_NOT_FOUND)
        {
            cout << "There are no files matching that path/mask\n" << endl;
        }
        else
        {
            cout << "FindFirstFile() returned error code " << ErrorCode << endl;
        }
        Continue = FALSE;
    }
    else
    {
        cout << FindData.cFileName << endl;
    }

    if (Continue)
    {
        while (FindNextFile(hFind, &FindData))
        {
            cout << FindData.cFileName << endl;
            
        }

        ErrorCode = GetLastError();

        if (ErrorCode == ERROR_NO_MORE_FILES)
        {
            cout << endl << "All files logged." << endl;
        }
        else
        {
            cout << "FindNextFile() returned error code " << ErrorCode << endl;
        }
        
        if (!FindClose(hFind))
        {
            ErrorCode = GetLastError();
            cout << "FindClose() returned error code " << ErrorCode << endl;
        }
    }
    
    
    cin.get();
}
