classdef Event < handle
   
   properties  (SetAccess = 'public')
        isTerminal;
        direction;
        value;
        type;
        funcHandles;
        rootValues;
   end
    
   methods
       
       function event = Event()
           
       end
       
   end
    
end