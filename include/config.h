/* config.h */
#pragma once
#include <string>
#include <fstream>
#include <cerrno>
#include "json.h"
#include "log.h"

static constexpr const char *DEFAULT_CONFIG_FILE_PATH = "config.json";

namespace cfg {

class Config : public logging::ILog {
   Config() {}
   static Config *_instance;
   nlohmann::json _data;
   bool Init(const char *filename);
public:
   const char* GetObjName() const override { return "Config"; }
   static Config* Instance(const char *filename = DEFAULT_CONFIG_FILE_PATH);
   const nlohmann::json& GetJson() const;

   template<typename T>
   T GetParameter(const char *name)
   {
      return _data[name].get<T>();
   }

   template<typename T>
   bool GetParameter(const char *name, T &result)
   {
      try {
         result = _data[name].get<T>();
      }
      catch (nlohmann::json::type_error &e) {
         logging::logger.FmtMsg(this, logging::ERR, "parameter: %s. %s",
                                name, e.what());
         return false;
      }
      return true;
   }
};

}