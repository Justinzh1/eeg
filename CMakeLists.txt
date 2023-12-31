cmake_minimum_required(VERSION 3.18)
project(eeg)

# Add the path to the eemagine directory, relative to the current directory
set(EEMAGINE_DIR "eemagine/sdk")
set(CMAKE_CXX_FLAGS "-Wall -O2 -Wextra")

# Add the include directory paths
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${EEMAGINE_DIR})

# Include all .cc and .h files in the eemagine directory
file(GLOB EEMAGINE_SOURCES "${EEMAGINE_DIR}/*.cc"
                           "${EEMAGINE_DIR}/*.h")

# Include your main source file
set(MAIN_SRC "src/main.cc")
file(GLOB OTHER_SRC "src/*.cc")

# Add the path to the directories to be used when linking
link_directories(${CMAKE_SOURCE_DIR}/linux/64bit)

# Add the executable using the main source file and the eemagine files
add_executable(${PROJECT_NAME} ${MAIN_SRC} ${OTHER_SRC} ${EEMAGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${EEMAGINE_SDK_DIR})

# Link against the dynamic linking loader library (for dlopen, dlerror, dlsym)
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} eego-SDK ncurses)