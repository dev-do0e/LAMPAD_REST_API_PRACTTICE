#pragma once
#include <string>
#include "../include/HttpClient.h"

class LampadApiClient {

    public :
        //url 서버에 로그인하여 Civet7Token 갖고와서 cookie.jar 파일로 저장하기
        bool doLogin(const std::string* url);

        //getFeeds()의 response가 저장되었으면 하는 데이터 타입에 맞추어서 사용.
        bool getFeeds(std::string* response, const std::string* url);
        struct Feed {
            std::string name;
            uint64_t from;
            uint64_t to;
        };
        bool getFeeds(std::vector<Feed>* response, const std::string* url);
        
        //getBps()의 response가 저장되었으면 하는 데이터 타입에 맞추어서 사용.
        bool getBps(std::string* response, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to);
        struct Bps {
            uint64_t timestamp;
            uint64_t value;
        };
        bool getBps(std::vector<Bps>* responseVec, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to);

        //getPps()의 response가 저장되었으면 하는 데이터 타입에 맞추어서 사용.
        bool getPps(std::string* response, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to);
        struct Pps {
            uint64_t timestamp;
            uint64_t unknown;
            uint64_t broadcast;
            uint64_t multicast;
            uint64_t unicast;
        };
        bool getPps(std::vector<Pps>* responseVec, const std::string* url, const std::string* feedName, const uint64_t* from, const uint64_t* to);
};