%
% matlab script to plot wispr raw data
%

clear all;

[file, dpath, filterindex] = uigetfile('./*.dat', 'Pick a data file');
name = fullfile(dpath,file);

% read header file
% should look like this:
%  %WISPR DATA FILE HEADER
%  file name: wispr_250921_070438
%  compression level: 0
%  start time sec: 1758438278
%  start time usec: 200000
%  samples per second: 125000
%  total samples: 0
%  bits per sample: 16
%  channels: 1
%  gain: 0
%  shift: 0
hdr_file = [name(1:end-3), 'txt'];
fp = fopen(hdr_file, 'r');
str = fgets(fp);  % ignore comment line
str = fgets(fp);  % ignore filename
str = fgets(fp);  % ignore compression level
str = fgets(fp);  hdr.sec = sscanf(str, 'start time sec: %d');
str = fgets(fp);  hdr.usec = sscanf(str, 'start time usec: %d');
str = fgets(fp);  hdr.fs = sscanf(str, 'samples per second: %d');
str = fgets(fp);  hdr.nsamps = sscanf(str, 'total samples: %d');
str = fgets(fp);  hdr.nsize = sscanf(str, 'bits per sample: %d');
str = fgets(fp);  hdr.nchans = sscanf(str, 'channels: %d');
str = fgets(fp);  hdr.gain = sscanf(str, 'gain: %d');
str = fgets(fp);  hdr.scale = sscanf(str, 'shift: %d');
fclose(fp);

t1=719529;   % this is the date number for Jan 1 1970
[year,month,day,hour,minute,second]  = datevec(t1+(hdr.sec/(24*3600)));
second = second + hdr.usec/1000000;
fprintf('%d/%02d/%02d %02d:%02d:%f\n', year,month,day,hour,minute,second);

fprintf('sec = %d\n', hdr.sec);
fprintf('usec = %d\n', hdr.usec);
fprintf('nsamps = %d\n', hdr.nsamps);
fprintf('nchans = %d\n', hdr.nchans);
fprintf('nsize = %d\n', hdr.nsize);
fprintf('fs = %d\n', hdr.fs);
fprintf('gain = %d\n', hdr.gain);
fprintf('scale = %d\n', hdr.scale);

vref = 5.0;  

% open data file 
fmt = 'ieee-le';
fprintf('opening %s\n', file);
fp = fopen(name, 'r', fmt);

if(hdr.nsize == 32)
    data = fread(fp, (hdr.nchans)*(hdr.nsamps), 'int32');
elseif(hdr.nsize == 16)
    data = fread(fp, (hdr.nchans)*(hdr.nsamps), 'int16');
end

if(isempty(data)) break; end

% data scaling, convert to volts
if(hdr.nsize == 16)
    q = vref*(2^(hdr.scale))/(2^23);
else
    q = 1.0;
end

sig = q*double(data);

% time vector
fs = hdr.fs;
time = (1:length(sig))/fs;
time = time*1e6; % usecs

% plot time series
figure(1); clf;
%plot(data, '.-');
plot(time, sig, '.-');
axis([time(1) time(end) -vref vref]);
ylabel('Volts');
xlabel('Time (usec)');
str = sprintf('Time Series Vpp = %f Volts',max(sig) - min(sig));
title(str);
grid on;

% Spectrogram
figure(2); clf;
nfft = 1024;
noverlap = 512;
win = hanning(nfft);
spectrogram(sig, win, noverlap, nfft, fs, 'yaxis');


fclose(fp);


