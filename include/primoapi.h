/* primoapi.h */
#pragma once
#include <string>
#include "log.h"
#include "config.h"
#include "json.h"
#include "restclient.h"

namespace primo {

class PrimoApi : public logging::ILog {
   std::string _url, _token;
   restapi::RestClient _rc;
   void AppendAuthHeader();
public:
   PrimoApi(const char *url);
   bool Authorize(const std::string &username, const std::string &password);
   bool StopAssignment(long id);
   bool StartAssignment(long id);
   bool GetAssignments(std::string &result);
   const char* GetObjName() const override;
};

}