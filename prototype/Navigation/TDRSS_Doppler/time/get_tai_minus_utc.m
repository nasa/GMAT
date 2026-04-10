
function taiminusutc = get_tai_minus_utc
% find the offset between tai and utc
    usna_url = 'http://maia.usno.navy.mil/ser7/tai-utc.dat';

    % read in the url
    [s, status] = urlread(usna_url);
    
    % split string
    s_split = strsplit(s);
    
    for i=1:length(s_split)
        if strcmp('TAI-UTC=',s_split{i})
            taiminusutc=str2double(s_split{i+1});
            found=1;
        elseif i==length(s_split) && ~found
            fprintf('tai-utc not found or the website is down\n')
        end
        
    end
  
end