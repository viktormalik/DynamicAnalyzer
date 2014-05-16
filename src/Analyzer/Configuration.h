/**
 * @file Configuration.h
 *
 * @brief Configuration class.
 *
 * Contains definition of Configuration class, which the analyzer configuration is stored in.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Mar 28, 2014
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include "Parser/InitialMsg.h"

#include <string>
#include <vector>

/**
 * @brief Configuration of the analyzer.
 * Class containing all configuration of the analyzer program. It is loaded from configuration file
 * that can be edited by user.
 * In addition, class is responsible for validating and parsing command line parameters.
 */
class Configuration {
public:
   /**
    * @brief Default constructor.
    * Creates empty configuration object.
    */
   Configuration();
   /**
    * @brief Default destructor.
    * Deletes initial message, which is created dynamically.
    */
   virtual ~Configuration();

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
   void parse(int argc, char **argv);

   /**
    * @brief Gets analyzed program name and parameters.
    * @return Vector of strings with program name and its parameters.
    */
   std::vector<std::string> getProgram();
   /**
    * @brief Gets initial message.
    * @return Pointer to the initial message created from configuration.
    */
   InitialMsg *getInitMsg();
   /**
    * @brief Gets list of variant groups.
    * @return Vector of variant group names.
    */
   std::vector<std::string> getVariants();
   /**
    * @brief Gets scheduler type.
    * @return String with scheduler type.
    */
   std::string getScheduler();
   /**
    * @brief Gets aggregator type.
    * @return String with aggregator type.
    */
   std::string getAggregator();
   /**
    * @brief Gets minimal subroutine size used for jumps detection.
    * @return Minimal subroutine size.
    */
   unsigned getSubroutine();
   /**
    * @brief Gets output type.
    * @return String with output type.
    */
   std::string getOutput();
   /**
    * @brief Gets output file destination.
    * @return String containing path to output file.
    */
   std::string getDestination();
   /**
    * @brief Checks if --help was specified, thus help should be printed.
    * @return Value of ::help.
    */
   bool checkHelp();
   /**
    * @brief Prints help.
    */
   void printHelp();
protected:
   /**
    * @brief If true, "--help" option has been given and help should be printed.
    */
   bool help;
   /**
    * @brief Vector containing tested program name and parameters to run it with.
    * First item is the program destination, the rest are parameters.
    */
   std::vector<std::string> testedProgram;
   /**
    * @brief Initial message containing control and notify functions lists.
    *    - @b Notify list contains information which functions performing the analyzer will be
    *    notified about.
    *    - @b Control list contains information which functions performing the analyzer will be
    *    able to affect.
    */
   InitialMsg *initMsg;
   /**
    * @brief List of variant groups specifying which variants will be used for affecting calls in tested
    * program.
    */
   std::vector<std::string> variants;
   /**
    * @brief Scheduler type to be used for analysis.
    * This type affects the strategy of controlling the flow of analyzed program in order
    * to explore its different branches.
    * Currently supported types:
    *    - bfs (algoithm based on breadth-first search, explores all possible branches of program)
    */
   std::string scheduler;
   /**
    * @brief Aggregator type to be used for analysis.
    * This defines when two calls are considered same, or different.
    * Currently supported types:
    *    - name (calls are distinguished by function name only)
    */
   std::string aggregator;
   /**
    * @brief Minimal subroutine size.
    * This number specifies minimal number of successive calls that could be considered as
    * subroutine if they repeated in the tested program.
    * If it is equal to 1, no jump detection will be performed.
    */
   int subroutine;
   /**
    * @brief Output type.
    * Currently supported types:
    *    - graph (output that can be further processed by external program @b dot)
    *    - json (output in JSON notation)
    */
   std::string output;
   /**
    * @brief Output path.
    * Specifies the output file name and its destination.
    */
   std::string destination;
private:
   /**
    * @brief Parses individual options.
    * Based on option name, parses its value and stores it in appropriate form to one of object
    * attributes.
    * @param option Option name.
    * @param value Option value.
    * @throws ConfigurationException In case of invalid configuration option or value.
    */
   void setOption(std::string option, std::string value);
   /**
    * Help message string.
    */
   static const std::string helpMsg;
};

#endif /* CONFIGURATION_H_ */
