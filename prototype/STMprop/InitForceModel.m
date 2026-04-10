function ForceModel = InitForceModel(ForceModel);

%     1 -> Mercury          8 -> Neptune
%     2 -> Venus            9 -> Pluto
%     3 -> Earth           10 -> Moon
%     4 -> Mars            11 -> Sun
%     5 -> Jupiter         12 -> Solar system barycenter
%     6 -> Saturn          13 -> Earth-Moon barycenter
%     7 -> Uranus          14 -> Nutations (longitude and obliquity)
%               15 -> Librations, if on file

%--------------------------------------------------------------------------
%  Set DE file index for central body
%--------------------------------------------------------------------------
if strcmp(ForceModel.CentralBody,'Mercury')
    ForceModel.CentralBodyIndex = 1;
end

if strcmp(ForceModel.CentralBody,'Venus')
    ForceModel.CentralBodyIndex = 2;
end

if strcmp(ForceModel.CentralBody,'Earth')
    ForceModel.CentralBodyIndex = 3;
end

if strcmp(ForceModel.CentralBody,'Mars')
    ForceModel.CentralBodyIndex = 4;
end

if strcmp(ForceModel.CentralBody,'Jupiter')
    ForceModel.CentralBodyIndex = 5;
end

if strcmp(ForceModel.CentralBody,'Saturn')
    ForceModel.CentralBodyIndex = 6;
end

if strcmp(ForceModel.CentralBody,'Uranus')
    ForceModel.CentralBodyIndex = 7;
end

if strcmp(ForceModel.CentralBody,'Neptune')
    ForceModel.CentralBodyIndex = 8;
end

if strcmp(ForceModel.CentralBody,'Pluto')
    ForceModel.CentralBodyIndex = 9;
end

if strcmp(ForceModel.CentralBody,'Luna')
    ForceModel.CentralBodyIndex = 10;
end

if strcmp(ForceModel.CentralBody,'Sun')
    ForceModel.CentralBodyIndex = 11;
end

%--------------------------------------------------------------------------
%  Set DE file indeces for point mass bodies
%--------------------------------------------------------------------------
j = 0;
for i = 1:size(ForceModel.PointMasses,2);

    if strcmp(ForceModel.PointMasses{i},'Mercury')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 1;
    end

    if strcmp(ForceModel.PointMasses{i},'Venus')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 2;
    end

    if strcmp(ForceModel.PointMasses{i},'Earth')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 3;
    end

    if strcmp(ForceModel.PointMasses{i},'Mars')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 4;
    end

    if strcmp(ForceModel.PointMasses{i},'Jupiter')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 5;
    end

    if strcmp(ForceModel.PointMasses{i},'Saturn')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 6;
    end

    if strcmp(ForceModel.PointMasses{i},'Uranus')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 7;
    end

    if strcmp(ForceModel.PointMasses{i},'Neptune')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 8;
    end

    if strcmp(ForceModel.PointMasses{i},'Pluto')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 9;
    end

    if strcmp(ForceModel.PointMasses{i},'Luna')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 10;
    end

    if strcmp(ForceModel.PointMasses{i},'Sun')
        j = j + 1;
        ForceModel.PointMassIndeces{j} = 11;
    end

end
