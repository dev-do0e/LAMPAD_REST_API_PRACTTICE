#include "../include/LampadRestApiPractice.h"
#include "../include/LampadApiClient.h"
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>

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

        //3. std::thread()로 BPS 분석
        std::thread bpsThread(analyzeBps_thread, &feeds, &url);

        //4. std::thread()로 PPS 분석
        std::thread ppsThread(analyzePps_thread, &feeds, &url);

        bpsThread.join();
        ppsThread.join();
    
        std::cout << std::endl << "bps, pps unicast 분석 끝" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

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

void analyzeBps_thread(const std::vector<LampadApiClient::Feed>* feeds, const std::string* url){

    for(const auto& feed : *feeds){

        const std::string feedName = feed.name;
        const uint64_t from = feed.to - 300;
        const uint64_t to = feed.to;

        LampadApiClient lampadApiClientForBps;
        std::vector<LampadApiClient::Bps> bpsData;

        if(!lampadApiClientForBps.getBps(&bpsData, url, &feedName, &from, &to)){
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

    }
    
}

void analyzePps_thread(const std::vector<LampadApiClient::Feed>* feeds, const std::string* url){


    for(const auto& feed : *feeds){

        const std::string feedName = feed.name;
        const uint64_t from = feed.to - 300;
        const uint64_t to = feed.to;

        LampadApiClient lampadApiClientForPps;
        std::vector<LampadApiClient::Pps> ppsData;

        if(!lampadApiClientForPps.getPps(&ppsData, url, &feedName, &from, &to)){
            return;
        }

        const LampadApiClient::Pps* minPps = nullptr;
        const LampadApiClient::Pps* maxPps = nullptr;
        for (const auto& pps : ppsData){
            if (pps.unicast == 0) continue; // 0은 제외

            if (!minPps || pps.unicast < minPps->unicast) {
                minPps = &pps;
            }
            if (!maxPps || pps.unicast > maxPps->unicast) {
                maxPps = &pps;
            }
        }
                    
        std::cout << std::endl << "피드 명 : " << feedName << std::endl;

        std::cout << "[최저 PPS Unicast] Value: " << minPps->unicast
        << ", Timestamp: " << minPps->timestamp << std::endl;

        std::cout << "[최대 PPS Unicast] Value: " << maxPps->unicast
        << ", Timestamp: " << maxPps->timestamp << std::endl;
    }

}