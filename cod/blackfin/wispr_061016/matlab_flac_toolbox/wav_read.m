function [data, wave] = wavread(wavfile)
%
%WAVREAD Read Microsoft WAVE (".wav") sound file.
%   [Y,WAV]=WAVREAD(FILE) reads a WAVE file specified by the string FILE,
%   returning the sampled data in Y. The wave file info is stored in WAV.
%

fp = fopen(wavfile,'rb');

% read basic flac header
ChunkID = fread(fp,4,'char');
ChunkSize = fread(fp, 1, 'uint32');
wave.Format = fread(fp, 4, 'uint8=>char');
SubChunk1ID = fread(fp, 4, 'uint8=>char');
SubChunk1Size = fread(fp, 1, 'uint32');
wave.AudioFormat = fread(fp, 1, 'uint16');
wave.NumChannels = fread(fp, 1, 'uint16');
wave.SampleRate = fread(fp, 1, 'uint32');
wave.ByteRate = fread(fp, 1, 'uint32');
wave.BlockAlign = fread(fp, 1, 'uint16');
wave.BitsPerSample = fread(fp, 1, 'uint16');

% If Extensible Format then tere are extra fields
if(wave.AudioFormat == 65534) 
    cbSize = fread(fp, 1, 'uint16');
    wave.ValidBitsPerSample = fread(fp, 1, 'uint16');
    wave.ChannelMask = fread(fp, 1, 'uint32');
    SubFormat = fread(fp, 16, 'uint8=>char');
end

SubChunk2ID = fread(fp, 4, 'uint8=>char');
SubChunk2Size = fread(fp, 1, 'uint32');

% If non-pcm then there's an extra block
if(strncmp(SubChunk2ID,'fact',4))
    wave.SampleLength = fread(fp, 1, 'uint32');
    % re-read chunk2 ID and size
    SubChunk2ID = fread(fp, 4, 'uint8=>char');
    SubChunk2Size = fread(fp, 1, 'uint32');
end    

% now read the data block
data = [];
wave.nsize = wave.BitsPerSample/8; % sample size in bytes
wave.nsamps = SubChunk2Size/wave.nsize; % num of samps

if(wave.BitsPerSample == 8) 
    data = fread(fp, wave.nsamps, 'bit8=>int8'); % read remainder of file using 24 bits
elseif(wave.BitsPerSample == 16) 
    data = fread(fp, wave.nsamps, 'bit16=>int16'); % read remainder of file using 24 bits
elseif(wave.BitsPerSample == 24) 
    data = fread(fp, wave.nsamps, 'bit24=>int32'); % read remainder of file using 24 bits
elseif(wave.BitsPerSample == 32) 
    data = fread(fp, wave.nsamps, 'bit32=>int32'); % read remainder of file using 24 bits
else
    fprintf('Unknown word size\n');
end

fclose(fp);
