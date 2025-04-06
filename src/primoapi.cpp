/* primo_restapi.cpp */
#include "primoapi.h"
using namespace primo;
using namespace std::string_literals;
using json = nlohmann::json;

PrimoApi::PrimoApi(const char *url) : _url{url} { }

bool PrimoApi::Authorize(const std::string &username,
                             const std::string &password)
{
   _token.clear();

   json js;
   js["username"] = username;
   js["password"] = password;

   _rc.ClearHeaders();
   _rc.AppendHeader("Content-Type: application/json");
   js = json::parse(_rc.Post((_url+"Account").c_str(), js.dump().c_str()));

   if(_rc.Success()) {
      _token = js["token"];
      return true;
   }
   return false;
}

void PrimoApi::AppendAuthHeader()
{
   _rc.AppendHeader(("Authorization: Bearer "s + _token).c_str());
}

bool PrimoApi::StopAssignment(long id)
{
   _rc.ClearHeaders();
   AppendAuthHeader();
   std::string url = _url + "Assignments/"s + std::to_string(id) + "/Stop"s;
   _rc.Put(url.c_str());
   return _rc.Success();
}

bool PrimoApi::StartAssignment(long id)
{
   _rc.ClearHeaders();
   AppendAuthHeader();
   std::string url = _url + "Assignments/"s + std::to_string(id) + "/Start"s;
   _rc.Put(url.c_str());
   return _rc.Success();
}

bool PrimoApi::GetAssignments(std::string &result)
{
   _rc.ClearHeaders();
   _rc.AppendHeader("Content-Type: application/json");
   AppendAuthHeader();
   result = _rc.Get((_url + "Assignments/v2").c_str());
   return _rc.Success();
}

const char* PrimoApi::GetObjName() const
{
   return "PrimoApi";
}