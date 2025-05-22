#include "../include/HttpClient.h"

#include <curl/curl.h>
#include <iostream>
#include <mutex>

static std::mutex curlCookieMutex; // 전역 mutex

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_ALL);
}

HttpClient::~HttpClient() {
    curl_global_cleanup();
}

size_t HttpClient::writeCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    auto* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<const char*>(contents), size * nmemb);
    return size * nmemb;
}

std::string HttpClient::get(const std::string& url) {
    std::lock_guard<std::mutex> lock(curlCookieMutex); // 동기화

    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        std::cerr << "[HttpClient] curl_easy_init() failed" << std::endl;
        return "";
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.jar");
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.jar");

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "[HttpClient] GET request failed: "
                  << curl_easy_strerror(res) << std::endl;
    }

    curl_easy_cleanup(curl);

    return response;
}

std::string HttpClient::post(const std::string& url,
                             const std::string& header,
                             const std::string& body) {
    std::lock_guard<std::mutex> lock(curlCookieMutex); // 동기화

    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        std::cerr << "[HttpClient] curl_easy_init() failed" << std::endl;
        return "";
    }

    struct curl_slist* slist = curl_slist_append(nullptr, header.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.jar"); // 요청 시 쿠키 읽어옴
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookie.jar");  // 응답 받은 쿠키 저장

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "[HttpClient] POST request failed: "
                  << curl_easy_strerror(res) << std::endl;
    }

    curl_slist_free_all(slist);
    curl_easy_cleanup(curl);

    return response;
}
