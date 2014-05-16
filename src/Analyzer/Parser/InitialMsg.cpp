/**
 * @file InitialMsg.cpp
 *
 * Contains implementation of methods of the class InitialMsg.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 01.04.2014
 *
 * Created on: Feb 15, 2014
 */

#include "Parser/InitialMsg.h"
#include "Call.h"
#include "Exceptions/ConfigurationException.h"

/**
 * @brief Default constructor.
 * Creates empty message with undefined type.
 */
InitialMsg::InitialMsg() :
      Message(), notifyType(NONE), notifyList(), controlType(NONE), controlList() {
}

/**
 * @brief Constructor with type initialization.
 * @param t Type of the message.
 */
InitialMsg::InitialMsg(msgTypes t) :
      Message(t), notifyType(NONE), notifyList(), controlType(NONE), controlList() {
}

/**
 * Default destructor.
 */
InitialMsg::~InitialMsg() {
}

/**
 * @brief Parse message from stirng.
 * @see Message::parse.
 * @param msg String to be parsed.
 * @warning Not implemented.
 */
void InitialMsg::parse(std::string msg) {
}

/**
 * @brief Compose message into string.
 * @see Message::compose.
 * @return Message string.
 */
std::string InitialMsg::compose() {
   std::string msg;
   // Write type + CRLF
   msg = Message::typesEnum[type];
   msg += "\r\n";

   if (type == OPTION) {
      // NOTIFICATION part
      msg += "NOTIFICATION\r\n";
      msg += listTEnum[notifyType];
      msg += "\r\n";

      // Write functions list (CRLF after each)
      for (auto fun : notifyList) {
         msg += fun;
         msg += "\r\n";
      }

      // CONTROL part
      msg += "CONTROL\r\n";
      msg += listTEnum[controlType];
      msg += "\r\n";

      // Write functions list (CRLF after each)
      for (auto fun : controlList) {
         msg += fun;
         msg += "\r\n";
      }
   }
   // Write ending CRLF
   msg += "\r\n";
   return msg;
}

/**
 * @brief Sets type of notification functions list.
 * @param t Type to be set from ::listType.
 */
void InitialMsg::setNotifyType(listType t) {
   notifyType = t;
}

/**
 * @brief Gets type of notification functions list.
 * @return List type from ::listType.
 */
listType InitialMsg::getNotifyType() {
   return notifyType;
}

/**
 * @brief Adds function to the notify list.
 * @param f Name of the function to be added.
 * @throws ConfigurationException if function is not valid (if the string is not found in
 * Call::functionsMap)
 */
void InitialMsg::addNotifyFunction(std::string f) {
   if (Call::functionExists(f))
      notifyList.push_back(f);
   else
      throw ConfigurationException("Invalid notify list");
}

/**
 * @brief Gets the notify functions list.
 * @return Vector containing the list.
 */
std::vector<std::string> InitialMsg::getNotifyFunctions() {
   return notifyList;
}

/**
 * @brief Sets type of control functions list.
 * @param t Type to be set from ::listType.
 */
void InitialMsg::setControlType(listType t) {
   controlType = t;
}

/**
 *
 * @brief Gets type of control functions list.
 * @return List type from ::listType.
 */
listType InitialMsg::getControlType() {
   return controlType;
}

/**
 * @brief Adds function to the control list.
 * @param f Name of the function to be added.
 * @throws ConfigurationException if function is not valid (if the string is not found in
 * Call::functionsMap)
 */
void InitialMsg::addControlFunction(std::string f) {
   if (Call::functionExists(f)){
      if (Call::isControlFunction(f))
         controlList.push_back(f);
      else
         throw ConfigurationException("Control list contains function that cannot be controlled");
   }
   else
      throw ConfigurationException("Invalid control list");
}

/**
 * @brief Gets the control functions list.
 * @return Vector containing the list.
 */
std::vector<std::string> InitialMsg::getControlFunctions() {
   return controlList;
}

/**
 * Initialization of static dictionary mapping list types from ::listType into their names as
 * strings.
 */
std::map<listType, std::string> InitialMsg::listTEnum = {
      { ALL, "ALL" },
      { NONE, "NONE" },
      { INCLUDE, "INCLUDE" }
};

