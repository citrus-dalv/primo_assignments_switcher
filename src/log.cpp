/* log.cpp */
#include "log.h"

using namespace logging;

// Function realization based on ...
void Logger::FmtMsg(const ILog *obj, LogType type, const char *fmt, ...)
{
#ifdef LOG
  va_list vl;
  va_start(vl, fmt);
  const char *head = nullptr;
  FILE *stream = stdout;

  switch (type) {
    case INFO:    head = "INFO   "; break;
    case WARN:    head = "WARNING"; break;
    case ERR:     head = "ERROR  "; stream = stderr; break;
    case DEB:     head = "DEBUG  "; break;
    default:
    //              throw "Log message type not recognized";
                  break;
  }
#ifndef DEBUGGING
  if(type == DEB) {
    va_end(vl);
    return;
  }
#endif
  fprintf(stream, "[%.5u] [ %s ] [%-9.9s]: ", ++msg_cnt, head, obj->GetObjName());
  vfprintf(stream, fmt, vl);
  fprintf(stream, "\n");
  va_end(vl);
#endif
}

void Logger::Msg(const ILog *obj, LogType type, const char *msg)
{
   FmtMsg(obj, type, "%s", msg);
}

logging::Logger logging::logger;

unsigned Logger::msg_cnt = 0;
