/**
 * @file GraphNode.h
 *
 * @brief GraphNode class.
 *
 * Contains definition of the class GraphNode which represents a node in control-flow graph of the
 * tested program.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 15.03.2014
 *
 * Created on: Mar 12, 2014
 */

#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include "Call.h"

#include <vector>
#include <iostream>

/**
 * @brief Node in control-flow graph.
 * Class representing a node in control-flow graph (graph of calls) of the tested program.
 * Each node can have multiple successors, but always only one predecessor.
 */
class GraphNode {
public:
   /**
    * @brief Constructor with call initialization.
    * Creates new node and initializes its call. The call is not in the graph yet, because has no
    * successors or predecessor.
    * @param c Call to be stored in the node.
    */
   GraphNode(Call *c);
   /**
    * @brief Default destructor.
    */
   virtual ~GraphNode();

   /**
    * @brief Adds new successor of the node.
    * @param index Index of the successor node.
    */
   void addSuccessor(unsigned index);
   /**
    * @brief Gets all successors of the node.
    * @return Vector of successors indexes in graph.
    */
   std::vector<unsigned> getSuccessors();
   /**
    * @brief Deletes last successor of the node.
    * If node has no successors, does nothing.
    */
   void deleteSuccessor();
   /**
    * @brief Gets the node call.
    * @return Pointer to the call stored in the node.
    */
   Call *getCall();
   /**
    * @brief Sets predecessor of the node.
    * @param p Index of predecessor in graph.
    */
   void setPredecessor(unsigned p);
   /**
    * @brief Gets predecessor of the node.
    * @return Predecessor index in the graph.
    */
   unsigned getPredecessor();
   /**
    * @brief Sets node to be final.
    */
   void setFinal();
   /**
    * @brief Checks if the node is final.
    * @return @a True if node is final, otherwise @a false.
    */
   bool isFinal();
protected:
   Call *call; /**< Call stored in the node */
   std::vector<unsigned> successors; /**< Vector of node's successors indexes in the graph */
   unsigned predecessor; /**< Node's predecessor index */
   bool final; /**< Flag if the node is final */
};

#endif /* GRAPHNODE_H_ */
