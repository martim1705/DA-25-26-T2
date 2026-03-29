#pragma once 

#include "Graph.h"
#include "DataEntities.h"

//Tiny namespace API
namespace MaxFlow {
    /**
     * @brief Cria um par de arestas residuais (forward e backward) entre dois vértices.
     * @param from O vértice de origem.
     * @param to O vértice de destino.
     * @param capacity A capacidade máxima de fluxo da aresta forward.
     * @note Complexidade Temporal: O(1)
     */
    void addResidualEdge(Vertex<NodeInfo>* from, Vertex<NodeInfo>* to, int capacity);

    /**
     * @brief Faz reset ao estado (visitado e caminho) de todos os vértices do grafo para uma nova travessia BFS.
     * @param graph O grafo da rede de fluxo.
     * @note Complexidade Temporal: O(V) onde V é o número de vértices.
     */
    void resetSearchState(Graph<NodeInfo>& graph);

    /**
      * @brief Executa uma pesquisa em largura (BFS) para encontrar um caminho de aumento no grafo residual.
      * @param graph O grafo da rede de fluxo.
      * @param source O vértice fonte (source).
      * @param sink O vértice sumidouro (sink).
      * @return Verdadeiro (true) se encontrar um caminho de aumento até ao sink, falso caso contrário.
      * @note Complexidade Temporal: O(V + E) onde V é o número de vértices e E o número de arestas.
      */
    bool bfsResidual(Graph<NodeInfo>& graph, Vertex <NodeInfo>* source, Vertex<NodeInfo>* sink);

    /**
     * @brief Calcula o fluxo máximo de uma rede utilizando o algoritmo de Edmonds-Karp.
     * @param graph O grafo da rede de fluxo.
     * @param source O vértice fonte (source).
     * @param sink O vértice sumidouro (sink).
     * @return O valor inteiro do fluxo máximo encontrado.
     * @note Complexidade Temporal: O(V * E^2) onde V é o número de vértices e E o número de arestas.
     */
    int edmondsKarp(Graph<NodeInfo>& graph, Vertex<NodeInfo>* source, Vertex<NodeInfo>* sink);
}