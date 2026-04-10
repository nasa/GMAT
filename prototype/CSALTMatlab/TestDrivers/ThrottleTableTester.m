clear all
close all
g0 = 9.805;

load ThrottleTable.mat

power = ThrottleTable(:,8);
efficiency = ThrottleTable(:,7);
Isp = ThrottleTable(:,6);
thrust = ThrottleTable(:,5);
voltage = ThrottleTable(:,4);
current = ThrottleTable(:,3);
mdot = ThrottleTable(:,2);

for i = 1:length(power)
	power_efficiency(i,1) = current(i)*voltage(i)/(power(i)*1000);
	thruster_efficiency(i,1) = (thrust(i)/1000);
end

plot(power_efficiency)
hold on
plot(efficiency)
stop

% plot(efficiency)
% hold on
% plot(thruster_efficiency)
% stop


unique_mdot = unique([min(mdot)-(max(mdot)-min(mdot))*.1;mdot;max(mdot)*1.1]);
unique_power = unique([min(power)-(max(power)-min(power))*.1;power;max(power)*1.1]);
unique_voltage = unique([min(voltage)-(max(voltage)-min(voltage))*.1;voltage;max(voltage)*1.1]);

[voltgrid,mdotgrid] = meshgrid(unique_voltage,unique_mdot);

% plot(mdotgrid,voltgrid,'x')
% xlabel('mdot')
% ylabel('voltage')
% hold on
% plot(mdot,voltage,'o','markersize',10)
% stop

thrustgrid = zeros(size(mdotgrid));
Ispgrid = zeros(size(mdotgrid));
powergrid = zeros(size(mdotgrid));
currentgrid = zeros(size(mdotgrid));
effgrid = zeros(size(mdotgrid));
delta_voltage_grid = zeros(size(mdotgrid));
delta_mdot_grid = zeros(size(mdotgrid));

for m = 1:length(unique_mdot)
    % for p = 1:length(unique_power)
    for p = 1:length(unique_voltage)
		
		if m > 1
			delta_mdot_grid(m-1,p) = mdotgrid(m,p) - mdotgrid(m-1,p);
		end
		
		if p > 1
			delta_voltage_grid(m,p-1) = voltgrid(m,p) - voltgrid(m,p-1);
		end
		
        local_mdot = mdotgrid(m,p);
        local_voltage = voltgrid(m,p);
        
        idx  = find(local_mdot >= mdot & local_voltage >= voltage);
		
		if isempty(idx)
			
	        thrustgrid(m,p) = 0;
	        Ispgrid(m,p) = 0;
			powergrid(m,p) = 0;
			currentgrid(m,p) = 0;
			efficiencygrid(m,p) = 0;
			power_efficiency_grid(m,p) = 0;
			actual_voltage_grid(m,p) =  0;
			actual_mdot_grid(m,p) = 0;
		else
        
	        dist = 100;
	        for j = idx'
	            loc_dist = sqrt((local_mdot-mdot(j))*(local_mdot-mdot(j)) + (local_voltage-voltage(j))*(local_voltage-voltage(j)));
	            % loc_dist = sqrt((local_mdot-mdot(j))*(local_mdot-mdot(j)) + (local_power-power(j))*(local_power-power(j)));
	            if loc_dist < dist
	                dist = loc_dist;
	                best_j = j;
	            end
	        end   
        
	        thrustgrid(m,p) = thrust(best_j);
	        Ispgrid(m,p) = Isp(best_j);
			powergrid(m,p) = power(best_j);
			currentgrid(m,p) = current(best_j);
			efficiencygrid(m,p) = efficiency(best_j);
			power_efficiency_grid(m,p) = power_efficiency(best_j);
			actual_voltage_grid(m,p) =  voltage(best_j);
			actual_mdot_grid(m,p) = mdot(best_j);
		
		end
    end
end

plot(powergrid,voltgrid,'x')
xlabel('power')
ylabel('voltage')
hold on
plot(power,voltage,'o','markersize',10)
stop

