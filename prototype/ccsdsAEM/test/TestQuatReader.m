%% Create an ephemeris object and parse the file
%
%  Note: if you are not running the test driver from the folder
%  ccsdsAEM\test folder, you will need to modify the ephem file paths

aemFile = AttitudeEphemeris();


%%  Tests for header errors
aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest.aem';
aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_CommentBeforeVersion.aem';
aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_NoVersionNumber.aem';
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_NoOriginator.aem';
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_NoCreationDate.aem';
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_InvalidLineInHeaderr.aem';


%%  Test for metadata errors
aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_InvalidLineInMetaData.aem';

%  specific to QUATERNION type
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_Quaternion_MissingQuaternionType.aem';
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_Quaternion_EulerRotSequenceSet.aem';
%aemFile.EphemerisFile = '..\ephemfiles\InvalidFileTest_Quaternion_RateFrameSet.aem';

%% Test for reading valid files

% Quaternion file test
%aemFile.EphemerisFile = '..\ephemfiles\QuaternionFile.aem';
aemFile.EphemerisFile = '..\ephemfiles\KARI_AEM_File.txt';

aemFile.Initialize();


%  TODO

%========    Numerical and Segment work
%  - Determine how to handle time system for interpolation
%  - Update epoch parsing to convert to interp time system
%  - Write SLERP interpolation algorithm for QUATERNION
%  - Add other attitude segment types
%  - Add Write functionality

%=========    Validation work
%  - Determine which RHS values will be accepted for things like Ref_Frame_A
%  - Validate all RHS values that can be validated
%  - Add higher order interpolation for quaternion
%  - Finish validation of possible errors in Metadata section
%  - Finish validation of possible errors in Data section
%  - Save comments for writing
%  - Review spec and determine all requirements on file that 
%    can be validated and validate the file for those requirements.
%  - Write tests cases for other file errors/warnings

