%
% matlab script to plot wispr wave data
%
%

clear all;

[file, dpath, filterindex] = uigetfile('./*.wav', 'Pick a wave file');
name = fullfile(dpath,file);
 
% read flac file
[sig, wave] = wav_read(name);
fs = wave.SampleRate;
nbps = wave.BitsPerSample;
nsamps = length(sig);

% data scaling to convert to volts
vref = 5.0;
scale = 2 * vref / (2^nbps); % scaling

% time vector
time = (1:length(sig))/fs;
%time = time*1e6; % usecs

% plot time series
figure(1); clf;
plot(time, scale * double(sig), '.-');
axis([time(1) time(end) -vref vref]);
ylabel('Volts');
xlabel('Time (usec)');
str = sprintf('WISPR Time Series, Vpp = %f Volts', max(sig) - min(sig));
title(str);
grid on;

% Spectrogram
figure(2); clf;
[y,f,t,p] = spectrogram(scale * double(sig),256, 128, 256, fs, 'yaxis');
surf(t,f,10*log10(abs(p)),'EdgeColor','none');
%surf(t,f,abs(p),'EdgeColor','none');
axis xy; axis tight; colormap(jet); view(0,90);
xlabel('Time (Sec)');
ylabel('Frequency (Hz)');

return;

h = spectrum.welch;                  % Create a Welch spectral estimator.
hpsd = psd(h,x,'ConfLevel',0.9);    % PSD with confidence level
plot(hpsd)
str = sprintf('WISPR Power Spectral Density Estimate');
title(str);
