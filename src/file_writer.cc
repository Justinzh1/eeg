// file_writer.cc

#include "./file_writer.h"

FileWriter::FileWriter() : isOpen(false) {
}

FileWriter::FileWriter(const std::string& filename) : isOpen(false) {
    Open(filename);
}

FileWriter::~FileWriter() {
    Close();
}

bool FileWriter::Open(const std::string& filename) {
    if (isOpen) {
        Close();
    }

    fileStream.open(filename.c_str(), std::ios::out | std::ios::trunc);

    if (fileStream.is_open()) {
        isOpen = true;
        return true;
    } else {
        std::cerr << "Failed to open file: " << filename << std::endl;
        isOpen = false;
        return false;
    }
}

bool FileWriter::IsOpen() const {
    return isOpen;
}

bool FileWriter::Write(const std::string& data) {
    if (isOpen) {
        fileStream << data;
        return true;
    } else {
        std::cerr << "File is not open." << std::endl;
        return false;
    }
}

void FileWriter::Close() {
    if (isOpen) {
        fileStream.close();
        isOpen = false;
    }
}
