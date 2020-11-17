% Small intro to spectrum analysis, noise and distortion in MATLAB/Octave
% By Jacques Berg
%
% If you are using Octave, you need to install (needed only the first time) and
% load signal processing package
%   pkg install -forge signal
%   pkg load signal

% First we add noise to the signal
SNR = 10;                   % dB, signal-to-noise ratio (SNR)
Fs = 8000;                  % sampling frequency
tt = 0:(1/Fs):1-1/Fs;       % time vector for one second duration (now real second instead of sample index)
ss = sin(2*pi*250*tt);      % sinusoidal signal with a frequency of 125Hz (fs/f, 8000/250=32 is integer to prevent spectral leakage)
sound(ss,Fs)

% SNR = 10log(Psignal/Pnoise)
% 10^(SNR/10) = (Psignal/Pnoise)
% Pnoise = Psignal/10^(SNR/10)

% Variance of the signal is the power of the signal (if zero mean signal)
var(ss)
nvar = var(ss)/10^(SNR/10);
nn = randn(size(ss))*sqrt(nvar);        % generate noise to match SNR (sqrt(variance)=standard deviation)
var(nn)                                 % check that we have the right variance (energy)
mean(nn)                                % and the the noise has zero mean
ssn = ss + nn;                          % then we are ready to add noise to the signal

sound(ssn,Fs)                           % signal and the noise
sound(nn,Fs)                            % just the noise for a comparision

% let's evaluate the spectra of the signal
figure(1); clf; hold on
%pwelch(ssn,2*length(ssn),[],[],Fs,'dB')
spk(ssn,Fs)

% Now we simulate the distortion caused by clipping
% Clipping is done by first amplifying the signal tentimes (10x) and then limiting to the range [-1,1]
ssn10 = ssn*10;                         % amplified, noisy sinusoidal signal
sound(ssn,Fs)
ssn10( ssn10 > 1 ) = 1;                 % upper limit clipping
ssn10( ssn10 <-1 ) =-1;                 % lower limit clipping
sound(ssn10,Fs)
sound(ssn,Fs)
sound(ssn10,Fs)
std(ssn10)

% then the spectra of the clipped signal
%pwelch(ssn10,1024,0.5,1024,Fs,'dB')
spk(ssn10,Fs)
grid; hold off

% let's study the frequency spectrum of a single pulse
fs = 1e6;                     % sampling frequency 1 MHz
n = 0:2^12;
u = (n >= 0) - (n >= 8);      % signal duration 8*(1/fs)=8us
stem(n(1:16)*(1/fs), u(1:16))
spk(u,fs)

% let's get a new sound sample (from the Windows sounds)
[fname,fpath] = uigetfile('c:\windows\Media\*.wav');
[yy, Fs] = audioread([fpath, fname]);
[fpath, fname]
sound(yy,Fs)

% and then the spectrogram ...
figure(2); specgram(yy(:,1),512,Fs)
