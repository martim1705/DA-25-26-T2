#include "MaxFlow.h"
#include <queue>
#include <algorithm>

namespace MaxFlow {
    void addResidualEdge(Vertex<NodeInfo>* from, Vertex<NodeInfo>* to, int capacity){
        Edge<NodeInfo>* forward = from->addEdge(to, capacity);
        Edge<NodeInfo>* backward = to->addEdge(from,0);

        forward->setReverse(backward);
        backward->setReverse(forward);

        forward->setFlow(0);
        backward->setFlow(0);
    }

    void resetSearchState(Graph<NodeInfo>& graph){
        for (auto v : graph.getVertexSet()){
            v->setVisited(false);
            v->setPath(nullptr);
        }
    }

    bool bfsResidual(Graph<NodeInfo>& graph, Vertex<NodeInfo>* source, Vertex<NodeInfo>* sink){
        resetSearchState(graph);

        std::queue<Vertex<NodeInfo>*> q;
        source->setVisited(true);
        q.push(source);

        while(!q.empty()){
            auto v = q.front();
            q.pop();

            for (auto e : v->getAdj()){
                double residual = e->getWeight() - e->getFlow();
                auto w = e->getDest();
                
                if (!w->isVisited() && residual > 0){
                    w->setVisited(true);
                    w->setPath(e);

                    if (w == sink) return true;
                    q.push(w);
                }
            }
        }

        return false;

    }

    int edmondsKarp(Graph<NodeInfo>& graph, Vertex<NodeInfo>* source, Vertex<NodeInfo>* sink){
        int maxFlow = 0;

        while (bfsResidual(graph, source, sink)){
            int bottle = std::numeric_limits<int>::max();

            for (auto v = sink; v!= source; v = v->getPath()->getOrig()){
                auto e = v->getPath();
                bottle = std::min(bottle, static_cast<int>(e->getWeight() - e->getFlow()));
            }

            for (auto v = sink; v != source; v = v->getPath()->getOrig()){
                auto e = v->getPath();
                e->setFlow(e->getFlow() + bottle);
                e->getReverse()->setFlow(e->getReverse()->getFlow() - bottle);
            }

            maxFlow += bottle;
        }
        
        return maxFlow;
    }
}