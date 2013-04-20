
alphas = [];
betas = [];
gammas = [];

for t=[0:2*pi/100:2*pi],
    [alpha,beta,gamma] = deltacalc(30*cos(t), 30*sin(t), 100, 1);
    alphas = [alphas; alpha/pi*180];
    betas = [betas; beta/pi*180];
    gammas = [gammas; gamma/pi*180];
end

% for t=[0:100],
%     [alpha,beta,gamma] = deltacalc(10, -15, 30+t);
%     alphas = [alphas; alpha/pi*180];
%     betas = [betas; beta/pi*180];
%     gammas = [gammas; gamma/pi*180];
% end

hold off;
plot(alphas, 'r');
hold;
plot(betas, 'b');
plot(gammas, 'g');