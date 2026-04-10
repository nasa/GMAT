 function tdrss = tdrss_tle_reader(varargin)
    %   tdrss = tdrss_tle_reader('use_config_tle',1) 
    %   Reads in the tdrss ephemeris from celestrak, parses the TLE
    %
    %   Fundamentals of Astrodynamics and Applications Fourth Edition
    %   Vallado
    % 
    %    varargin: use_config_tle = 1 uses a configured tle file'
    
    % norad url with tdrss data
    norad_ephem_url = 'http://celestrak.com/NORAD/elements/tdrss.txt';
    
    if strcmp(varargin{1},'use_config_tle')
        use_config_tle=varargin{2};
    end
    
    % read in the url
    if isempty(varargin)
        % look up tles
        fprintf('Grabbing tdrs tle from the web \n\n')
        [s, status] = urlread(norad_ephem_url);
        str=strsplit(s);
        % use this if its the config tle
    elseif use_config_tle
        fprintf('Using tle config file for tdrss \n\n')
        s=fileread('tdrss_tle_lookup');
        str=strsplit(s);
        status=1;
        
    else
        error('Improper input!')
    end
    
    
    % parse data
    if status
        % split the string
        
        % find TDRSS TLE's
        j=1;
        for i=1:length(str)
            if strcmp(str(i),'TDRS')
                tdrss.id(j) = str(i+1);
                % parse the data
                tdrss.card_num1(j) = str(i+2);
                tdrss.sat_num1(j) = str(i+3);
                tdrss.intdes(j) = str(i+4);
                tdrss.epoch_str(j) = str(i+5);
                tdrss.year(j) = str2double(strcat('20',tdrss.epoch_str{j}(1:2)));
                tdrss.doy(j)  = str2double(tdrss.epoch_str{j}(3:end));
                [mon,day,hr,minute,sec] = days2mdh ( tdrss.year(j),tdrss.doy(j));
                tdrss.epoch(j)= datenum([tdrss.year(j),mon,day,hr,minute,sec]);
                tdrss.juliandate(j)=jdayall(tdrss.year(j),mon,day,hr,minute,sec,'g');
                tdrss.dndt(j) = str2double(str(i+6));
                tdrss.dndt2(j) = (str(i+7));
                % process B*
                tdrss.bstar_str(j) = (str(i+8));
                % find sign
                sign_index = strfind(char(tdrss.bstar_str(j)),'-');
                if isempty(sign_index)
                    sign_index = strfind(char(tdrss.bstar_str(j)),'+');
                end
                %construct value
                if ~isempty(sign_index)
                    n=str2num(tdrss.bstar_str{j}(sign_index+1:end));
                    num = str2num(strcat('0.',tdrss.bstar_str{j}(1:sign_index-1)));
                    tdrss.bstar(j) = num*10^(-n);
                   
                else
                    n=str2num(tdrss.bstar_str{j}(sign_index+1:end));
                    num = str2num(strcat('0.',tdrss.bstar_str{j}(1:sign_index-1)));
                    tdrss.bstar(j) = num*10^(+n);
                    
                end
                
                tdrss.eph(j) = str(i+9);
                tdrss.elem_num(j) = str(i+10);
                tdrss.card_num2(j) = str(i+11);
                tdrss.sat_num2(j) = str(i+12);
                tdrss.inc(j) = str2double(str(i+13));
                tdrss.raan(j) = str2double(str(i+14));
                tdrss.ecc(j) = str2double(strcat('.',(str(i+15))));
                tdrss.argp(j) = str2double(str(i+16));
                tdrss.M(j) = str2double(str(i+17));
                tdrss.n(j) = str2double(str(i+18));

                % increment the satellite
                j=j+1;
                
            end
        end


    else
       error('unable to read ephemeris from NORAD url') 
    end

end