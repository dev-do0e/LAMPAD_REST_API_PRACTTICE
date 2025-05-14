#include "../include/Manager.h"
#include "../include/HttpClient.h"
#include <nlohmann/json.hpp>
#include <Windows.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <vector>
#include <chrono>

void Manager::run() {

    //0. config.json 파일이 없거나, url, feedInformationPath 키가 없으면 프로그램 종료
    std::string url;
    std::string feedsInformationPath;
    if(!readConfig("./config.json", &url, &feedsInformationPath)) return;

    //1. LAMPAD 로그인 성공하여 cookie.jar 파일 및 Feeds 정보 저장
    if(!fetchCivet7Token(&url, &feedsInformationPath)) return;

    //2. 각각의 Feed의 BPS 5분 데이터를 가져와 저장
    if(!fetchBps(&url, &feedsInformationPath)) return;

}

bool fetchCivet7Token(const std::string* url, const std::string* feedsInformationPath) {

    //1-1. Login REST API에 필요한 정보 저장
    std::string apiPath = *url + "/q/login";
    std::string body = "{"
                      "\"username\" : \"lampad\", "
                      "\"password\" : \"1q2w3e!Q@W#E\""
                      "}";
    std::string header = "Content-Type: application/json";
        
    //1-2. login REST API 요청
    HttpClient client;
    std::string response = client.post(apiPath, body, header);
    if(response.empty()){
        std::cout << "[Error] url : " << *url << "서버에 login 실패" << std::endl;
        return false;
    }
 
    //LAMPAD 로그인하여 cookie.jar 파일 가져오기 성공
    std::cout << *url << "에 로그인 성공" << std::endl;

    //1-3. 성공하여 response 저장
    if(!writeResponse(*feedsInformationPath, &response)) return false;

    return true;
}

bool fetchBps(const std::string* url, const std::string* feedsInformationPath) {

    //2-1. *feedsInformationPath 파일이 생성되어 잘 있는 지 확인
    std::ifstream file(*feedsInformationPath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] " << *feedsInformationPath << " 파일을 열 수 없습니다." << std::endl;
        return false;
    }

    nlohmann::json j;
    file >> j;
    
    //std::vector<std::thread> threads;
    //std::atomic<bool> hasError = false;

    //2-2. *feedsInformationPath 파일에 저장되어 있는 피드들의 정보를 활용하여 각각의 최근 5분 BPS를 파일로 저장
    for(const auto& feed : j){

        //병렬처리
        //threads.emplace_back([&, feed]() {

            //2-2-1. refinery REST API에 필요한 정보 저장
            std::string header = "Content-Type: application/json";
            std::string apiPath = *url + "/q/feed/" + feed["name"].get<std::string>() + "/refinery";
            std::string body = "{"
                        "\"from\" : \"" + std::to_string((feed["to"].get<long long>() - 300)) + "\", "
                        "\"to\" : \"" + std::to_string(feed["to"].get<long long>()) + "\", "
                        "\"type\" : \"bps\""
                        "}";

            //2-2-2. refinery REST API 요청해서 jobid 받아오기
            HttpClient client;
            std::string response = client.post(apiPath, body, header);
            if (response.empty()) {
                std::cerr << "[Error] url : " << *url << " 서버에 연결이 이상함" << std::endl;
                //hasError = true;
                return false;
            }
            size_t beforeJobidLength = response.find('\n');
            std::string jobid = response.substr(beforeJobidLength + 1);

            //2-2-3. jobid로 BPS로 데이터를 요청하여 갖고 옴.
            //          하지만 서버에서 데이터 가공이 완료되지 않아서 http status code 202를 return하면 5초 뒤에 데이터 재요청.
            std::string StatusCode;
            int i = 0;
            do{
                if(i > 0) {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
                i++;

                apiPath = *url + "/q/feed/" + feed["name"].get<std::string>() + "/refinery/" + jobid;
                response = client.get(apiPath);

                if (response.empty()) {
                    std::cerr << "[Error] url : " << *url << " 서버에 연결이 이상함" << std::endl;
                    //hasError = true;
                    return false;
                }

                size_t beforeStatusCodeLength = response.find("Error ") + 6;
                size_t afterStatusCodeLength = response.find(':', beforeStatusCodeLength);

                StatusCode = response.substr(beforeStatusCodeLength, afterStatusCodeLength - beforeStatusCodeLength);
            }while(StatusCode == "202");

            //2-2-4. BPS 데이터를 요청하여 갖고 왔으면 bpsResult/ 경로에 feed 이름으로 데이터 저장
            if(!writeResponse("bpsResult/" + feed["name"].get<std::string>() + ".json", &response)){
                //hasError = true;
                return false;
            }

        //});
    }
    
    return true;

    //병렬처리한 쓰레드가 모두 끝날 때 까지 기다리기
    //for (auto& t : threads) {
    //    if (t.joinable()) t.join();
    //}

    //return !hasError;
}

bool readConfig(const std::string& fileName, std::string* url, std::string* feedsInformationPath) {

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
    } else if (!j.contains("FeedsInformationPath")) {
        std::cerr << "[ERROR] config.json에 'FeedsInformationPath' 키가 없습니다." << std::endl;
        return false;
    }

    *url = j["ServerUrl"].get<std::string>();
    *feedsInformationPath = j["FeedsInformationPath"].get<std::string>();

    return true;
}

bool readBody(const std::string& fileName, std::string* body) {

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[ERROR] " << fileName << " 파일을 열 수 없습니다." << std::endl;
        return false;
    }

    *body = std::string(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    std::cout << *body << std::endl;

    return true;
}

// response.json 저장 함수
bool writeResponse(const std::string& fileName, const std::string* response) {

    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[ERROR] " << fileName << " 파일을 열 수 없습니다." << std::endl;
        return false;
    }

    try {
        nlohmann::json parsed = nlohmann::json::parse(*response);
        file << parsed.dump(4) << std::endl;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "[ERROR] Response는 JSON 포맷이 아닙니다. 파일: " << fileName << " 에 원본 저장함" << std::endl;
        file << *response;
    }

    file.close();

    return true;
}


