classdef  Map < handle
    
    properties (SetAccess = 'public')
        Contents = {};
    end
    %----- Set the public properties
    properties  (SetAccess = 'private')

        Ids      = {};
        numObj   = 0;

    end
    
    %-----
    
    methods
        
        %----- Add item to the map
        function map = Add(map,obj,Id)
            
            if ~map.GetIndex(Id)
                map.numObj               = map.numObj + 1;
                map.Contents{map.numObj} = obj;
                map.Ids{map.numObj}      = Id;
            end
            
        end % Add
        
        %----- Get an item from the map
        function Objects = Get(map,Ids)
                        
            numIds  = size(Ids,1);
            Objects = cell(numIds,1);
            for i = 1:numIds
                %  If Id is a cell aray
                if iscell(Ids)
                    currentId = Ids{i};
                %  If Id is a string
                elseif ischar(Ids)
                    currentId = Ids;
                %  If Id is a real array
                else
                    currentId = Ids(i);
                end
                index = map.GetIndex(currentId);
                if index
                    % If only one Id, output the object itself
                    if numIds == 1
                        Objects = map.Contents{index};
                    % If more than one Id, output all in cell array
                    else
                        Objects{i} = map.Contents{index};
                    end
                end
            end
            
        end %  Get
        
        %----- Get the index of item in map
        function flag = GetIndex(map,Id)
           
            %  Return 0 if Id is not in the map
            flag  = 0;
            count = 1;
            while count <= map.numObj & flag == 0
                if isequal(Id,map.Ids{count})
                   flag = count;    
                end
                count = count + 1;
            end
            
        end  % GetIndex
        
    end
        
end