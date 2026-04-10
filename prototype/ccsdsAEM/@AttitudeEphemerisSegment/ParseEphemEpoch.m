function serialDate = ParseEphemEpoch(obj,dateStr)

dateIsType1 = ~isempty(regexpi(dateStr, ...
    '\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z?'));
dateIsType2 = ~isempty(regexpi(dateStr, ...
    '\d{4}-\d{3}T\d{2}:\d{2}:\d{2}(?:\.\d+)?Z?'));

if dateIsType1
    dateVec = sscanf(dateStr, '%4u-%2u-%2uT%2u:%2u:%f');
    serialDate = datenum(dateVec');%- datenum('01-Jan-2000');
elseif dateIsType2
    dateVec = sscanf(dateStr, '%4u-%3uT%2u:%2u:%f');
    serialDate = datenum([dateVec(1), 0, dateVec(2:end)']);
else
    throw(MException('Events:ParseEphemeris:InvalidDateFormat', ...
        'Cannot convert date string %s', dateStr));
end

end