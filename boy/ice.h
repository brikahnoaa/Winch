// ice.h
#ifndef H_ICE
#define H_ICE

/*
 * Use this equation for the freezing temp of sea water under the sea ice
 * %Freezing temp of sea water in celcius
 * p=100; %in dB (10 dB ~ 10 m)
 * S=34;  %in per mil
 * tf=(-0.0575+1.710523E-3*sqrt(abs(S))-2.154996E-4*S)*S -7.53E-4*p;
 * fprintf('Freezing temp in C %f\n',tf);
 */

typedef struct IceInfo {
  bool on;            // do ice check
} IceInfo;

#endif
