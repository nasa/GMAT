function Initialize(obj,struct)

%  These are required fields

if ~isempty(struct.OBJECT_NAME)
    obj.OBJECT_NAME = struct.OBJECT_NAME;
else
    disp(['Warning: OBJECT_NAME is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.OBJECT_ID)
    obj.OBJECT_ID = struct.OBJECT_ID;
else
    disp(['Warning: OBJECT_ID is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.CENTER_NAME)
    obj.CENTER_NAME = struct.CENTER_NAME;
else
    disp(['Warning: CENTER_NAME is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.REF_FRAME_A)
    obj.REF_FRAME_A = struct.REF_FRAME_A;
else
    disp(['Warning: REF_FRAME_A is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.REF_FRAME_B)
    obj.REF_FRAME_B = struct.REF_FRAME_B;
else
    disp(['Warning: REF_FRAME_B is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.REF_FRAME_A)
    obj.REF_FRAME_A = struct.REF_FRAME_A;
else
    disp(['Warning: REF_FRAME_A is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.ATTITUDE_DIR)
    obj.ATTITUDE_DIR = struct.ATTITUDE_DIR;
else
    disp(['Warning: ATTITUDE_DIR is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.TIME_SYSTEM)
    % TODO: validate TIME_SYTEM value
    obj.TIME_SYSTEM = struct.TIME_SYSTEM;
else
    disp(['Warning: TIME_SYSTEM is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.START_TIME)
    obj.START_TIME = struct.START_TIME;
else
    disp(['Warning: TIME_SYSTEM is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.USEABLE_START_TIME)
    obj.USEABLE_START_TIME = struct.USEABLE_START_TIME;
else
    disp(['Warning: USEABLE_START_TIME is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.USEABLE_STOP_TIME)
    obj.USEABLE_STOP_TIME = struct.USEABLE_STOP_TIME;
else
    disp(['Warning: USEABLE_STOP_TIME is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.STOP_TIME)
    obj.STOP_TIME = struct.STOP_TIME;
else
    disp(['Warning: STOP_TIME is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.ATTITUDE_TYPE)
    obj.ATTITUDE_TYPE = struct.ATTITUDE_TYPE;
else
    disp(['Warning: ATTITUDE_TYPE is a required Metadata field but was '...
        ' not provided for a segment in AEM file']);
end

if ~isempty(struct.INTERPOLATION_METHOD)
    %  TODO: Validate 
    obj.INTERPOLATION_METHOD = struct.INTERPOLATION_METHOD;
end

if ~isempty(struct.INTERPOLATION_DEGREE)
    obj.INTERPOLATION_DEGREE = struct.INTERPOLATION_DEGREE;
end

obj.InitializeUniqueFields(struct)

