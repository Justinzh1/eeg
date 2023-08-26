#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <eemagine/sdk/factory.h>
#include <chrono>
#include <ncurses.h>
#include <getopt.h>

const std::string EEG_DATA_FILE = "/home/callosum/Documents/burning_man/burning man/eeg_data.csv";

struct EegParams {
    bool debug;
    int sample_frequency;
    std::string channel_mask;
};

EegParams getArgs(int argc, char *argv[]) {
    EegParams params = EegParams{};
    int opt;
    bool d_flag = false;
    int sample_frequency = 500;
    std::string channel_mask = "1000000";
    while ((opt = getopt(argc, argv, "hdc:s:")) != -1) {
        switch(opt) {
        case 'd':
            d_flag = true;
            break;
        case 's':
            sample_frequency = atoi(optarg);
            break;
        case 'c':
            channel_mask = std::string(optarg);
            break;
        case 'h':
            fprintf(stderr,
                "Eego handler \n"
                "Usage: eego [OPTION]...\n"
                "Example: eego -s500 -c1000000\n"
                "\n"
                "  -h        Print out this help\n"
                "  -d        Debug mode to show more logs\n"
                "  -c        Channels provided as a bitmask (eg. 10000000 will provide channel 1)"
                "  -s        Sample frequency (eg. 500, 512, 1000, 1024, 2000, 2048, 4000, 4096, 8000, 8192, 16000, 16384)\n"
            );
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    params.debug = d_flag;
    params.sample_frequency = sample_frequency;
    params.channel_mask = channel_mask;
    
    std::cout << "using debug mode = " << params.debug << "\n";
    std::cout << "using sample frequency = " << params.sample_frequency << "\n";
    std::cout << "using channel mask = " << params.channel_mask << "\n";

    return params;
}

void printChannelData(std::vector<eemagine::sdk::channel> channels) {
    std::cout << "Found " << channels.size() << " channels.\n";
}

void processStream(eemagine::sdk::stream* stream, bool debug) {
    eemagine::sdk::buffer buf = stream->getData();

    // Process Channel Data
    auto channelCount = buf.getChannelCount();
    auto debugLog = "Found channels " + std::to_string(channelCount) + " \n";
    addstr(debugLog.c_str());
    auto sampleCount = buf.getSampleCount();
    for (int c = 0; c < channelCount; c++) {
        std::string line = "";
        for (int i = 0; i < sampleCount; i++) {
            // Always take the first channel (we assume there is only 1 channel after we mask)
            auto sample = buf.getSample(c, i);
            line += std::to_string(sample) + ",";
        }

        // Log the stream data to the terminal if debug mode is enabled
        if (debug) {
            auto debugLine = "[" + std::to_string(c) + "] " + line + "\n";
            addstr(debugLine.c_str());
        }

        // Write the stream data to a file
        // Only write the data from the first channel
        if (c == 0) {
            std::fstream eeg_data_file = std::fstream(EEG_DATA_FILE, std::fstream::in | std::fstream::out | std::fstream::app);
            eeg_data_file << line;
            eeg_data_file.close();
        }
    }
}

int main(int argc, char *argv[]) {
    auto args = getArgs(argc, argv);

    // Instantiate EEG SDK
    std::cout << "Initializing EEG...\n";
    using namespace eemagine::sdk;
    factory fact("libeego-SDK.so");

    // Get Amplifier
    eemagine::sdk::amplifier* amp;
    try {
        amp = fact.getAmplifier();
    } catch(...) {
        std::cout << "Unable to find amplifiers. Is the ant neuro plugged in?\n";
        return -1;
    }

    if (amp) {
        auto channels = amp->getChannelList();
        printChannelData(channels);

        std::vector<eemagine::sdk::channel> channel_list;
        for (int i = 0; i < args.channel_mask.length(); i++) {
            if (args.channel_mask[i] == '1') {
                channel_list.push_back(channels[i]);
            } 
        }
        std::cout << "Keeping data from ";

        // Initiate the terminal 
        initscr();
        cbreak();
        noecho();
        scrollok(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        stream* eegStream = amp->OpenEegStream(500, 1, 4, channel_list);
        addstr("Measuring eeg. Press s to exit\n");

        while (true) {
            auto c = getch();

            // Stop the loop
            if (c == 's') {
                break;
            }

            processStream(eegStream, args.debug);

            // Sleep
            napms(500);
        }

        // Clean up
        std::cout<<"exited: "<<std::endl;
        delete eegStream;
        delete amp;
        endwin();
        return 0; 

    } else {
        std::cout << "No amplifiers found. Check that everything is plugged in.\n";
        return -1;
    }

    return 0;
}
