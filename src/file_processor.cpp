#include "file_processor.h"
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> FileProcessor::read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot open: " + path);
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

void FileProcessor::write_file(const std::string& path, 
                                const std::vector<uint8_t>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot create: " + path);
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}