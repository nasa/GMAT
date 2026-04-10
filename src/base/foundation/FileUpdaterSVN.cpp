//$Id$
//------------------------------------------------------------------------------
//                                  FileUpdaterSVN
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Thomas G. Grubb/GSFC
// Created: 2016.01.12
//
/**
 * Definition of the FileUpdaterSVN class.  This is the SVN implementation
 * for a class that updates data files from a repository
 *
 */
//------------------------------------------------------------------------------

#include "FileUpdaterSVN.hpp"
#include "xercesc/framework/LocalFileInputSource.hpp"
#include <xercesc/util/PlatformUtils.hpp>
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/dom/DOM.hpp"

#include "errno.h"
// Other include files, declarations, and non-Xerces-C++ initializations.

using namespace xercesc;

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
FileUpdaterSVN::FileUpdaterSVN(std::string location, std::string server) :
   IFileUpdater(location, server)
{
   Initialize();
}


/**
 * Destructor
 */
//------------------------------------------------------------------------------
FileUpdaterSVN::~FileUpdaterSVN()
{
   Finalize();
}


bool FileUpdaterSVN::CheckForUpdates()
{
   #ifndef _WIN32  // Windows
      MessageInterface::PopupMessage(Gmat::ERROR_, "Data Update Tool has not been implemented for non-Windows platforms");
      return false;
   #endif

   FileManager *fm = FileManager::Instance();
   std::string xmlFile = fm->GetAbsPathname(FileManager::FILE_UPDATE_PATH);
   if (!GmatFileUtil::DoesDirectoryExist(xmlFile))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, "File Update Path Directory does not exist: " +
         xmlFile);
      return false;
   }
   else if (!GmatFileUtil::DoesDirectoryExist(xmlFile+".svn"+GmatFileUtil::GetPathSeparator()))
   {
      MessageInterface::PopupMessage(Gmat::ERROR_, "Update Directory is not an SVN directory: " +
         xmlFile);
      return false;
   }
   xmlFile = xmlFile +"svnresults.xml";

   // copy required versioned files
   CopyVersionedFiles();
   // execute SVN
   bool status = ExecuteCheck( xmlFile );
   if (status)
      // parse results
      ParseUpdateCheck( xmlFile );
   return status;
}


