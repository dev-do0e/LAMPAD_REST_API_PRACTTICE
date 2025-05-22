#include <iostream>
#include <thread>
#include <chrono>

#include "../include/LampadApiClient.h"
#include "../include/HttpClient.h"
#include <nlohmann/json.hpp>

bool LampadApiClient::doLogin(const std::string* url) {

    //1. 필요한 정보 저장
    std::string apiPath = *url + "/q/login";
    std::string header = "Content-Type: application/json";
    std::string body = "{\"username\":\"lampad\",\"password\":\"1q2w3e!Q@W#E\"}";

    //2. REST API 요청
    HttpClient client;
    std::string response = client.post(apiPath, header, body);
    if(response.empty()){
        std::cout << "[Error] url : " << url << "서버에 login 실패" << std::endl;
        return false;
    }
 
    //LAMPAD 로그인하여 cookie.jar 파일 저장하기 성공
    std::cout << "URL : " << *url << " 에 로그인 성공!!" << std::endl;

    return true;
}

bool LampadApiClient::getFeeds(std::string* response, const std::string* url) {

    //1. 필요한 정보 저장
    std::string apiPath = *url + "/q/feed";

    //2. REST API 요청
    HttpClient client;
    *response = client.get(apiPath);
    if(response->empty()){
        std::cout << "[Error] url : " << *url << "서버에 login 실패" << std::endl;
        return false;
    }

    //3. 피드 정보 갖고오기 성공
    return true;
}

bool LampadApiClient::getFeeds(std::vector<Feed>* responseVec, const std::string* url) {

    //1. 필요한 정보 저장
    std::string apiPath = *url + "/q/feed";

    //2. REST API 요청
    HttpClient client;
    std::string response = client.get(apiPath);
    if(response.empty()){
        std::cout << "[Error] url : " << *url << "서버에 login 실패" << std::endl;
        return false;
    }

    try {
        nlohmann::json feeds = nlohmann::json::parse(response);

        for (const auto& record : feeds) {
            Feed feed;
            feed.name = record.at("name").get<std::string>();
            feed.from = record.at("from").get<uint64_t>();
            feed.to   = record.at("to").get<uint64_t>();
            responseVec->push_back(feed);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] 피드 JSON 파싱 실패: " << e.what() << std::endl;
        return false;
    }

    //3. 피드 정보 갖고오기 성공
    return true;
}

bool LampadApiClient::getBps(std::string* response, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to) {

    //1. jobid 요청에 필요한 준비물.
    std::string apiPath = *url + "/q/feed/" + *feedName + "/refinery";
    std::string header = "Content-Type: application/json";
    std::string body = "{\"from\":\"" + std::to_string(*from) + "\",\"to\":\"" + std::to_string(*to) + "\",\"type\":\"bps\"}";

    //2. jobid가 포함되어 reponse를 받아오기 위한 REST API 요청
    HttpClient client;
    *response = client.post(apiPath, header, body);
    std::cout << apiPath << std::endl;
    if(response->empty()){
        std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
        return false;
    }

    //3. reponse에서 jobid 따로 저장
    std::string jobid;
    size_t pos = response->find('\n');
    if (pos != std::string::npos && pos + 1 < response->size()) {
        jobid = response->substr(pos + 1);
    }

    //4. jobid를 활용하여 진짜 bps 데이터를 받아오기 위한 REST API 요청
    std::string statusCode;
    bool succesed = true;
    do{
        //reponse가 202라면 즉, 아직 서버에서 데이터를 다 만들지 못했다면 3초 뒤에 다시 요청
        if(!succesed) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        succesed = false;

        apiPath = apiPath + "/" + jobid;
        *response = client.get(apiPath);

        if (response->empty()) {
             std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
            return false;
        }

        //reponse가 202인지 확인하기 위해 statusCodedp 저장해서 확인해보기
        size_t beforeStatusCodeLength = response->find("Error ") + 6;
        size_t afterStatusCodeLength = response->find(':', beforeStatusCodeLength);
        statusCode = response->substr(beforeStatusCodeLength, afterStatusCodeLength - beforeStatusCodeLength);

    }while(statusCode == "202");

    //5. 성공
    return true;

}

bool LampadApiClient::getBps(std::vector<Bps>* responseVec, const std::string* url, const std::string* feedName, const  uint64_t* from, const uint64_t* to) {

    //1. jobid 요청에 필요한 준비물.
    std::string apiPath = *url + "/q/feed/" + *feedName + "/refinery";
    std::string header = "Content-Type: application/json";
    std::string body = "{\"from\":\"" + std::to_string(*from) + "\",\"to\":\"" + std::to_string(*to) + "\",\"type\":\"bps\"}";

    //2. jobid가 포함되어 reponse를 받아오기 위한 REST API 요청
    HttpClient client;
    std::string response = client.post(apiPath, header, body);
    if(response.empty()){
        std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
        return false;
    }

    //3. reponse에서 jobid 따로 저장
    std::string jobid;
    size_t pos = response.find('\n');
    if (pos != std::string::npos && pos + 1 < response.size()) {
        jobid = response.substr(pos + 1);
    }

    //4. jobid를 활용하여 진짜 bps 데이터를 받아오기 위한 REST API 요청
    std::string statusCode;
    bool succesed = true;
    do{
        //reponse가 202라면 즉, 아직 서버에서 데이터를 다 만들지 못했다면 3초 뒤에 다시 요청
        if(!succesed) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        succesed = false;

        apiPath = apiPath + "/" + jobid;
        response = client.get(apiPath);

        if (response.empty()) {
             std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
            return false;
        }

        //reponse가 202인지 확인하기 위해 statusCodedp 저장해서 확인해보기
        size_t beforeStatusCodeLength = response.find("Error ") + 6;
        size_t afterStatusCodeLength = response.find(':', beforeStatusCodeLength);
        statusCode = response.substr(beforeStatusCodeLength, afterStatusCodeLength - beforeStatusCodeLength);

    }while(statusCode == "202");

    //5. response "std::string"을 responseVec "std::vector"에 넣기
    try {
        nlohmann::json bpsData = nlohmann::json::parse(response);

        for (const auto& record : bpsData) {
            Bps bps;
            bps.timestamp = record.at("timestamp").get<uint64_t>();
            bps.value = record.at("value").get<uint64_t>();
            responseVec->push_back(bps);
        }

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON 파싱 실패: " << e.what() << std::endl;
        std::cerr << "[ERROR] 서버 응답이 JSON이 아닙니다. 내용: " << response.substr(0, 100) << std::endl;
        return false;
    }

    //6. 성공
    return true;

}

