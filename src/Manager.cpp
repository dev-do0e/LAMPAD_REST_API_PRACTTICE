#include "../include/Manager.h"
#include "../include/HttpClient.h"
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

void Manager::run(int argc, char* argv[]) {
    //프로그램 실행 시 인자값이 3개 미만이면 실행 안됨.
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << "  POST <endpoint>\n"
                  << "  GET <endpoint>\n";
        return;
    }

    std::string method = argv[1];
    std::string endpoint = argv[2];

    std::string url;
    //config.json 파일이 없거나, url의 키 값이 없으면 프로그램 종료
    if(!readConfig("./config.json", &url)) return;
    std::string apiPath = url + endpoint;

    //REST API 요청 주체 선언
    HttpClient client;
    std::string response;
    bool success = false;

    //Request 진행
    if (method == "GET") {
        response = client.get(apiPath);
        success = !response.empty();

    } else if (method == "POST") {
        std::string body;
        //body.json 파일이 없으면 프로그램 종료
        if(!readBody("./body.json", &body)) return;
        std::vector<std::string> headers = {
            "Content-Type: application/json"
        };
    
        response = client.post(apiPath, body, headers);
        success = !response.empty();
    
    } else {
        std::cerr << "[ERROR] 지원하지 않는 method : " << method << std::endl;
        return;
    }

    if (success) {
        std::cout << "[RESULT] 요청 성공" << std::endl;
        // response.json 저장 함수
        writeResponse("response.json", response);
    } else {
        std::cout << "[RESULT] 요청 실패" << std::endl;
    }
}

bool readConfig(const std::string& fileName, std::string* url) {
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

    return true;
}

// response.json 저장 함수
void writeResponse(const std::string& fileName, const std::string& content) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[ERROR] " << fileName << " 파일을 열 수 없습니다." << std::endl;
        return;
    }

    try {
        nlohmann::json parsed = nlohmann::json::parse(content);
        file << parsed.dump(4) << std::endl;
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "[ERROR] Response는 JSON 포맷이 아닙니다. 원본 그대로 저장합니다." << std::endl;
        file << content;
    }

    file.close();
}


