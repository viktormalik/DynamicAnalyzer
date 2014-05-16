/**
 * @file GraphNode.cpp
 *
 * Contains implementation of methods of the class GraphNode.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 15.03.2014
 *
 * Created on: Mar 12, 2014
 */

#include "Aggregator/GraphNode.h"

#include <algorithm>

/**
 * @brief Constructor with call initialization.
 * Creates new node and initializes its call. The call is not in the graph yet, because has no
 * successors or predecessor.
 * @param c Call to be stored in the node.
 */
GraphNode::GraphNode(Call *c) :
      call(c), successors(), predecessor(0), final(false) {
}

/**
 * @brief Default destructor.
 */
GraphNode::~GraphNode() {
}

/**
 * @brief Adds new successor of the node.
 * @param index Index of the successor node.
 */
void GraphNode::addSuccessor(unsigned index) {
   successors.push_back(index);
}

/**
 * @brief Gets the node call.
 * @return Pointer to the call stored in the node.
 */
Call *GraphNode::getCall() {
   return call;
}

/**
 * @brief Gets all successors of the node.
 * @return Vector of successors indexes in graph.
 */
std::vector<unsigned> GraphNode::getSuccessors() {
   return successors;
}

/**
 * @brief Deletes last successor of the node.
 * If node has no successors, does nothing.
 */
void GraphNode::deleteSuccessor() {
   if (!successors.empty())
      successors.pop_back();
}

/**
 * @brief Sets predecessor of the node.
 * @param p Index of predecessor in graph.
 */
void GraphNode::setPredecessor(unsigned p) {
   predecessor = p;
}

/**
 * @brief Gets predecessor of the node.
 * @return Predecessor index in the graph.
 */
unsigned GraphNode::getPredecessor() {
   return predecessor;
}

/**
 * @brief Sets node to be final.
 */
void GraphNode::setFinal() {
   final = true;
}

/**
 * @brief Checks if the node is final.
 * @return @a True if node is final, otherwise @a false.
 */
bool GraphNode::isFinal() {
   return final;
}

