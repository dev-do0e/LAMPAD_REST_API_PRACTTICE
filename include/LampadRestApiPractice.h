#pragma once
#include <string>

class LampadRestApiPractice {
    public:
        void run();

    private:
        bool readConfig(const std::string& fileName, std::string* url);
};