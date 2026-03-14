#include <string> 

enum class NodeType{source,sink,submission,reviewer};

struct Node{

    NodeType type;
    int id;

    int primaryDomain;
    int secondaryDomain;

    std::string title;
    std::string authors;
    std::string reviewerName;
    std::string email;
};