

import urllib
# --- needed (for some reason) to run on Mac
import urllib.request
import urllib.error
# ---
import os
import re
from ftplib import FTP
import time
import datetime

class GMATFileManager:
    
    """
    Utility for updating GMAT data files
     
      Status: Prototype utility not ready for operations
      
      Functionality:
          Updates all files or individually updates selected files
          Writes status to screen and log file
          Optionally archives old file versions with date pre-appended
          NOT DONE: DOES NOT UPDATE IF FILE HAS NOT CHANGED
          NOT DONE: WHEN UPDATING ALL, RECOVERS GRACEFULLY IF DOWNLOAD FAILS 
                    A FILE (MOSTLY DONE)
          
    
      Usage Example:
    
          # Update all GMAT data files managed by this utility
          fManager = GMATFileManager(PATH_TO_GMAT_ROOT_DIRECTORY)
          fManager.UpdateAllFiles()
          
          # Alternatively update files on a per file basis
          fManager = GMATFileManager(PATH_TO_GMAT_ROOT_DIRECTORY)
          fManager.UpdateGMATLeapSecondFile()
          fManager.UpdateGMATEOPFile()
          fManager.UpdateSPICELeapSecondFile()
          fManager.UpdateSPICEPlanetaryPCKFile()
          fManager.UpdateSPICEBPCFiles()
          fManager.UpdateSPICETFFiles()
		  fManager.UpdateIGRZFile()
          fManager.UpdateCSSISpaceWeatherFile()
		  fManager.UpdateCSSISpaceWeatherAllFile()
          fManager.UpdateApFileForIRIModel()
          
      NOTE: The class archives old files to the current working directory by
      default, appending the time of update to the file name.  The archive 
      location can be set by kwarg, as well as whether or not to archive old
      versions.  
      
      NOTE: When the Luna (moon) BPC and TF files are updated for a more recent 
      ephmemeris (e.g. DE430), the lunaBPCPrefix (and probably the lunaTFPrefix)
      will need to be updated (below).
    
      Author:  T. Grubb(NASA/GSFC) and S. Hughes (GSFC) and W. Shoan (GSFC)
      Created: 2017/02/26 

      """
      
    def __init__(self,gmatRootDirectory,archiveDirectory = '',
                 archiveOldFiles = True,
                 logFileName = 'GMATFileManager.log'):
       
       # catch unset path error
       if (gmatRootDirectory == ''):
          # print ('Error: please edit the python script to specify a data directory\n')
          raise ValueError
       
       # Convert string paths to python path strings
       gmatRootDirectory = os.path.normpath(gmatRootDirectory)
       if (archiveDirectory == ''):
          archiveDirectory = os.getcwd() + "//"
          print ('Archiving data to current directory\n')
       else:
          archiveDirectory = os.path.normpath(archiveDirectory) + "//"
       self.CheckPaths(gmatRootDirectory,archiveDirectory)    
       
       # directories
       
       self.gmatRootDirectory            = gmatRootDirectory
       self.eopDirectory                 = '//data//planetary_coeff//'
       self.leapSecDirectory             = '//data//time//'
       self.spaceWeatherDirectory        = '//data//atmosphere//earth//'
       self.ionoDataDirectory            = '//data//IonosphereData//'
       
       # GMAT data file names
       
       self.spkLeapSecondFileName        = 'SPICELeapSecondKernel.tls'
       self.planetaryPCKFileName         = 'SPICEPlanetaryConstantsKernel.tpc'
       self.earthHighPrecBPCFilename     = 'earth_latest_high_prec.bpc'
       self.earthBPCFilename             = 'SPICEEarthCurrentKernel.bpc'
       self.earthBPCPredictFilename      = 'SPICEEarthPredictedKernel.bpc'
       self.lunaBPCFilename              = 'SPICELunaCurrentKernel.bpc'
       self.lunaFKFilename               = 'SPICELunaFrameKernel.tf'
       self.gmatLeapSecondFileName       = 'tai-utc.dat'
       self.gmatEOPFileName              = 'eopc04_08.62-now'
       self.cssiSpaceWeatherAllFileName  = 'SpaceWeather-All-v1.2.txt'
       self.cssiSpaceWeatherFileName     = 'SpaceWeather-v1.2.txt'
       self.cssiSpaceDataFileName        = 'sw19571001.txt'
       self.apFileForIRIModel            = 'ap.dat'
       self.igrzDataFileName             = 'ig_rz.dat'
       
       # URLs and remote directories
       
       self.usnoLeapSecURL               = 'https://maia.usno.navy.mil/ser7/tai-utc.dat'
       self.naifURL                      = 'https://naif.jpl.nasa.gov/'
       self.eopURL                       = 'ftp://hpiers.obspm.fr/iers/series/opa/eopc04'
       self.spaceWeatherURL              = 'https://ftp.agi.com/pub/DynamicEarthData/SpaceWeather-v1.2.txt'
       self.spaceWeatherAllURL           = 'https://ftp.agi.com/pub/DynamicEarthData/SpaceWeather-All-v1.2.txt'
       self.apDataURL                    = 'https://ftp.agi.com/pub/DynamicEarthData/SpaceWeather-All-v1.2.txt'
       self.igrzDataURL                  = 'http://irimodel.org//indices//ig_rz.dat'

       # Source data file prefixes, extensions, types
              
       self.naifLSK                      = 'pub/naif/generic_kernels/lsk/latest_leapseconds.tls'
       self.naifPCK                      = 'pub/naif/generic_kernels/pck/pck00011.tpc'
       self.naifLunaBPC                  = 'pub/naif/generic_kernels/pck/moon_pa_de421_1900-2050.bpc'
       self.naifEarthHighPrecBPCFilename = 'pub/naif/generic_kernels/pck/earth_latest_high_prec.bpc'
       self.naifEarthBPCPredictFilename  = 'pub/naif/generic_kernels/pck/earth_2025_250826_2125_predict.bpc'
       self.naifBPCFilename              = 'pub/naif/generic_kernels/pck/earth_620120_250826.bpc'
       self.naifLunaTFFilename           = 'pub/naif/generic_kernels/fk/satellites/moon_080317.tf'
       
	   # archive info
       
       self.archiveOldFiles              = archiveOldFiles
       self.archiveDirectory             = archiveDirectory;
       self.logFile                      = archiveDirectory + logFileName


    def CheckPaths(self,gmatRootDirectory,archiveDirectory):
       currentDir = os.getcwd();
       try:
          os.chdir(gmatRootDirectory)  
       except OSError:
          print ('Error changing to gmat directory: ', gmatRootDirectory)
          os.chdir(currentDir)
       try:
          os.chdir(archiveDirectory)
       except OSError:
          print ('Error changing to archive directory: ', archiveDirectory)
          os.chdir(currentDir)
       os.chdir(currentDir)

    def UpdateAllFiles(self):
        
       #  Update all files
       updateSucceded = True;
       leapSecondOK = self.UpdateGMATLeapSecondFile()
       eopOK = self.UpdateGMATEOPFile()
       naifLskOK = self.UpdateSPICELeapSecondFile()
       naifPckOK = self.UpdateSPICEPlanetaryPCKFile()
       naifBpcOK = self.UpdateSPICEBPCFiles()
       naifLunaFrameOK = self.UpdateSPICETFFiles()
       igrzOK = self.UpdateIGRZFile()
       cssiSpaceWeatherOK = self.UpdateCSSISpaceWeatherFile()
       cssiSpaceWeatherAllOK = self.UpdateCSSISpaceWeatherAllFile()
       apForIRIDataOK = self.UpdateApFileForIRIModel()
       
       #  Write results of update to the screen and report.
       self.PrintToScreenAndLog('\n')
       self.PrintToScreenAndLog(\
            '------------------- FILE UPDATE RESULTS ------------------ \n')
       if (not leapSecondOK):
           self.PrintToScreenAndLog('*** ERROR updating GMAT leap second file!\n')
           updateSucceded = False
       if (not eopOK):    
           self.PrintToScreenAndLog('*** ERROR updating GMAT EOP file!\n')
           updateSucceded = False
       if (not naifLskOK): 
           self.PrintToScreenAndLog('*** ERROR updating SPICE leap second file!\n')
           updateSucceded = False
       if (not naifBpcOK):
           self.PrintToScreenAndLog('*** ERROR updating SPICE BPC files!\n')
           updateSucceded = False
       if (not naifPckOK): 
           self.PrintToScreenAndLog('*** ERROR updating SPICE PCK files!\n')
           updateSucceded = False
       if (not cssiSpaceWeatherOK):
           self.PrintToScreenAndLog('*** ERROR updating CSSI space weather file!\n')
           updateSucceded = False
       if (not cssiSpaceWeatherAllOK): 
           self.PrintToScreenAndLog('*** ERROR updating CSSI space weather file!\n')
           updateSucceded = False
       if (not apForIRIDataOK): 
           self.PrintToScreenAndLog('*** ERROR updating ap Data for IRI model!\n')
           updateSucceded = False
       if (not naifLunaFrameOK): 
           self.PrintToScreenAndLog('*** ERROR updating SPICE Lunar Frame Data!\n')
           updateSucceded = False
       if (not igrzOK):
           self.PrintToScreenAndLog('*** ERROR updating IRI model data!\n')
           updateSucceded = False
           
       if (not updateSucceded):    
          self.PrintToScreenAndLog('*** ERROR updating one or more files!\n')
          return False;
       else:
          self.PrintToScreenAndLog('SUCCESS: All files updated successfully!\n')
          return True;

    def GetArchivePrefix(self):
       currentTime = time.time()
       return datetime.datetime.fromtimestamp(currentTime).strftime('%Y-%m-%d-%Hh%Mm%Ss') + '_'
       
    def PrintToScreenAndLog(self,statusString):
      print(statusString)
      file = open(self.logFile, 'a+')
      file.write(statusString + "\n" )
      file.close()

    def PrintTimeToScreenAndLog(self, statusString):
       currentTime = time.time()
       timeString = datetime.datetime.fromtimestamp(currentTime).strftime('%Y-%m-%d-%H:%M:%S');
       self.PrintToScreenAndLog(statusString + timeString)
       
    def UpdateGMATLeapSecondFile(self):
       " Replaces the GMAT leap second file with latest version "
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING GMAT LEAP SECOND FILE --------------------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       try:
          # LEAP_SECS_FILE = TIME_PATH/tai-utc.dat
          self.Downloadfile( self.usnoLeapSecURL, self.gmatLeapSecondFileName )

          currentLoc = self.gmatRootDirectory + self.leapSecDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.gmatLeapSecondFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
        
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.leapSecDirectory
          self.Movefile(currentLoc,newLoc,self.gmatLeapSecondFileName)
       except:
          isOK = False;
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;

    def UpdateGMATEOPFile(self):
       " Replaces the GMAT EOP file with latest version including predicted data "
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING GMAT EOP FILE --------------------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       # EOP_FILE = PLANETARY_COEFF_PATH/eopc04_08.62-now
       try:
          returnFlag = self.Downloadfile(self.eopURL, self.gmatEOPFileName)
          if not returnFlag:
             print("GMAT EOP File Failed to download and update")
                           
          currentLoc = self.gmatRootDirectory + self.eopDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.gmatEOPFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
                                    
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.eopDirectory
          self.Movefile(currentLoc,newLoc,self.gmatEOPFileName)
       except:
          isOK = False
          self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;
                                                            
                                                            
    def UpdateSPICELeapSecondFile(self):
       " Replaces the SPICE leap second file with latest version "
       
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('---------UPDATING SPICE LEAP SECOND FILE -----------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       
       try:

          self.Downloadfile(self.naifURL + self.naifLSK, self.spkLeapSecondFileName)
    
          currentLoc = self.gmatRootDirectory + self.leapSecDirectory

          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.spkLeapSecondFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
        
          currentLoc = os.getcwd() + '//'          
          newLoc = self.gmatRootDirectory + self.leapSecDirectory
          self.Movefile(currentLoc,newLoc,self.spkLeapSecondFileName)
          
       except:
           
          isOK = False;
          
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;
    
    def UpdateSPICEPlanetaryPCKFile(self):
       " Replaces the SPICE planetary PCK file with latest version "
       
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING SPICE PLANETARY PCK FILE ---------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       
       # PLANETARY_PCK_FILE = PLANETARY_COEFF_PATH/pck00010.tpc
       
       try:
           
          self.Downloadfile(self.naifURL + self.naifPCK, self.planetaryPCKFileName)

          currentLoc = self.gmatRootDirectory + self.eopDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.planetaryPCKFileName,
          self.GetArchivePrefix(),self.archiveOldFiles)
             
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.eopDirectory
          self.Movefile(currentLoc,newLoc,self.planetaryPCKFileName)
          
       except:
           
          isOK = False
          
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;


    def UpdateSPICEBPCFiles(self):
       " Replaces the SPICE Earth high precsion file with latest version "
       
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING SPK EARTH AND MOON BPC KERNELS -----------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;

       # try to download the other earth and moon files
       self.PrintTimeToScreenAndLog(
            "Attempting to download the other earth and moon pck kernels ")
            
       try:
                     
          currentLoc = self.gmatRootDirectory + self.eopDirectory
          newLoc = self.archiveDirectory
          
          # Luna BPC kernel
          
          self.Downloadfile(self.naifURL + self.naifLunaBPC, self.lunaBPCFilename)

          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.lunaBPCFilename,
                             self.GetArchivePrefix(),self.archiveOldFiles)
          
          # Earth high-precision EOP BPC kernel
          
          self.Downloadfile(self.naifURL + self.naifEarthHighPrecBPCFilename, self.earthHighPrecBPCFilename)

          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.earthHighPrecBPCFilename,
                             self.GetArchivePrefix(),self.archiveOldFiles)
          
          # Earth predict EOP kernel
          
          self.Downloadfile(self.naifURL + self.naifEarthBPCPredictFilename, self.earthBPCPredictFilename)

          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.earthBPCPredictFilename,
                             self.GetArchivePrefix(),self.archiveOldFiles)

          # Earth predict EOP kernel

          self.Downloadfile(self.naifURL + self.naifBPCFilename, self.earthBPCFilename)

          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.earthBPCFilename,
                             self.GetArchivePrefix(),self.archiveOldFiles)

          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.eopDirectory
          
          self.Movefile(currentLoc,newLoc,self.lunaBPCFilename)          
          self.Movefile(currentLoc,newLoc,self.earthHighPrecBPCFilename)
          self.Movefile(currentLoc,newLoc,self.earthBPCPredictFilename)
          self.Movefile(currentLoc,newLoc,self.earthBPCFilename)
          
       except:
           
         isOK = False
   
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;

    def UpdateSPICETFFiles(self):
       " Replaces the SPICE Luna FK kernel with latest versions "
       
       self.PrintToScreenAndLog( '')
       isOK = True;

       try:
           
          # Download the frame kernel for Luna (add others if needed)
          self.PrintToScreenAndLog('')
          self.PrintToScreenAndLog('--------UPDATING SPICE LUNA FK FILE ---------------------')
          self.PrintTimeToScreenAndLog("Process Began At ")
          
          self.Downloadfile(self.naifURL + self.naifLunaTFFilename, self.lunaFKFilename)

          currentLoc = self.gmatRootDirectory + self.eopDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.lunaFKFilename,
                             self.GetArchivePrefix(),self.archiveOldFiles)
                                  
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.eopDirectory
          self.Movefile(currentLoc,newLoc,self.lunaFKFilename)
       
       except:
           
          isOK = False
          self.PrintTimeToScreenAndLog("Process Finished At ")
          
       return isOK;
                                                      
    def UpdateIGRZFile(self):
       " Replaces the ig_rz.dat file used by IRI model with latest version "
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING IG_RZ FILE ----------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;

       try:
          self.Downloadfile(self.igrzDataURL,
                            self.igrzDataFileName )
    
          currentLoc = self.gmatRootDirectory + self.ionoDataDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.igrzDataFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
        
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.ionoDataDirectory
          self.Movefile(currentLoc,newLoc,self.igrzDataFileName)
       except:
          isOK = False
          
       if not isOK:
          self.PrintToScreenAndLog('ERROR downloading igrz.dat file!!!')  
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;
	
	
    def UpdateCSSISpaceWeatherFile(self):
       " Replaces the CSSI space weather file with latest version "
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING CSSI SPACE WEATHER FILE ----------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       # CSSI_FLUX_FILE = ATMOSPHERE_PATH/CSSI_2004To2026.txt
       try:
          self.Downloadfile(self.spaceWeatherURL,
                            self.cssiSpaceWeatherFileName )
    
          currentLoc = self.gmatRootDirectory + self.spaceWeatherDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.cssiSpaceWeatherFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
        
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.spaceWeatherDirectory
          self.Movefile(currentLoc,newLoc,self.cssiSpaceWeatherFileName)
       except:
          isOK = False
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;

    def UpdateCSSISpaceWeatherAllFile(self):
       " Replaces the CSSI space weather file with latest version "
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog('--------UPDATING CSSI SPACE WEATHER FILE ----------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       # CSSI_FLUX_FILE = ATMOSPHERE_PATH/CSSI_2004To2026.txt
       try:
          self.Downloadfile(self.spaceWeatherAllURL,
                            self.cssiSpaceWeatherAllFileName )
    
          currentLoc = self.gmatRootDirectory + self.spaceWeatherDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.cssiSpaceWeatherAllFileName,
                             self.GetArchivePrefix(),self.archiveOldFiles)
        
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.spaceWeatherDirectory
          self.Movefile(currentLoc,newLoc,self.cssiSpaceWeatherAllFileName)
       except:
          isOK = False
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;
	   
    def UpdateApFileForIRIModel(self):
       """ Replaces ap.dat used in IRI model by extacting data from CSSS Space
         Weather File"""
       self.PrintToScreenAndLog('')
       self.PrintToScreenAndLog(\
        '--------UPDATING ap.data file used by IRI model --------------------')
       self.PrintTimeToScreenAndLog("Process Began At ")
       isOK = True;
       try:
          self.Downloadfile( self.apDataURL, self.cssiSpaceDataFileName )
       
          # this is where we need to read the file and create our own mini-file
          step = 1
          inFile  = open(self.cssiSpaceDataFileName);
          outFile = open(self.apFileForIRIModel, 'w');
          # skip over records before 1958
          writeTheData = False;
          for line in inFile:
             line.rstrip()
             line.lstrip()
             if step == 1:
                if line.startswith("BEGIN OBSERVED"):
                   step = 2
             elif step == 2:
                if line.startswith("END OBSERVED"):
                   step = 3
                else:
                   writeTheData = self.ExtractContentAndWrite(line, outFile,
                                                              writeTheData)
             elif step == 3:
                if line.startswith("BEGIN DAILY_PREDICTED"):
                   step = 4
             elif step == 4:
                if line.startswith("END DAILY_PREDICTED"):
                   step = 5
                else:
                   writeTheData = self.ExtractContentAndWrite(line, outFile,
                                                              writeTheData)
       
          inFile.close()
          outFile.close()
          
          # delete the downloaded file
          os.remove(self.cssiSpaceDataFileName)
          self.PrintToScreenAndLog('Downloaded file ' +
                        self.cssiSpaceDataFileName + ' deleted successfully')
          
          currentLoc = self.gmatRootDirectory + self.ionoDataDirectory
          newLoc = self.archiveDirectory
          self.Handleoldfile(currentLoc,self.archiveDirectory,
                             self.apFileForIRIModel,
                             self.GetArchivePrefix(),self.archiveOldFiles)
                              
          currentLoc = os.getcwd() + '//'
          newLoc = self.gmatRootDirectory + self.ionoDataDirectory
          self.Movefile(currentLoc,newLoc,self.apFileForIRIModel)
       except:
          isOK = False
          # todo - do we need to make sure the files are closed here?
       self.PrintTimeToScreenAndLog("Process Finished At ")
       return isOK;
    
    def Movefile(self,currentLoc,newLoc,fileName):
        " Move file to new location "
        try:
           currentNameAndPath = currentLoc + fileName
           newNameAndPath = newLoc + fileName
           os.rename(currentNameAndPath,newNameAndPath)
        except urllib.error.URLError:
            self.PrintToScreenAndLog('Error moving ' + fileName)
            raise
        else:
            self.PrintToScreenAndLog(fileName + ' updated successfully')
        return
        
    def Downloadfile(self, url, file_name ):
        "Download the file from `url` and save it locally under `file_name`"
        flag = True
        try:
            print ('--------Downloading', file_name)
            urllib.request.urlretrieve(url, file_name)
        except urllib.error.URLError:
           self.PrintToScreenAndLog('Error downloading ' + file_name)
           flag = False
           raise
        except:
           self.PrintToScreenAndLog('Error downloading ' + file_name)
           flag = False
           raise
        else:
            self.PrintToScreenAndLog(file_name + ' downloaded successfully ')
        return flag
        
    
    def DownloadFTP( self, url, directory, prefix, file_ext, newfilename = '' ):
        "Download files that start with Prefix and Ends with Suffix"
        ftp = FTP(url)
        ftp.login() # anonymous login
        ftp.cwd(directory)
    
        # iterate through list of files and get all 'PREFIX*SUFFIX' files
        files = ftp.nlst()
    
        numFiles = 0
        try:                                                                       
            for i,v in enumerate(files,1):
                filename, file_extension = os.path.splitext(v)
                
                if (file_extension.lower() == file_ext) and \
                   (filename.lower().startswith(prefix)):
                    self.PrintToScreenAndLog('--------Downloading ' +  v)
                    if (newfilename == ''):
                        local_filename = v
                    else:
                        local_filename = newfilename
                    file = open(local_filename, 'wb')
                    # ftp.retrlines
                    ftp.retrbinary("RETR " + v, file.write)
                    self.PrintToScreenAndLog(
                         local_filename + ' downloaded successfully')
                    numFiles = numFiles + 1
                    file.close()
        except:
           raise
        finally:
            ftp.quit()
            if numFiles == 0:
               raise FileNotFoundError
               return False
        return True
        
    def Handleoldfile(self,currentLoc,newLoc,fileName,filePrefix,archiveMode):
        " Archives or deletes old files depending upon user config"
        if os.path.isfile(currentLoc+fileName):
           if archiveMode:
              self.Archivefile(currentLoc,newLoc,fileName,filePrefix)
           else: 
              os.remove(currentLoc+fileName)
        else:
            self.PrintToScreenAndLog("Warning: " + currentLoc+fileName +
                 " not found so no archiving or moving performed ")
            
        
    def Archivefile(self,currentLoc,newLoc,fileName,filePrefix):
        " Move file to new location and rename with defined prefix "
        #filecmp.cmp(filename1, filename2, shallow=False)
        try:
           currentNameAndPath = currentLoc + fileName
           newNameAndPath = newLoc + filePrefix + fileName
           os.rename(currentNameAndPath,newNameAndPath)
        except urllib.error.URLError:
            self.PrintToScreenAndLog('Error archiving ' + fileName)
            raise
        else:
            self.PrintToScreenAndLog(fileName + ' archived successfully to ' +
                                     filePrefix + fileName )

    def ExtractContentAndWrite(self, line, theFile, writeIt):
       if writeIt == False:
          if (line[2] == '5') and (line[3] == '8'):
             writeIt = True
       newString = ' '
       # extract the year
       if line[2] == '0':
          newString += ' '
       else:
          newString += line[2]
       newString += line[3]
       
       # extract the month
       newString += ' '
       if line[5] == '0':
          newString += ' '
       else:
          newString += line[5]
       newString += line[6]
      
       # extract the day
       newString += ' '
       if line[8] == '0':
          newString += ' '
       else:
          newString += line[8]
       newString += line[9]
       
       # extract the Ap1 - Ap8
       #newString += line[46:78]
       for n in range (0,8):
          index = 46 + (n*4)
          newString += line[index + 1]
          newString += line[index + 2]
          newString += line[index + 3]

       # extract the F10.7  (C++ only went to 97 - I need 98 here)
       newString += line[93:98]
       
       newString += '\n'

       # if we are up to 1958 (since we're skipping dates before then)
       if writeIt == True:
          theFile.write(newString)
       return writeIt


