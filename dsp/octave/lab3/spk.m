function d = spk(s,Fs)
% Calculates spectrum of the first 1024 samples of the given signal
% Based on ideas from
%  https://www.sjsu.edu/people/burford.furman/docs/me120/FFT_tutorial_NI.pdf
nn = 1:(2^10);                % indexes
xx = s(nn);                   % first 1024 samples
fxx = fft(xx);                % Discrete Fourier Transform (DFT), implemented using Fast Fourier Transformation
Px = fxx .* conj(fxx);        % = abs(fxx).^2 = power spectrum
dBPx = 10*log10(Px/max(Px));  % scaled (to 0 dB) power in dB
% for plotting
df = Fs / length(xx);
inds = 1:(length(xx)/2+1);    % half + 1
ff = (inds-1)*df;             % uniform sampling
% spectrum 
plot(ff,dBPx(inds)); grid
xlabel('f [Hz]'); ylabel('P_x [dB]')
% because one DFT bin is smaller than the whole noise bandwidth
disp('FFT bin effect'); disp(10*log10(length(nn)));
