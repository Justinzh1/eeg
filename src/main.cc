#include <iostream>
#include <sstream>
#include <eemagine/sdk/factory.h>
#include <chrono>
#include "file_writer.h"

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

int write_eeg_data(FileWriter* writer) {
    // Construct the mock data to write
    std::vector<double> channelData = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    auto formattedData = format_data(channelData);

    // Check if the file is open
    if (writer->IsOpen()) {
        // Write data to the file
        if (writer->Write(formattedData)) {
            std::cout << "Data written to the file." << std::endl;
        } else {
            std::cerr << "Failed to write data to the file." << std::endl;
        }

        // Close the file
        writer->Close();
        std::cout << "File closed." << std::endl;
    } else {
        std::cerr << "File is not open." << std::endl;
    }
    return 0;
}

int main() {
    std::cout << "Initializing EEG...\n";

    // Instantiate EEG SDK
    using namespace eemagine::sdk;
    // We are using dynaic binding, but in case it doesn't work..
    // Use `sudo ldconfig` to reload the library if the amplifiers are not showing up
    factory fact("libeego-SDK.so");

    FileWriter* writer = new FileWriter("eeg-data.txt");

    // Get Amplifiers
    auto amps = fact.getAmplifiers();
    if (amps.size() > 0) {
        std::cout << "Found " << amps.size() << "amplifiers!\n";

        // TODO process the data from amplifiers and write to a new file
    } else {
        std::cout << "No amplifiers found.\n";
    }

    // Test Write Data
    write_eeg_data(writer);
    
    return 0;
}
