/**
 * @file Message.h
 *
 * @brief Message class.
 *
 * Contains definition of class Message, abstract class used for all message types.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 27.03.2014
 *
 * Created on: Feb 14, 2014
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <map>

/**
 * @brief Message types.
 *  Enum type with existing message types including undefined type used when creating new message
 * object.
 */
enum msgTypes {
   INIT, OPTION, CONTROL, NOTIFY, EXEC, ACK, RETURN, UNDEF
};

/**
 * @brief Message in the communication.
 * Abstract class representing any message in communication with tested program.
 */
class Message {
public:
   /**
    * Static dictionary mapping type names into type values from ::msgTypes.
    */
   static std::map<std::string, msgTypes> typesStr;
   /**
    * Static dictionary mapping type values from ::msgTypes into type names.
    */
   static std::map<msgTypes, std::string> typesEnum;

   /**
    * @brief Default constructor.
    * Initializes message type to undefined.
    */
   Message();
   /**
    * @brief Constructor with type initialization.
    * @param t Type of the message created.
    */
   Message(msgTypes t);
   /**
    * Default destructor.
    */
   virtual ~Message() {
   }

   /**
    * @brief Parse string with message.
    * Virtual method, used for parsing incoming message into the message object.
    * @param msg Message to be parsed.
    */
   virtual void parse(std::string msg) = 0;

   /**
    * @brief Compose message into string.
    * Virtual method, used for composing outcoming message into string.
    * @return Composed string.
    */
   virtual std::string compose() = 0;

   /**
    * @brief Get message type.
    * @return Message type value from ::msgTypes.
    */
   msgTypes getType();
   /**
    * @brief Sets message type.
    * @param t Type to be set. Is from ::msgTypes.
    */
   void setType(msgTypes t);
protected:
   msgTypes type; /**< Type of the message */
};

#endif /* MESSAGE_H_ */
