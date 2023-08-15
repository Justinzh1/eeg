#include <iostream>
#include <eemagine/sdk/factory.h>

int main() {
    std::cout << "Hello world from EEG\n";

    using namespace eemagine::sdk;

    factory fact("libeego-SDK.so");
    amplifier* amp = fact.getAmplifier();
    delete amp;
    
    return 0;
}
