%
% plot WISPR spectrogram saved as a pgm image
%
clear all

[file, dpath, filterindex] = uigetfile('./*.pgm', 'Pick a PGM Spectrogram file');
name = fullfile(dpath,file);

hdr = imfinfo(name)

fp = fopen(name);

% read header
str1 = fgets(fp);  % 
str2 = fgets(fp);  % # WISPR Spectrogram: df=366.211 Hz, dt=0.005464 sec
str3 = fgets(fp);
str4 = fgets(fp);

% parse comment line
a = sscanf(str2, '# WISPR Spectrogram: df=%f Hz, dt=%f sec');
dfreq = a(1);
dtime = a(2);

time = (1:hdr.Height)*dtime;
freq = (1:hdr.Width)*dfreq / 1000;

S = fread(fp, [hdr.Width  hdr.Height], 'uint16');

fclose(fp);

pcolor(time, freq, 10*log10(S));
shading flat;
xlabel('Time (Sec)');
ylabel('Frequency (kHz)');
title('Spectrogram');
