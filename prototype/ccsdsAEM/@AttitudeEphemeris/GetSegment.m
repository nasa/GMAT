function [segment] = GetSegment(obj,segIdx)

%  Returns the requested segment if it exists

if segIdx > obj.numSegments
    segment = [];
    disp(['Error:  Requested segment number ' num2str(segIdx) ...
          ' but only ' num2str(obj.numSegments) ' are on the file']);
else
    segment = obj.Segments{segIdx};
end

