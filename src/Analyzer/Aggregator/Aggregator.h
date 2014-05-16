/**
 * @file Aggregator.h
 *
 * @brief Aggregator class.
 *
 * Contains definition of class Aggregator, which is responsible for aggregating calls from
 * different flows of tested program into control-flow graph.
 *
 * @author Viktor Malik <xmalik11@stud.fit.vutbr.cz>
 *
 * @date 03.04.2014
 *
 * Created on: Mar 12, 2014
 */

#ifndef AGGREGATOR_H_
#define AGGREGATOR_H_

#include "Aggregator/GraphNode.h"
#include "Call.h"
#include "Trace.h"
#include "JumpDetector.h"

#include <vector>
#include <string>
#include <fstream>

/**
 * @brief Aggregates calls from different runs of tested program into one graph.
 *
 * Every time a message with call arrives from tested program, aggregator is called and checks
 * whether such call has already arrived and in case not, it adds it into the graph.
 * Also is able to represent graph in various formats for output.
 * The graph structure can be modified by object of JumpDetector class which is therefore set as
 * friend class.
 */
class Aggregator {
public:
   /**
    * @brief Constructor with members initialization.
    * Creates the first empty node in the graph, which represents starting point of the program.
    * Also creates jumps detector with specified minimal subroutine size and opens output file.
    * @param subroutineSize Minimal subroutine size for jump detector.
    * @param path Destination of the output file.
    * @throws ConfigurationException if output file can not be opened.
    */
   Aggregator(unsigned subroutineSize, std::string path);
   /**
    * @brief Default destructor.
    * Destroys jump detector and all nodes in the graph.
    */
   virtual ~Aggregator();

   /**
    * @brief Adds new node into the graph if it does not exist yet.
    * First checks if current node (node that arrived last) has no such successor. If not, the
    * method creates new node with given call, pushes it to the end of vector containing graph
    * and creates necessary relations (updates successors and predecessors members of current node).
    * @param call Call to be stored in node.
    */
   void newNode(Call *call);
   /**
    * @brief Tells aggregator a new run will begin with next call.
    * Sets current node final and resets certain members of the object.
    */
   void nextRun();
   /**
    * @brief Checks if node has been inserted in the run.
    * @return @a True if any node has been inserted, otherwise @a false.
    */
   bool nodeInserted();

   /**
    * @brief Creates output from graph for program dot.
    * For every graph node, creates link to all its successors labeled by the successor call string.
    */
   void drawGraph();
   /**
    * @brief Creates output from graph in JSON syntax.
    * Creates list of nodes and list of links between these nodes.
    */
   void printJson();
protected:
   std::vector<GraphNode *> graph; /**< Vector of graph nodes representing graph. */
   int currentNode; /**< Index of current node - node with call that arrived as last. */
   bool _nodeInserted; /**< Flag set when a node is inserted and cleared before every new run. */
   std::vector<unsigned> insertedNodes; /**< List of inserted nodes into the graph during run. */
   JumpDetector *jumpDetector; /**< Jump detector responsible for finding jumps in the graph. */
   std::ofstream output; /**< File to write the output into */
private:
   /**
    * @brief Deletes last inserted node from graph.
    */
   void deleteLastNode();
   /**
    * @brief Checks whether call exists in graph.
    * Searches all successors of current call and compares them to the call given.
    * @param call Call to compare with.
    * @return If such call exists, returns its node index, otherwise return 0.
    */
   unsigned exists(Call *call);
   /**
    * @brief Converts call into its string representation.
    * This method is used to decide which calls are same and which are not.
    * Different aggregator types use different abstraction for creating this strings.
    * When creating new Aggregator type inherited from this class, this method must be redefined.
    * @param call Call to be converted.
    * @return String representation of the call.
    */
   virtual std::string toString(Call *call);
   /**
    * @brief Converts trace into its string representation.
    * @param trace Trace to be converted.
    * @return String representation of the trace.
    */
   std::string toString(Trace *trace);

   /**
    * JumpDetector is a friend class because it needs to manipulate the graph directly.
    */
   friend class JumpDetector;
};

#endif /* AGGREGATOR_H_ */
