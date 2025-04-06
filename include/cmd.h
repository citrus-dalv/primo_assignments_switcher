/* cmd.h */
#pragma once

#include <iostream>
#include <regex>
#include <sstream>
#include <vector>
#include <map>
#include <limits>

#ifdef _WIN32
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

#ifdef max
#undef max
#endif

#ifdef ERROR
#undef ERROR
#endif

#include <list>
#include "log.h"

namespace cmd {

using params_t = std::vector<std::string>;

void set_stdin_echo(bool enable = true);
std::string secure_input(const char *msg);


// NOTICE:
// Following classes not in use right now.
// Planned for future functionality in interactive mode

class Flag {
   std::string flag;
   params_t params;
public:
   Flag(const std::string &f) : flag{f} {}
   void AddParam(const std::string &p) { params.push_back(p); }
   const params_t& GetParams() const { return params; }
   const std::string& GetFlag() const { return flag; }
   friend std::ostream& operator<<(std::ostream &out, const Flag &f)
      { return out << f.flag; }
};

using flags_t = std::vector<Flag>;
using commands_t = std::vector<std::string>;

class CommParser {
   std::regex sflag{"^-[a-zA-Z]+"}, lflag{"^--[a-zA-Z]\\w+"};
   void ExtractShortFlags(const std::string &token, flags_t &flags) const;
   void Distribute(std::stringstream &iss, commands_t &adds,
                   flags_t &flags) const;
public:
   bool IsFlag(const std::string &str) const;
   void Parse(const std::string &src, std::string &com,
              commands_t &adds, flags_t &flags) const;
};

class CommExpr {
private:
   std::string command;
   std::string root;
   commands_t adds;
   flags_t flags;
   CommParser parser;
   void Clear();
public:
   CommExpr();
   CommExpr(const std::string &src);
   CommExpr(const char *src);
   CommExpr& operator=(const std::string &src);
   CommExpr& operator=(const char *src);
   Flag GetFlag(const std::string &f) const;
   const flags_t &GetFlags() const { return flags; }
   const commands_t& GetAdds() const { return adds; }
   const std::string& GetRoot() const { return root; }
};

enum ExecRes {
   OK, FAIL, UNKNOWN
};

class IPerformer {
public:
   virtual ~IPerformer() = default;
   virtual ExecRes Execute(const CommExpr &c) = 0;
};

template <typename T>
class CommKeeper {
   using commands_t = std::map<std::string,ExecRes(T::*)(const CommExpr&)>;
   commands_t commands;
public:
   CommKeeper(const commands_t &com) : commands{com} {}
   ExecRes Call(T &obj, const CommExpr &c)
   {
      auto found = commands.find(c.GetRoot());
      if (found == commands.end())
         return UNKNOWN;
      ExecRes (T::*func)(const CommExpr&) = commands[c.GetRoot()];
      return (obj.*func)(c);
   }
};

class CommLine : public logging::ILog {
   CommExpr command;
   std::list<IPerformer*> performers;
   ExecRes FindPerformer(const CommExpr &c);
   ExecRes result;
public:
   ~CommLine() { for (auto &x : performers) delete x; };
   void Start();
   void PrintPerformStatus() const;
   void AddPerformer(IPerformer *p) { performers.push_back(p); }
   const char* GetObjName() const override { return "CommLine"; }
};

}
