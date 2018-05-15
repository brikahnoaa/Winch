%
% matlab script to plot wispr wave data
%
%

clear all;

name = 'test_1khz.wav';

nbps = 16;
fs = 125000;
fc = 1000;
T = 10.0;
time = 0:(1/fs):T;

sig = sin(2*pi*fc*time);

wavwrite(sig, fs, nbps, name);

a = wavread(name);
plot(time, a, '.-');


