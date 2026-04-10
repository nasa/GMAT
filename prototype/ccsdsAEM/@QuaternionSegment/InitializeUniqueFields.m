function InitializeUniqueFields(obj,struct)

%  Set unique fields
if ~isempty(struct.QUATERNION_TYPE)
    obj.QUATERNION_TYPE = struct.QUATERNION_TYPE;
else
    disp(['Error: QUATERNION_TYPE is a required Metadata field  for '...
          ' segment of type QUATERNION but was  not provided for a' ...
          ' segment in AEM file']);
end

%  Throw warnings for fields that are set but not applicable
if ~isempty(struct.EULER_ROT_SEQ)
    disp(['Warning: EULER_ROT_SEQUENCE is set for QUATERNION type' ...
          ' segment and will not be used']);
end

if ~isempty(struct.RATE_FRAME)
    disp(['Warning: RATE_FRAME is set for QUATERNION type' ...
          ' segment and will not be used']);
end

