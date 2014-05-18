/**
 * @file Controller.cpp
 *
 * Contains implementation of methods of the class Controller.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 03.04.2014
 *
 * Created on: Feb 15, 2014
 */

#include "Controller.h"
#include "Tracer.h"
#include "Scheduler/BFSScheduler.h"
#include "Exceptions/ConfigurationException.h"
#include "Aggregator/BaseParamAggregator.h"

/**
 * @brief Constructor with initialization of members.
 * Gets the configuration and creates socket, controller and aggregator according to the options.
 * @param c Configuration of the analyzer.
 */
Controller::Controller(Configuration *c) :
      completed(false), configuration(c) {

   // Create new socket
   socket = new Socket("/tmp/analyserSocket");

   // Create new scheduler according to type specified in configuration
   std::string schedType = configuration->getScheduler();
   if (schedType == "bfs")
      scheduler = new BFSScheduler(this, configuration->getVariants());
   else
      throw ConfigurationException("Unsupported scheduler type");

   // Create new aggregator according to type specified in configuration
   std::string aggregType = configuration->getAggregator();
   if (aggregType == "name")
      aggregator = new Aggregator(configuration->getSubroutine(), configuration->getDestination());
   else if (aggregType == "base_param") {
      aggregator = new BaseParamAggregator(configuration->getSubroutine(),
            configuration->getDestination());
   }
   else
      throw ConfigurationException("Unsupported aggregator type");
}

/**
 * @brief Default destructor.
 * Destroys all dynamic objects created in constructor and also deletes all runs.
 */
Controller::~Controller() {
   delete socket;
   delete scheduler;
   delete aggregator;
   for (auto trace : runs) {
      delete trace;
   }
}

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
void Controller::startAnalysis() {
   // Get initial message from configuration
   InitialMsg *initMsg = configuration->getInitMsg();

   // First tracer
   Tracer tracer(socket, this, configuration->getProgram());

   // First initial message:
   // - control list is empty (type is NONE)
   // - notify list is union of control list and notify list from configuration init message
   InitialMsg firstInitMsg(OPTION);
   firstInitMsg.setControlType(NONE);

   // If one of original types is ALL, union will be ALL
   if (initMsg->getNotifyType() == ALL || initMsg->getControlType() == ALL) {
      firstInitMsg.setNotifyType(ALL);
   }
   // If one of original types is INCLUDE, union will be include
   else if (initMsg->getNotifyType() == INCLUDE || initMsg->getControlType() == INCLUDE) {
      firstInitMsg.setNotifyType(INCLUDE);
      // If notify type is INCLUDE, add all its functions to first init message notify list
      if (initMsg->getNotifyType() == INCLUDE) {
         for (auto fun : initMsg->getNotifyFunctions()) {
            firstInitMsg.addNotifyFunction(fun);
         }
      }
      // If control type is INCLUDE, add all its functions to first init message notify list
      if (initMsg->getControlType() == INCLUDE) {
         for (auto fun : initMsg->getControlFunctions()) {
            firstInitMsg.addNotifyFunction(fun);
         }
      }
   }
   // Both original types are NONE, union is NONE
   else {
      firstInitMsg.setNotifyType(NONE);
   }

   // Get first run
   tracer.init(&firstInitMsg);
   runs.push_back(tracer.trace());

   if (runs[0]->getSize() != 0) {
      // Main program loop getting different flows until it is stopped by scheduler
      while (!completed) {
         // Reset aggregator
         aggregator->nextRun();
         // Create new Tracer and get new flow
         Tracer tracer(socket, this, configuration->getProgram());
         tracer.init(initMsg);
         Run *run = tracer.trace();
         // If the run is different from existing runs, add it into vector of runs
         if (aggregator->nodeInserted())
            runs.push_back(run);
         // If the run is same as one of existing flows, delete it
         else
            delete run;
         scheduler->nextRun();
      }
      // Reset aggregator once more in order to make last node final
      aggregator->nextRun();
   }

   // Close socket
   socket->closeWelcomeSocket();

   // Print output graph from aggregator in format given in configuration file
   std::string output = configuration->getOutput();
   if (output == "dot")
      aggregator->drawGraph();
   else if (output == "json")
      aggregator->printJson();
   else
      throw ConfigurationException("Unsupported output format");
}

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
OutboundMsg *Controller::controlCall(Call *call, Run *run, unsigned callNum) {
   // Schedule next call and get response to CONTROL message
   OutboundMsg *msg = scheduler->scheduleCall(call, run, callNum);
   // Insert new call into existing graph
   aggregator->newNode(call);
   // Return response
   return msg;
}

/**
 * @brief Reaction to the message of type NOTIFY.
 * Method called by tracer every time a message of type NOTIFY arrives.
 * Is responsible for aggregating the call to existing graph (calls Aggregator) and informing
 * scheduler that such call has been performed.
 * @param call The call which is performed by tested program.
 * @param callNum The number of call in current run of tested program.
 * @return OutboundMsg of type ACK as it is only possible response to NOTIFY message.
 */
OutboundMsg *Controller::notifyCall(Call *call, unsigned callNum) {
   // Inform scheduler about performing next call
   scheduler->notifCall(callNum);
   // Insert new call into existing graph
   aggregator->newNode(call);
   // Return response (always ACK)
   return new OutboundMsg(ACK);
}

/**
 * @brief Get run from given index.
 * @param index Index of run.
 * @return Pointer to the run at given index.
 */
Run *Controller::getRun(unsigned index) {
   return runs[index];
}

/**
 * @brief Set the analysis is completed after finishing current run.
 */
void Controller::setCompleted() {
   completed = true;
}

/**
 * @brief Gets number of different runs already obtained.
 * @return Number of runs.
 */
unsigned Controller::getRunsCnt() {
   return runs.size();
}

/**
 * @brief Prints traces to standard output.
 * @warning For testing purposes only.
 */
void Controller::printTraces() {
   std::cout << std::endl;
   for (auto trace : runs) {
      trace->print();
   }
   std::cout << std::endl;
}

/**
 * @brief Converts trace into its string representation.
 * @param trace Trace to be converted.
 * @return String representation of the trace.
 */
void Controller::printReport(){
   std::cout << std::endl;
   std::cout << "Analysis successfuly terminated." << std::endl;
   std::cout << "Number of nodes in graph: " << aggregator->nodesCount() << std::endl;
   std::cout << "Number of different runs: " << runs.size() << std::endl;
   std::cout << std::endl;
}
