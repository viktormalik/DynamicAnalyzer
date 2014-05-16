/**
 * @file OutboundMsg.h
 *
 * @brief OutboundMsg class.
 *
 * Contains definition of class OutboundMsg representing outgoing message into tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 06.03.2014
 *
 * Created on: Feb 14, 2014
 */

#ifndef OUTBOUNDMSG_H_
#define OUTBOUNDMSG_H_

#include "Parser/Message.h"

#include <string>

/**
 * @brief Outgoing message into the tested program.
 * Represents outcoming message during main communication (not initial phase).
 * This message serves as response to incoming message.
 * Extends Message by implementing virtual method ::compose.
 */
class OutboundMsg: public Message {
public:
   /**
    * @brief Constructor with assigning type.
    * Usually used for type ACK.
    * @param t Type of the message.
    */
   OutboundMsg(msgTypes t);
   /**
    * @brief Constructor with initialization of type, function and variant.
    * Usually used for type EXECUTE.
    * @param t Type of the message.
    * @param f Function the message informs about.
    * @param var Variant that the tested program shall use.
    */
   OutboundMsg(msgTypes t, std::string f, int var);
   /**
    * @brief Default destructor.
    */
   virtual ~OutboundMsg();
   /**
    * @brief Parse incoming message.
    * @see Message::parse.
    * @param msg Message string to be parsed.
    * @warning Not implemented.
    */
   void parse(std::string msg);
   /**
    * @brief Compose message into string.
    * @see Message::compose
    * @return Message stirng.
    */
   std::string compose();
protected:
   std::string function; /**< Function the message informs about. Used only for type EXECUTE. */
   int variant; /**< Variant that the tested program shall use when performing current call.
                 * Used only for type EXECUTE.
                 */
};

#endif /* OUTBOUNDMSG_H_ */
