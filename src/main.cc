#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <eemagine/sdk/factory.h>
#include <chrono>

const std::string EEG_DATA_FILE = "data.txt";

std::string format_data(std::vector<double>& channels) {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::chrono::seconds unixTimestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    std::string timestampStr = std::to_string(unixTimestamp.count());

    std::stringstream ss;

    ss << timestampStr + " ";
    
    // Use an iterator to traverse the vector
    for (auto it = channels.begin(); it != channels.end(); ++it) {
        ss << *it; // Append the current element to the stringstream
        
        // Add a space unless it's the last element
        if (std::next(it) != channels.end()) {
            ss << " ";
        }
    }
    
    // Convert the stringstream to a string
    ss << "\n";
    return ss.str();
}

int write_eeg_data(std::vector<double> channel_data) {
    std::fstream eeg_data_file = std::fstream(EEG_DATA_FILE, std::fstream::in | std::fstream::out | std::fstream::trunc);
    auto formatted_data = format_data(channel_data);
    eeg_data_file << formatted_data;
    eeg_data_file.close();
    return 0;
}


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

    // Get Amplifiers
    auto amps = fact.getAmplifiers();
    if (amps.size() > 0) {
        std::cout << "Found " << amps.size() << "amplifiers!\n";
        // TODO process the data from amplifiers and write to a new file
    } else {
        std::cout << "No amplifiers found.\n";

        // Using test data if no amplifiers are found
        channel_data = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};

        using frames = std::chrono::duration<std::int64_t, std::ratio<1, 2>>;
        auto next_start = std::chrono::steady_clock::now() + frames{0};
        for (uint32_t i = 0; i < 60; ++i)
        {
            // do work here
            std::cout << "write eeg data " << i << "\n";
            write_eeg_data(channel_data);
            next_start += frames{1};
            std::this_thread::sleep_until(next_start);
        }
    }

    return 0;
}
