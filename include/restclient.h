/* restclient.h */
#pragma once
#include <string>
#include <exception>
#include <stdexcept>
#include "log.h"

#ifdef _WIN32
#include "../curl/curl.h"
#elif __linux__
#include <curl/curl.h>
#endif

namespace restapi {

class RestClient : public logging::ILog {
   std::string _responseBody,
               _responseHeader;
   long _responseCode = -1;
   CURLcode _res;
   CURL *_curl = nullptr;
   curl_slist *_headers = nullptr;
   void CleanUP();
   static size_t WriteResponse(void *contents, size_t size,
                               size_t nmemb, std::string *s);
   void SetupRequest(const char* url, const char* body, const char* method);
   void PerformRequest();
public:
   RestClient();
   ~RestClient();
   void AppendHeader(const char *h);
   void ClearHeaders();
   const std::string& Get(const char *url);
   const std::string& Post(const char *url, const char *body = nullptr);
   const std::string& Put(const char *url, const char *body = nullptr);
   const std::string& GetResponseBody() const;
   const std::string& GetResponseHeader() const;
   long GetResponseCode() const;
   bool Success() const;
   const char* GetObjName() const override;
};

}
