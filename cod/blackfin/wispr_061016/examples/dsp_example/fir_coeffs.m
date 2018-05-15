function [h, gd] = fir_coeffs(nfilt, r)
%
% Matlab function to generate FIR decimation filter coefficients
% and group delay
%
h = fir1(nfilt, 1/r);

% print c version, cut/paste into code 
fprintf('%s\n%s%s\n', ...
sprintf('#define FIR_NCOEFFS %d',nfilt), ...
sprintf('float iir_b_%d[FIR_NCOEFFS] = {',r), ...
[sprintf('%f, ', h(2:end)), '};'] );

[gd,w] = grpdelay(h,nfilt);
gd = mean(gd);
fprintf('float fir_group_delay = %0.6f;\n', gd);
