#pragma once
#include <string>

//main method
bool fetchCivet7Token(const std::string* url, const std::string* feedsInformationPath);
bool fetchBps(const std::string* url, const std::string* feedsInformationPath);

//support method
bool readConfig(const std::string& fileName, std::string* url, std::string* feedsInformationPath);
bool readBody(const std::string& fileName, std::string* body);
bool writeResponse(const std::string& fileName, const std::string* content);

class Manager {
    public:
        void run();
};