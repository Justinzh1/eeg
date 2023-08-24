#include "mex.h"
#include "class_handle.hpp"

#define EEGO_SDK_BIND_DYNAMIC // How to bind the DLL
#define _UNICODE
#define WIN32

// EEGO SDK headers to be included full path:
#include "eemagine\sdk\factory.h"
#include "eemagine\sdk\amplifier.h"
#include "eemagine\sdk\stream.h"
#include "eemagine\sdk\wrapper.cc" // Wrapper Code to be compiled.

// System relevant headers
#include <thread>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <vector>

// The class that we are interfacing to are the classes defined in the eego-SDK

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{	
// =================================================================================//

	// Declaration of Variables
	int ChannelC;
	int SampleC;
	int elements;
	int exit = 0;
	double *pointer;
	mwSize index;

	// preCompiled Variables
	#define ROWS SampleC
	#define COLUMNS ChannelC
	#define ELEMENTS elements
	
// =================================================================================//

    // Get the command string
    char cmd[64];
    // if no input argument or no valid argument retrievable
	if (nrhs < 1 || mxGetString(prhs[0], cmd, sizeof(cmd)))
		mexErrMsgTxt
		("First input should be a command string less than 64 characters long.");
        
// --------------------------------------------------------------------------------//
// Open an amplifier and return its address to matlab 					mexlock (+1)
// --------------------------------------------------------------------------------//
    if (!strcmp("new", cmd)) {
        // Check parameters
        if (nlhs != 1)
            mexErrMsgTxt("New: One output expected.");
            
        using namespace eemagine::sdk;
        factory fact("eego-SDK.dll");

		// hackery dackery
		eemagine::sdk::factory::version vers(fact.getVersion());
		std::cout << "version: " << vers.build << std::endl;

        amplifier* amp = fact.getAmplifier();
		// Return a handle to a new C++ instance
        plhs[0] = convertPtr2Mat<amplifier>(amp);
        return; // --> Back to MatLab
    }
    
// --------------------------------------------------------------------------------//
// This is initiated when handle should be manipulated
// Check whether there is a second input, which should be the class instance handle
// --------------------------------------------------------------------------------//
    if (nrhs < 2)
		mexErrMsgTxt("Second input should be a class instance handle.");

// --------------------------------------------------------------------------------//
// Delete the AmplifierHandle									 Unlock the mex (-1)
// --------------------------------------------------------------------------------//
    if (!strcmp("delete", cmd)) {
        // Destroy the C++ object
        using namespace eemagine::sdk;
        destroyObject<amplifier>(prhs[1]);
        // Warn if other commands were ignored
        if (nlhs != 0 || nrhs != 2)
            mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return; // --> Back to MatLab
    }

// Open either eeg, calibration or impedance stream via the adress of the amplifier
// and returns the adress og the stream in the memory to matlab:
// In matlab: 
// this.StreamHandle = eego('command',this.AmplifierHandle,'optional',Samplingrate);
// --------------------------------------------------------------------------------//
//   n an eeg-stream													mexlock (+1)
// --------------------------------------------------------------------------------//
    // open eego stream
    if (!strcmp("open_eeg_stream", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Train: Unexpected arguments.");
        
        // Get the class instance pointer from the second input
    	using namespace eemagine::sdk;
    	amplifier *Amp_instance = convertMat2Ptr<amplifier>(prhs[1]);
    	using namespace std;
        
		// Call the method
		int SR = (int)mxGetScalar(prhs[3]);
		using namespace eemagine::sdk;
		stream* eegstream = Amp_instance->OpenEegStream(SR,0.15);

		// wait until the object is instantiated
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
		// convert the handle and give back to matlab  
		plhs[0] = convertPtr2Mat<stream>(eegstream);
        return; // --> Back to MatLab
    }
    
// --------------------------------------------------------------------------------//
// Open a impedance-stream; there is no samplingrate needed				mexlock (+1)
// --------------------------------------------------------------------------------//
        if (!strcmp("open_impedance_stream", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Train: Unexpected arguments.");
        
        // Get the class instance pointer from the second input
    	using namespace eemagine::sdk;
    	amplifier *Amp_instance = convertMat2Ptr<amplifier>(prhs[1]);
    	using namespace std;
        
		// Call the method
		using namespace eemagine::sdk;
		stream* impstream = Amp_instance->OpenImpedanceStream();

		// wait until the object is instantiated
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		// convert the handle and give back to matlab  
		plhs[0] = convertPtr2Mat<stream>(impstream);
        return; // --> Back to MatLab
    }
    
// Get the data using the adress of the stream object
// and returns the data as a matrix, whereas the columns are indexed channels and
// the rows are the indexed samples of the stream data.
// In matlab: this.OutputData = eego('command',this.StreamHandle);
// --------------------------------------------------------------------------------//
// Retrieve the data from the stream
// --------------------------------------------------------------------------------//
    // Get data from the stream 
    if (!strcmp("getdata", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Test: Unexpected arguments.");
        
		using namespace eemagine::sdk;
        // Get the class instance pointer from the second input
        stream *Str_instance = convertMat2Ptr<stream>(prhs[1]);
		buffer buf = Str_instance->getData(); // Retrieve data from stream

		SampleC = buf.getSampleCount(); // Index of maximum Sample
		ChannelC = buf.getChannelCount(); // Index of maximum Channel

		elements = SampleC * ChannelC;
				
		if (elements == 0) { // do nothing
		} else { // write data into an C++ array and then transform to Matlab matrix
		
		// Allocate memory for new C++ 1D array with size of 'elements'
		double* OutputArray;
		OutputArray = new double[elements];
		
		// Write retrieved data into the preallocated 1D Array 
		// OutputArray = [Samp1(Ch1), Samp2(Ch1), ... , Samp1(Ch2), Samp2(Ch2), ...]
		for (int ChannelIndex = 0; ChannelIndex < ChannelC; ChannelIndex++)
		{
			for (int SampleIndex = 0; SampleIndex < SampleC; SampleIndex++)
			{
				int SampleJump = ChannelIndex * SampleC;
					OutputArray[SampleIndex+SampleJump] = 
					buf.getSample(ChannelIndex, SampleIndex);
			}
		} using namespace std;
					
		// Create empty Matlab matrix with dimension ROWS x COLUMN. The rows of the
		// matrix will index samples and the columns will index channels
    	plhs[0] = mxCreateNumericMatrix(ROWS, COLUMNS, mxDOUBLE_CLASS, mxREAL);
    	// Get a pointer which points to the first element of the empty Matrix
		pointer = mxGetPr(plhs[0]);
		
		// Copy data into the mxArray. The format of the C++ array and Matlab matrix
		// are chosen so because by increasing 'index', pointer[index] will first go
		// vertically alone the different samples before changing to the next column.
    	for ( index = 0; index < elements; index++ ) {
        pointer[index] = OutputArray[index];
    	}	
    	
    	// After making the copy, the preallocated memory has to be released
    	delete [] OutputArray;
    	// delete only releases the memory without reseting the pointer. This is done
    	// with the next line.
    	OutputArray = NULL;
    	
		return; // --> Back to MatLab
    	}
    }
    
// --------------------------------------------------------------------------------//
// Close the selected stream, should always be called
// before a new stream can be opened							 
// In matlab: eego('command',this.StreamHandle);				 Unlock the mex (-1)
// --------------------------------------------------------------------------------//
    if (!strcmp("close_stream", cmd)) {
        // Check parameters
        if (nlhs < 0 || nrhs < 2)
            mexErrMsgTxt("Test: Unexpected arguments.");
            
        // Call destroyer
    	using namespace eemagine::sdk;
    	destroyObject<stream>(prhs[1]);
    	
    	if (nlhs != 0 || nrhs != 2)
        mexWarnMsgTxt("Delete: Unexpected arguments ignored.");
        return; // --> Back to MatLab
    }
    
    // Got here, so no command were being recognized
    mexErrMsgTxt("Command not recognized.");
}
