function swarmConfiguration
    
    %clean simulation environment
    clear all;
    close all;
    clc;

%--------------------------------------------------------------------------
%DEFINE INITIAL CONDITIONS
%--------------------------------------------------------------------------

    xR = 65;                  %x radius of swarm boundary
    yR = 30;                  %y radius of swarm boundary
    n = 8;                    %number of SATs
    r = 1;                    %radius of each SAT
    m(1, 1:n) = 1;            %mass of each SAT
    dt = 1/100;               %change in time
    tFinal = 150;             %final time
    w = 0.1;                  %angular velocity
    rV = 1;                   %length of velocity vector for each SAT
    xCent = 0;                %x component of center of swarm
    yCent = 0;                %y component of center of swarm
    rVSwarm = 0;              %length of velocity vector for swarm
    phiSwarm = 0;             %angle of rotation for entire swarm
    wSwarm = 0;               %change in rotation for entire swarm
    xDeltaR = 0;              %change in x radius of bigR (dependant on time)
    yDeltaR = 0;              %change in y radius of bigR (dependant on time)
    video = 0;                %choose to make a video
    
    % make small rectangular obstacle
    % EDIT 19/04 - RED CAGE?
    pos = xR*2;
    neg = xR;
    posS = xR/8;
    negS = -xR/8;
    xBound = [negS negS posS posS negS];
    yBound = [neg pos pos neg neg];


    num = 1;
    while num == 1
        %define random velocity directions for each SAT
        theta = rand(n,1)*2*pi;
        xVel = rV*cos(theta);
        yVel = rV*sin(theta);
    
%       %define a random velocity direction for the whole swarm
%       gamma=randi(6,1);
%       xVelSwarm=rVSwarm*cos(gamma);
%       yVelSwarm=rVSwarm*sin(gamma);

        %define swarm initial velocity
        xVelSwarm = 0;
        yVelSwarm = rVSwarm;
  
        %set initial SAT distribution within swarm bounds
        if xR >= yR
            newR = yR;
        else
            newR = xR;
        end
        thetaPositions = rand(n, 1)*2*pi;
        rPositions = rand(n, 1)*(newR-(1 + r));
        xPos = rPositions.*cos(thetaPositions) + xCent;
        yPos = rPositions.*sin(thetaPositions) + yCent;
        
        %check distances between particles/swarm boundary
        %for simulation purposes
        num = checkDistEllipse(n, r, xR, yR, xPos, yPos, xCent, yCent);
    end
    
    %Setup iteration and waitbar
    nextOne = 1;
    h = waitbar(0, 'Running scenario...');
    
%--------------------------------------------------------------------------
%BEGIN MOVING SATS
%--------------------------------------------------------------------------

%set iteration for video counter
vid = 0;

for z = 0:dt:tFinal
    
    %save last values
    xxPos = xPos;
    yyPos = yPos;
    
    %move SATs
    xPos = xPos + xVel*dt;
    yPos = yPos + yVel*dt;
    
    %use angular velocity to change direction of velocity vectors
    beta = w*dt;
    xx = xVel*cos(beta) - yVel*sin(beta);
    yy = xVel*sin(beta) + yVel*cos(beta);
    xVel = xx;
    yVel = yy;

    %move swarm as a whole
    xCent = xCent + xVelSwarm*dt;
    yCent = yCent + yVelSwarm*dt;
    
    %changing the size of bigR
    xR = xR + xDeltaR*dt;
    yR = yR + yDeltaR*dt;
    
    %changing the rotation of the swarm
    phiSwarm = phiSwarm + wSwarm*dt;
    
    %make initial swarm boundary
    theta1 = 0:0.01:2*pi; 
    xS = xCent + (xR*cos(theta1)*cos(phiSwarm) - yR*sin(theta1)*sin(phiSwarm));
    yS = yCent + (yR*sin(theta1)*cos(phiSwarm) + xR*cos(theta1)*sin(phiSwarm));
    
    
    %checks for collisions
    %check distances between centers to detect particle collisions
    for k = 1:n-1                   
        for k1 = k + 1:n            
            distance = sqrt((xPos(k1) - xPos(k))^2 + (yPos(k1) - yPos(k))^2);
            
            %SET DISTANCE TO DANGER BOUND [TO FIX]
            if (distance <= 2*r)
                
                %space out the SATs that overlap
                display('COLLISION RISK');
                [xPos(k), yPos(k), xPos(k1), yPos(k1)] = fixSwarm(xPos(k), yPos(k), xPos(k1), yPos(k1), r);
                
                %change resulting velocities after SAT-SAT collision
                [xVel(k), yVel(k), xVel(k1), yVel(k1)] = particleCollision(xVel(k), yVel(k), m(k), xVel(k1), yVel(k1), m(k1));
            end
        end
    end 
  
    %check for SATs crossing swarm boundary & and change (x,y),velocities
    [xVel, yVel, xPos, yPos] = boundCollisionEllipse(n, r, xR, yR, xPos, yPos, xVel, yVel, xCent, yCent, phiSwarm);
    
    %check for collisions between SATs and wall/obstacle boundaries
    [xVel, yVel] = wallCollisions(xVel, yVel, xPos, yPos, r, xBound, yBound, xxPos, yyPos, n, xS, yS);
    
    %IN PROGRESS
    %check for collisions between SATs and corners of wall/obstacles
    %[xVel,yVel]=cornerCollisions(xxPos,yyPos,xPos,yPos,xVel,yVel,xBound,yBound,xS,yS,r);    
    %check for stray SATs, wall/obstacle boundaries that hold SATs back
    %[xR,yR,xDeltaR,yDeltaR,phiSwarm,wSwarm]=moldSwarm(xR,yR,xPos,yPos,r,xCent,yCent,xBound,yBound,xDeltaR,yDeltaR,xS,yS,dt,phiSwarm,wSwarm);
    
    %when to show next frame    
    nextOne = nextOne + 1;
    cam = mod(nextOne, 100);

    %draw particles & boundary in one frame
    if (cam == 0)    
        vid = makeSwarmEllipse(n, xR, yR, xPos, yPos, r, xVel, yVel, rVSwarm, xCent, yCent, xBound, yBound, xS, yS, video, vid);
    end  
    
    %update waitbar
    waitbar(z/tFinal, h)  
end
end