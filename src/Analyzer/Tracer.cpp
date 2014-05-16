/**
 * @file Tracer.cpp
 *
 * Contains implementation of methods of the class Tracer.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Feb 15, 2014
 */

#include "Tracer.h"
#include "Exceptions/ProtocolException.h"
#include "Parser/InboundMsg.h"
#include "Parser/OutboundMsg.h"
#include "Exceptions/SocketClosedException.h"
#include "Exceptions/ConfigurationException.h"

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <sys/wait.h>
#include <cstring>
#include <iostream>

/**
 * @brief Constructor with members initialization.
 * Initializes pointers to socket and controller. Also converts given vector of string with
 * tested program name and parameters into array of @a char*.
 * @param s Pointer to the Socket object used for communication.
 * @param c Pointer to the Controller object which created this Tracer.
 * @param prog Vector of strings with tested program destination and parameters.
 */
Tracer::Tracer(Socket *s, Controller *c, std::vector<std::string> prog) :
      callNum(0), socket(s), controller(c), programArgsSize(prog.size()) {

   // Create array of c-string from vector of std::string
   programArgs = new char*[programArgsSize + 1]();
   for (unsigned i = 0; i < programArgsSize; ++i) {
      programArgs[i] = new char[100]();
      strcpy(programArgs[i], prog[i].c_str());
   }
   // Last member of the array must be NULL, because it will be used in execv() function
   programArgs[programArgsSize] = NULL;
}

/**
 * @brief Default destructor.
 * Deletes array of @a char* storing tested program parameters.
 */
Tracer::~Tracer() {
   for (unsigned i = 0; i < programArgsSize; ++i) {
      delete programArgs[i];
   }
   delete programArgs;
}

/**
 * @brief Executes tested program and initializes communication with it.
 * Creates new process in which tested program is executed. After succesfull execution realizes
 * initial phase of the communication with tested program (receives INIT message and sends
 * OPTION message as response).
 * @param msg Object containing OPTION message with analysis options sent to tested program.
 */
void Tracer::init(InitialMsg *optionMsg) {
   pid_t newProcess;
   // Create new process
   newProcess = fork();
   if (newProcess >= 0) {
      if (newProcess == 0) {
         socket->closeWelcomeSocket();
         // Add shared library to LD_PRELOAD
         char ldPreloadTxt[] = "LD_PRELOAD=bin/lib_filesystem.so";
         putenv(ldPreloadTxt);
         // Execute tested program
         if (execv(programArgs[0], programArgs) == -1)
            throw ConfigurationException("Program cannot be executed");
      }
      else {
         // Create socket connection to other process
         socket->acceptConnection();

         // Initialization phase
         // Receive first message and parse it
         std::string msgStr = socket->recvMsg();
         InboundMsg initMsg;
         initMsg.parse(msgStr);
         if (initMsg.getType() == INIT) {
            // If INIT message received, send OPTION message
            socket->sendMsg(optionMsg->compose());
         }
         else {
            throw ProtocolException("INIT not received.");
         }
      }
   }
}

/**
 * @brief Traces the running tested program and gets its run.
 * In an infinite loop receives messages from tested program, forwards them to controller and
 * sends back the generated answers. Also receives RETURN messages with information about return
 * value of the call.
 * The loop is breaked after there are no more messages pending in the socket, or the socket has
 * been closed by tested program.
 * @return Flow of the tested program.
 */
Run *Tracer::trace() {

   Run *resultRun = new Run();
   std::string msgStr;
   InboundMsg *inMsg = new InboundMsg();
   OutboundMsg *outMsg;

   // Loop while tested program is running
   while (1) {
      // Receive message with current call and parse it
      try {
         msgStr = socket->recvMsg();
      } catch (SocketClosedException &e) {
         break;
      }
      inMsg->parse(msgStr);
      if (inMsg->getType() == NOTIFY || inMsg->getType() == CONTROL) {
         // Generate response depending on message type
         if (inMsg->getType() == CONTROL) {
            outMsg = controller->controlCall(inMsg->getCall(), resultRun, callNum++);
         }
         else {
            outMsg = controller->notifyCall(inMsg->getCall(), callNum++);
         }
         // Push the call to the run
         resultRun->nextCall(inMsg->getCall());
         try {
            // Send response
            socket->sendMsg(outMsg->compose());
         } catch (SocketClosedException &e) {
            // Socket has been closed by tested program -> end loop
            delete outMsg;
            break;
         }
         delete outMsg;

         // Receive RETURN message with return value of the call (automatically updates call)
         try {
            msgStr = socket->recvMsg();
         } catch (SocketClosedException &e) {
            break;
         }
         inMsg->parse(msgStr);
         if (inMsg->getType() == RETURN) {
            // Generate response (always ACK)
            outMsg = new OutboundMsg(ACK);
            try {
               socket->sendMsg(outMsg->compose());
            } catch (SocketClosedException &e) {
               // Socket has been closed by other program -> end loop
               delete outMsg;
               break;
            }
            delete outMsg;
         }
      }
      else {
         // There are no more messages pending in the socket
         break;
      }
   }
   delete inMsg;
   int status;
   wait(&status);
   socket->closeClientSocket();
   return resultRun;
}

