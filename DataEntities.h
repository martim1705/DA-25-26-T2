#pragma once
#include <string>

enum class NodeType{source,sink,submission,reviewer};

struct NodeInfo{

    NodeType type;
    int id;

    int primaryDomain;
    int secondaryDomain;

    std::string title;
    std::string authors;
    std::string reviewerName;
    std::string email;

    bool operator==(const NodeInfo& other) const {
        return (this->id == other.id) && (this->type == other.type);
    }
};