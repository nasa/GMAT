function Initialize(obj)

%  Read an AEM file and configure an AttitudeEphemeris object

%  Open the file
OpenFile(obj)

%  Read the file header
%ParseAttitudeEphemerisHeader(obj);

%  Parse the file
ParseFile(obj)

%  Close the file
CloseFile(obj)