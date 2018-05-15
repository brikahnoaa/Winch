function [b, a, gd] = iir_filter_coeffs(r)
%
% Matlab function to generate IIR decimation filter coefficients
% and group delay
%

nfilt = 2;

% IIR filter
rip = .05;	% passband ripple in dB
[b,a] = cheby1(nfilt, rip, 0.8/r);
while all(b==0) || (abs(filtmag_db(b,a,0.8/r)+rip)>1e-6)
    nfilt = nfilt - 1;
    if nfilt == 0
        break
    end
    [b,a] = cheby1(nfilt, rip, 0.8/r);
end
if nfilt == 0
    error('Bad Chebyshev design, likely R is too big; try mult. decimation (R=R1*R2).')
end

nstages = nfilt / 2;

% print c version, cut/paste into code 
fprintf('%s\n%s\n%s\n%s\n%s\n', ...
sprintf('#define IIR_NSTAGES %d',nstages), ...
sprintf('float iir_b_%d[2*IIR_NSTAGES+1] = {',r), ...
[sprintf('%f, ', b), '};'], ...
sprintf('float iir_a_%d[2*IIR_NSTAGES] = {',r), ...
[sprintf('%f, ', a(2:end)), '};'] );

[gd,w] = grpdelay(b,a,nfilt);
gd = max(gd);
fprintf('float iir_group_delay = %0.6f;\n', gd);

%--------------------------------------------------------------------------
function H = filtmag_db(b,a,f)
%FILTMAG_DB Find filter's magnitude response in decibels at given frequency.

nb = length(b);
na = length(a);
top = exp(-j*(0:nb-1)*pi*f)*b(:);
bot = exp(-j*(0:na-1)*pi*f)*a(:);

H = 20*log10(abs(top/bot));

