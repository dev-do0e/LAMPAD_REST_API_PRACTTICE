#include "../include/LampadRestApiPractice.h"
#include "../include/LampadApiClient.h"
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

std::mutex coutMutex;

void LampadRestApiPractice::run() {

    //프로그램 처음 시작 시, config.json에서 프로그램 설정 갖고 오기
    std::string url;
    if(!LampadRestApiPractice::readConfig("./config.json", &url)){
        return;
    }

    LampadApiClient lampadApiClient;

    while(1){

        //1. LampadApiClient에 설정한 url에 로그인 진행.
        if(!lampadApiClient.doLogin(&url)){
            return;
        }

        //2. url에서 feed 정보 갖고 오기
        std::vector<LampadApiClient::Feed> feeds;
        if(!lampadApiClient.getFeeds(&feeds, &url)){
            return;
        }

        //3. feed 정보 활용해서
        //   각 feed의 최근 5분을 기준으로 아래 내용 구하기 (단, 0은 제외)
        //   BPS의 최대 값과 최대 값의 시간, 최저 값과 최저 값의 시간
        //   PPS unicast의 최대 값과 최대 값의 시간, 최저 값과 최저 값의 시간

        std::vector<std::thread> bpsThreads;
        std::vector<std::thread> ppsThreads;

        for(const auto& feed : feeds){

            const std::string feedName = feed.name;
            const uint64_t from = feed.to - 300;
            const uint64_t to = feed.to;

            //쓰래드로 돌리자 마지막!!!
            bpsThreads.emplace_back([url, feedName, from, to]() {

                LampadApiClient lampadApiClientForBps;
                std::vector<LampadApiClient::Bps> bpsData;
                if(!lampadApiClientForBps.getBps(&bpsData, &url, &feedName, &from, &to)){
                    return;
                }

                const LampadApiClient::Bps* minBps = nullptr;
                const LampadApiClient::Bps* maxBps = nullptr;
                for (const auto& bps : bpsData){
                    if (bps.value == 0) continue; // 0은 제외

                    if (!minBps || bps.value < minBps->value) {
                        minBps = &bps;
                    }
                    if (!maxBps || bps.value > maxBps->value) {
                        maxBps = &bps;
                    }
                }
                
                std::cout << std::endl << "피드 명 : " << feedName << std::endl;

                std::cout << "[최저 BPS] Value: " << minBps->value
                << ", Timestamp: " << minBps->timestamp << std::endl;

                std::cout << "[최대 BPS] Value: " << maxBps->value
                << ", Timestamp: " << maxBps->timestamp << std::endl;
            
            });

            //쓰래드로 돌리자 마지막!!!
            ppsThreads.emplace_back([url, feedName, from, to]() {

                LampadApiClient lampadApiClientForPps;
                std::vector<LampadApiClient::Pps> ppsData;
                if(!lampadApiClientForPps.getPps(&ppsData, &url, &feedName, &from, &to)){
                    return;
                }

                const LampadApiClient::Pps* minPps = nullptr;
                const LampadApiClient::Pps* maxPps = nullptr;
                for (const auto& pps : ppsData) {
                    if (pps.unicast == 0) continue; // 0은 제외

                    if (!minPps || pps.unicast < minPps->unicast) {
                        minPps = &pps;
                    }
                    if (!maxPps || pps.unicast > maxPps->unicast) {
                        maxPps = &pps;
                    }
                }
                
                std::cout << std::endl << "피드 명 : " << feedName << std::endl;

                std::cout << "[최저 Unicast] Value: " << minPps->unicast
                << ", Timestamp: " << minPps->timestamp << std::endl;

                std::cout << "[최대 Unicast] Value: " << maxPps->unicast
                << ", Timestamp: " << maxPps->timestamp << std::endl;

            });
        
        }

        // 동기화
        for (auto& t : bpsThreads) t.join();
        for (auto& t : ppsThreads) t.join();

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}


bool LampadRestApiPractice::readConfig(const std::string& fileName, std::string* url) {

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[ERROR] config.json 파일을 열 수 없습니다. " << fileName << std::endl;
        return false;
    }

    nlohmann::json j;
    file >> j;

    if (!j.contains("ServerUrl")) {
        std::cerr << "[ERROR] config.json에 'ServerUrl' 키가 없습니다." << std::endl;
        return false;
    }

    *url = j["ServerUrl"].get<std::string>();

    return true;
}

