/**
 * @file Aggregator.cpp
 *
 * Contains implementation of methods of the class Aggregator.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 03.04.2014
 *
 * Created on: Mar 12, 2014
 */

#include "Aggregator/Aggregator.h"
#include "Exceptions/ConfigurationException.h"

#include <fstream>

/**
 * @brief Constructor with members initialization.
 * Creates the first empty node in the graph, which represents starting point of the program.
 * Also creates jumps detector with specified minimal subroutine size and opens output file.
 * @param subroutineSize Minimal subroutine size for jump detector.
 * @throws ConfigurationException if output file can not be opened.
 */
Aggregator::Aggregator(unsigned subroutineSize, std::string path) :
      graph(), currentNode(0), _nodeInserted(false) {
   // Create and push back first empty node
   graph.push_back(new GraphNode(NULL));
   // Create jump detector
   if (subroutineSize > 1)
      jumpDetector = new JumpDetector(subroutineSize, this);
   else
      jumpDetector = NULL;
   // Open output file for writing
   output.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
   if (!output.is_open())
      throw ConfigurationException("Destination file could not be opened");

}

/**
 * @brief Default destructor.
 * Destroys jump detector and all nodes in the graph.
 */
Aggregator::~Aggregator() {
   delete jumpDetector;
   for (auto node : graph) {
      delete node;
   }
}

/**
 * @brief Adds new node into the graph if it does not exist yet.
 * First checks if current node (node that arrived last) has no such successor. If not, the
 * method creates new node with given call, pushes it to the end of vector containing graph
 * and creates necessary relations (updates successors and predecessors members of current node).
 * @param call Call to be stored in node.
 */
void Aggregator::newNode(Call *call) {
   unsigned newIndex;
   if (graph[currentNode]->getSuccessors().empty() || !(newIndex = exists(call))) {
      // If current node has no successor, or does not have same successor, create new one
      GraphNode *newNode = new GraphNode(call);
      // Insert new node into graph
      graph.push_back(newNode);
      newIndex = graph.size() - 1;
      // Create relationships for new node
      graph[currentNode]->addSuccessor(newIndex);
      graph[newIndex]->setPredecessor(currentNode);
      // Note node insertion
      _nodeInserted = true;
      insertedNodes.push_back(newIndex);
      // Move to next node
      currentNode = newIndex;
      // Call jump detector to find potential jumps
      if (jumpDetector != NULL)
         jumpDetector->findJumps();
   }
   else
      // If such node already exists, move to next node
      currentNode = newIndex;
}

/**
 * @brief Tells aggregator a new run will begin with next call.
 * Sets current node final and resets certain members of the object.
 */
void Aggregator::nextRun() {
   graph[currentNode]->setFinal();
   currentNode = 0;
   _nodeInserted = false;
   insertedNodes.clear();
}

/**
 * @brief Checks if node has been inserted in the run.
 * @return @a True if any node has been inserted, otherwise @a false.
 */
bool Aggregator::nodeInserted() {
   return _nodeInserted;
}

/**
 * @brief Creates output from graph for program dot.
 * For every graph node, creates link to all its successors labeled by the successor call string.
 */
void Aggregator::drawGraph() {
   // Write graph header
   output << "digraph g {" << std::endl;

   output << "  0" << std::endl;
   // Write graph content
   for (unsigned i = 0; i < graph.size(); ++i) {
      // Links to successors
      for (auto succ : graph[i]->getSuccessors()) {
         output << "  " << i << " -> " << succ << " [label=\"" << toString(graph[succ]->getCall())
               << "\"]" << std::endl;
      }
      // Link to final state
      if (graph[i]->isFinal())
         output << "  " << i << " -> " << "F" << " [label=\"&#949;\"]" << std::endl;
   }

   // Write graph footer
   output << "}" << std::endl;
}

/**
 * @brief Creates output from graph in JSON syntax.
 * Creates list of nodes and list of links between these nodes.
 */
void Aggregator::printJson() {
   // Start of JSON object
   output << "{" << std::endl;
   // Write nodes
   output << "  \"nodes\" : [" << std::endl;
   for (unsigned i = 0; i < graph.size(); ++i) {
      output << "    {" << std::endl;
      output << "      \"id\" : " << i << "," << std::endl;
      output << "      \"label\" : \"" << i << "\"" << std::endl;
      output << "    }";
      output << ",";
      output << std::endl;
   }
   // Final node
   int finalId = graph.size();
   output << "    {" << std::endl;
   output << "      \"id\" : " << finalId << "," << std::endl;
   output << "      \"label\" : \"F\"" << std::endl;
   output << "    }";
   output << std::endl;

   output << "  ]," << std::endl;

   // Write edges
   output << "  \"links\" : [" << std::endl;
   bool isFirst = true;
   for (unsigned i = 0; i < graph.size(); ++i) {
      for (auto link : graph[i]->getSuccessors()) {
         if (!isFirst) {
            output << ",";
            output << std::endl;
         }
         output << "    {" << std::endl;
         output << "      \"source\" : " << i << "," << std::endl;
         output << "      \"target\" : " << link << "," << std::endl;
         output << "      \"label\" : \"" << toString(graph[link]->getCall()) << "\"" << std::endl;
         output << "    }";
         isFirst = false;
      }
      if (graph[i]->isFinal()){
         output << ",";
         output << std::endl;
         output << "    {" << std::endl;
         output << "      \"source\" : " << i << "," << std::endl;
         output << "      \"target\" : " << finalId << "," << std::endl;
         output << "      \"label\" : \"\"" << std::endl;
         output << "    }";
      }
   }
   output << std::endl;
   output << "  ]" << std::endl;
   // End of JSON object
   output << "}" << std::endl;
}

/**
 * @brief Gets the number of nodes in graph.
 * @return Number of nodes.
 */
unsigned Aggregator::nodesCount(){
   return graph.size();
}

/**
 * @brief Deletes last inserted node from graph.
 */
void Aggregator::deleteLastNode() {
   GraphNode *node = graph.back();
   graph.pop_back();
   delete node;
}

/**
 * @brief Checks whether call exists in graph.
 * Searches all successors of current call and compares them to the call given.
 * @param call Call to compare with.
 * @return If such call exists, returns its node index, otherwise return 0.
 */
unsigned Aggregator::exists(Call *call) {
   for (auto succ : graph[currentNode]->getSuccessors()) {
      if (toString(graph[succ]->getCall()) == toString(call))
         return succ;
   }
   return 0;
}

/**
 * @brief Converts call into its string representation.
 * This method is used to decide which calls are same and which are not.
 * Different aggregator types use different abstraction for creating this strings.
 * Basic aggregator uses only call function name as output stirng.
 * When creating new Aggregator type inherited from this class, this method must be redefined.
 * @param call Call to be converted.
 * @return String representation of the call.
 */
std::string Aggregator::toString(Call *call) {
   return call->getName();
}

/**
 * @brief Converts trace into its string representation.
 * @param trace Trace to be converted.
 * @return String representation of the trace.
 */
std::string Aggregator::toString(Trace *trace) {
   std::string result;
   for (auto call : trace->getCalls()) {
      result += toString(call);
      result += ";";
   }
   return result;
}