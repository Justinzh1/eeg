#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <eemagine/sdk/factory.h>
#include <chrono>
#include <ncurses.h>

const std::string EEG_DATA_FILE = "data.txt";

int main() {
    std::cout << "Initializing EEG...\n";

    // Create output data file 

    // Instantiate EEG SDK
    using namespace eemagine::sdk;
    // We are using dynaic binding, but in case it doesn't work..
    // Use `sudo ldconfig` to reload the library if the amplifiers are not showing up
    factory fact("libeego-SDK.so");

    // Create a vector of channel data
    // TODO (jzhong) store the amplifier data in this vector
    std::vector<double> channel_data;

    // Get Amplifier
    auto amp = fact.getAmplifier();
    if (amp) {
        // Collect channel data
        auto channels = amp->getChannelList();
        std::cout << "Found " << channels.size() << " channels.\n";
        for (uint32_t i = 0; i < channels.size(); ++i) {
            auto channel = channels[i];
            std::cout << "Channel Type: " << channel.getType() << " index: " << channel.getIndex() << "\n";
        }

        // Collect impedance data
        stream* impStream = amp->OpenImpedanceStream(channels);
        std::cout<<"press c to continue and stop measuring impedance\n";

        initscr();
        cbreak();
        noecho();
        scrollok(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        addstr("Measuring impedance. Press c to continue\n");
        while (true) {
            if (getch() == 'c') {
                break;
            }
            buffer buf = impStream->getData();
            auto channelCount = buf.getChannelCount(); auto sampleCount = buf.getSampleCount();
            std::string line = "[" + std::to_string(sampleCount) + "] ";
            for (uint32_t j = 0; j < channelCount; j++) {
                auto sample = buf.getSample(j, sampleCount - 1);
                line += std::to_string(sample) + " ";
            }
            line += "\n";
            // Uncomment to log data
            addstr(line.c_str());
            napms(1000);
        }
        delete impStream;
      
        // Collect eeg data
        stream* eegStream = amp->OpenEegStream(1000);
        addstr("Measuring eeg. Press s to exit\n");
        while (true) {
            if (getch() == 's') {
                break;
            }
            buffer buf = eegStream->getData();
            auto channelCount = buf.getChannelCount(); auto sampleCount = buf.getSampleCount();
            std::string line = "[" + std::to_string(sampleCount) + "] ";
            for (uint32_t j = 0; j < channelCount; j++) {
                auto sample = buf.getSample(j, sampleCount - 1);
                line += std::to_string(sample) + " ";
            }
            line += "\n";
            // Uncomment to log data
            // addstr(line.c_str());

            std::fstream eeg_data_file = std::fstream(EEG_DATA_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
            eeg_data_file << line;
            eeg_data_file.close();
            napms(1000);
        }

        std::cout<<"exited: "<<std::endl;
        delete eegStream;
        delete amp;
        endwin();
        return 0; 

    } else {
        std::cout << "No amplifiers found.\n";
    }

    return 0;
}
