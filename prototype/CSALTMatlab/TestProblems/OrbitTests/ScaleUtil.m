classdef ScaleUtil
    %UNTITLED5 Summary of this class goes here
    %   Detailed explanation goes here
    
    properties
        TU
        VU
        DU
        MU
        ACCU
        MDOTU
        RefEpoch
    end
    
    methods
        function state = ScaleState(obj,state)
           state(1) = state(1)/obj.DU; 
           state(2) = state(2)/obj.DU;
           state(3) = state(3)/obj.DU;
           state(4) = state(4)/obj.VU;
           state(5) = state(5)/obj.VU;
           state(6) = state(6)/obj.VU;
           state(7) = state(7)/obj.MU;
        end
        function state = UnScaleState(obj,state)
           state(1) = state(1)*obj.DU; 
           state(2) = state(2)*obj.DU;
           state(3) = state(3)*obj.DU;
           state(4) = state(4)*obj.VU;
           state(5) = state(5)*obj.VU;
           state(6) = state(6)*obj.VU;
           state(7) = state(7)*obj.MU;
        end
        function state = ScaleStateRate(obj,state)
           state(1) = state(1)/obj.VU; 
           state(2) = state(2)/obj.VU;
           state(3) = state(3)/obj.VU;
           state(4) = state(4)/obj.ACCU;
           state(5) = state(5)/obj.ACCU;
           state(6) = state(6)/obj.ACCU;
           state(7) = state(7)/obj.MDOTU;
        end
        function time = ScaleTime(obj,time)
            time = (time - obj.RefEpoch)*86400/obj.TU;
        end
        function time = UnScaleTime(obj,time)
            time = time*obj.TU/86400 + obj.RefEpoch;
        end

    end
    
end

