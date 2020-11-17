SS = 68;
Gain = SS + 2;
ssnGain = ssn * Gain;

ssnGain( ssnGain > 1 )  = 1;
ssnGain( ssnGain < -1 ) = -1;

spk(ssnGain,Fs)

grid; hold off
