# EEG Project

## Setup
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

Run the executable
```bash
./eeg
```