void FileUpdaterSVN::CopyVersionedFiles()
{
   FileManager *fm = FileManager::Instance();
   std::string versionDir = fm->GetAbsPathname(FileManager::FILE_UPDATE_PATH)+"RequiredFiles"+GmatFileUtil::GetPathSeparator();

   // copy required versioned files
   Integer retCode;
   std::string errmsg;
   std::string filename;
   try
   {
      // EOP file
      filename = fm->GetFullPathname(FileManager::EOP_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
      // planetary pck file
      filename = fm->GetFullPathname(FileManager::PLANETARY_PCK_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
      // leap secs file
      filename = fm->GetFullPathname(FileManager::LEAP_SECS_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
      // LSK file
      filename = fm->GetFullPathname(FileManager::LSK_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
      // CSSI FLUX file
      filename = fm->GetFullPathname(FileManager::CSSI_FLUX_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
      // SCHATTEN file
      filename = fm->GetFullPathname(FileManager::SCHATTEN_FILE);
      if (!fm->CopyFile( filename, 
                    versionDir+GmatFileUtil::ParseFileName(filename),
                    retCode, true ))
      {
         errmsg = "Error copying file \"";
         errmsg += filename + "\" to \"";
         errmsg += versionDir+GmatFileUtil::ParseFileName(filename) + "\"\n";
         MessageInterface::ShowMessage(errmsg);
      }
   }
   catch (UtilityException &e)
   {
         MessageInterface::ShowMessage("Cannot copy versioned file: %s",
            e.GetFullMessage().c_str());
   }
}


bool FileUpdaterSVN::ExecuteCheck(const std::string &fileName)
{
   // Does it exist?
   FileManager *fm = FileManager::Instance();
   std::string cmd = "cd /d "+fm->GetAbsPathname(FileManager::FILE_UPDATE_PATH)+" && svn status -u --xml > "+fileName;
   errno = 0;
   int ret_val = system(cmd.c_str());
   return (ret_val == 0 && errno == 0);
}

bool FileUpdaterSVN::Finalize()
{
   XMLPlatformUtils::Terminate();
   return true;
}


bool FileUpdaterSVN::Initialize()
{
   XMLPlatformUtils::Initialize();
   return true;
}


bool FileUpdaterSVN::SaveUpdateScript( const std::string &filename )
{
   std::vector<IFileUpdater::repoFile>::iterator it;
   std::string cmd;
   std::ofstream outputFile;
   outputFile.open(filename.c_str());
   FileManager *fm = FileManager::Instance();
   std::string versionDir = fm->GetAbsPathname(FileManager::FILE_UPDATE_PATH);
   bool isEmpty = true;

   //outputFile << "cd /d \""+fm->GetStartupFileDir()+"\"";
   //outputFile << "\n";
   
   for(it=files.begin() ; it < files.end(); it++ ) {
      cmd = fm->GetAbsPathname(FileManager::FILE_UPDATE_PATH)+it->name;
      if ((it->status != IFileUpdater::ADDED) && 
          (!GmatFileUtil::DoesFileExist(cmd))) continue;
      cmd = "\""+cmd+"\"";
      switch (it->operation)
      {
         case IFileUpdater::NONE:
            cmd = "";
            break;
         case IFileUpdater::UPDATE:
            switch (it->status)
            {
               case IFileUpdater::NOMINAL:
               case IFileUpdater::ADDED:
               case IFileUpdater::DELETED:
               case IFileUpdater::MODIFIED:
                  cmd = "svn update "+cmd;
                  break;
               case IFileUpdater::LOCALMODIFIED:
                  cmd = "svn revert "+cmd;
                  break;
               case IFileUpdater::LOCALMISSING:
                  cmd = "svn checkout "+cmd;
                  break;
            }
            break;
         case IFileUpdater::REVERT:
            cmd = "svn revert "+cmd;
            break;
      }
      if (cmd == "") continue;
      cmd = cmd + " >> updateGmat.log 2>&1";
      outputFile << cmd;
      outputFile << "\n";
      isEmpty = false;
      // copy versioned files back to their original location
      if (it->fileType != FileManager::BEGIN_OF_PATH)
      {
         // copy file suppressing any overwrite prompts
         cmd = "copy /y \""+versionDir+it->name+"\" \""+
            fm->GetFullPathname(it->fileType)+"\"";
         cmd = cmd + " >> updateGmat.log 2>&1";
         outputFile << cmd;
         outputFile << "\n";
      }
   }

   outputFile.close();
   if (isEmpty)
      remove(filename.c_str());
   return !isEmpty;
}


bool FileUpdaterSVN::ParseUpdateCheck(const std::string &fileName)
{
   files.clear();
   XercesDOMParser* parser = new XercesDOMParser();
   FileManager *fm = FileManager::Instance();

   try {
      parser->parse(fileName.c_str());
      
      repoFile aFile;
      std::string filename;
      DOMNode *pNode;
      const XMLCh *aName;
      const XMLCh *wcStatusItem;
      const XMLCh *repoStatusItem;
      DOMNode::NodeType nt;
      DOMNode *wcStatus;
      DOMNodeList *wcStatuses;
      DOMNodeList *repoStatuses;
      DOMNodeList *pChilds = parser->getDocument()->getChildNodes();

      pNode = pChilds->item(0);
      nt = pNode->getNodeType();
      aName = pNode->getNodeName();
      DOMNodeList *pEntries = pNode->getChildNodes();
      if (pEntries->getLength() == 0) return false;

      // find "target" element
      for (int i=0; i < pEntries->getLength(); i++)
      {
         pNode = pEntries->item(i);
         nt = pNode->getNodeType();
         aName = pNode->getNodeName();
         if (XMLString::equals(aName, XMLString::transcode("target"))) break;
      }
      pEntries = pNode->getChildNodes();

      for (int i=0; i < pEntries->getLength(); i++)
      {
         // skip everything but "entry" elements
         pNode = pEntries->item(i);
         aName = pNode->getNodeName();
         nt = pNode->getNodeType();
         if (!XMLString::equals(XMLString::transcode("entry"), pNode->getNodeName())) continue;

         aFile.name =  XMLString::transcode(((DOMElement *) pNode)->getAttribute(XMLString::transcode("path")));

         // if wc-status is unversioned, ignore it
         wcStatuses = ((DOMElement *) pNode)->getElementsByTagName(XMLString::transcode("wc-status"));
         if ((wcStatuses == NULL) || (wcStatuses->getLength() == 0)) continue;

         wcStatusItem = ((DOMElement *) wcStatuses->item(0))->getAttribute(XMLString::transcode("item"));
         if (XMLString::equals(XMLString::transcode("unversioned"), wcStatusItem)) continue;

         // now get repoStatus
         repoStatuses = ((DOMElement *) pNode)->getElementsByTagName(XMLString::transcode("repos-status"));
         if ((repoStatuses == NULL) || (repoStatuses->getLength() == 0)) 
            repoStatusItem = XMLString::transcode("nominal");
         else
            repoStatusItem = ((DOMElement *) repoStatuses->item(0))->getAttribute(XMLString::transcode("item"));

         // set file info
         if (XMLString::equals(repoStatusItem,XMLString::transcode("modified"))) 
            aFile.status = IFileUpdater::MODIFIED;
         else if (XMLString::equals(repoStatusItem,XMLString::transcode("added"))) 
            aFile.status = IFileUpdater::ADDED;
         else if (XMLString::equals(repoStatusItem,XMLString::transcode("deleted"))) 
            aFile.status = IFileUpdater::DELETED;
         else if (XMLString::equals(wcStatusItem,XMLString::transcode("modified"))) 
            aFile.status = IFileUpdater::LOCALMODIFIED;
         else if (XMLString::equals(wcStatusItem,XMLString::transcode("deleted"))) 
            aFile.status = IFileUpdater::LOCALMISSING;
         else
            aFile.status = IFileUpdater::NOMINAL;

         aFile.comment = "";
         aFile.operation = IFileUpdater::NONE; // updated by GUI
         aFile.version = "";
         aFile.fileType = FileManager::BEGIN_OF_PATH;
         if (GmatStringUtil::StartsWith(aFile.name, "RequiredFiles"))
         {
            filename = GmatFileUtil::ParseFileName(aFile.name);
            if (fm->GetFilename(FileManager::EOP_FILE) == filename)
               aFile.fileType = FileManager::EOP_FILE;
            else if (fm->GetFilename(FileManager::PLANETARY_PCK_FILE) == filename)
               aFile.fileType = FileManager::PLANETARY_PCK_FILE;
            else if (fm->GetFilename(FileManager::LEAP_SECS_FILE) == filename)
               aFile.fileType = FileManager::LEAP_SECS_FILE;
            else if (fm->GetFilename(FileManager::LSK_FILE) == filename)
               aFile.fileType = FileManager::LSK_FILE;
            else if (fm->GetFilename(FileManager::CSSI_FLUX_FILE) == filename)
               aFile.fileType = FileManager::CSSI_FLUX_FILE;
            else if (fm->GetFilename(FileManager::SCHATTEN_FILE) == filename)
               aFile.fileType = FileManager::SCHATTEN_FILE;
         }
             
         // add to list of files
         files.push_back(aFile);

      }
   }
   catch (const XMLException& toCatch) {
      char* message = XMLString::transcode(toCatch.getMessage());
      MessageInterface::ShowMessage
         ("FileUpdaterSVN::ParseUpdateCheck error parsing svn xml file: %d\n", message);
      XMLString::release(&message);
      return false;
   }
   catch (const DOMException& toCatch) {
      char* message = XMLString::transcode(toCatch.msg);
      MessageInterface::ShowMessage
         ("FileUpdaterSVN::ParseUpdateCheck error parsing svn xml file: %d\n", message);
      XMLString::release(&message);
      return false;
   }

   delete parser;
   return true;
}



