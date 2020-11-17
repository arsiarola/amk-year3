SSS = 768;
A = 1;
Fs = 8000;
F = Fs / 2^(mod(SSS, 5) + 3);
SNR = 50;
tt = 0:(1/Fs):1/Fs * (2^12-1);
ss = sin(2*pi*F*tt);
ss = ss * A; % set amplitude
sound(ss,Fs);	

var(ss)
nvar = var(ss) / 10^(SNR/10);
nn = randn(size(ss))*sqrt(nvar);
var(nn)
mean(nn)
ssn = ss + nn;

plot(tt(1:100), ss(1:100)); hold on;
plot(tt(1:100), ssn(1:100));

sound(ssn, Fs)
sound(nn, Fs)
