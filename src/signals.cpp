/* signals.h */
#include "signals.h"
using namespace signals;

volatile sig_atomic_t SignalHandler::_shouldStop = 0;

void SignalHandler::SigIntDefaultHandler(int sig)
{
   signal(SIGINT, SIG_DFL);
   int save_errno = errno;
   fflush(stdout);
   fflush(stderr);
   logging::logger.Msg(&(logging::logger), logging::INFO, "Interrupt");
   _shouldStop = 1;
   errno = save_errno;
}

bool SignalHandler::NeedStop() const
{
   return static_cast<bool>(_shouldStop);
}

bool SignalHandler::SetDispose(int sig, HandlerType handler)
{
   if(handler != nullptr) {
      return signal(sig, handler) != SIG_ERR;
   } else {
      switch(sig) {
         case SIGINT:
            return signal(sig, SigIntDefaultHandler) != SIG_ERR;
            break;
         default:
            return false;
      }
   }
}

const char* SignalHandler::GetObjName() const
{
   return "SignalHandler";
}