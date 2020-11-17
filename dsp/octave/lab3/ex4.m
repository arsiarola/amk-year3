SSS = 768;
Fs = SSS * 10;
tt = 0:1/Fs:1-1/Fs;
F = chirp(tt, 100, 1, 200)
A = exp(-tt/0.2);
S = A .* F .+ (randn(size(tt)) .* 0.02);

specgram(S, 128, Fs)