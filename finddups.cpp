#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <set>

namespace fs = std::filesystem;

std::string calculateSHA256(const fs::path& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return ""; // Return empty string on error
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    // Simple (and not cryptographically secure) example for demonstration.
    // For real SHA-256, use a library like OpenSSL or Crypto++.
    // This example uses a very basic hash for demonstration.
    size_t hash = 0;
    char buffer[4096];
    while (file.read(buffer, sizeof(buffer))) {
        for (size_t i = 0; i < file.gcount(); ++i) {
            hash = (hash * 31) + static_cast<unsigned char>(buffer[i]); // Simple hash
        }
    }
    for(int i = 0; i < 8; i++){
        ss << std::setw(8) << (hash & 0xFFFFFFFF);
        hash >>= 32;
    }

    return ss.str();
}

std::map<std::string, std::vector<fs::path>> findDuplicateFiles(const fs::path& directory, const std::set<std::string>& excludedExtensions) {
    std::map<std::string, std::vector<fs::path>> fileHashes;

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (fs::is_regular_file(entry)) {
            std::string extension = entry.path().extension().string();
            if (excludedExtensions.find(extension) == excludedExtensions.end()) {
                std::string hash = calculateSHA256(entry.path());
                if (!hash.empty()) {
                    fileHashes[hash].push_back(entry.path());
                }
            }
        }
    }

    std::map<std::string, std::vector<fs::path>> duplicates;
    for (const auto& pair : fileHashes) {
        if (pair.second.size() > 1) {
            duplicates.insert(pair);
        }
    }

    return duplicates;
}

int main() {
    fs::path directory;
    std::cout << "Enter directory to search: ";
    std::cin >> directory;

    std::set<std::string> excludedExtensions = {".cpp", ".png", ".exe", ".py", ".pyc", ".plist"}; // Add more extensions as needed.

    std::map<std::string, std::vector<fs::path>> duplicates = findDuplicateFiles(directory, excludedExtensions);

    if (duplicates.empty()) {
        std::cout << "No duplicate files found." << std::endl;
    } else {
        for (const auto& pair : duplicates) {
            std::cout << "Duplicate files (hash: " << pair.first << "):" << std::endl;
            for (const auto& filePath : pair.second) {
                std::cout << "  - " << filePath << std::endl;
            }
            std::cout << "------------------------" << std::endl;
        }
    }

    return 0;
}
