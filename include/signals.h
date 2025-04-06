/* signals.h */
#pragma once
#include <signal.h>
#include <cerrno>
#include "log.h"

namespace signals {

using HandlerType = void(*)(int);

class SignalHandler : public logging::ILog {
   static volatile sig_atomic_t _shouldStop;
   static void SigIntDefaultHandler(int sig);
public:
   const char* GetObjName() const override;
   bool NeedStop() const;
   bool SetDispose(int sig, HandlerType handler = nullptr);
   const SignalHandler* GetInstance() const { return this; }
};

}