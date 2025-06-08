#pragma once

#include "../include/LampadApiClient.h"

#include <vector>
#include <string>

void analyzeBps_thread(const std::string* url);
void analyzePps_thread(const std::string* url);

class LampadRestApiPractice {
    public:
        void run();

    private:
        bool readConfig(const std::string& fileName, std::string* url);
};