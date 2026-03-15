#include "CentralManager.h"
/**SubSets*/
void CentralManager::addSubmission(const int id, const NodeInfo& info) {
    // 1. Inserir o nó fisicamente no Grafo
    graph.addVertex(info);

    // 2. Procurar o ponteiro exato que o Grafo acabou de criar e guardá-lo no mapa
    submissions[id] = graph.findVertex(info);
}

void CentralManager::addReviewer(const int id, const NodeInfo& info) {
    // 1. Inserir o nó fisicamente no Grafo
    graph.addVertex(info);

    // 2. Guardar o ponteiro no respetivo mapa
    reviewers[id] = graph.findVertex(info);
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
void CentralManager::runBatchMode(const std::string &string, const std::string &risk_file) {
    std::cout << "Batch Mode\n";
}



void CentralManager::runInteractiveMenu() {
    int op;
    bool running = true;
    while (running) {

        std::cout << "------------------------------------\n";

        std::cout << "\t1 - Load input file\t\n";
        std::cout << "\t2 - Show submissions\t\n";
        std::cout << "\t3 - Show reviewers\t\n";
        std::cout << "\t4 - Load input file\t\n";
        std::cout << "\t5 - Show submissions\t\n";
        std::cout << "\t6 - Show reviewers\t\n";
        std::cout << "\t7 - Load input file\t\n";
        std::cout << "\t8 - Show submissions\t\n";
        std::cout << "\t0 - Exit\t";

        std::cout << "\n------------------------------------\n";
        std::cout << "Operation: ";
        std::cin >> op;

        switch(op) {

            case 0:
                running = false;
                break;

            case 1:
                break;

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
        }
    }
}
