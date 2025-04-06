// Primo Assignments Switcher
// This program manages via Primo API assignments with enable/disable functionality

// Copyright (C) 2024, Yusov Vladislav, yusovvladislav@gmail.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "main.h"

// Command line arguments flags
constexpr static const char *stop_flag = "--stop";   // Flag to disable assignments
constexpr static const char *start_flag = "--start"; // Flag to enable assignments
constexpr static const char *not_authorized = "Not authorized";

// Global configuration variables
static std::string workfile,     // File to store/read assignment states
                   username,     // User credentials
                   restapi_url;  // Primo API endpoint URL

// Assignment states in the Primo system
enum AssignmentStatus {
   ENABLED  = 1,  // Assignment is active and running
   HAND     = 2,  // Assignment is in manual operation mode
   DISABLED = 3   // Assignment is inactive/stopped
};

// Loads configuration from the config file Retrieves workfile path and REST API URL
static void load_config()
{
   cfg::Config *config = cfg::Config::Instance();
   workfile    = config->GetParameter<std::string>("workfilename");
   restapi_url = config->GetParameter<std::string>("restapiurl");
}

// Gets username from config or prompts user if not configured
static std::string get_username()
{
   std::string username;
   cfg::Config *config = cfg::Config::Instance();
   try {
      username = config->GetParameter<std::string>("username");
   }
   catch(const std::exception &ex) {
      std::cerr << ex.what() << std::endl;
      std::cout << "Username: "; getline(std::cin, username);
   }
   return username;
}

// Global signal handler for graceful shutdown
signals::SignalHandler handler;

// Disables all active assignments and saves their states to file
static void disable_all(const nlohmann::json &js, primo::PrimoApi &primo,
                        const std::string &dropfile)
{
   using namespace std::string_literals;
   std::ofstream ofile(dropfile);
   if(!ofile.is_open())
      throw std::runtime_error(dropfile + ": "s + std::strerror(errno));
   
   std::string assignments;
   if(primo.GetAssignments(assignments)) {
      nlohmann::json js = nlohmann::json::parse(assignments);
      for(const auto &x : js) { // Process each assignment
         if(handler.NeedStop()) // Check for interrupt signal
            break;
         if(x["status"] == ENABLED) { // Process only enabled assignments
            if(primo.StopAssignment(x["id"]))
               ofile << x["id"] << ' ' << x["name"] << '\n';
            else {
               std::string name = x["name"];
               logging::logger.FmtMsg(&logging::logger, logging::ERR,
                                      "%s not disabled", name.c_str());
            }
         }
      }
   }
   ofile.close();
}

// Enables previously disabled assignments from saved state file
// Parameters:
//   primo    - Primo API client instance
//   datafile - File containing saved assignment states
static void enable_all(primo::PrimoApi &primo, const std::string &datafile)
{
   using namespace std::string_literals;
   std::ifstream ifile(datafile);
   if(!ifile.is_open()) {
      throw std::runtime_error(datafile + ": "s + std::strerror(errno));
   }
   long id;
   std::string name;
   while(ifile >> id && ifile >> name) { // Read and process each saved assignment
      if(handler.NeedStop())             // Check for interrupt signal
         break;
      if(!primo.StartAssignment(id))
         logging::logger.FmtMsg(&logging::logger, logging::ERR,
                                "%s not enabled", name.c_str());
   }
   ifile.close();
}

// Checks if a specific flag exists in command line arguments
// Parameters:
//   args - Vector of command line arguments
//   flag - Flag to search for
// Returns: true if flag is found, false otherwise
static bool has_flag(const std::vector<std::string> &args, const std::string &flag)
{
    return std::find(args.begin(), args.end(), flag) != args.end();
}

int main(int argc, char **argv)
{
   try {
      handler.SetDispose(SIGINT); // Set up signal handler for Ctrl+C

      // Parse command line arguments
      std::vector<std::string> args(argv + 1, argv + argc);
      bool start = has_flag(args, start_flag);
      bool stop  = has_flag(args, stop_flag);
      
      // Validate command line arguments - exactly one flag must be specified
      if(!(start ^ stop)) {
         std::string err_msg = "Error. Expects "
            + std::string(stop_flag) + std::string(" or ")
               + std::string(start_flag);
         throw std::runtime_error(err_msg.c_str());
      }

      load_config();
      primo::PrimoApi primo(restapi_url.c_str());
      
      std::string username = get_username(); // Handle authentication
      if(!primo.Authorize(username, cmd::secure_input("Password"))) {
         throw std::runtime_error(not_authorized);
      }

      if(stop) { // Execute requested operation
         std::string assignments;
         if(primo.GetAssignments(assignments)) {
            nlohmann::json js = nlohmann::json::parse(assignments);
            disable_all(js, primo, workfile);
         }
      } else if(start)
         enable_all(primo, workfile);
   }
   // Errors handling
   catch (std::exception &ex) {
      std::cerr << ex.what() << std::endl;
      return 1;
   }
   catch (const char *msg) {
      std::cerr << msg << std::endl;
      return 1;
   }

   return 0;
}
