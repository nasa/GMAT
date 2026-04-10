function Plot_OCH(mData)

mControl = mData(:, end-2:end);
for i = 1:length(mControl)
    mControl_Unit(i,:) = mControl(i,:)./norm(mControl(i,:));
    mMag(i) = norm(mControl(i,:));
end


h = figure;
set(h, 'color', 'w');
subplot(2,1,1);
plot(mData(:,1), mMag, 'LineWidth',1.5);
ylabel('Magnitude', 'interpreter', 'latex', 'fontsize', 12);

grid on;
subplot(2,1,2);
plot(mData(:,1), mControl_Unit(:,1), 'LineWidth',1.5);
hold on;
plot(mData(:,1), mControl_Unit(:,2), 'LineWidth',1.5);
plot(mData(:,1), mControl_Unit(:,3), 'LineWidth',1.5);
l = legend('$\hat{u}_1$', '$\hat{u}_2$', '$\hat{u}_3$');
set(l, 'interpreter', 'latex', 'location', 'northwest', 'fontsize', 12);
ylabel('Direction', 'interpreter', 'latex', 'fontsize', 12);

grid on;