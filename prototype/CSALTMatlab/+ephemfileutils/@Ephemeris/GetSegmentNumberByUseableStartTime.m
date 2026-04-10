function segmentNumber = GetSegmentNumberByUseableStartTime(obj, epoch)
    %GETSEGMENTNUMBERBYUSEABLESTARTTIME Return the number of the segment with the given USEABLE_START_TIME.
    
    %   Author: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    
    segmentNumber = find([obj.Segments.UseableStartTime] == epoch);
    
end
