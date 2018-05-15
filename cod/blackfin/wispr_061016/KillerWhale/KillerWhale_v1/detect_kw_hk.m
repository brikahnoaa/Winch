%function ret = detect_kw(buf, nsamps, fs, t0)
%
%  Simulation of WISPR Killer Whale click detection algorithm
%

clear all;

verbose_level = 3;

DETECT_PASSED			= 1;
DETECT_PROCESSING		= 2;
DETECT_FAILED			= 4;
DETECT_ERROR			= 8;

% wave file used for testing
wavfile = 'C:\Users\Chris\Documents\Wispr\applications\WISPR_KillerWhale\dtx_test_signals.wav';
%fp = fopen('kw-matlab.wav');
fp = fopen(wavfile, 'r');
hdr = fread(fp, 44);  % read the header

%--------------------------------------------------------------------------
% User defined inputs

% system adc parameters to mimic wispr
fs = 93750;
duration = 10.0;  % size of adc buffer in seconds
blksize = 4096;   

% detection filters
%[b1,a1]=butter(2, [43000 46000]/(fs/2));  % holgers original bandpass
[b1,a1]=butter(2, [40000 43000]/(fs/2));
[b2,a2]=butter(2, [5000 8000]/(fs/2));

% detection parameters
winsize = 512;
min_clicks_per_detection = 10;	% min number of clicks for a detection, see detect.h
num_bufs_per_detection = 6;	    % number of adc bufs that define the detection duration
ici_thresh = 0.33; % number of good clicks / total clicks
ici_min = 0.005; % min ici seconds
ici_max = 0.15; %0.25;  % max ici seconds
thresh_factor = 170.0;  % used in calculating ratio_thresh

%--------------------------------------------------------------------------

% num samples in the adc buffer
nsamps_per_buf = fix(duration * fs);

% number of data blocks in each adc buffer
nblks = fix(nsamps_per_buf / blksize);

% force the nsamps to be a multiple of blksize
nsamps_per_buf = fix(nblks * blksize);  % actual nsamps per buffer
duration = nsamps_per_buf / fs; % actual buffer duration

% force winsize to be an even multiple of buffer size
winsize = fix(nsamps_per_buf / fix(nsamps_per_buf / winsize));

% num of filter windows per adc buffer 
nwins_per_buf = (nsamps_per_buf / winsize);

% total_nwins defines the total detect time window which can be made up of 
% multiple adc buffers
total_nwins = nwins_per_buf * num_bufs_per_detection;

dtx_ibuf = zeros(winsize,1);
dtx_ratio = zeros(total_nwins,1);
dtx_max1 = zeros(total_nwins,1);
dtx_max2 = zeros(total_nwins,1);

tke = zeros(total_nwins,1);
tke_good = zeros(total_nwins,1);

dtx_good_count_total = 0;
dtx_index = 1;

t0 = 0.0;  % start time of first buffer

dtx_mean_max1 = 0;
dtx_mean_max2 = 0;

go = 1;
while(go)  % loop over file reading chunks of data of size 'duration' seconds
    
    % read a buffer of data, justthe adc
    buffer = fread(fp, nsamps_per_buf, 'int16');

    % scale numbers to unity
    buffer = buffer / 32767;
    
    if(length(buffer) < nsamps_per_buf)
        break;
    end

%    fprintf('buf_max = %f, mean = %f\n', max(buffer), mean(buffer));
    
    % loop over subwindow in the buffer
    k = 1;
    for n = 1:nwins_per_buf

        status = DETECT_PROCESSING;
        
        % copy a window of data into ibuf
        for i=1:winsize
            ibuf(i) = 0;
            if((i+k) < nsamps_per_buf)
                ibuf(i) = buffer(i+k);
            end
        end

        % apply the high band filter to the window
        obuf = filter(b1, a1, ibuf);

        % find max of high band filtered data
        max1 = max(obuf);

        % apply the low band filter to the window
        obuf = filter(b2, a2, ibuf);

        % find max of lower band filtered data
        max2 = max(obuf);

        % check for zeros
