/**
 * @file Socket.h
 *
 * @brief Socket class.
 *
 * Contains definition of the class Socket, which assures a socket connection to the process with
 * tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 16.02.2014
 *
 * Created on: Feb 15, 2014
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>

/**
 * @brief Socket connection.
 *
 * Class assuring socket connection to the process with tested program. Also it is responsible
 * for communication through this socket (sending and receiving messages).
 * Named socket is used.
 */
class Socket {
public:
   /**
    * @brief Constructor with specification of the named socket destination.
    * Creates local welcome socket (AF_UNIX type), binds it to the given destination and starts
    * listening on the socket.
    * @param s File destination where named socket is to be created.
    * @throws SocketException In case socket creating or binding fails.
    */
   Socket(std::string s);
   /**
    * @brief Default destructor.
    */
   virtual ~Socket();

   /**
    * @brief Accepts connection from another process.
    * Accepts connection request to the welcome socket and creates new client socket which the
    * communication will be performed on.
    * Waits for the connection for 2 seconds, if no request arrives, it is considered that no tracked
    * function is called in tested program.
    * @throws SocketException In case accepting the connection fails.
    * @throws ConfigurationException In case timeout expires.
    */
   void acceptConnection();
   /**
    * @brief Send message through socket.
    * @param msg Message to be sent.
    * @throws SocketClosedException In case the socket has been closed be other side.
    * @throws SocketException In case an error occured during sending.
    */
   void sendMsg(std::string msg);
   /**
    * @brief Receive message from socket.
    * @return Message received.
    */
   std::string recvMsg();
   /**
    * @brief Closes welocme socket.
    */
   void closeWelcomeSocket();
   /**
    * @brief Closes client socket.
    */
   void closeClientSocket();
protected:
   int welcomeSocket; /**< Welcome socket used for accepting connection request. */
   int clientSocket; /**< Socket where the communication is performed. */
};

#endif /* SOCKET_H_ */
