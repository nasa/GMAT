
function [observed, predicted] = earth_orientation
    % Specifies earth orientation parameters. 
    % Source: http://www.celestrak.com/SpaceData/eop20080101.txt


    usno_url = 'http://www.celestrak.com/SpaceData/eop20080101.txt';
     % read data from the url
    [file_str,status]=urlwrite(usno_url,'usno_final.txt');
    fid = fopen('usno_final.txt');

    % parse the data
    tline=fgets(fid);
    read_observed=0;
    read_predicted=0;

    % error handling
    if ~status
        error('Could not read data from site!')
    end
    i=1;
    j=1;
    while ischar(tline);
         tline=fgets(fid);
         if isnumeric(tline)
             break
         end
         if strcmp('BEGIN OBSERVED',deblank(tline))
             read_observed =1;
             tline=fgets(fid);
         elseif strcmp('END OBSERVED',deblank(tline))
             read_observed=0;
         elseif strcmp('BEGIN PREDICTED',deblank(tline))
             read_predicted =1;
             tline=fgets(fid);
         elseif strcmp('END PREDICTED',deblank(tline))
             read_predicted=0;

         end

         if read_observed
                line_read = strsplit(tline);
                observed.utc_year(i)=str2double(line_read(1));
                observed.utc_month(i)=str2double(line_read(2));
                observed.utc_day(i)=str2double(line_read(3));
                observed.utc_num(i)=datenum([observed.utc_year(i),observed.utc_month(i),observed.utc_day(i)]);
                observed.observedmjd(i)=str2double(line_read(4));
                observed.x(i)=str2double(line_read(5));
                observed.y(i)=str2double(line_read(6));
                observed.UT1minusUTC(i) = str2double(line_read(7));
                observed.lod(i)=str2double(line_read(8));
                observed.dpsi(i)=str2double(line_read(9));
                observed.deps(i)=str2double(line_read(10));
                observed.dx(i)=str2double(line_read(11));
                observed.dy(i)=str2double(line_read(12));
                observed.dat(i)=str2double(line_read(13));
                i=i+1;
         end
         if read_predicted
                line_read = strsplit(tline);
                predicted.utc_year(j)=str2double(line_read(1));
                predicted.utc_month(j)=str2double(line_read(2));
                predicted.utc_day(j)=str2double(line_read(3));
                predicted.utc_num(j)=datenum([predicted.utc_year(j),predicted.utc_month(j),predicted.utc_day(j)]);
                predicted.predictedmjd(j)=str2double(line_read(4));
                predicted.x(j)=str2double(line_read(5));
                predicted.y(j)=str2double(line_read(6));
                predicted.UT1minusUTC(j) = str2double(line_read(7));
                predicted.lod(j)=str2double(line_read(8));
                predicted.dpsi(j)=str2double(line_read(9));
                predicted.deps(j)=str2double(line_read(10));
                predicted.dx(j)=str2double(line_read(11));
                predicted.dy(j)=str2double(line_read(12));
                predicted.dat(j)=str2double(line_read(13));
                j=j+1;
         end

    end
end
