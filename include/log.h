/* log.h */
#pragma once
#include <cstdio>
#include <cstdarg>

namespace logging {

// Defines interface for classes that want to use logger
class ILog {
public:
  virtual const char* GetObjName() const = 0; // Returns class name
  virtual ~ILog() = default;
};

enum LogType {
  INFO,
  WARN,
  ERR,
  DEB
};

class Logger : public ILog {
public:
  // Print log (usual and format versions)
  void Msg(const ILog* obj, LogType type, const char *msg);
  void FmtMsg(const ILog*, LogType, const char*, ...);
private:
  static unsigned msg_cnt; // Logs counter
  const char* GetObjName() const override { return "Logger"; }
};

extern Logger logger;

}
