params = struct('a', 70, 'b', 225, 'c', 50, 'd', 50);

r = 5;
d = r;
l = 10;
xs=[];
ys=[];
tboule1 = linspace(-pi, pi/2, 50);
tverge = linspace(0, pi, 75);
tboule2 = linspace(pi/2, 2*pi, 50);
%boule 1
xs = [xs r/2+r/2*cos(tboule1)];
ys = [ys r/2*sin(tboule1)];
%verge
xs = [xs r/2*cos(tverge)];
ys = [ys r/2+l*sin(tverge)];
%boule 2
xs = [xs -r/2+r/2*cos(tboule2)];
ys = [ys r/2*sin(tboule2)];
%plot
xs = (xs * 7) - 0;
ys = (ys * 7) - 35;
plot(xs, ys, 'o');
axis square
axis equal

csvwrite('xs.txt',(xs));
csvwrite('ys.txt',(ys));

alphas = zeros(1,length(xs));
betas = zeros(1,length(xs));;
gammas = zeros(1,length(xs));;
z = 230;
for t=1:length(xs)
   [alpha,beta,gamma] = deltacalc(params, xs(t), ys(t), z, 0);
   alphas(t) = alpha/pi*180;
   betas(t) = beta/pi*180;
   gammas(t) = gamma/pi*180;
end

csvwrite('alphas.txt',(alphas));
csvwrite('betas.txt',(betas));
csvwrite('gammas.txt',(gammas));