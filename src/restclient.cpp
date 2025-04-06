/* restclient.cpp */
#include "restclient.h"
using namespace restapi;

RestClient::RestClient()
{
   curl_global_init(CURL_GLOBAL_DEFAULT);
   _curl = curl_easy_init();
   if(!_curl)
      throw std::runtime_error("Failed to initialize CURL");
}

RestClient::~RestClient()
{
   ClearHeaders();
   CleanUP();
   curl_global_cleanup();
}

void RestClient::CleanUP()
{
   if(_curl) {
      curl_easy_cleanup(_curl);
      _curl = nullptr;
   }
   _responseHeader.clear();
   _responseBody.clear();
   _responseCode = -1;
}

void RestClient::AppendHeader(const char *h)
{
   _headers = curl_slist_append(_headers, h);
}

void RestClient::ClearHeaders()
{
   curl_slist_free_all(_headers);
   _headers = nullptr;
}

void RestClient::SetupRequest(const char* url, const char* body, const char* method)
{
   CleanUP();
   _curl = curl_easy_init();
   if(!_curl)
      throw std::runtime_error("Failed to initialize CURL");

   curl_easy_setopt(_curl, CURLOPT_URL, url);
   curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
   curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, RestClient::WriteResponse);
   curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, RestClient::WriteResponse);
   curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_responseBody);
   curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, &_responseHeader);
   curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 2L);
   curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 2L);

   if(method)
      curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, method);

   if(body)
      curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, body);
}

void RestClient::PerformRequest()
{
   _res = curl_easy_perform(_curl);
   curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_responseCode);

   if(_res != CURLE_OK)
      throw std::runtime_error(curl_easy_strerror(_res));
}

const std::string& RestClient::Post(const char *url, const char *body)
{
   SetupRequest(url, body, "POST");
   PerformRequest();
   return GetResponseBody();
}

const std::string& RestClient::Put(const char *url, const char *body)
{
   SetupRequest(url, body, "PUT");
   PerformRequest();
   return GetResponseBody();
}

const std::string& RestClient::Get(const char *url)
{
   SetupRequest(url, nullptr, nullptr);
   PerformRequest();
   return GetResponseBody();
}

const std::string& RestClient::GetResponseBody() const
{
   return _responseBody;
}

const std::string& RestClient::GetResponseHeader() const
{
   return _responseHeader;
}

size_t RestClient::WriteResponse(void *contents, size_t size,
                                 size_t nmemb, std::string *s)
{
   if(!s)
      return 0;
   size_t length = size * nmemb;
   s->append(static_cast<char*>(contents), length);
   return length;
}

const char* RestClient::GetObjName() const
{
   return "RestClient";
}

long RestClient::GetResponseCode() const
{
   return _responseCode;
}

bool RestClient::Success() const
{
   return _responseCode == 200;
}