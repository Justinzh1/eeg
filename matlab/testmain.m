
function [D] = testmain(mode, refSamplingRate, refTempResolution)
% Function to test the functionality of the eego to matlab interface.
% mode input:           Type string, either 'eeg' or 'imp'. 'eeg' returns eeg data
%                       and 'imp' returns the impedace of the electrode.
% refSamplingRate:      Type integer, Sampling rate of the amplifier.
% refTempResolution:    Type integer, duration of the intervall of each getdata(). 

% transfer input variables.
c_mode = mode;
SR = refSamplingRate;
P = refTempResolution;

% buffer prototyping
tempData = cell(10000, 2);

% Selection of channels to be displayed
chl1 = 1;
chl2 = 64;
repeats = 10; % number of getdata procedures

% index initialization
i = 1;
a = 0;

PreviousSampleEnd = 0;

if strcmp(c_mode, 'eeg')
%% EEG Procedure
    disp(' *********** EEG-Mode has been selected ***********');
    eegoObj= eego_interface(); % open the amplifier
    openstream_eeg(eegoObj, SR); % open an eeg stream
    
    %  show the address of the stream object
    StrA = ['The amplifier is located at: ',  num2str(eegoObj.AmplHandle)]; disp(StrA);
    StrB = ['The stream is located at: ', num2str(eegoObj.StrmHandle)]; disp(StrB);
    disp('--------------------------------------------------------------------------------');
    figure
    
    pause(1.5);
    
    while a <= repeats % limit of getdata cycles
        eego_getData(eegoObj); % retrieve data from buffer
        SampleSize = size(eegoObj.OutputData, 1);
        NewSampleEnd =  PreviousSampleEnd + SampleSize; % update total number of retrieved samples
        tempData(i, 1) = {eegoObj.OutputData} ; % buffering data
        D = cell2mat(tempData); % join data set
        
        % adapt time axis
        if PreviousSampleEnd < 1000
                subplot(2,1,1); plot(D(:, chl1)); axis ([0 (NewSampleEnd) -2 2]);
                subplot(2,1,2); plot(D(:, chl2)); axis ([0 (NewSampleEnd) -2 2]);
        else
                subplot(2,1,1); plot(D(:, chl1)); axis ([(PreviousSampleEnd - 1000) (NewSampleEnd +10) -2 2]);
                subplot(2,1,2); plot(D(:, chl2)); axis ([(PreviousSampleEnd - 1000) (NewSampleEnd +10) -2 2]);
        end
        drawnow;
        
        pause(P);
        
        i = i+1; % increment index i
        PreviousSampleEnd = NewSampleEnd;
        a = a+1; % increment index a
        
        pause(0.5);
    end;

elseif strcmp(c_mode, 'imp')
%% Impedance procedure
    disp(' *********** IMPEDANCE-Mode has been selected ***********');
    eegoObj= eego_interface(); % open the amplifier
    openstream_imp(eegoObj); % open the impedance stream
    
    %  show the address of the stream object
    StrA = ['The amplifier is located at: ',  num2str(eegoObj.AmplHandle)]; disp(StrA);
    StrB = ['The stream is located at: ', num2str(eegoObj.StrmHandle)]; disp(StrB);
    disp('--------------------------------------------------------------------------------');
        
    while a <= repeats
        eego_getData(eegoObj); % retrieve data from buffer
        ImpData = eegoObj.OutputData'; % transpose current impedance data
        
        chlLabel = cell(64, 2);
        for j =1:1:64
            chlLabel { j, 1 } = strcat('Chl - ' ,int2str( j));
            chlLabel { j, 2 } = ImpData (j);
        end
        D = chlLabel;
        pause(P);
        
        a = a+1; % increment index a
    end 
    
end
    
% tidy up and free buffer
close_stream(eegoObj);
clear eegoObj;

msgbox('end');
