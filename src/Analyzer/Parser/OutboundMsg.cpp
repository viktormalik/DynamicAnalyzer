/**
 * @file OutboundMsg.cpp
 *
 * Contains implementation of methods of the class OutboundMsg.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 06.03.2014
 *
 * Created on: Feb 14, 2014
 */

#include "Parser/OutboundMsg.h"

#include <sstream>

/**
 * @brief Constructor with assigning type.
 * Usually used for type ACK.
 * @param t Type of the message.
 */
OutboundMsg::OutboundMsg(msgTypes t) :
      Message(t), variant(0) {
}

/**
 * @brief Constructor with initialization of type, function and variant.
 * Usually used for type EXECUTE.
 * @param t Type of the message.
 * @param f Function the message informs about.
 * @param var Variant that the tested program shall use.
 */
OutboundMsg::OutboundMsg(msgTypes t, std::string f, int var) :
      Message(t), function(f), variant(var) {
}

/**
 * @brief Default destructor.
 */
OutboundMsg::~OutboundMsg() {
}

/**
 * @brief Parse incoming message.
 * @see Message::parse.
 * @param msg Message string to be parsed.
 * @warning Not implemented.
 */
void OutboundMsg::parse(std::string msg) {
}

/**
 * @brief Compose message into string.
 * @see Message::compose
 * @return Message stirng.
 */
std::string OutboundMsg::compose() {
   std::string msg;
   // Write message type + CRLF
   msg = Message::typesEnum[type];
   msg += "\r\n";
   if (type == EXEC) {
      // Write function name + CRLF
      msg += function;
      msg += "\r\n";

      // Write variant + CRLF
      std::stringstream ss;
      ss << variant;
      msg += ss.str();
      msg += "\r\n";
   }
   // Write ending CRLF
   msg += "\r\n";
   return msg;
}

