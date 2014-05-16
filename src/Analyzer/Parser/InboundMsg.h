/**
 * @file InboundMsg.h
 *
 * @brief InboundMsg class.
 *
 * Contains class InboundMsg representing incoming message in the communication.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 16.02.2014
 *
 * Created on: Feb 14, 2014
 */

#ifndef INBOUNDMSG_H_
#define INBOUNDMSG_H_

#include "Parser/Message.h"
#include "Call.h"

/**
 * @brief Incoming message from tested program.
 * Class representing incoming messages during main communication and initial phase.
 * This message informs about a call that is to be performed in the tested program.
 * Extends Message and implements it virtual method ::parse.
 */
class InboundMsg: public Message {
public:
   /**
    * @brief Default constructor.
    * Creates empty message.
    */
   InboundMsg();
   /**
    * @brief Default destructor.
    */
   virtual ~InboundMsg() {
   }
   /**
    * @brief Parses incoming message.
    * @see Message::parse.
    * @param msg Stirng to be parsed.
    */
   void parse(std::string msg);
   /**
    * @brief Composes message into string.
    * @see Message::compose
    * @return String composed.
    * @warning Not implemented.
    */
   std::string compose();
   /**
    * @brief Gets the call contained in the message.
    * @return Pointer to the call.
    */
   Call *getCall();
protected:
   Call *call; /**< Call that the message informed about. */
};

#endif /* INBOUNDMSG_H_ */
