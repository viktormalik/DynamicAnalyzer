/**
 * @file Controller.h
 *
 * @brief Controller class
 *
 * Contains definition of Controller class, used for controlling the whole analysis.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 03.04.2014
 *
 * Created on: Feb 15, 2014
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Socket.h"
#include "Run.h"
#include "Call.h"
#include "Parser/OutboundMsg.h"
#include "Parser/InboundMsg.h"
#include "Parser/InitialMsg.h"
#include "Scheduler/Scheduler.h"
#include "Aggregator/Aggregator.h"
#include "Configuration.h"

#include <vector>
#include <map>

/**
 * @brief Class for controlling the analysis,
 * Main class which serves for controlling the process of analysis. Also is responsible for
 * generating reaction to the calls that come from analyzed program.
 * Creates and saves individual program runs, calls scheduler and aggregator.
 */
class Controller {
public:
   /**
    * @brief Constructor with initialization of members.
    * Gets the configuration and creates socket, controller and aggregator according to the options.
    * @param c Configuration of the analyzer.
    */
   Controller(Configuration *c);
   /**
    * @brief Default destructor.
    * Destroys all dynamic objects created in constructor and also deletes all runs.
    */
   virtual ~Controller();

   /**
    * @brief Method controlling the process of analysis.
    * Main method where the whole analysis takes place.
    * At first it creates the first run with notification-only calls (normal behavior of the
    * analyzed program). After that other different runs are created in the cycle, which is
    * stopped by scheduler.
    * For each run, this method creates individual Tracer object, which is responsible for tracing
    * the analyzed program from beginning to the end. Tracer during its proceeding calls the
    * functions ::controlCall and ::notifyCall every time a call arrives.
    */
   void startAnalysis();
   /**
    * @brief Reaction to the message of type CONTROL.
    * Method called by tracer every time a message of type CONTROL arrives.
    * Is responsible for scheduling how the behavior of the call will be affected (calls Scheduler)
    * and aggregating the call to existing graph (calls Aggregator).
    * @param call The call which is to be controlled.
    * @param run Current run of the tested program.
    * @param callNum The number of current call in the current run of tested program.
    * @return Generated response to the CONTROL message in the form of OutboundMsg.
    */
   OutboundMsg *controlCall(Call *call, Run *run, unsigned callNum);
   /**
    * @brief Reaction to the message of type NOTIFY.
    * Method called by tracer every time a message of type NOTIFY arrives.
    * Is responsible for aggregating the call to existing graph (calls Aggregator) and informing
    * scheduler that such call has been performed.
    * @param call The call which is performed by tested program.
    * @param callNum The number of call in current run of tested program.
    * @return OutboundMsg of type ACK as it is only possible response to NOTIFY message.
    */
   OutboundMsg *notifyCall(Call *call, unsigned callNum);

   /**
    * @brief Get run from given index.
    * @param index Index of run.
    * @return Pointer to the run at given index.
    */
   Run *getRun(unsigned index);
   /**
    * @brief Set the analysis is completed after finishing current run.
    */
   void setCompleted();
   /**
    * @brief Gets number of different runs already obtained.
    * @return Number of runs.
    */
   unsigned getRunsCnt();
   /**
    * @brief Prints traces to standard output.
    * @warning For testing purposes only.
    */
   void printTraces();
protected:
   /**
    * @brief List of runs obtained.
    * Vector containing different runs of the tested program.
    */
   std::vector<Run *> runs;
   /**
    * When set to true, analysis will end after finishing current run.
    */
   bool completed;
   /**
    * @brief Communication socket.
    * Socket object responsible for communication with analyzed program. It is used by Tracer
    * objects for getting messages from analyzed program and sending reactions to them.
    */
   Socket *socket;
   /**
    * @brief Scheduler creatig responses to CONTROL messages.
    * Scheduler object responsible for generating different responses to CONTROL messages in order
    * to get different runs of the tested program.
    * As Message is abstract class, it is typically an object of one of its inherited classes.
    */
   Scheduler *scheduler;
   /**
    * @brief Aggregator joining different traces into control-flow graph.
    * Aggregator object responsible for aggregating all runs into one graph structur that
    * represents the control-flow graph of the tested program.
    * Can be instance of class Aggregator or of one of its inherited classes.
    */
   Aggregator *aggregator;
   /**
    * @brief Configuration of the analyzer.
    * Configuration object containing all configuration of the analyzer obtained from config file.
    * Controller uses this configuration to set up Tracer, Scheduler and Aggregator objects.
    */
   Configuration *configuration;
};

#endif /* CONTROLLER_H_ */
