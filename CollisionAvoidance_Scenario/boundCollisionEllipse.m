function [xVel, yVel, xPos, yPos] = boundCollisionEllipse(n, r, xR, yR, xPos, yPos, xVel, yVel, xCent, yCent, ~)

        %find the minimum distance from SAT to swarm boundary
        %create a smaller arbitrary set of points for swarm boundary
        
        arbInt = 1000;
        thetaBound = 0:2*pi/arbInt:2*pi;
        xSwarm = xR*cos(thetaBound);
        ySwarm = yR*sin(thetaBound);

        %check for particles over boundary as a result of this
        for i = 1:n
            %put the SATs in relation to the origin to test this
            %rotate SATs as needed
            
            xx = xPos(i) - xCent;
            yy = yPos(i) - yCent;
            test = (xx^2/(xR - r)^2) + (yy^2/(yR - r)^2);
            
            %see if the SAT is inside
            
            if (test >= 1)
                
                %check to see if which distances are <=r 
                distBound = sqrt((xx - xSwarm).^2 + (yy - ySwarm).^2);
                findIndex = find(distBound == min(distBound));
                ind = findIndex(1);
                
                %use that index to find the angle to change the velocity
                beta = atan2((ySwarm(ind) - yy), (xSwarm(ind) - xx));
                
                %use that angle to change the velocity*
                %*rotate velocities, flip y, rotate back
                alpha = pi/2+beta;
                xVelNew = xVel(i)*cos(-alpha) - yVel(i)*sin(-alpha);
                yVelNew = xVel(i)*sin(-alpha) + yVel(i)*cos(-alpha);
                xVel(i) = xVelNew*cos(alpha) + yVelNew*sin(alpha);
                yVel(i) = xVelNew*sin(alpha) - yVelNew*cos(alpha);
                
                %change the position so they don't leave the swarm
                xv = xSwarm(ind) - xx; 
                yv = ySwarm(ind) - yy; 
                x1 = xSwarm(ind) - r*xv/sqrt(xv^2 + yv^2);
                y1 = ySwarm(ind) - r*yv/sqrt(xv^2 + yv^2);
                xPos(i) = x1 + xCent;
                yPos(i) = y1 + yCent;
            end
        end
end
