
% plots mercator projection of the earth. 
function mercator
    img = imread('earthhi','jpg'); 

    image([-180 180],[90,-90],img,'CDataMapping','scaled'); grid on; hold on;

    axis([-180 180 -90 90]); grid on; hold on;
    
    axis image
    
    set(gca,'YDir','normal')


end
