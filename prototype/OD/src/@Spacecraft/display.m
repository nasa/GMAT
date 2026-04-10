function display(Sat);

%  Extract the fi
fieldNames = fieldnames(Sat);
numFields  = size(fieldNames);

%  Determine the maximum field length
maxLength  = 0;
for i = 1:numFields
    lengthField = length(fieldNames{i});
    if lengthField > maxLength
        maxLength = lengthField;
    end
end

%  Write the fields
for i = 1:numFields
    
    lengthField = length(fieldNames{i});
    padding = num2str(maxLength);
    tempStr = sprintf([' %' padding 's'], fieldNames{i});
    currField = Sat.(fieldNames{i});
    tempStr   = [tempStr ':  '];
    
    % if a string
     if ischar(currField)
        tempStr = [tempStr currField];
        disp(tempStr);
     else 
         [m,n]   = size(currField);
         %  if a scalar real
         if m == 1 & n == 1;
             tempStr = [tempStr  num2str(currField,8)];
             disp(tempStr);
         else
             %  if an array of reals
             for i = 1:m
                 if i == 1
                     rowStr = [tempStr];
                 else
                     rowStr = sprintf([' %' num2str(maxLength + 3) 's'],'');
                 end
                 for j = 1:n
                     newNum = sprintf('%10.5e',currField(i,j));
                     rowStr = [rowStr   newNum '  '];
                 end
                 disp(rowStr);
             end
         end
     end
    
end

