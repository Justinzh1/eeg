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

## Issues
The eego-sdk provides a shared object (`libeego-SDK.so`) that is x86 compatible. This does not work with the raspberry pi's ARM architecture.

The following is as far as we can get when we attempt to create the factory per this line:

```cpp
factory fact("libeego-SDK.so");
```

Compilation error
```bash
/home/eeg/linux/64bit/libeego-SDK.so: file not recognized: file format not recognized
collect2: error: ld returned 1 exit status
make[2]: *** [CMakeFiles/eeg.dir/build.make:118: eeg] Error 1
make[1]: *** [CMakeFiles/Makefile2:95: CMakeFiles/eeg.dir/all] Error 2
make: *** [Makefile:103: all] Error 2
```
