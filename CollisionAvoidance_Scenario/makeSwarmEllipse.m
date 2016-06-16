function vid = makeSwarmEllipse(n, xR, yR, xPos, yPos, r, xVel, yVel, rVSwarm, xCent, yCent, xBound, yBound, x, y, video, vid)

    %make titles/labels/setup
    clf;
    hold on;
    box on;
    
    %plot adaptative axis so SATs are always in sight
    axis([(-xR - 5 + xCent) (xR + 5 + xCent) (-yR - 5 + yCent) (yR + 5 + yCent)]);
    axis square
    axis equal
    
    %create zero matrix for position monitoring
    Observer = zeros(n, 2);
    
    %axis normal
    title('SAT Swarm Scenario');
    
    %create boundary(circle)
    p = plot(x, y);
    set(p, 'Color', 'b', 'LineWidth', 2);

    for k = 1:n        
        
        %create particles
        theta = 0:0.1:2*pi;
        x = xPos(k) + r*cos(theta);
        y = yPos(k) + r*sin(theta);
        x2 = xPos(k) + 4*r*cos(theta);
        y2 = yPos(k) + 4*r*sin(theta);
        %disp(x);
        Observer(k,1) = mean(x);
        Observer(k,2) = mean(y);
        
        %DISPLAY POSITION MATRIX
        string = sprintf([repmat('       %.2f\t  ', 1, size(Observer, 2)) '\n'], Observer');
        xlabel({'Swarm positions (x,y)', string});
 
        patch(x2, y2, [0.6,0.8,1], 'EdgeColor', 'none', 'FaceAlpha', 0.2);
        patch(x, y, [0.6,0.6,1]);
                        
        %create the main velocity line
        newX = xPos(k) + xVel(k);
        newY = yPos(k) + yVel(k);
        x1 = [xPos(k) newX];
        y1 = [yPos(k) newY];
        p1 = plot(x1, y1);
        set(p1, 'Color', 'b', 'LineWidth', 1);
                
        %make arrow for velocity line
        theta1 = atan2(yVel(k), xVel(k));
        xArrow = xVel(k)*cos(-theta1) - yVel(k)*sin(-theta1); 
        yArrow = xVel(k)*sin(-theta1) + yVel(k)*cos(-theta1);
        
        %field of view area
        xArr = [(xArrow + 25) (xArrow + 25) xArrow];
        yArr = [(yArrow + 15) (yArrow - 15) yArrow];
        xArr1 = xArr*cos(theta1) - yArr*sin(theta1);
        yArr1 = xArr*sin(theta1) + yArr*cos(theta1);
        xArr2 = xArr1 + xPos(k);
        yArr2 = yArr1 + yPos(k);


        patch(xArr2, yArr2, [1,1,0.6], 'EdgeColor', 'yellow', 'FaceAlpha', .5, 'LineStyle', ':');
        
        display(xArr1);
               
    end
    
    %saves picture to format into a video for later use if specified
    if (video == 1) 
        saveas(gcf, [pwd '\swarmControls' num2str(vid)], 'png')
        vid = vid + 1;
    end
end