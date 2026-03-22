#pragma once 

#include "Graph.h"
#include "DataEntities.h"

//Tiny namespace API
namespace MaxFlow {
    void addResidualEdge(Vertex<NodeInfo>* from, Vertex<NodeInfo>* to, int capacity);
    void resetSearchState(Graph<NodeInfo>& graph);
    bool bfsResidual(Graph<NodeInfo>& graph, Vertex <NodeInfo>* source, Vertex<NodeInfo>* sink);
    int edmondsKarp(Graph<NodeInfo>& graph, Vertex<NodeInfo>* source, Vertex<NodeInfo>* sink);
}