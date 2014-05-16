/**
 * @file Configuration.cpp
 *
 * Contains implementation of methods of the class Configuration.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 28, 2014
 */

#include "Configuration.h"
#include "Exceptions/ConfigurationException.h"

#include <fstream>
#include <istream>
#include <sstream>
#include <iostream>

/**
 * @brief Default constructor.
 * Creates empty configuration object.
 */
Configuration::Configuration() :
      help(false), initMsg(NULL), subroutine(0) {
   initMsg = new InitialMsg(OPTION);
}

/**
 * @brief Default destructor.
 * Deletes initial message, which is created dynamically.
 */
Configuration::~Configuration() {
   delete initMsg;
}

/**
 * @brief Parses input configuration file.
 * Parses command line parameters, where the configuration file destination should be specified.
 * Then reads this file line by line, each line contains one configuration option in form:
 * @verbatim <option> = <value>@endverbatim
 * @param argc Number of command line parameters (argument of function main()).
 * @param argv Array of command line parameters (argument of function main()).
 * @throws ConfigurationException In case of invalid command line parameters or configuraiton
 * file syntax.
 */
void Configuration::parse(int argc, char **argv) {
   if (argc == 2) {
      std::string arg(argv[1]);
      if (arg == "--help") {
         help = true;
      }
      else {
         // Open configuration file
         std::ifstream configFile;
         configFile.open(arg, std::ifstream::in);
         if (!configFile.is_open())
            // Open failure
            throw ConfigurationException("Error opening configuration file");

         std::string line;
         // Read config file line by line
         while (std::getline(configFile, line)) {
            if (line[0] != '#') { // hash starts comment
               std::istringstream isLine(line);
               std::string option;
               // Get part of line before '='
               if (std::getline(isLine, option, '=')) {
                  std::string value;
                  // Get part of line after '='
                  if (std::getline(isLine, value)) {
                     // Get rid of trailing spaces in option name
                     size_t pos = option.find_last_not_of(" ");
                     if (pos != std::string::npos)
                        option.erase(pos + 1);
                     // Get rid of leading spaces in value string
                     pos = value.find_first_not_of(" ");
                     value.erase(0, pos);
                     // Parse option value
                     setOption(option, value);
                  }
                  else
                     throw ConfigurationException("Incorrect configuration file syntax");
               }
               else
                  throw ConfigurationException("Incorrect configuration file syntax");
            }
         }
      }
   }
   else
      // Only configuration file name must be given as command line parameter
      throw ConfigurationException("Wrong number of parameters specified");
}

/**
 * @brief Parses individual options.
 * Based on option name, parses its value and stores it in appropriate form to one of object
 * attributes.
 * @param option Option name.
 * @param value Option value.
 * @throws ConfigurationException In case of invalid configuration option or value.
 */
void Configuration::setOption(std::string option, std::string value) {
   // Switch option name
   if (option == "program") {
      // Program name and parameters are divided by white spaces
      std::istringstream isProgram(value);
      std::string arg;
      while (isProgram >> arg) {
         testedProgram.push_back(arg);
      }
   }
   else if (option == "control") {
      // If control option is "all" or "none", set appropriate type
      if (value == "all" || value == "ALL") {
         initMsg->setControlType(ALL);
      }
      else if (value == "none" || value == "NONE") {
         initMsg->setControlType(NONE);
      }
      // If control option contains functions list, set type to INCLUDE and create list
      else {
         initMsg->setControlType(INCLUDE);
         std::istringstream isControl(value);
         std::string function;
         while (std::getline(isControl, function, ',')) {
            // Add new function to control list, also checks functions validity
            initMsg->addControlFunction(function);
         }
      }
   }
   else if (option == "notify") {
      // If notify option is "all" or "none", set appropriate type
      if (value == "all" || value == "ALL") {
         initMsg->setNotifyType(ALL);
      }
      else if (value == "none" || value == "NONE") {
         initMsg->setNotifyType(NONE);
      }
      // If notify option contains functions list, set type to INCLUDE and create list
      else {
         initMsg->setNotifyType(INCLUDE);
         std::istringstream isNotify(value);
         std::string function;
         while (std::getline(isNotify, function, ',')) {
            // Add new function to notify list, also checks functions validity
            initMsg->addNotifyFunction(function);
         }
      }
   }
   else if (option == "variants") {
      // Variants is a list of groups divided by ','
      std::istringstream isVariants(value);
      std::string variant;
      while (std::getline(isVariants, variant, ',')) {
         variants.push_back(variant);
      }
   }
   else if (option == "scheduler") {
      // Scheduler is simple string, its validation will be performed elsewhere
      scheduler = value;
   }
   else if (option == "aggregator") {
      // Aggregator is simple string, its validation will be performed elsewhere
      aggregator = value;
   }
   else if (option == "subroutine") {
      // Subroutine must be a number greater than 0
      std::stringstream ss(value);
      if (ss >> subroutine) {
         if (subroutine <= 0)
            throw ConfigurationException("Minimal subroutine size must be greater than 1");
      }
      else
         throw ConfigurationException("Subroutine must be a number");
   }
   else if (option == "output") {
      // Output type is simple string, its validation will be performed elsewhere
      output = value;
   }
   else if (option == "destination") {
      // Destination is simple string, its validation will be performed elsewhere
      destination = value;
   }
   else {
      // Other options are not supported
      throw ConfigurationException("Invalid option in configuration file");
   }
}

