function [alpha,beta,gamma] = deltacalc(X,Y,Z,inverted)
%function deltacalc()
    c = 130;
    a = 70;
    b = 150;
    C1 = [c*sqrt(3)/2*cos(-150/180*pi); c*sqrt(3)/2*sin(-150/180*pi); 0];
    C2 = [c*sqrt(3)/2*cos(90/180*pi); c*sqrt(3)/2*sin(90/180*pi); 0];
    C3 = [c*sqrt(3)/2*cos(-30/180*pi); c*sqrt(3)/2*sin(-30/180*pi); 0];
    
    orthoC1 = [sin(-150/180*pi); -cos(-150/180*pi); 0];
    orthoC2 = [sin(90/180*pi); -cos(90/180*pi); 0];
    orthoC3 = [sin(-30/180*pi); -cos(-30/180*pi); 0];
    
%     X = 0;
%     Y = 0;
%     Z = 100;
    
    d = 145;
    D1 = [X+d*sqrt(3)/2*cos(-150/180*pi); Y+d*sqrt(3)/2*sin(-150/180*pi); Z];
    D2 = [X+d*sqrt(3)/2*cos(90/180*pi); Y+d*sqrt(3)/2*sin(90/180*pi); Z];
    D3 = [X+d*sqrt(3)/2*cos(-30/180*pi); Y+d*sqrt(3)/2*sin(-30/180*pi); Z];

    %alphav
    Dp1 = line_projection([0;0;Z], C1 + [0;0;Z], D1);
    d0 = norm(Dp1 - C1);
    bp = sqrt(b^2-norm(D1-Dp1)^2);

    Dp1u = (C1 - Dp1)/norm(C1 - Dp1);
    Dp1ut = cross(Dp1u, orthoC1);
    if (inverted)
        Dp1ut = - Dp1ut;
    end;
    cc_a = (bp^2 - a^2 + norm(Dp1 - C1)^2) / (2*norm(Dp1 - C1));
    cc_h = sqrt(bp^2 - cc_a^2);
    P1 = Dp1 + cc_a * Dp1u - cc_h * Dp1ut;
    
    %alpha = acos((norm(C1)^2 + norm(C1-P1)^2 - norm(P1)^2) / (2*norm(C1)*norm(C1-P1)))
    %alpha = pi - atan(P1(3)/norm([P1(1); P1(2); 0]-C1))
    if (inverted)
        alpha = asin(P1(3) / a);
    else
        alpha = pi - asin(P1(3) / a);
    end;
    
    %beta
    Dp2 = line_projection([0;0;Z], C2 + [0;0;Z], D2);
    d0 = norm(Dp2 - C2);
    bp = sqrt(b^2-norm(D2-Dp2)^2);
    
    Dp2u = (C2 - Dp2)/norm(C2 - Dp2);
    Dp2ut = -cross(Dp2u, orthoC2);
    if (inverted)
        Dp2ut = - Dp2ut;
    end
    cc_a = (bp^2 - a^2 + norm(Dp2 - C2)^2) / (2*norm(Dp2 - C2));
    cc_h = sqrt(bp^2 - cc_a^2);
    P2 = Dp2 + cc_a * Dp2u - cc_h * Dp2ut;
    
    %beta = acos((norm(C2)^2 + norm(C2-P2)^2 - norm(P2)^2) / (2*norm(C2)*norm(C2-P2)))
    %beta = pi - atan(P2(3)/norm([P2(1); P2(2); 0]-C2))
    if (inverted)
        beta = asin(P2(3) / a);
    else
        beta = pi - asin(P2(3) / a);
    end;
    
    %gamma
    Dp3 = line_projection([0;0;Z], C3 + [0;0;Z], D3);
    d0 = norm(Dp3 - C3);
    bp = sqrt(b^2-norm(D3-Dp3)^2);
    
    Dp3u = (C3 - Dp3)/norm(C3 - Dp3);
    Dp3ut = -cross(Dp3u, orthoC3);
    if (inverted)
        Dp3ut = - Dp3ut;
    end
    cc_a = (bp^2 - a^2 + norm(Dp3 - C3)^2) / (2*norm(Dp3 - C3));
    cc_h = sqrt(bp^2 - cc_a^2);
    P3 = Dp3 + cc_a * Dp3u - cc_h * Dp3ut;
    
    %gamma = acos((norm(C3)^2 + norm(C3-P3)^2 - norm(P3)^2) / (2*norm(C3)*norm(C3-P3)))
    %gamma = pi - atan(P3(3)/norm([P3(1); P3(2); 0]-C3))
    if (inverted)
        gamma = asin(P3(3) / a);
    else
        gamma = pi - asin(P3(3) / a);
    end;
    
    %alpha/pi*180, beta/pi*180, gamma/pi*180
    
%     Hp1 = C1 + [cos(pi-alpha)*a * cos(-150/180*pi); cos(pi-alpha)*a * sin(-150/180*pi); a*sin(pi-alpha)];
%     norm(Hp1-D1)
%     
%     Hp2 = C2 + [cos(pi-beta)*a * cos(90/180*pi); cos(pi-beta)*a * sin(90/180*pi); a*sin(pi-beta)];
%     norm(Hp2-D2)
%     
%     Hp3 = C3 + [cos(pi-gamma)*a * cos(-30/180*pi); cos(pi-gamma)*a * sin(-30/180*pi); a*sin(pi-gamma)];
%     norm(Hp3-D3)
end

function res = line_projection(A, B, P)
    res = A + (dot(B-A, P-A) / (norm(B-A)^2)) * (B-A);
end
