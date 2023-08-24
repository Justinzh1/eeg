#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <eemagine/sdk/factory.h>
#include <chrono>
#include <ncurses.h>
#include <getopt.h>

const std::string EEG_DATA_FILE = "data.txt";

struct EegParams {
    bool debug;
    int sample_frequency;
    int sleep_ms;
};

EegParams getArgs(int argc, char *argv[]) {
    EegParams params = EegParams{
        debug: false,
        sample_frequency: 500,
        sleep_ms: 1000,
    };
    int opt;
    bool d_flag = false;
    int sample_frequency = 500;
    int sleep_ms = 1000;
    while ((opt = getopt(argc, argv, "hdsw:")) != -1) {
        switch(opt) {
        case 'd':
            d_flag = true;
            break;
        case 's':
            sample_frequency = atoi(optarg);
            break;
        case 'w':
            sleep_ms = atoi(optarg);
            break;
        case 'h':
            fprintf(stderr,
                "Eego handler \n"
                "Usage: eego [OPTION]...\n"
                "Example: eego -d -s500 -w500\n"
                "\n"
                "  -h        Print out this help\n"
                "  -d        Debug mode to show more logs\n"
                "  -s        Sample frequency (eg. 500, 512, 1000, 1024, 2000, 2048, 4000, 4096, 8000, 8192, 16000, 16384)\n"
                "  -w        The time to sleep beteween each file write\n"
            );
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    params.debug = d_flag;
    params.sample_frequency = sample_frequency;
    params.sleep_ms = sleep_ms;
    
    std::cout << "using debug mode = " << params.debug << "\n";
    std::cout << "using sample frequency = " << params.sample_frequency << "\n";
    std::cout << "using sleep ms = " << params.sleep_ms << "\n";

    return params;
}

void printChannelData(std::vector<eemagine::sdk::channel> channels) {
    std::cout << "Found " << channels.size() << " channels.\n";
        for (uint32_t i = 0; i < channels.size(); ++i) {
            auto channel = channels[i];
            std::cout << "Channel Type: " << channel.getType() << " index: " << channel.getIndex() << "\n";
    }
}

std::string processStream(eemagine::sdk::stream* stream) {
    eemagine::sdk::buffer buf = stream->getData();

    // Process Channel Data
    auto channelCount = buf.getChannelCount(); auto sampleCount = buf.getSampleCount();
    std::string line = "[" + std::to_string(sampleCount) + "] ";
    for (uint32_t j = 0; j < channelCount; j++) {
        auto sample = buf.getSample(j, sampleCount - 1);
        line += std::to_string(sample) + " ";
    }
    line += "\n";
    return line;
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

        // Initiate the terminal 
        initscr();
        cbreak();
        noecho();
        scrollok(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        // Collect eeg data
        stream* eegStream = amp->OpenEegStream(args.sample_frequency);
        addstr("Measuring eeg. Press s to exit\n");
        while (true) {
            if (getch() == 's') {
                break;
            }

            auto line = processStream(eegStream);

            // Log the stream data to the terminal if debug mode is enabled
            if (args.debug) {
                addstr(line.c_str());
            }

            std::fstream eeg_data_file = std::fstream(EEG_DATA_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            eeg_data_file << line;
            eeg_data_file.close();

            // Sleep
            napms(args.sleep_ms);
        }

        std::cout<<"exited\n";
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
