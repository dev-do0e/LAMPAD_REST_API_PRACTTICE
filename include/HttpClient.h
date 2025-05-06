#pragma once

#include <string>
#include <vector>

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    std::string post(const std::string& url,
                     const std::string& body,
                     const std::vector<std::string>& headersVec);

    std::string get(const std::string& url);

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
};
