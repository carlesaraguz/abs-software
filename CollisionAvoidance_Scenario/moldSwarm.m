% function [xR,yR,xDeltaR,yDeltaR,phiSwarm,wSwarm]=moldSwarm(xR,yR,xPos,yPos,r,xCent,yCent,xBound,yBound,xDeltaR,yDeltaR,xS,yS,dt,phiSwarm,wSwarm)
%     
%     %check to see if wall points are inside the swarm bounds
%     inSwarm=inpolygon(xBound,yBound,xS,yS);
% 
%     %go through all consecutive points in the boundary arrays 
%     for k=1:length(xBound)-1
%         x1=[xBound(k) xBound(k+1)];
%         y1=[yBound(k) yBound(k+1)];
%         num=1;
%         
%         %find if and where the points intersect
%             if (inSwarm(k) == 1) && (inSwarm(k + 1) == 1)
%                 xA = xBound(k);
%                 yA = yBound(k);
%                 xB = xBound(k + 1);
%                 yB = yBound(k + 1);
%             elseif (inSwarm(k) == 1) && (inSwarm(k + 1) == 0)
%                 [x0 ,y0] = intersections(x1, y1, xS, yS, 1);
%                 xA = xBound(k);
%                 yA = yBound(k);
%                 xB = x0;
%                 yB = y0;
%             elseif (inSwarm(k) == 0) && (inSwarm(k + 1) == 1)
%                 [x0, y0] = intersections(x1, y1, xS, yS, 1);
%                 xA = x0;
%                 yA = y0;
%                 xB = xBound(k + 1);
%                 yB = yBound(k + 1);
%             elseif (inSwarm(k) == 0) && (inSwarm(k + 1) == 0)
%                 [x0,y0]=intersections(x1, y1, xS, yS, 1);
%                 
%                 %check to see if this boundary passes through the swarm
%                 if isempty(x0)==0
%                     xA = x0(1);
%                     yA = y0(1);
%                     xB = x0(2);
%                     yB = y0(2);
%                 else
%                     num = 0;
%                 end
%             end
% 
%         if num == 1
%             numA = 1;
%             %make polygon array for testing, if lines intersect
%             %find the point that has the lower angle to start with
%             thetaA = atan2((yA - yCent),(xA - xCent));
%             thetaB = atan2((yB - yCent),(xB - xCent));
%             if thetaA == thetaB   
%                 if (thetaA < 0) && (thetaB < 0)
%                     if thetaA > thetaB
%                         xx1 = xB;
%                         yy1 = yB;
%                         xx2 = xA;
%                         yy2 = yA;
%                         thetaIteration = (thetaA - thetaB) / 100;
%                         theta1 = thetaB;
%                         theta2 = thetaA;
%                     else 
%                         xx1 = xA;
%                         yy1 = yA;
%                         xx2 = xB;
%                         yy2 = yB;
%                         thetaIteration = (thetaB - thetaA) / 100;
%                         theta1 = thetaA;
%                         theta2 = thetaB;
%                     end
%                 elseif (thetaA > 0) && (thetaB > 0)
%                     if thetaA < thetaB
%                         xx1 = xA;
%                         yy1 = yA;
%                         xx2 = xB;
%                         yy2 = yB;
%                         thetaIteration = (pi*2 + thetaB) - (pi*2 + thetaA) / 100;
%                         theta1 = pi*2 + thetaA;
%                         theta2 = pi*2 + thetaB;
%                         
%                     else
%                         xx1 = xB;
%                         yy1 = yB;
%                         xx2 = xA;
%                         yy2 = yA;
%                         thetaIteration = (pi*2 + thetaA) - (pi*2 + thetaB) / 100;
%                         theta1 = pi*2 + thetaB;
%                         theta2 = pi*2 + thetaA;
%                     end
%                 elseif (thetaA < 0) && (thetaB > 0)
%                     xx1 = xA;
%                     yy1 = yA;
%                     xx2 = xB;
%                     yy2 = yB;
%                     theta1 = pi*2 + thetaA;
%                     theta2 = pi*2 + thetaB;
%                     thetaIteration = (theta2 - theta1) / 100;
%                 elseif (thetaA > 0) && (thetaB < 0)
%                     xx1 = xB;
%                     yy1 = yB;
%                     xx2 = xA;
%                     yy2 = yA;
%                     theta1 = pi*2 + thetaB;
%                     theta2 = pi*2 + thetaA;
%                     thetaIteration = (theta2 - theta1) / 100;
%                 end
%             else
%                 numA = 0;
%             end 
%             
%             %make the polygon array if necessary
%             if numA == 1
%                 thetaShape = theta1:thetaIteration:theta2;
%                 xx = xCent + xR*cos(thetaShape);
%                 yy = yCent + yR*sin(thetaShape);
%                 xShape = [xx1 xx xx2];
%                 yShape = [yy1 yy yy2];
%                 inShape = inpolygon(xPos, yPos, xShape, yShape);
%                 
%                 %see if any SATs are in the new polygon
%                 if sum(inShape) >= 1
%                     len = length(xShape);
%                     dist1 = sqrt((xShape(1) - xShape(2))^2 + (yShape(1) - yShape(2))^2);
%                     dist2 = sqrt((xShape(len) - xShape(len - 1))^2 + (yShape(len) - yShape(len - 1))^2);
%                     
%                     %change to accomodate for different slopes of obstacles
%                     if dist1<2*sum(inShape)*r && dist2<2*sum(inShape)*r 
%                         if (xDeltaR ~= 0) || (yDeltaR ~= 0)
%                             bigR = bigR + abs(xDeltaR)*dt;
%                         else
%                             distShape = sqrt((xShape(len) - xShape(1))^2 + (yShape(len) - yShape(1))^2);
%                             bigR = bigR + distShape;
%                         end
%                     end
%                 end   
%             end   
%         end
%     end
% %end function
% end