bool LampadApiClient::getPps(std::string* response, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to) {

    //1. jobid 요청에 필요한 준비물.
    std::string apiPath = *url + "/q/feed/" + *feedName + "/refinery";
    std::string header = "Content-Type: application/json";
    std::string body = "{\"from\":\"" + std::to_string(*from) + "\",\"to\":\"" + std::to_string(*to) + "\",\"type\":\"pps\"}";

    //2. jobid가 포함되어 reponse를 받아오기 위한 REST API 요청
    HttpClient client;
    *response = client.post(apiPath, header, body);
    if(response->empty()){
        std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
        return false;
    }

    //3. reponse에서 jobid 따로 저장
    std::string jobid;
    size_t pos = response->find('\n');
    if (pos != std::string::npos && pos + 1 < response->size()) {
        jobid = response->substr(pos + 1);
    }

    //4. jobid를 활용하여 진짜 bps 데이터를 받아오기 위한 REST API 요청
    std::string statusCode;
    bool succesed = true;
    do{
        //reponse가 202라면 즉, 아직 서버에서 데이터를 다 만들지 못했다면 3초 뒤에 다시 요청
        if(!succesed) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        succesed = false;

        apiPath = apiPath + "/" + jobid;
        *response = client.get(apiPath);

        if (response->empty()) {
             std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
            return false;
        }

        //reponse가 202인지 확인하기 위해 statusCodedp 저장해서 확인해보기
        size_t beforeStatusCodeLength = response->find("Error ") + 6;
        size_t afterStatusCodeLength = response->find(':', beforeStatusCodeLength);
        statusCode = response->substr(beforeStatusCodeLength, afterStatusCodeLength - beforeStatusCodeLength);

    }while(statusCode == "202");

    //5. 성공
    return true;

}

bool LampadApiClient::getPps(std::vector<Pps>* responseVec, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to) {

    //1. jobid 요청에 필요한 준비물.
    std::string apiPath = *url + "/q/feed/" + *feedName + "/refinery";
    std::string header = "Content-Type: application/json";
    std::string body = "{\"from\":\"" + std::to_string(*from) + "\",\"to\":\"" + std::to_string(*to) + "\",\"type\":\"pps\"}";

    //2. jobid가 포함되어 reponse를 받아오기 위한 REST API 요청
    HttpClient client;
    std::string response = client.post(apiPath, header, body);
    if(response.empty()){
        std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
        return false;
    }

    //3. reponse에서 jobid 따로 저장
    std::string jobid;
    size_t pos = response.find('\n');
    if (pos != std::string::npos && pos + 1 < response.size()) {
        jobid = response.substr(pos + 1);
    }

    //4. jobid를 활용하여 진짜 bps 데이터를 받아오기 위한 REST API 요청
    std::string statusCode;
    bool succesed = true;
    do{
        //reponse가 202라면 즉, 아직 서버에서 데이터를 다 만들지 못했다면 3초 뒤에 다시 요청
        if(!succesed) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        succesed = false;

        apiPath = apiPath + "/" + jobid;
        response = client.get(apiPath);

        if (response.empty()) {
             std::cout << "[Error] url : " << *url << " 서버와의 연결 상태가 이상합니다." << std::endl;
            return false;
        }

        //reponse가 202인지 확인하기 위해 statusCodedp 저장해서 확인해보기
        size_t beforeStatusCodeLength = response.find("Error ") + 6;
        size_t afterStatusCodeLength = response.find(':', beforeStatusCodeLength);
        statusCode = response.substr(beforeStatusCodeLength, afterStatusCodeLength - beforeStatusCodeLength);

    }while(statusCode == "202");

    //5. response "std::string"을 responseVec "std::vector"에 넣기
    try {
        nlohmann::json ppsData = nlohmann::json::parse(response);

        for (const auto& record : ppsData) {
            Pps pps;
            pps.timestamp = record.at("timestamp").get<uint64_t>();
            pps.unknown = record.at("unknown").get<uint64_t>();
            pps.broadcast = record.at("broadcast").get<uint64_t>();
            pps.multicast = record.at("multicast").get<uint64_t>();
            pps.unicast = record.at("unicast").get<uint64_t>();
            responseVec->push_back(pps);
        }
    
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] JSON 파싱 실패: " << e.what() << std::endl;
        std::cerr << "[ERROR] 서버 응답이 JSON이 아닙니다. 내용: " << response.substr(0, 100) << std::endl;
        return false;
    }

    //6. 성공
    return true;

}