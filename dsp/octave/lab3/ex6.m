fs = 1e6; % 1Mhz
n = 0:2^12;
u4  = (n >= 0) - (n >= 4);
u8  = (n >= 0) - (n >= 8);
u16 = (n >= 0) - (n >= 16);

hold on;
spk(u4, fs);
spk(u8, fs);
spk(u16, fs);
hold off;


