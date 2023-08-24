# EEG Project

## Setup
The following assums a unix environment. You should have gcc and cmake installed already.

```bash
sudo apt-get update
sudo apt-get install cmake
sudo apt install gcc
```

Unzip the `eego-sdk.zip` (you will need your own copy) file at the project root. You should have the following directory structure

```bash
├── eemagine
│   └── sdk
│       └── ...
├── linux
│   └── 64bit
│       └── libeego-SDK.so
├── src
│   └── main.cc
└── ...
```

To build, first make a directory `build`
```bash
mkdir build
```

Navigate to the directory
```bash
cd build
```

Build the CMakeList and then the Makefile
```bash
cmake .. && make
```

## Usage
Run the executable. The executable should be runnable from anywhere using the following command (I've included it in the path)
```bash
eeg
```

Alternatively, you can run this command directly assuming you have built the executable.
```
/home/callosum/Code/eeg/build/eeg
```

## Next Steps

- [] **True time synchronization** - Provide a time synchronized solution to poll the eego device
- [] **Channel bit mask** - Provide a way to mask the desired channels
- [] **Matlab native support** - Provide a way to directly interact with eego with matlab
