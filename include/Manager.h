#pragma once
#include <string>

bool readConfig(const std::string& fileName, std::string* url);
bool readBody(const std::string& fileName, std::string* body);
void writeResponse(const std::string& fileName, const std::string& content);

class Manager {
    public:
        void run(int argc, char* argv[]);
};