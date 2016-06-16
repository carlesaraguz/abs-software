function [x1, y1, x2, y2] = fixSwarm(xp1, yp1, xp2, yp2, r)

    %find distance between centers
    distance = sqrt((xp2 - xp1)^2 + (yp2 - yp1)^2);
    dist = (2*r - distance) / 2;
    
    %find where the new centers will be
    if (xp1 > xp2)
         xx = xp1 - xp2; 
         yy = yp1 - yp2;
         x1 = xp1 + dist*xx/(sqrt(xx^2 + yy^2));
         y1 = yp1 + dist*yy/(sqrt(xx^2 + yy^2));
         x2 = xp2 - dist*xx/(sqrt(xx^2 + yy^2));
         y2 = yp2 - dist*yy/(sqrt(xx^2 + yy^2));
         
    elseif (xp2 > xp1)
         xx = xp2 - xp1; 
         yy = yp2 - xp1;
         x1 = xp1 - dist*xx/(sqrt(xx^2 + yy^2));
         y1 = yp1 - dist*yy/(sqrt(xx^2 + yy^2));
         x2 = xp2 + dist*xx/(sqrt(xx^2 + yy^2));
         y2 = yp2 + dist*yy/(sqrt(xx^2 + yy^2));
         
    else
         if (yp1 > yp2)
            yy = yp1 - yp2;
            x1 = xp1;
            y1 = yp1 + dist;
            x2 = xp2;
            y2 = yp2 - dist;
            
         elseif (yp2 > yp1) 
            yy = yp1 - yp2;
            x1 = xp1;
            y1 = yp1 - dist;
            x2 = xp2;
            y2 = yp2 + dist;
            
         else
            x1 = xp1;
            y1 = yp1 + r;
            x2 = xp2;
            y2 = yp2 - r;
         end
    end 

    if (distance == 2*r)
        x1 = xp1;
        y1 = yp1;
        x2 = xp2;
        y2 = yp2;
    end
end       
