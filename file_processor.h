#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <vector>
#include <string>
#include <cstdint>

class FileProcessor {
public:
    static std::vector<uint8_t> read_file(const std::string& path);
    static void write_file(const std::string& path, const std::vector<uint8_t>& data);
    static bool ensure_directory_exists(const std::string& file_path);
};

#endif