/* config.cpp */
#include "config.h"
using namespace cfg;
using namespace std;
using namespace logging;

using json = nlohmann::json;

Config* Config::_instance = 0;

Config* Config::Instance(const char *filename)
{
   if(_instance == 0) {
      _instance = new Config;
      if(!_instance->Init(filename)) {
         delete _instance;
         _instance = 0;
      }
   }
   return _instance;
}

bool Config::Init(const char *filename)
{
   ifstream ifile(filename);
   try {
      if(!ifile.is_open())
         throw std::strerror(errno);
      _data = json::parse(ifile);
      if(_data.empty())
         throw "Configuration empty";
      return true;
   }
   catch (const exception &ex) { logger.Msg(this, ERR, ex.what()); }
   catch (const char *msg)     { logger.Msg(this, ERR, msg); }
   return false;
}

const nlohmann::json& Config::GetJson() const
{
   return _data;
}
