function [xVel, yVel] = wallCollisions(xVel, yVel, xPos, yPos, r, xBound, yBound, xxPos, yyPos, n, xS, yS)
    for k=1:n
        for i=1:length(xBound)-1
            
           %check to see if there is a distance between the points
           distance = sqrt((xBound(i) - xBound(i+1))^2 + (yBound(i) - yBound(i + 1))^2);
           if distance == 0
               continue
           end
           
           %set upper and lower limits on x-axis
           if (xBound(i) < xBound(i + 1))
               xLimStart = xBound(i);
               xLimEnd = xBound(i + 1);
           
           elseif (xBound(i)>xBound(i+1))
               xLimStart=xBound(i+1);
               xLimEnd=xBound(i);
           else
               xLim=xBound(i);
           end
           
           %set upper and lower limits on y-axis
           if yBound(i)<yBound(i+1)
               yLimStart=yBound(i);
               yLimEnd=yBound(i+1);
           elseif yBound(i)>yBound(i+1)
               yLimStart=yBound(i+1);
               yLimEnd=yBound(i);
           else
               yLim=yBound(i);
           end
           
           %find slope and intercepts
           m=(yBound(i+1)-yBound(i))/(xBound(i+1)-xBound(i));
           bBound=yBound(i)-m*xBound(i);
           bPos=yPos(k)-m*xPos(k);
           bPosBefore=yyPos(k)-m*xxPos(k);
           num=0;
           
           %perform collision based on slope, y-int of that line
           %check to see if the point is in the interval of the line
           %have num be 1 (true) if there is a collision to perform it
           if m==0
               if xPos(k)>=xLimStart && xPos(k)<=xLimEnd
                    %wabove
                    if yPos(k)+r>=yLim && yyPos(k)<=yLim
                        yPos(k)=yLim+r;
                        num=1;
                    %below
                    elseif yPos(k)-r<=yLim && yyPos(k)>=yLim
                        yPos(k)=yLim-r;
                        num=1;
                    end
               end
               
           elseif m>0 && m<Inf
               if xPos(k)>=xLimStart && xPos(k)<=xLimEnd 
                    %above
                    if bPos+r>=bBound && bPosBefore<=bBound
                        num=1;
                    %below
                    elseif bPos-r<=bBound && bPosBefore>=bBound
                        num=1;
                    end
               end
               
           elseif m<0 && m>-Inf
               if xPos(k)>=xLimStart && xPos(k)<=xLimEnd 
                    %above
                    if bPos+r>=bBound && bPosBefore<=bBound
                        num=1;
                    %below
                    elseif bPos-r<=bBound && bPosBefore>=bBound
                        num=1;
                    end  
               end
               
            else
               if yPos(k)>=yLimStart && yPos(k)<=yLimEnd
                    %above
                    if xPos(k)+r>=xLim && xxPos(k)<=xLim
                        xPos(k)=xLim+r;
                        num=1;
                    %below
                    elseif xPos(k)-r<=xLim && xxPos(k)>=xLim
                        xPos(k)=xLim-r;
                        num=1;
                    end
               end
           end
           
           %get resulting velocities after boundary collision
           if num==1; 
                alpha=atan2((yBound(i+1)-yBound(i)),(xBound(i+1)-xBound(i)));
                xVelNew=xVel(k)*cos(-alpha)-yVel(k)*sin(-alpha);
                yVelNew=xVel(k)*sin(-alpha)+yVel(k)*cos(-alpha);
                xVel(k)=xVelNew*cos(alpha)+yVelNew*sin(alpha);
                yVel(k)=xVelNew*sin(alpha)-yVelNew*cos(alpha);
           end
        end
    end
end