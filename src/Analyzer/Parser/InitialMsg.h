/**
 * @file InitialMsg.h
 *
 * @brief InitialMsg class.
 *
 * Contains definition of class InitialMsg which is used for messages sent during initialization
 * phase of the communication.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 01.04.2014
 *
 * Created on: Feb 15, 2014
 */

#ifndef INITIALMSG_H_
#define INITIALMSG_H_

#include "Parser/Message.h"

#include <vector>
#include <string>
#include <map>

/**
 * Types of the functions lists used in message of type INIT.
 */
enum listType {
   ALL, /**< All functions are included in the list */
   NONE, /**< No function is included in the list */
   INCLUDE /**< Only specified functions are included in the list */
};

/**
 * @brief Message used during initialization.
 *
 * Represents message used in initialization phase for sending options of the analysis to the
 * tested program (type OPTION).
 * Extends class Message by implementing method ::compose. Also adds numerous members related to
 * the message type OPTION.
 */
class InitialMsg: public Message {
public:
   /**
    * @brief Default constructor.
    * Creates empty message with undefined type.
    */
   InitialMsg();
   /**
    * @brief Constructor with type initialization.
    * @param t Type of the message.
    */
   InitialMsg(msgTypes t);
   /**
    * Default destructor.
    */
   virtual ~InitialMsg();
   /**
    * @brief Parse message from stirng.
    * @see Message::parse.
    * @param msg String to be parsed.
    * @warning Not implemented.
    */
   void parse(std::string msg);
   /**
    * @brief Compose message into string.
    * @see Message::compose.
    * @return Message string.
    */
   std::string compose();

   /**
    * @brief Sets type of notification functions list.
    * @param t Type to be set from ::listType.
    */
   void setNotifyType(listType t);
   /**
    * @brief Gets type of notification functions list.
    * @return List type from ::listType.
    */
   listType getNotifyType();
   /**
    * @brief Sets type of control functions list.
    * @param t Type to be set from ::listType.
    */
   void setControlType(listType t);
   /**
    *
    * @brief Gets type of control functions list.
    * @return List type from ::listType.
    */
   listType getControlType();
   /**
    * @brief Adds function to the notify list.
    * @param f Name of the function to be added.
    * @throws ConfigurationException if function is not valid (if the string is not found in
    * Call::functionsMap)
    */
   void addNotifyFunction(std::string f);
   /**
    * @brief Gets the notify functions list.
    * @return Vector containing the list.
    */
   std::vector<std::string> getNotifyFunctions();
   /**
    * @brief Adds function to the control list.
    * @param f Name of the function to be added.
    */
   void addControlFunction(std::string f);
   /**
    * @brief Gets the control functions list.
    * @return Vector containing the list.
    * @throws ConfigurationException if function is not valid (if the string is not found in
    * Call::functionsMap)
    */
   std::vector<std::string> getControlFunctions();

   /**
    * Static dictionary mapping list type into its string representation.
    */
   static std::map<listType, std::string> listTEnum;
protected:
   listType notifyType; /**< Information telling execution of which functions the controller will
                         * be notified about. */
   std::vector<std::string> notifyList; /**< In case notify type is INCLUDE, list of functions
                                         * included in this list. */
   listType controlType; /**< Information telling execution of which functions the controller will
                          * be able to control. */
   std::vector<std::string> controlList; /**< In case control type is INCLUDE, list of functions
                                          * included in this list. */
};

#endif /* INITIALMSG_H_ */
