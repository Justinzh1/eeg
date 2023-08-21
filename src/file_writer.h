// FileWriter.h

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <iostream>
#include <fstream>
#include <string>

class FileWriter {
public:
    FileWriter();
    FileWriter(const std::string& filename);
    ~FileWriter();

    bool Open(const std::string& filename);
    bool IsOpen() const;
    bool Write(const std::string& data);
    void Close();

private:
    std::ofstream fileStream;
    bool isOpen;
};

#endif // FILEWRITER_H
