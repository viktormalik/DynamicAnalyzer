/**
 * @file Tracer.h
 *
 * @brief Tracer class.
 *
 * Contains definition of class Tracer, which serves for tracing tested program and creating
 * its flows.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 02.04.2014
 *
 * Created on: Feb 15, 2014
 */

#ifndef TRACER_H_
#define TRACER_H_

#include "Socket.h"
#include "Controller.h"
#include "Parser/InitialMsg.h"
#include "Configuration.h"
#include "Run.h"

#include <vector>
#include <string>

/**
 * @brief Serves for tracing tested program and creating its runs.
 * Class for tracing the tested program. It runs the tested program with parameters given
 * by configuration and then receives messages from it informing about performation of the calls.
 * Also sends generated answers back.
 * From these calls, it creates the run of the program.
 * Each object of this class is used to create one single run - sequence of calls from beginning
 * of tested program to its end.
 */
class Tracer {
public:
   /**
    * @brief Constructor with members initialization.
    * Initializes pointers to socket and controller. Also converts given vector of string with
    * tested program name and parameters into array of @a char*.
    * @param s Pointer to the Socket object used for communication.
    * @param c Pointer to the Controller object which created this Tracer.
    * @param prog Vector of strings with tested program destination and parameters.
    */
   Tracer(Socket *s, Controller *c, std::vector<std::string> prog);
   /**
    * @brief Default destructor.
    * Deletes array of @a char* storing tested program parameters.
    */
   virtual ~Tracer();

   /**
    * @brief Executes tested program and initializes communication with it.
    * Creates new process in which tested program is executed. After succesfull execution realizes
    * initial phase of the communication with tested program (receives INIT message and sends
    * OPTION message as response).
    * @param msg Object containing OPTION message with analysis options sent to tested program.
    */
   void init(InitialMsg *msg);
   /**
    * @brief Traces the running tested program and gets its run.
    * In an infinite loop receives messages from tested program, forwards them to controller and
    * sends back the generated answers. Also receives RETURN messages with information about return
    * value of the call.
    * The loop is breaked after there are no more messages pending in the socket, or the socket has
    * been closed by tested program.
    * @return Run of the tested program.
    */
   Run *trace();
protected:
   int callNum; /**< Current call number in the run */
   Socket *socket; /**< Pointer to the Socket object used for communication with tested program. */
   Controller *controller; /**< Pointer to the Controller where parsed messages are being sent
                            * in order to get answers to those messages. */
   char **programArgs; /**< Array of c-strings with tested program destination as the first item
                        * and its parameters as the rest of items. */
   unsigned programArgsSize; /**< Size of ::programArgs array */
};

#endif /* TRACER_H_ */
