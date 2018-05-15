%
% matlab script to plot wispr flac data
%
% Uses matlab flac toolbox found at:
% http://www.mathworks.com/matlabcentral/fileexchange/10118-vorbis-flac-audio-encoding-decoding
%

clear all;

[file, dpath, filterindex] = uigetfile('./*.flac', 'Pick a flac file');
name = fullfile(dpath,file);

% read flac file
[sig, fs, bps, info] = flac_read(name);
fs = info.SampleRate;
nbps = info.BitsPerSample;

vref = 5.0;
scale = 2 * vref / (2^nbps); % scaling

% time vector
time = (1:length(sig))/fs;
time = time*1e6; % usecs

nsamps = length(sig);

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



bufsize = 1024;
nbufs = nsamps / bufsize;

for n = 1:nbufs

    in = input('Hit Enter to continue: ')
    if(in == 0) break; end

    m = (1:bufsize) + n*bufsize;
    x = scale * double(sig(m));
    
    % plot time series
    figure(1); clf;
    plot(time(m), x, '.-');
    axis([time(m(1)) time(m(end)) -vref vref]);
    ylabel('Volts');
    xlabel('Time (usec)');
    str = sprintf('WISPR Time Series, Vpp = %f Volts', max(sig(m)) - min(sig(m)));
    title(str);
    grid on;

    % Spectrogram
    figure(2); clf;
    h = spectrum.welch;                  % Create a Welch spectral estimator.
    hpsd = psd(h,x,'ConfLevel',0.9);    % PSD with confidence level
    plot(hpsd)
    str = sprintf('WISPR Power Spectral Density Estimate');
    title(str);

end