# ----------------------- EDIT BELOW THIS LINE ONLY ----------------------------
# NOTE - these must be changed to point to your GMAT location and the location
# where you want the file(s) to be backed up.  You may also specify a name for
# your log file.

gmatLocation = r"C:\path\to\gmat\toplevel"
archLoc = "C://Temp//"
logName = "fileupdatelog.txt"

try:
    
   fManager = GMATFileManager(gmatLocation,archiveDirectory = archLoc,
                           logFileName = logName  )
                           
   isOK = True
   
   # Run this to download/archive/update all of the files
   isOK = fManager.UpdateAllFiles()
   
   # OR run the updater(s) only for those files you wish to be updated
   #isOK = isOK and fManager.UpdateGMATLeapSecondFile()
   #isOK = isOK and fManager.UpdateSPICELeapSecondFile()
   #isOK = isOK and fManager.UpdateGMATEOPFile()
   #isOK = isOK and fManager.UpdateSPICEPlanetaryPCKFile()
   #isOK = isOK and fManager.UpdateSPICEBPCFiles()
   #isOK = isOK and fManager.UpdateSPICETFFiles()
   #isOK = isOK and fManager.UpdateIGRZFile()
   #isOK = isOK and fManager.UpdateCSSISpaceWeatherFile()
   #isOK = isOK and fManager.UpdateCSSISpaceWeatherAllFile()
   #isOK = isOK and fManager.UpdateApFileForIRIModel()

   if (not isOK):
      fManager.PrintToScreenAndLog(
            'ERROR - there was an issue loading the file(s) specified\n')
            
except ValueError:
   print ('Error: please edit the python script to specify data and archive directories\n')

