/**
 * @file Socket.cpp
 *
 * Implementation of methods of the class Socket.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 16.02.2014
 *
 * Created on: Feb 15, 2014
 */

#include "Socket.h"
#include "Exceptions/SocketException.h"
#include "Exceptions/SocketClosedException.h"
#include "Exceptions/ConfigurationException.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>

/**
 * @brief Constructor with specification of the named socket destination.
 * Creates local welcome socket (AF_UNIX type), binds it to the given destination and starts
 * listening on the socket.
 * @param s File destination where named socket is to be created.
 * @throws SocketException In case socket creating or binding fails.
 */
Socket::Socket(std::string s) :
      clientSocket(0) {
   // Create welcome socket
   if ((welcomeSocket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      throw SocketException("Creating socket failed");
   }
   // Create structure containing socket address
   struct sockaddr_un saddr;
   saddr.sun_family = AF_UNIX;
   strcpy(saddr.sun_path, s.c_str());

   // Unlink previous socket created in the path
   unlink(s.c_str());

   // Bind welcome socket to the local file
   if ((bind(welcomeSocket, (struct sockaddr *) &saddr, sizeof(saddr))) < 0) {
      throw SocketException("Binding socket failed");
   }
   // Listen on the socket
   listen(welcomeSocket, 1);
}

/**
 * @brief Default destructor.
 */
Socket::~Socket() {
}

/**
 * @brief Accepts connection from another process.
 * Accepts connection request to the welcome socket and creates new client socket which the
 * communication will be performed on.
 * Waits for the connection for 2 seconds, if no request arrives, it is considered that no tracked
 * function is called in tested program.
 * @throws SocketException In case accepting the connection fails.
 * @throws ConfigurationException In case timeout expires.
 */
void Socket::acceptConnection() {
   // Timeout
   struct timeval tv;
   tv.tv_sec = 2;
   tv.tv_usec = 0;
   // File descriptors set for select call
   fd_set readfds;
   FD_ZERO(&readfds);
   FD_SET(welcomeSocket, &readfds);
   // Wait for connection with timeout
   int selectedSockets = select(welcomeSocket + 1, &readfds, NULL, NULL, &tv);
   if (selectedSockets > 0) {
      // Accept connection
      if ((clientSocket = accept(welcomeSocket, NULL, NULL)) == -1) {
         throw SocketException("Accepting connection failed");
      }
   }
   else if (selectedSockets == 0)
      // Timeout expired
      throw ConfigurationException("Program calls none of selected functions");
   else
      throw SocketException("Accepting connection failed");
}

/**
 * @brief Send message through socket.
 * @param msg Message to be sent.
 * @throws SocketClosedException In case the socket has been closed be other side.
 * @throws SocketException In case an error occured during sending.
 */
void Socket::sendMsg(std::string msg) {
   if ((send(clientSocket, msg.c_str(), msg.length(), MSG_NOSIGNAL)) < 0) {
      if (errno == EPIPE)
         throw SocketClosedException();
      else
         throw SocketException("Sending the message failed");
   }
}

/**
 * @brief Receive message from socket.
 * @return Message received.
 * @throws SocketClosedException In case the socket has been closed by other side.
 */
std::string Socket::recvMsg() {
   char buf[250];
   if (recv(clientSocket, buf, 250, 0) != 0) {
      std::string msg(buf);
      return msg;
   }
   else
      throw SocketClosedException();
}

/**
 * @brief Closes welocme socket.
 */
void Socket::closeWelcomeSocket() {
   close(welcomeSocket);
}

/**
 * @brief Closes client socket.
 */
void Socket::closeClientSocket() {
   close(clientSocket);
}
