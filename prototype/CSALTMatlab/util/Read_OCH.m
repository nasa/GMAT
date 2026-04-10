function mData = Read_OCH(sDirectory, sFileName)

mData = [];

sDirectory_Full = [sDirectory,'\', sFileName];

cData_Raw = importdata(sDirectory_Full);

aIndex_Start = find(ismember(cData_Raw,'DATA_START')) + 1;
aIndex_Stop  = find(ismember(cData_Raw,'DATA_STOP')) - 1;

for vIndex_1 = 1:length(aIndex_Start)
    for vIndex_2 = aIndex_Start(vIndex_1):aIndex_Stop(vIndex_1)
        aData = str2num(cData_Raw{vIndex_2});
        
        if length(aData) == 8
            aData = [aData 0 0 0];
        end
        tData = mData;
        mData = [mData;aData];
    end
end