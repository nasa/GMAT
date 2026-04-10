

%==========================================================================
%==========================================================================
%------  Intialize the Object store 
%==========================================================================
%==========================================================================

global ObjectStore;

%---- Hard code initializing objects into Object Store.  Eventually this
%     should be done in an automated way.

ODSat.pointer = 1;
ObjectStore.Objects{1} = ODSat; 
ObjectStore.Names{1}   = 'ODSat';
ObjectStore.Types{1}   = 1;

Canberra.pointer = 2;
ObjectStore.Objects{2} = Canberra; 
ObjectStore.Names{2}   = 'Canberra';
ObjectStore.Types{2}   = 2;

CanberraData.pointer = 3;
ObjectStore.Objects{3} = CanberraData; 
ObjectStore.Names{3}   = 'CanberraData';
ObjectStore.Types{3}   = 3;

BLS.pointer = 4;
ObjectStore.Objects{4} = BLS; 
ObjectStore.Names{4}   = 'BLS';
ObjectStore.Types{4}   = 4;

ODProp.pointer = 5;
ObjectStore.Objects{5} = ODProp; 
ObjectStore.Names{5}   = 'ODProp';
ObjectStore.Types{5}   = 5;

ObjectStore.Objects{6} = 'RunEstimator';
ObjectStore.Names{6} = 'RunEstimator';
ObjectStore.Types{6} = 'RunEstimator';

%==========================================================================
%==========================================================================
%------  Intialize the objects
%==========================================================================
%==========================================================================

for i = 1:size(ObjectStore.Objects,2)
    
    if ObjectStore.Types{i} == 1
        
       ObjectStore.Objects{i} = Spacecraft_Initialize(ObjectStore.Objects{i});
       
    end
    
    if ObjectStore.Types{i} == 4
        
       ObjectStore.Objects{i} = Estimator_Initialize(ObjectStore.Objects{i});
       
    end
    
end

%==========================================================================
%==========================================================================
%------  Intialize the commands
%==========================================================================
%==========================================================================

ObjectStore.Objects{5} = RunEstimator_Initialize(ObjectStore.Objects{5},ObjectStore.Objects{4});
       