/**
 * @brief Gets analyzed program name and parameters.
 * @return Vector of strings with program name and its parameters.
 */
std::vector<std::string> Configuration::getProgram() {
   return testedProgram;
}

/**
 * @brief Gets initial message.
 * @return Pointer to the initial message created from configuration.
 */
InitialMsg *Configuration::getInitMsg() {
   return initMsg;
}

/**
 * @brief Gets list of variant groups.
 * @return Vector of variant group names.
 */
std::vector<std::string> Configuration::getVariants() {
   return variants;
}

/**
 * @brief Gets scheduler type.
 * @return String with scheduler type.
 */
std::string Configuration::getScheduler() {
   return scheduler;
}

/**
 * @brief Gets aggregator type.
 * @return String with aggregator type.
 */
std::string Configuration::getAggregator() {
   return aggregator;
}

/**
 * @brief Gets minimal subroutine size used for jumps detection.
 * @return Minimal subroutine size.
 */
unsigned Configuration::getSubroutine() {
   return subroutine;
}

/**
 * @brief Gets output type.
 * @return String with output type.
 */
std::string Configuration::getOutput() {
   return output;
}

/**
 * @brief Gets output file destination.
 * @return String containing path to output file.
 */
std::string Configuration::getDestination() {
   return destination;
}

/**
 * @brief Checks if --help was specified, thus help should be printed.
 * @return Value of ::help.
 */
bool Configuration::checkHelp() {
   return help;
}

/**
 * @brief Prints help.
 */
void Configuration::printHelp(){
   std::cout << helpMsg;
}

const std::string Configuration::helpMsg =
      "Program for dynamic analysis of a binary and creating its control-flow graph.\n"
      "Usage:\n"
      "   analyzer (FILENAME|--help)\n"
      "\n"
      "--help Prints this help.\n"
      "\n"
      "Otherwise starts analysis whose configuration is given in file FILENAME.\n"
      "\n"
      "Configuration file syntax:\n"
      "Each line contains one configuration option in form:\n"
      "   <option> = <value>\n"
      "Lines starting with \'#\' are ignored.\n"
      "\n"
      "Possible options with their value description:\n"
      "   program - program to be analyzed (with all program parameters)\n"
      "\n"
      "   control - list of functions to be controlled\n"
      "             possible values: \"all\" - all functions\n"
      "                              \"none\" - no functions\n"
      "                              list of functions separated by \',\'\n"
      "\n"
      "   notify  - list of functions only to observe their calling\n"
      "             possible values: \"all\" - all functions\n"
      "                              \"none\" - no functions\n"
      "                              list of functions separated by \',\'\n"
      "\n"
      "   variants - list of variant groups used for analysis\n"
      "              possible variant groups:\n"
      "                 inval - errors \"bad file descriptor\" (EBADF) and \"invalid value\" (EINVAL)\n"
      "                 io - input-output error (EIO)\n"
      "                 access - error in access rights (EACCES)\n"
      "                 memory - errors \"invalid pointer\" (EFAULT) and \"unsufficent memory\" (ENOMEM)\n"
      "                 interrupt - interruption of the calls by external signal (EINTR)\n"
      "                 path - errors in file path as parameter of the call\n"
      "                 limits - errors of unsufficent sources limits\n"
      "                 permissions - errors of permissions of the filesystem\n"
      "                 file - errors of file given as parameter of the call\n"
      "\n"
      "   scheduler - algorithm of scheduling analysis\n"
      "               possible values:\n"
      "                  bfs - breadth-first search scheduler\n"
      "\n"
      "   aggregator - type of calls aggregation\n"
      "                possible values:\n"
      "                  name - calls are aggregated by function name\n"
      "                  base_param - calls are aggregated by function name and base parameter\n"
      "\n"
      "   subroutine - minimal subroutine size (used for detecting jumps in tested program)\n"
      "                must be superior to 0, 1 means no jump detection\n"
      "\n"
      "   output - output type\n"
      "            possible values:\n"
      "              dot - source with graph for program dot\n"
      "              json - graph in JSON notation\n"
      "\n"
      "   destination - output file destination\n"
      "\n";