%        if(max1 == 0) max1 = 1; end
%        if(max2 == 0) max2 = 1; end

        % calc ratio of max values
        ratio = max1 / max2;

        if(ratio < 1.0)  % don't even consider it
            ratio = 0.0;
        else
            ratio = 20.0*log10(ratio); % use db value
        end

        % store the means of the max
        dtx_mean_max1 = dtx_mean_max1 + max1;
        dtx_mean_max2 = dtx_mean_max2 + max2;

        % save result
        dtx_ratio(dtx_index) = ratio;
        dtx_max1(dtx_index) = max1;
        dtx_max2(dtx_index) = max2;

        % increment the ratio buffer index
        dtx_index = dtx_index + 1;
        
        k = k + winsize;

    end  % loop over subwindows

    % if dtx_ration buffer is full then start the detection
    if(dtx_index >= total_nwins)

        % now reset dtx_index
        dtx_index = 1;

        % If it gets to this point the detection ratio buffer is full and ready
        % to be processed
        status = 0;  % clear status flag

        % normalize the means with the total nwins used
        dtx_mean_max1 = dtx_mean_max1 / total_nwins;
        dtx_mean_max2 = dtx_mean_max2 / total_nwins;

        % calc detection threshold
        if(dtx_mean_max2 == 0.0)  % special case if zero
            ratio_thresh = thresh_factor/120.0;  % is this a reasonable value???
        else
            ratio_thresh = thresh_factor/abs(log10(abs(dtx_mean_max2)));
        end
        
        fprintf('detect: mean_high=%f, mean_low=%f, ratio_thresh=%f\n', ...
            dtx_mean_max1, dtx_mean_max2, ratio_thresh);
        
        % reset means
        dtx_mean_max1 = 0.0;
        dtx_mean_max2 = 0.0;

        % initialize detection parameters
        dt = winsize / fs;
        prev_tc = 0.0;
        mean_ratio = 0.0;
        mean_tke = 0.0;
        mean_ici = 0.0;

        % now loop over all the windows in the full ratio buffer
        % to detect individual clicks
        dtx_good_count = 0;
        dtx_total_count = 0;

        for n = 1:total_nwins

            % apply teager-kaiser transform to ratio
            if(n == 1)
                tke(n) = dtx_ratio(n)*dtx_ratio(n) - dtx_ratio(n)*dtx_ratio(n+1);
            elseif(n == total_nwins)
                tke(n) = dtx_ratio(n)*dtx_ratio(n) - dtx_ratio(n-1)*dtx_ratio(n);
            else
                tke(n) = dtx_ratio(n)*dtx_ratio(n) - dtx_ratio(n-1)*dtx_ratio(n+1);
            end

            tke_good(n) = 0;
            
            % threshold the tke ratio
            if(tke(n) >= ratio_thresh)
                tc = t0 + dt * n; % click time stamp
                ici = tc - prev_tc;
                prev_tc = tc;
                % it's a good click if the ici falls within a specified range
                if((ici <= ici_max) && (ici >= ici_min))
                    dtx_good_count = dtx_good_count + 1;
                    mean_ratio = mean_ratio + dtx_ratio(n);
                    mean_tke = mean_tke + tke(n);
                    mean_ici = mean_ici + ici;
                    tke_good(n) = 1;
                end
                dtx_total_count = dtx_total_count + 1;
            end

        end

        figure(1);
        t = dt*(1:total_nwins);
        figure(1);
        plot(t(find(tke_good)), tke(find(tke_good)), 'ro', t, tke, '-', [t(1) t(end)],[ratio_thresh ratio_thresh], '--' );
        title('Plot of TKE (Detections are marked with RED circles)');
        xlabel('Seconds');
        ylabel('TKE');
        
        if(dtx_good_count > 1)
            mean_ratio = mean_ratio / dtx_good_count;
            mean_tke = mean_tke / dtx_good_count;
            mean_ici = mean_ici / dtx_good_count;
        end

        % test for a good detection
        % check for the miniumun number of good clicks and ratio of good clicks to total
        if((dtx_good_count >= min_clicks_per_detection) && ((dtx_good_count / dtx_total_count) >= ici_thresh))

            status = DETECT_PASSED;

            dtx_str = sprintf('DTX,%d,%.3f,%.3f,%.3f,%.3f\n', dtx_good_count, mean_tke, ratio_thresh, mean_ratio, mean_ici);
            fprintf('%s', dtx_str);

            % keep track of total number of detections
            dtx_good_count_total =  dtx_good_count_total + dtx_good_count;

        else
            
            status = DETECT_FAILED;

        end

        pause(1);
        %go = input('Hit ENTER to continue (0 to quit): ');
        %if(isempty(go)) go = 1; end

    end

    t0 = t0 + duration;

end

fclose(fp);

fprintf('%d total detections found in file\n', dtx_good_count_total);

