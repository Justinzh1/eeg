% CLASS_INTERFACE - MATLAB class wrapper to an underlying C++ class
classdef eego_interface < handle
    properties (SetAccess = private, Hidden = false)
        AmplHandle; % Handle to the amplifier object
        StrmHandle; % Handle to the stream object
        OutputData; % Variable to save the streamed data [rows - samples : columns - channels]
    end
    % Funktionen:
    methods
        %% Constructor - Create a new C++ class instance: Reserve the memory needed for the EEG-Stream
        function this = eego_interface(varargin)
            this.AmplHandle = eego('new', varargin{:});
        end
        
        %% Destructor - Destroy the C++ class instance: Release the memory which was previously reserved
        function delete(this)
            eego('delete', this.AmplHandle);
        end

        %% --- Open the eeg-stream
        function openstream_eeg(this, varargin)
            this.StrmHandle = eego('open_eeg_stream', this.AmplHandle, 'Samplingrate',varargin{:});
        end

        %% --- Open the impedance-stream
        function openstream_imp(this)
            this.StrmHandle = eego('open_impedance_stream', this.AmplHandle);
        end
       
        %% --- Retrieve data from the stream
        function getData(this)
            this.OutputData = eego('getdata', this.StrmHandle);
        end
        
        %% --- Close the current stream
        function close_stream(this, varargin)
            eego('close_stream', this.StrmHandle, varargin{:});
        end
    end
end