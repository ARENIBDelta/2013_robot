function misctester()
%     angles = 0:2*pi/100:2*pi;
%     radius = 60;
%     alphas = [];
%     for i=1:100,
%         an=angles(i);
%         [radius*cos(an), radius*sin(an), 100]
%         command = sprintf('C:\\Users\\ZWM\\Desktop\\arenib\\delta\\maths_c\\a.exe %f %f %f', radius*cos(an), radius*sin(an), 100);
%         [~, result] = dos(command);
%         if strcmp(result, '')
%             alpha = 0;
%             beta = 0;
%             gamma = 0;
%         else
%             comps=sscanf(result, '%f');
%             alphas = [alphas; comps(1)];
%         end    
%     end
%     plot(alphas);

    a = 110;
    c = 130;
    z1 = 250;
    [alphas, betas, gammas, alphayes1] = jeux_calc(a, c, 140, z1-85, 75);
    surf(alphas);
    hold;

    [alphas, betas, gammas, alphayes2] = jeux_calc(a, c, 140, z1, 75);
    surf(alphas);
    return;

    fid = fopen('valids.txt', 'w');

    for c = 30:5:150,
        for a = 20:5:130,
            for z1 = 200:5:350,
                [c, a, z1]
                [alphas, betas, gammas, alphayes1] = jeux_calc(a, c, 140, z1-85, 75);
                %surf(alphas);
                %hold;

                [alphas, betas, gammas, alphayes2] = jeux_calc(a, c, 140, z1, 75);
                %surf(alphas);

                alphayes = alphayes1 & alphayes2;
                if sum(alphayes) > 0
                    fprintf(fid,'%d %d %d => %d\r\n',c, a, z1, sum(alphayes));
                    disp('Oui');
                end    
            end
        end
    end
    
    fclose(fid);
end

function [alphas,betas,gammas, alphayes] = jeux_calc(a, c, d, z, radius)
    alphas = zeros(100, 25);
    alphayes = zeros(1, 25);
    betas = zeros(100, 25);
    gammas = zeros(100, 25);

    angles = 0:2*pi/100:2*pi;
    
    for r=1:25,
        ratio = 1+4*(r/25);
        %params = struct('a', 70, 'b', 70*ratio, 'c', 55, 'd', 65);
        params = struct('a', a, 'b', a*ratio, 'c', c, 'd', d);
        viable_run = 1;
        for i=1:100,
            an=angles(i);
            %[alpha,beta,gamma] = deltacalc(params, radius*cos(an),radius*sin(an),115,0);
            %[alpha,beta,gamma] = deltacalc(params, radius*cos(an), radius*sin(an), z, 1);
            [alpha] = deltacalc(params, radius*cos(an), radius*sin(an), z, 1);

            if (isreal(alpha) && alpha > 0)
                dalpha = alpha;
            else
                dalpha = 0;
                viable_run = 0;
            end
%             if (isreal(beta) && beta > 0)
%                 dbeta = beta;
%             else
%                 dbeta = 0;
%                 viable_run = 0;
%             end
%             if (isreal(gamma) && gamma > 0)
%                 dgamma = gamma;
%             else
%                 dgamma = 0;
%                 viable_run = 0;
%             end
            alphas(i, r) = dalpha/pi*180;
%             betas(i, r) = dbeta/pi*180;
%             gammas(i, r) = dgamma/pi*180;
        end;
        alphayes(1, r) = viable_run;
    end;
end