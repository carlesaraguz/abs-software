function [x1, y1, x2, y2] = particleCollision(xV1, yV1, m1, xV2, yV2, m2)
    
    %change SAT speed after particle collision
    x1 = (xV1*(m1 - m2) + 2*m2*xV2) / (m1 + m2);
    y1 = (yV1*(m1 - m2) + 2*m2*yV2) / (m1 + m2);
    x2 = (xV2*(m1 - m2) + 2*m2*xV1) / (m1 + m2);
    y2 = (yV2*(m1 - m2) + 2*m2*yV1) / (m1 + m2);
end