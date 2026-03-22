#include "CentralManager.h"
#include "Parser.h"
#include "MaxFlow.h"
#include <algorithm>
#include <fstream>
#include <iostream>



void CentralManager::buildPrimaryOnlyNetwork(
    Graph<NodeInfo>& network,
    std::unordered_map<int, Vertex<NodeInfo>*>& netSubs,
    std::unordered_map<int, Vertex<NodeInfo>*>& netRevs,
    Vertex<NodeInfo>*& source,
    Vertex<NodeInfo>*& sink,
    int excludedReviewerId
) const {
    // Create source and sink nodes for the flow network.
    NodeInfo src{NodeType::source, -1, -1, -1, "", "", "", ""};
    NodeInfo snk{NodeType::sink, -2, -1, -1, "", "", "", ""};

    network.addVertex(src);
    network.addVertex(snk);

    source = network.findVertex(src);
    sink = network.findVertex(snk);

     // Add reviewer vertices, optionally skipping one reviewer for risk analysis.
    for (const auto& [id, v] : reviewers) {
        if (id == excludedReviewerId) continue;

        NodeInfo info = v->getInfo();
        network.addVertex(info);
        netRevs[id] = network.findVertex(info);
    }
    //std::cout << "first for loop passed...\n";
    // Add submission vertices to the flow network.
    for (const auto& [id, v] : submissions) {
        NodeInfo info = v->getInfo();
        network.addVertex(info);
        netSubs[id] = network.findVertex(info);
    }

    // Limit the maximum number of assignments per reviewer.
    for (const auto& [rid, rv] : netRevs) {
        MaxFlow::addResidualEdge(source, rv, maxReviewsPerReviewer);
    }

    // Connect reviewers to submissions when their primary domains match.
    for (const auto& [rid, rv] : netRevs) {
        for (const auto& [sid, sv] : netSubs) {
            if (rv->getInfo().primaryDomain == sv->getInfo().primaryDomain) {
                MaxFlow::addResidualEdge(rv, sv, 1);
            }
        }
    }

    // Require each submission to receive the minimum number of reviews.
    for (const auto& [sid, sv] : netSubs) {
        MaxFlow::addResidualEdge(sv, sink, minReviewsPerSubmission);
    }
}
/**Caller to LoadFiles*/
bool CentralManager::loadFiles(const std::string &filename) {
    Parser parser(*this);
    const bool success = parser.loadData(filename);
    if (!success) {
        std::cerr<<"Error: CentralManager Failed to Load Data from"<< filename<<"\n"<<std::endl;
    }
    std::cout << "loadFiles() passed.\n"; // correto. erro não é aqui
    return success;

}
/**SubSets*/
void CentralManager::addSubmission(const int id, const NodeInfo& info) {
    // 1. Inserir o nó fisicamente no Grafo
    graph.addVertex(info);

    // 2. Procurar o ponteiro exato que o Grafo acabou de criar e guardá-lo no mapa
    submissions[id] = graph.findVertex(info);
}

bool CentralManager::hasSubmission(const int id) const {
    return submissions.find(id) != submissions.end();
}


void CentralManager::addReviewer(const int id, const NodeInfo& info) {
    // 1. Inserir o nó fisicamente no Grafo
    graph.addVertex(info);

    // 2. Guardar o ponteiro no respetivo mapa
    reviewers[id] = graph.findVertex(info);
}

bool CentralManager::hasReviewer(const int id) const {
    return reviewers.find(id) != reviewers.end();
}

const std::unordered_map<int, Vertex<NodeInfo>*>& CentralManager::getSubmissions() const {
    return this->submissions;
}

const std::unordered_map<int, Vertex<NodeInfo>*>& CentralManager::getReviewers() const {
    return this->reviewers;
}

/**Parametros*/
void CentralManager::setMinReviewsPerSubmission(const int value) {
    this->minReviewsPerSubmission = value;
}
int CentralManager::getMinReviewsPerSubmission() const {
    return this->minReviewsPerSubmission;
}

void CentralManager::setMaxReviewsPerReviewer(const int value) {
    this->maxReviewsPerReviewer = value;
}
int CentralManager::getMaxReviewsPerReviewer() const {
    return this->maxReviewsPerReviewer;
}


/**Parâmetros Redundantes até tirar dúvida**/
void CentralManager::setPrimaryReviewerExpertise(const bool value) {
    this->primaryReviewerExpertise = value;
}

void CentralManager::setSecondaryReviewerExpertise(const bool value) {
    this->secondaryReviewerExpertise = value;
}

void CentralManager::setPrimarySubmissionDomain(const bool value) {
    this->primarySubmissionDomain = value;
}

