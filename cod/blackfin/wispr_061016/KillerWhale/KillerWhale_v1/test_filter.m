%
%  test filters for WISPR Killer Whale click detection algorithm
%

clear all;

verbose_level = 3;

% wave file used for testing
fp = fopen('dtx_test_signals.wav');
hdr = fread(fp, 44);  % read the header

% system parameters to mimic wispr
fs = 93750;
duration = 10.00;
blksize = 4096;   
winsize = 512;

nsamps_per_buf = fix(duration * fs);
nblks = fix(nsamps_per_buf / blksize);
nsamps_per_buf = fix(nblks * blksize);  % actual nsamps per buffer
duration = nsamps_per_buf / fs; % actual buffer duration
winsize = fix(nsamps_per_buf / fix(nsamps_per_buf / winsize));
nwins_per_buf = (nsamps_per_buf / winsize);

num_bufs_per_detection = 6; % number of adc bufs that define the detection duration
total_nwins = nwins_per_buf * num_bufs_per_detection;

filename = 'filter_test_40_43khz.txt';

% detection filters
%Wp = [40000 43000]/(fs/2); 
%Ws = [38000 46000]/(fs/2);
%Rp = 6; Rs = 20;
%[N1,Wn1] = buttord(Wp,Ws,Rp,Rs)
%[b1,a1] = butter(N1,Wn1);

%Wp = [5000 8000]/(fs/2); 
%Ws = [3000 10000]/(fs/2);
%Rp = 6; Rs = 20;
%[N2,Wn2] = buttord(Wp,Ws,Rp,Rs)
%[b2,a2] = butter(N2,Wn2);

N = 2;
[b1,a1]=butter(N, [40000 43000]/(fs/2));
[b2,a2]=butter(N, [5000 8000]/(fs/2));

[h1, w1] = freqz(b1,a1,128,fs);
[h2, w2] = freqz(b2,a2,128,fs);

figure(2);
plot(w1, abs(h1), w2, abs(h2));
title('Butterworth Bandpass Filter')

% print c version, cut/paste into code 
fprintf('%s\n%s\n%s\n%s\n%s\n', ...
sprintf('#define IIR_NSTAGES %d',N), ...
['float iir_b_high[2*IIR_NSTAGES+1] = {', sprintf('%f, ', b1), '};'], ...
['float iir_a_high[2*IIR_NSTAGES] = {', sprintf('%f, ', a1(2:end)), '};'], ...
['float iir_b_low[2*IIR_NSTAGES+1] = {', sprintf('%f, ', b2), '};'], ...
['float iir_a_low[2*IIR_NSTAGES] = {', sprintf('%f, ', a2(2:end)), '};'])

buf1 = zeros(nwins_per_buf*winsize, 1);
buf2 = zeros(nwins_per_buf*winsize, 1);
buf3 = zeros(nwins_per_buf*winsize, 1);

fp1 = fopen('filter_input.dat', 'r');
fp2 = fopen('filter_output1.dat', 'r');
fp3 = fopen('filter_output2.dat', 'r');

%test = load(filename);
%test = test / 32767;
%test1 = test(:,1);
%test2 = test(:,2)
%test3 = test(:,3);
%clear test

for k = 1:num_bufs_per_detection

    test1 = fread(fp1, winsize*nwins_per_buf, 'int16');
    test1 = test1 / 32767;
    test2 = fread(fp2, winsize*nwins_per_buf, 'int16');
    test2 = test2 / 32767;
    test3 = fread(fp3, winsize*nwins_per_buf, 'int16');
    test3 = test3 / 32767;

    for n = 1:nwins_per_buf

        % read a buffer of data, justthe adc
        buffer = fread(fp, winsize, 'int16');

        % scale numbers to unity
        buffer = buffer / 32767;

        if(length(buffer) < winsize)
            break;
        end

        m = (n-1)*winsize + (1:winsize);
        buf1(m) = buffer;
        buf2(m) = filter(b1, a1, buffer);
        buf3(m) = filter(b2, a2, buffer);

        buf2_max(n) = max(buf2(m));
        buf3_max(n) = max(buf3(m));

        test2_max(n) = max(test2(m));
        test3_max(n) = max(test3(m));

    end  % loop over subwindows

    test2_mean(k) = mean(test2_max);
    test3_mean(k) = mean(test3_max);
    fprintf('mean_high=%f, mean_low=%f\n', test2_mean(k), test3_mean(k));
    
    buf2_mean(k) = mean(buf2_max);
    buf3_mean(k) = mean(buf3_max);
    fprintf('mean_high=%f, mean_low=%f\n', buf2_mean(k), buf3_mean(k));

    figure(1);
    subplot(3,1,1);
    plot((1:length(buf1))/fs, buf1, (1:length(test1))/fs, test1);
    subplot(3,1,2);
    plot((1:length(buf2))/fs, buf2, (1:length(test2))/fs, test2);
    subplot(3,1,3);
    plot((1:length(buf3))/fs, buf3, (1:length(test3))/fs, test3);

    pause;
    
end

fprintf('mean_high=%f, mean_low=%f\n', mean(test2_mean), mean(test3_mean));
fprintf('mean_high=%f, mean_low=%f\n', mean(buf2_mean), mean(buf3_mean));

fclose(fp);
fclose(fp1);
fclose(fp2);
fclose(fp3);

