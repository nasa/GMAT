function datevec = datestr2vec(str)


yr  = str2num(str(8:12));
mon = str(4:6);
day = str2num(str(1:2));
hr  = str2num(str(13:14));
min = str2num(str(16:17));
sec = str2num(str(19:24));

%  Convert month to integer
if strcmp(mon,'Jan')
    mon = 1;
end
if strcmp(mon,'Feb')
    mon = 2;
end
if strcmp(mon,'Mar')
    mon = 3;
end
if strcmp(mon,'Apr')
    mon = 4;
end
if strcmp(mon,'May')
    mon = 5;
end
if strcmp(mon,'Jun')
    mon = 6;
end
if strcmp(mon,'Jul')
    mon = 7;
end
if strcmp(mon,'Aug')
    mon = 8;
end
if strcmp(mon,'Sep')
    mon = 9;
end
if strcmp(mon,'Oct')
    mon = 10;
end
if strcmp(mon,'Nov')
    mon = 11;
end
if strcmp(mon,'Dec')
    mon = 12;
end

if isstr(mon)
    disp('Format of month in given date is incorrect in datestr2vec!!')
    stop
end

datevec = [yr mon day hr min sec];