#pragma once

#include <string>
#include <vector>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    std::string post(const std::string& url,
                     const std::string& header,
                     const std::string& body);

    std::string get(const std::string& url);

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
