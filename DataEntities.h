#pragma once
#include <string>

/// @brief Enumeração de cada tipo de nó para o grafo de fluxo.
enum class NodeType{source,sink,submission,reviewer};

/// @brief Estrutura de dados que guarda a informação de cada nó.
struct NodeInfo{

    NodeType type;
    int id;

    int primaryDomain;
    int secondaryDomain;

    std::string title;
    std::string authors;
    std::string reviewerName;
    std::string email;

    ///@brief  Função necessária para encontrar nós comparando id e tipo.
    bool operator==(const NodeInfo& other) const {
        return (this->id == other.id) && (this->type == other.type);
    }
};