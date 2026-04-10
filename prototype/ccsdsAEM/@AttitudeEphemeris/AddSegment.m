function AddSegment(obj,Segment)

%  Adds a segment to an AttitudeEphemeris object

obj.numSegments = obj.numSegments + 1;
obj.Segments{obj.numSegments} = Segment;