clc;clear all;close all;format long g;

% *.och filename
fname = 'DROtoL2.och';

% Get data
[nx, nu, data] = getOchData(fname);

% Extract Data
time = data(:,1);
state = data(:,2:1+nx);
control = data(:,1+nx+1:1+nx+nu);

figure(1)
hold on
plot(state(:,1),state(:,2),'linewidth',2)
quiver(state(:,1),state(:,2),state(:,4),state(:,5))
hold off
axis equal
xlabel('X [ND]')
ylabel('Y [ND]')
grid on
set(gca,'fontsize',14)

% % Plot states
% for i = 1:nx
%     figure(i)
%     plot(time,state(:,i))
%     xlabel('time')
%     ylabel(sprintf('state %d',i))
%     grid on
% end
% 
% % Plot controls
% for i = 1:nu
%     figure(nx+i)
%     plot(time,control(:,i))
%     xlabel('time')
%     ylabel(sprintf('control %d',i))
%     grid on
% end





function [nx, nu, data] = getOchData(fname)
fid = fopen(fname,'r');
indata = false;
data = [];
while ~feof(fid)
    line = split(fgetl(fid));
    if any(strcmp(line,'NUM_STATES'))
        nx = str2double(line{4});
    end
    if any(strcmp(line,'NUM_CONTROLS'))
        nu = str2double(line{4});
    end
    if any(strcmp(line,'DATA_START'))
        indata = true;
        n = 1+nx+nu;
        continue;
    end
    if any(strcmp(line,'DATA_STOP'))
        indata = false;
    end
    if indata
        ldata = zeros(1,n);
        j = 1;
        for i = 1:length(line)
            if ~isempty(line{i})
                ldata(j) = str2double(line{i});
                j = j+1;
            end
        end
        data = [data;ldata];
    end
end
fclose(fid);
end