void CentralManager::setSecondarySubmissionDomain(const bool value) {
    this->secondarySubmissionDomain = value;
}

/**Parametros de Controlo*/
void CentralManager::setGenerateAssignments(const int gen) {
    this->GenerateAssignments = gen;
}

void CentralManager::setRiskAnalysis(const int risk) {
    this->RiskAnalysis = risk;
}

void CentralManager::setOutputFilename(const std::string& name) {
    this->outputFilename = name;
}


// Menus
void CentralManager::runBatchMode(const std::string &input_file, const std::string &risk_file) {
    std::cout << "Batch Mode\n";
    if (!loadFiles("../input/" + input_file)) {
        std::cout << "Error: loadFiles() error on file: " << input_file << "\n";
        return;
    }
    std::cout << "Done\n";

}
// teste



void CentralManager::runInteractiveMenu() {
    int op;
    bool running = true;
    while (running) {

        std::cout << "------------------------------------\n";

        std::cout << "\t1 - Load input file\t\n";
        std::cout << "\t2 - Show submissions\t\n";
        std::cout << "\t3 - Show reviewers\t\n";
        std::cout << "\t4 - Show parameters\t\n";
        std::cout << "\t5 - Run Assignment Algorithm\t\n";
        std::cout << "\t6 - Run Risk Analysis\t\n";
        std::cout << "\t7 - Export Results\t\n";
        std::cout << "\t0 - Exit\t";

        std::cout << "\n------------------------------------\n";
        std::cout << "Operation: ";
        std::cin >> op;

        std::string filename;
        switch(op) {

            case 0:
                running = false;
                break;

            case 1:

                std::cout << "Select input filename: ";
                std::cin >> filename;
                if (!loadFiles("./input/" + filename)) {
                    std::cout << "Error: Could not load file " << filename << "\n";
                    break;
                }
                auto* network = new Graph<NodeInfo>();
                std::unordered_map<int, Vertex<NodeInfo>*> netSubs;
                std::unordered_map<int, Vertex<NodeInfo>*> netRevs;
                Vertex<NodeInfo>* source;
                Vertex<NodeInfo>* sink;
                int excludedReviewerId = 0;

                //std::cout << "Starting buildPrimaryOnlyNetwork()...\n";
                std::cout << network << "\n";
                buildPrimaryOnlyNetwork(*network,netSubs,netRevs,source, sink,excludedReviewerId);
                if (GenerateAssignments != 0) { // Extract assignments
                    std::cout << "GenerateAssignments = " << GenerateAssignments << "\n";
                    MaxFlow::edmondsKarp(*network,source,sink);
                    printNetwork(*network);
                    delete network;
                    break;
                }

                //std::cout << "buildPrimaryOnlyNetwork() passed.\n";
                MaxFlow::edmondsKarp(*network,source,sink);
                printNetwork(*network);
                std::cout << "DONE!\n";
                delete network;
                break;
/*
            case 2:
                break;

            case 3:
                break;

            case 4:
                break;

            case 5:
                break;

            case 6:
                break;

            case 7:
                break;

            case 8:
                break;

            default:

                std::cout << "Operation unavailable\n";
                */
        }
    }
}

// Usada para testar apenas. Verificar se existe nó source e sink
void CentralManager::printNetwork(Graph<NodeInfo>& network) const {
    for (auto v : network.getVertexSet()) {
        NodeInfo info = v->getInfo();

        std::cout << "Node ID: " << info.id << " | Type: ";

        if (info.type == NodeType::submission) std::cout << "Submission";
        else if (info.type == NodeType::reviewer) std::cout << "Reviewer";
        else if (info.type == NodeType::source) std::cout << "Source";
        else if (info.type == NodeType::sink) std::cout << "Sink";

        std::cout << "\n";

        // Imprimir arestas (se existirem)
        for (auto e : v->getAdj()) {
            auto dest = e->getDest()->getInfo();

            std::cout << "   -> " << dest.id
                      << " (capacidade: " << e->getWeight()
                      << ", flow: " << e->getFlow() << ")\n";
        }

        std::cout << "\n";
    }
}


const std::vector<std::tuple<int,int,int>> extractAssignment(const std::unordered_map<int, Vertex<NodeInfo>*>& netRevs) {


    std::vector<std::tuple<int,int,int>> result;

    for (auto [rid, rv] : netRevs) {
        for (auto e : rv->getAdj()) {
            if (e->getFlow() > 0) {
                auto dest = e->getDest()->getInfo();
                if (dest.type == NodeType::submission) {
                    int submissionId = dest.id;
                    int reviewerID = rv->getInfo().id;
                    int domain = dest.primaryDomain;

                    result.emplace_back(submissionId, reviewerID,domain);
                }
            }
        }
    }
    return result;
}