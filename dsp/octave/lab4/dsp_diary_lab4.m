% Small intro to discrete time system analysis in Octave/MATLAB
% By Jacques Berg
%
% Beginning of your Octave session you need to load the signal processing package to the memory
%   pkg load signal

%% Polynomials in Octave
% Octave/MATLAB represents polynomials as row vectors containing coefficients ordered by descending powers
% For example, p = [1 -4 4] represent the quadratic polynomial p(x) = x^2 -4x + 4
% Note that p can also represent z-transformed signal X(z) = 1 - 4z^-1 + 4z^-2 which is also a polynomial


%% A system in frequency domain
% An example system:
% y[n] = (x[n] + x[n-1] + x[n-2])/3
bb = ones(1,3)/3; % X(z) = (1 + z^-1 + z^-2)/3
aa = [1, 0, 0];   % Y(z) = 1 or [1 0 0] Zeros because 2nd order system and there is no y[n-1] and y[n-2] terms

% Zeros and poles of H(z)
zz = roots(bb)    % zeros of the system
pp = roots(aa)    % poles of the system

% Poles are at the origin ... thus a FIR system!
zplane(bb,aa)     % Argand's diagram
poly([0 0])       % you can also generate a polynomial if you know the roots
zplane(zz,pp)     % an another way to call the zplane (by giving the roots)
bb
zz
freqz(bb,aa)          % frequency response, look documentation for freqz for more info
Fs = 213854;          % an arbitrary sampling frequency
freqz(bb,aa,1024,Fs)  % frequency response, now with physical frequency
figure; zplane(zz,pp)

%% Let's move the poles away from the orign ...
% .. but of course inside the unit circle (stability condition)
[x,y]=ginput(1)       % coordinates of the pole
ppn = [x+i*y; x-i*y]; % conjugate pair

%% System parameters matching those zeros and poles
bbn = poly(zz)
aan = poly(ppn)
% now we have feedback (there are nonzero aan coefficients for y[n-1] and y[n-2])
figure; zplane(bbn,aan)
figure; freqz(bbn,aan,1024,Fs)

% Let's scale to the unity gain
gg = max(abs(freqz(bbn,aan,1024,Fs)))
bbns = bbn/gg; % input scaling

% Cheking the result
figure; freqz(bbns,aan,1024,Fs)

% Close all open graphic windows
close all

%% Cartesian and polar coordinates
% z = x + y*i
% z = r*exp(i*omega)
angle = pi/23;      % angle frequency (in rad)
% angle frequency and physical frequency
ff = 10000;         % 10 kHz, physical frequency
angle = 2*pi*ff/Fs; % matching angle (angle frequency)

% Point on the unit circle
zztop = exp(i*angle);
zztop = [zztop; conj(zztop)]
zplane(zztop)

% Then create pole pair at the same frequency:
% but inside the unit circle
pptop = 0.9*zztop;
zplane(zztop,pptop)

% Let's find the coefficients
aatop = poly(pptop);
bbtop = poly(zztop);

% Then we can find out the frequency response
figure; freqz(bbtop,aatop)
freqz(bbtop,aatop,10240,Fs) % now with the right frq scale
