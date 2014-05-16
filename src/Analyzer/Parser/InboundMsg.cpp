/**
 * @file InboundMsg.cpp
 *
 * Contains implementation of methods of the class InboundMsg.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 16.03.2014
 *
 * Created on: Feb 14, 2014
 */

#include "Parser/InboundMsg.h"
#include "Param.h"

/**
 * @brief Default constructor.
 * Creates empty message.
 */
InboundMsg::InboundMsg() :
      Message(), call(NULL) {
}

/**
 * @brief Parses incoming message.
 * @see Message::parse.
 * @param msg Stirng to be parsed.
 */
void InboundMsg::parse(std::string msg) {
   size_t oldPos, newPos, endPos;

   // Find message end
   endPos = msg.find("\r\n\r\n");

   // Find first CRLF
   newPos = msg.find("\r\n");
   // First line is message type
   type = Message::typesStr[msg.substr(0, newPos)];

   if (type == CONTROL || type == NOTIFY) {
      call = new Call();
      // Find second CRLF
      oldPos = newPos + 2;
      newPos = msg.find("\r\n", oldPos);
      // Second line is call name
      call->setFunctionName(msg.substr(oldPos, newPos - oldPos));

      // Find all lines with parameters and parse them
      Param *param;
      while (newPos != endPos) {
         oldPos = newPos + 2;
         newPos = msg.find("\r\n", oldPos);
         std::string paramVal = msg.substr(oldPos, newPos - oldPos).c_str();
         param = new Param(paramVal);
         call->addParam(param);
      }
   }
   else if (type == RETURN) {
      // Find ending CRLF
      oldPos = newPos + 2;
      newPos = msg.find("\r\n", oldPos);
      // Second line is return value
      call->setReturnVal(msg.substr(oldPos, newPos - oldPos));
   }
}

/**
 * @brief Composes message into string.
 * @see Message::compose
 * @return String composed.
 * @warning Not implemented.
 */
std::string InboundMsg::compose() {
   return "";
}

/**
 * @brief Gets the call contained in the message.
 * @return Pointer to the call.
 */
Call *InboundMsg::getCall() {
   return call;
}