% mesh(mdotgrid,powergrid,thrustgrid)
mesh(mdotgrid,voltgrid,powergrid)

stop


n1 = 100;
n2 = 100;
[c1,c2] = meshgrid(0:1.2/n1:8,-.1:1.2/n1:1.1); % (power,mdot)

thrust_surf = zeros(size(c1));
Isp_surf = zeros(size(c1));
power_surf = zeros(size(c))

for i_c1 = 1:n1
	for i_c2 = 1:n2
				
		power_val = c1(i_c1,i_c2)		
				
		for m = 1:length(unique_mdot)
			
			for v = 1:length(unique_voltage)
				
				
			
			end	
		end
		
	end
end

% plot(mdot,power,'x')
% xlabel('mdot')
% ylabel('power')
% hold on

% plot(mdotgrid,powergrid,'x')
% xlabel('mdot')
% ylabel('power')
% hold on
% plot(mdot,power,'o','markersize',10)

% 
% for i = 1:length(unique_mdot)
%
%     power_j = [];
%     for j = 1:length(power)
%         if mdot(j) == unique_mdot(i)
%             power_j = [power_j,j];
%         end
%     end
%
%     if length(power_j) == 1
%         power(power_j) = 0;
%         power = [power;1];
%         mdot = [mdot;mdot(power_j)];
%         thrust = [thrust;thrust(power_j)];
%         Isp = [Isp;Isp(power_j)];
%     else
%
%         min_power = min(power(power_j));
%         min_power = 0;
%         max_power = max(power(power_j));
%
%         for j = 1:length(power_j)
%            power(power_j(j)) = (power(power_j(j))-min_power)/(max_power-min_power);
%         end
%     end
% end
%
% % next_power = zeros(length(power),1);
% % next_mdot = zeros(length(power),1);
% % next_power(1) = 10;
% % next_power(length(power)) = 10;
% % for i = length(power)-2:-1:1
% %
% %     if mdot(i) == mdot(i+1)
% %         next_power(i+1) = power(i);
% %     else
% %         next_power(i+1) = 10;
% %     end
% %
% % end
%
% mdot_scaled = (mdot - 0)/(max(mdot)-0);
% unique_mdot = unique(mdot_scaled);
%
% for i = 1:length(mdot_scaled)
%
%     j = find(mdot_scaled(i) == unique_mdot);
%     if j == length(unique_mdot)
%         next_mdot(i) = 10;
%     else
%         next_mdot(i) = unique_mdot(j+1);
%     end
%
% end
%
% power_scaled = (power - min(power)) / (max(power) - min(power));
% figure(1)
% plot(mdot_scaled,power,'x')
% xlabel('mdot')
% ylabel('power')
% stop

