/* cmd.cpp */
#include "cmd.h"
using namespace cmd;
using namespace std;
using namespace logging;

static const char *INPUT_PROMPT = "-> ";

void cmd::set_stdin_echo(bool enable) {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    if(!enable)
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;
    SetConsoleMode(hStdin, mode);
#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if(!enable)
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

string cmd::secure_input(const char *msg)
{
   std::string pass;
   std::cout << msg << ": ";
   cmd::set_stdin_echo(false);
   std::cin >> pass;
   cmd::set_stdin_echo(true);
   std::cin.clear();
   std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
   std::cout << '\n';
   return pass;
}

CommExpr::CommExpr() = default;

CommExpr::CommExpr(const std::string &src) : command{src}
{
   parser.Parse(command, root, adds, flags);
}

CommExpr::CommExpr(const char *src) : command{src}
{
   parser.Parse(command, root, adds, flags);
}

void CommExpr::Clear()
{
   root.clear();
   command.clear();
   flags.clear();
   adds.clear();
}

CommExpr& CommExpr::operator=(const std::string &src)
{
   Clear();
   command = src;
   parser.Parse(command, root, adds, flags);
   return *this;
}

CommExpr& CommExpr::operator=(const char *src)
{
   Clear();
   command = src;
   parser.Parse(command, root, adds, flags);
   return *this;
}

Flag CommExpr::GetFlag(const std::string &f) const
{
   for(const auto &x : flags)
      if(x.GetFlag() == f)
         return x;
   return Flag("");
}

bool CommParser::IsFlag(const string &str) const
{
   return regex_match(str, sflag) || regex_match(str, lflag);
}

void CommParser::Parse(const string &src, string &root,
                       commands_t &adds, flags_t &flags) const
{
   stringstream iss(src);
   string token;
   iss >> token;
   if(IsFlag(token))
      return;
   root = token;
   Distribute(iss, adds, flags);
}

void CommParser::Distribute(stringstream &iss, commands_t &adds,
                            flags_t &flags) const
{
   string token;
   while(iss >> token) {
      if(std::regex_match(token, sflag)) {
         ExtractShortFlags(token, flags);
      } else if(std::regex_match(token, lflag)) {
         flags.push_back(token.substr(2,token.size()-2));
      } else if(!flags.empty()) {
         flags.back().AddParam(token);
      } else {
         adds.push_back(token);
      }
   }
}

void CommParser::ExtractShortFlags(const string &token, flags_t &flags) const
{
   for(size_t i = 1; i < token.size(); i++)
      flags.push_back(token.substr(i,1));
}

void CommLine::Start()
{
   std::string in;
   while(printf("%s", INPUT_PROMPT) && getline(std::cin, in)) {
      if(in == "exit") break;
      FindPerformer(command = in);
      PrintPerformStatus();
   }
}

void CommLine::PrintPerformStatus() const
{
   if(result == OK)
      logger.Msg(this, INFO, "Successful");
   else if(result == FAIL)
      logger.Msg(this, ERR, "Fail");
   else
      logger.Msg(this, ERR, "Unknown");
}

ExecRes CommLine::FindPerformer(const CommExpr &c)
{
   if(!c.GetRoot().length())
      result = UNKNOWN;
   for(IPerformer *p : performers) {
      result = p->Execute(c);
      if(result != UNKNOWN)
         break;
   }
   return result;
}