ClearAll

load(['all_data_before_optimization_HiFiDynamics_no_minimum_power_and_no_bus_power.mat']) 

obj1 = obj;

clear obj

load(['all_data_before_optimization_LoFiDynamics.mat']) 

obj2 = obj;

clear obj

constraintNames = obj1.GetConstraintVectorNames();

[row,col] = find(obj2.phaseList{2}.conSparsityPattern-obj1.phaseList{2}.conSparsityPattern);

[m,n] = size(obj2.phaseList{1}.conSparsityPattern);

for i = 1:length(row)
    row_string = constraintNames{m+row(i)};
	% disp(['col = ',num2str(col(i)),' row = ',num2str(row(i))])
    which = mod(col(i),10);
    if col(i) == 1
       col_string = 't0';
    elseif col(i) == 2
       col_string = 'tf';
    else
        switch which
            case 1
                col_string = 'uy';
            case 2
                col_string = 'uz';
            case 3
                col_string = 'x';
            case 4
                col_string = 'y';
            case 5
                col_string = 'z';
            case 6
                col_string = 'vx';
            case 7
                col_string = 'vy';
            case 8
                col_string = 'vz';
            case 9
                col_string = 'mass';
            case 0
                col_string = 'ux';
        end
    end
    
    %disp([row_string,' with respect to mesh point ',num2str(floor(col(i)/10)),': ',col_string])

end

[row,col] = find(obj1.phaseList{2}.conSparsityPattern);

for i = 1:length(row)
    row_string = constraintNames{m+row(i)};
	% disp(['col = ',num2str(col(i)),' row = ',num2str(row(i))])
    which = mod(col(i),10);
    if col(i) == 1
       col_string = 't0';
    elseif col(i) == 2
       col_string = 'tf';
    else
        switch which
            case 1
                col_string = 'uy';
            case 2
                col_string = 'uz';
            case 3
                col_string = 'x';
            case 4
                col_string = 'y';
            case 5
                col_string = 'z';
            case 6
                col_string = 'vx';
            case 7
                col_string = 'vy';
            case 8
                col_string = 'vz';
            case 9
                col_string = 'mass';
            case 0
                col_string = 'ux';
        end
    end
	if ceil(row(i)/7) ~= 60
		continue
	end
    disp([row_string,' with respect to mesh point ',num2str(floor(col(i)/10)),': ',col_string])

end

% max(abs(obj2.decisionVector-obj1.decisionVector))
% max(abs(obj2.decisionVecLowerBound-obj1.decisionVecLowerBound))
% max(abs(obj2.decisionVecUpperBound-obj1.decisionVecUpperBound))
% max(abs(obj2.allConUpperBound-obj1.allConUpperBound))
% max(abs(obj2.allConLowerBound-obj1.allConLowerBound))
% max(max(abs(obj2.sparsityPattern-obj1.sparsityPattern)))
% max(max(abs(obj2.phaseList{1}.nlpConstraintJac-obj1.phaseList{1}.nlpConstraintJac)))
% max(max(abs(obj2.phaseList{1}.nlpCostJac-obj1.phaseList{1}.nlpCostJac)))
% max(max(abs(obj2.phaseList{1}.conSparsityPattern-obj1.phaseList{1}.conSparsityPattern)))
% max(max(abs(obj2.phaseList{1}.costSparsityPattern-obj1.phaseList{1}.costSparsityPattern)))
% max(max(abs(obj2.phaseList{2}.costSparsityPattern-obj1.phaseList{2}.costSparsityPattern)))
% max(max(abs(obj2.phaseList{2}.conSparsityPattern-obj1.phaseList{2}.conSparsityPattern)))
% max(max(abs(obj2.phaseList{2}.nlpCostJac-obj1.phaseList{2}.nlpCostJac)))
% max(max(abs(obj2.phaseList{2}.nlpConstraintJac-obj1.phaseList{2}.nlpConstraintJac)))



