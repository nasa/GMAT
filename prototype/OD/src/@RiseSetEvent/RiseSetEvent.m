classdef RiseSetEvent < Event
   
   properties  (SetAccess = 'public')
        Station
        Sat
        functionValue
        
   end
    
   methods
       
       function event = RiseSetEvent()
           
       end
       
       function event = Initialize(event,Sandbox)
           
           event.Sat = Sandbox.GetHandle(event.Sat);
           event.Station = Sandbox.GetHandle(event.Station);
           
       end
       
       function event = EvaluateEvent(event)
           
           stationLoc = event.Station.InertialState(event.Sat.Epoch);
           satLoc     = [event.Sat.X event.Sat.Y event.Sat.Z]';
           rangeVec   = satLoc - stationLoc;
           event.functionValue    = rangeVec'*stationLoc/norm(rangeVec)/norm(stationLoc);
           
       end
       
   end
    
end