% n1 = 10000;
% n2 = 10;
% [c1,c2] = meshgrid(0:max(unique_power+1)/n1:max(unique_power+1),0:max(unique_mdot+1)/n2:max(unique_mdot)+1);
% %
% % stop
% thrust_surf = zeros(size(c1));
% Isp_surf = zeros(size(c1));
%
% heaviside_A = zeros(size(c1));
% heaviside_B = zeros(size(c1));
% heaviside_C = zeros(size(c1));
% heaviside_D = zeros(size(c1));
% heaviside_E = zeros(size(c1));
%
%
% heaviside_1 = zeros(size(c1));
% heaviside_2 = zeros(size(c1));
% heaviside_3 = zeros(size(c1));
% heaviside_4 = zeros(size(c1));
% % % power_heavisider= zeros(size(c1));
% %
% default_sharpness = 100;
%
% target = 1e-4;
%
% max_exp = log(1.7976931348623157*10^308);
%
% power_range = 30;
% mdot_range = max(unique_mdot);
%
% for m = 1:length(unique_mdot)
% 	for p = 1:length(unique_power)
%
% 		local_mdot = mdotgrid(m,p);
% 		local_power = powergrid(m,p);
%
% 		if p+1 > length(unique_power)
% 			next_powergrid(m,p) = 100;
% 		else
% 			next_powergrid(m,p) = powergrid(m,p+1);
% 		end
%
% 		width(m,p) = next_powergrid(m,p) - local_power;
%
% 		if m+1 > length(unique_mdot)
% 			next_mdotgrid(m,p) = 100;
% 		else
% 			next_mdotgrid(m,p) = mdotgrid(m+1,p);
% 		end
%
% 		height(m,p) = next_mdotgrid(m,p) - local_mdot;
%
%
% 		needed_sharpness_power(m,p) = -log(1/target-1)/(-width(m,p)*.5);
%
% 		needed_sharpness_mdot(m,p) = -log(1/target-1)/(-height(m,p)*.5);
%
% 	end
% end
%
% max_sharpness_mdot = max(max(needed_sharpness_mdot));
%
% max_input_mdot = mdot_range * max_sharpness_mdot
%
% max_sharpness_power = max(max(needed_sharpness_power));
%
% max_input_power = power_range * max_sharpness_power;
%
% if power_allowable_range > power_range
% 	disp('Power is all good')
% 	power_scaling = 1;
% else
% 	disp('power needs scaling')
% 	power_scaling = power_allowable_range/power_range;
% end
%
% for m = 1:length(unique_mdot)
% 	for p = 1:length(unique_power)
%
% 		local_mdot = mdotgrid(m,p);
% 		local_power = powergrid(m,p);
%
% 		if p+1 > length(unique_power)
% 			next_powergrid(m,p) = 100;
% 		else
% 			next_powergrid(m,p) = powergrid(m,p+1);
% 		end
%
% 		width(m,p) = next_powergrid(m,p) - local_power;
%
% 		if m+1 > length(unique_mdot)
% 			next_mdotgrid(m,p) = 100;
% 		else
% 			next_mdotgrid(m,p) = mdotgrid(m+1,p);
% 		end
%
% 		height(m,p) = next_mdotgrid(m,p) - local_mdot;
%
%
% 		needed_sharpness_power(m,p) = -log(1/target-1)/(-width(m,p)*power_scaling*.5);
%
% 		needed_sharpness_mdot(m,p) = -log(1/target-1)/(-height(m,p)*mdot_scaling*.5);
%
% 	end
% end
%
% max_sharpness_mdot = max(max(needed_sharpness_mdot));
%
% mdot_allowable_range = max_exp / max_sharpness_mdot;
%
% if mdot_allowable_range > mdot_range
% 	disp('Mdot is all good')
% else
% 	disp('Mdot needs scaling')
% end
%
% max_sharpness_power = max(max(needed_sharpness_power));
%
% power_allowable_range = max_exp / max_sharpness_power;
%
% if power_allowable_range > power_range
% 	disp('Power is all good')
% 	power_scaling = 1;
% else
% 	disp('power needs scaling')
% 	power_scaling = power_allowable_range/power_range;
% end
%
% % stop
%
% figure(1)
% subplot(2,1,1)
% plot(unique_power,needed_sharpness_power)
% subplot(2,1,2)
% plot(unique_mdot,needed_sharpness_mdot)
% stop
%
%
% for m = 1:length(unique_mdot)
% 	for p = 1:length(unique_power)
% 		heaviside_B = [zeros(1,n1+1)];
% 		power_plot = zeros(1,n1+1);
% 		for i_c1 = 1:n1+1
% 			for i_c2 = round(n2/2)%1:n2+1
% 				% for i_c2 = 1:n2+1
%
% 				c1val = c1(i_c2,i_c1);
% 				c2val = c2(i_c2,i_c1);
%
% 				local_mdot = mdotgrid(m,p);
% 				local_power = powergrid(m,p);
%
% 			   	next_power = next_powergrid(m,p);
% 				next_mdot = next_mdotgrid(m,p);
%
%
% 				if p == 1 || width(m,p-1) > width(m,p)
% 					left_sharpness = needed_sharpness_power(m,p);
% 				else
% 					left_sharpness = needed_sharpness_power(m,p-1);
% 				end
%
% 				if left_sharpness < default_sharpness
% 					left_sharpness = default_sharpness;
% 				end
%
% 				if p == length(unique_power)
% 					right_sharpness = 10;
% 					left_sharpness = 10000;
% 				elseif p +1 == length(unique_power)
% 					right_sharpness = 10000;
% 				elseif width(m,p) < width(m,p+1)
% 					right_sharpness = needed_sharpness_power(m,p);
% 				else
% 					right_sharpness = needed_sharpness_power(m,p+1);
% 				end
%
% 				if right_sharpness < default_sharpness
% 					right_sharpness = default_sharpness;
% 				end
%
% 				if m == 1 || height(m-1,p) > height(m,p)
% 					bottom_sharpness = needed_sharpness_mdot(m,p);
% 				else
% 					bottom_sharpness = needed_sharpness_mdot(m-1,p);
% 				end
%
% 				if bottom_sharpness < default_sharpness
% 					bottom_sharpness = default_sharpness;
% 				end
%
% 				if m == length(unique_mdot) || height(m+1,p) < height(m,p)
% 					top_sharpness = needed_sharpness_power(m,p);
% 				else
% 					top_sharpness = needed_sharpness_power(m+1,p);
% 				end
%
% 				if top_sharpness < default_sharpness
% 					top_sharpness = default_sharpness;
% 				end
%
%
% 				expfun1 = exp(-bottom_sharpness*(c2val-local_mdot));
% 				expfun2 = exp(-left_sharpness*(c1val-local_power));
% 				%                 expfun3 = exp(-sharpness*(c2val-next_mdot));
% 				%                 expfun4 = exp(-sharpness*(c1val-next_power));
% 				expfun5 = exp(top_sharpness*(c2val-next_mdot));
% 				expfun6 = exp(right_sharpness*(c1val-next_power));
%
% 				power_plot(i_c1) = c1val;
% 				heaviside_B(i_c1) = 1/(1+expfun1)*1/(1+expfun2)*1/(1+expfun5)*1/(1+expfun6);
% 				%                 heaviside_A(i_c2,i_c1) = heaviside_A(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2)*1/(1+expfun3)*1/(1+expfun4);
% 				% heaviside_B(i_c2,i_c1) = heaviside_B(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2)*1/(1+expfun5)*1/(1+expfun6);
% 				%                 heaviside_C(i_c2,i_c1) = heaviside_C(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2) - 1/(1+expfun3)*1/(1+expfun4);
% 				%                 heaviside_D(i_c2,i_c1) = heaviside_D(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2) - 1/(1+expfun3) - 1/(1+expfun4);
% 				%                 heaviside_E(i_c2,i_c1) = heaviside_E(i_c2,i_c1) + 1/(1+expfun1) + 1/(1+expfun2) - 1/(1+expfun3) - 1/(1+expfun4);
% 				%
% 				%                 heaviside_1(i_c2,i_c1) = heaviside_1(i_c2,i_c1) + 1/(1+expfun1);
% 				%                 heaviside_2(i_c2,i_c1) = heaviside_2(i_c2,i_c1) + 1/(1+expfun2);
% 				%                 heaviside_3(i_c2,i_c1) = heaviside_3(i_c2,i_c1) + 1/(1+expfun3);
% 				%                 heaviside_4(i_c2,i_c1) = heaviside_4(i_c2,i_c1) + 1/(1+expfun4);
%
% 				thrust_surf(i_c2,i_c1) = thrust_surf(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2)*1/(1+expfun5)*1/(1+expfun6) * thrustgrid(m,p);
% 				Isp_surf(i_c2,i_c1) = Isp_surf(i_c2,i_c1) + 1/(1+expfun1)*1/(1+expfun2)*1/(1+expfun5)*1/(1+expfun6) * Ispgrid(m,p);
%
% 				%                 if sum(sum(heaviside_B))
% 				%                    local_mdot
% 				%                    local_power
% 				%                    next_power
% 				%                    next_mdot
% 				%                 end
%
% 				%                 dist1 = sqrt(c1val*c1val + c2val*c2val);
% 				%                 dist2 = sqrt(power(i)*power(i) + mdot(i)*mdot(i));
% 				%                 expfun3 = exp(-sharpness*(dist1-dist2));
% 				%                 expfun1 = exp(-sharpness*(c1val-next_power(i)));
% 				%                 expfun2 = exp(-sharpness*(c2val-next_mdot(i)));
% 				%                 negative_heavi = 1/(1+expfun1)*1/(1+expfun2);
% 				%                 if (positive_heavi-negative_heavi) < 0
% 				%                     disp('less than zero!')
% 				%                     keyboard
% 				%                 elseif (positive_heavi-negative_heavi) > 1
% 				%                     disp('greater than 1!')
% 				%                 elseif isnan(negative_heavi)
% 				%                     keyboard
% 				%                 elseif isnan(positive_heavi)
% 				%                     keyboard
% 				%                 end
% 				%
% 				%                 heaviside(i_c1,i_c2) = heaviside(i_c1,i_c2) + positive_heavi - negative_heavi;
% 				%                 neg_heaviside(i_c1,i_c2) = neg_heaviside(i_c1,i_c2) + negative_heavi;
% 				%                 pos_heaviside(i_c1,i_c2) = pos_heaviside(i_c1,i_c2) + positive_heavi;
%
% 				%             expfun = exp(-sharpness*(c1val-mdot_scaled(i)));
% 				%             positive_mdot_heavi = .5/(1+expfun);
% 				%
% 				%             expfun = exp(-sharpness*(c1val-next_mdot(i)));
% 				%             negative_mdot_heavi = .5/(1+expfun);
% 				%             if (positive_mdot_heavi-negative_mdot_heavi) < 0
% 				%                 disp('less than zero!')
% 				%                 keyboard
% 				%             elseif (positive_mdot_heavi-negative_mdot_heavi) > 1
% 				%                 disp('greater than 1!')
% 				%             end
% 				%             mdot_heaviside(i_c1,i_c2) = mdot_heaviside(i_c1,i_c2) + (negative_mdot_heavi);
% 				%             power_heaviside(i_c1,i_c2) = power_heaviside(i_c1,i_c2) + (positive_power_heavi-negative_power_heavi);
% 				%             thrust_surf(i_c1,i_c2) = thrust_surf(i_c1,i_c2) + thrust(i) * (positive_mdot_heavi-negative_mdot_heavi + positive_power_heavi - negative_power_heavi);
% 				%             thrust_surf(i_c1,i_c2) = thrust_surf(i_c1,i_c2) + (positive_mdot_heavi-negative_mdot_heavi + positive_power_heavi - negative_power_heavi);
% 				%             thrust_surf(i_c1,i_c2) = thrust_surf(i_c1,i_c2) + (positive_mdot_heavi-negative_mdot_heavi);
% 			end
%
% 		end
%         figure(2)
% 		hold on
% 		plot(power_plot,heaviside_B)
%
% 	end
% end
%
%
%
% figure(3)
% % axis([6.04 6.06 -.05 1.05])
% % clf
% subplot(2,1,1)
% plot(c2(:,round(n1/2)),thrust_surf(:,round(n1/2)))
% xlabel('mdot')
% subplot(2,1,2)
% plot(c1(round(n2/2),:),thrust_surf(round(n2/2),:))
% xlabel('power')



% figure(2)
% surf(c1,c2,thrust_surf,'linestyle','none')
% ylabel('power')
% xlabel('mdot')
% view([0 0])

% figure(3)
% 
% plot(mdot)
