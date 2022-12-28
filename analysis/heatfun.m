% example adapted from 'Exploring ODEs', page 282 u0 = chebfun('exp(-x^2 * 12)');
pdefun = @(t, x, u) diff(u, 2);
bc.left = @(t, u) u;
bc.right = @(t, u) u;
opts = pdeset('plot', 'off');
[t, u] = pde15s(pdefun, [0 0.005 0.010], u0, bc, opts);

x = linspace(-1.0, 1.0, 500).';
all_outputs = u(x);
output = all_outputs(:, end);
csvwrite('matlab.csv', output);
