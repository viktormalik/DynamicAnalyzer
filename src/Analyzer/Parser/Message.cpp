/**
 * @file Message.cpp
 *
 * Contains implementation and initialization of members of class Message.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 27.03.2014
 *
 * Created on: Feb 14, 2014
 */

#include "Message.h"

/**
 * @brief Default constructor.
 * Initializes message type to undefined.
 */
Message::Message() :
      type(UNDEF) {
}

/**
 * @brief Constructor with type initialization.
 * @param t Type of the message created.
 */
Message::Message(msgTypes t) :
      type(t) {
}

/**
 * @brief Get message type.
 * @return Message type value from ::msgTypes.
 */
msgTypes Message::getType() {
   return type;
}

/**
 * @brief Sets message type.
 * @param t Type to be set. Is from ::msgTypes.
 */
void Message::setType(msgTypes t) {
   type = t;
}

/**
 * Initialization of static maps between type names and type values from ::msgTypes.
 */
std::map<std::string, msgTypes> Message::typesStr = {
      { "INIT", INIT },
      { "OPTION", OPTION },
      { "CONTROL", CONTROL },
      { "NOTIFY", NOTIFY },
      { "EXEC", EXEC },
      { "ACK", ACK },
      { "RETURN", RETURN }
};

std::map<msgTypes, std::string> Message::typesEnum = {
      { INIT, "INIT" },
      { OPTION, "OPTION" },
      { CONTROL, "CONTROL" },
      { NOTIFY, "NOTIFY" },
      { EXEC, "EXEC" },
      { ACK, "ACK" },
      { RETURN, "RETURN" }
};
