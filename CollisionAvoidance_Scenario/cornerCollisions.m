% function [xVel, yVel] = cornerCollisions(xxPos, yyPos, xPos, yPos, xVel, yVel, xBound, yBound, xS, yS, r)
% 
%     %see if the corner point is within the swarm boundary
%     inSwarm = inpolygon(xBound, yBound, xS, yS);
%         for k = 1:length(xBound)
%             if inSwarm(k) == 1
%                 
%                 %see if distance between any SAT and that corner is
%                 %greater than or equal to r
%                 d = sqrt((xBound(k) - xPos).^2 + (yBound(k) - yPos).^2);
%                 dBefore = sqrt((xBound(k) - xxPos).^2 + (yBound(k) - yyPos).^2);
%                 
%                 if isempty(find(d <= r,1)) == 0
%                     for i = 1:length(xPos)
%                         if d(i) <= r
%                             
%                             %fix position of the SAT
%                             xv = xPos(i) - xBound(k);
%                             yv = yPos(i) - yBound(k);
%                             xPos(i) = xBound(k) + r*1.01*xv/sqrt(xv^2 + yv^2);
%                             yPos(i) = yBound(k) + r*1.01*yv/sqrt(xv^2 + yv^2);
%                             
%                             %find the angle of the corner to the SAT, get
%                             %the perpendicular angle, then flip v, flip y,
%                             %then flip v back
%                             alpha = pi/2 + (atan2((yBound(k) - yPos(i)),(xBound(k) - xPos(i))));
%                             xVelNew = xVel(i)*cos(-alpha) - yVel(i)*sin(-alpha);
%                             yVelNew = xVel(i)*sin(-alpha) + yVel(i)*cos(-alpha);
%                             xVel(i) = xVelNew*cos(alpha) + yVelNew*sin(alpha);
%                             yVel(i) = xVelNew*sin(alpha) - yVelNew*cos(alpha);
%                         end
%                     end
%                 end
%             end
%         end
% end