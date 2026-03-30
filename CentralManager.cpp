#include "CentralManager.h"
#include "Parser.h"
#include "MaxFlow.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>


namespace {
    // Junta os IDs numa única string para mostrar ou escrever no ficheiro de risco.
    std::string joinReviewerIds(const std::vector<int>& reviewerIds) {
        std::ostringstream oss;
        for (std::size_t i = 0; i < reviewerIds.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << reviewerIds[i];
        }
        return oss.str();
    }
}

int CentralManager::getMatchDomain(
    const NodeInfo& reviewer,
    const NodeInfo& submission
) const {
    if (primaryReviewerExpertise) {
        if (primarySubmissionDomain &&
            reviewer.primaryDomain == submission.primaryDomain) {
            return reviewer.primaryDomain;
        }

        if (secondarySubmissionDomain &&
            reviewer.primaryDomain == submission.secondaryDomain) {
            return reviewer.primaryDomain;
        }
    }

    if (secondaryReviewerExpertise && reviewer.secondaryDomain >= 0) {
        if (primarySubmissionDomain &&
            reviewer.secondaryDomain == submission.primaryDomain) {
            return reviewer.secondaryDomain;
        }

        if (secondarySubmissionDomain &&
            reviewer.secondaryDomain == submission.secondaryDomain) {
            return reviewer.secondaryDomain;
        }
    }

    return -1;
}

void CentralManager::buildPrimaryOnlyNetwork(
    Graph<NodeInfo>& network,
    std::unordered_map<int, Vertex<NodeInfo>*>& netSubs,
    std::unordered_map<int, Vertex<NodeInfo>*>& netRevs,
    Vertex<NodeInfo>*& source,
    Vertex<NodeInfo>*& sink,
    int excludeReviewerId
) const {
    // Create source and sink nodes for the flow network.
    NodeInfo src{NodeType::source, -1, -1, -1, "", "", "", ""};
    NodeInfo snk{NodeType::sink, -2, -1, -1, "", "", "", ""};

    network.addVertex(src);
    network.addVertex(snk);

    source = network.findVertex(src);
    sink = network.findVertex(snk);

    // Add reviewer vertices, optionally skipping one reviewer for risk analysis.
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator itReviewer;
    for (itReviewer = reviewers.begin(); itReviewer != reviewers.end(); ++itReviewer) {
        int id = itReviewer->first;
        Vertex<NodeInfo>* v = itReviewer->second;

        if (id == excludeReviewerId) continue;

        NodeInfo info = v->getInfo();
        network.addVertex(info);
        netRevs[id] = network.findVertex(info);
    }
    //std::cout << "first for loop passed...\n";
    // Add submission vertices to the flow network.
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator itSubmission;
    for (itSubmission = submissions.begin(); itSubmission != submissions.end(); ++itSubmission) {
        int id = itSubmission->first;
        Vertex<NodeInfo>* v = itSubmission->second;

        NodeInfo info = v->getInfo();
        network.addVertex(info);
        netSubs[id] = network.findVertex(info);
    }

    // Limit the maximum number of assignments per reviewer.
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator itNetReviewer;
    for (itNetReviewer = netRevs.begin(); itNetReviewer != netRevs.end(); ++itNetReviewer) {
        MaxFlow::addResidualEdge(source, itNetReviewer->second, maxReviewsPerReviewer);
    }

    // Connect reviewers to submissions when their enabled domains match.
    for (itNetReviewer = netRevs.begin(); itNetReviewer != netRevs.end(); ++itNetReviewer) {
        Vertex<NodeInfo>* reviewerVertex = itNetReviewer->second;

        for (itSubmission = netSubs.begin(); itSubmission != netSubs.end(); ++itSubmission) {
            Vertex<NodeInfo>* submissionVertex = itSubmission->second;

            int matchDomain = getMatchDomain(
                reviewerVertex->getInfo(),
                submissionVertex->getInfo()
            );

            if (matchDomain >= 0) {
                MaxFlow::addResidualEdge(reviewerVertex, submissionVertex, 1);
            }
        }
    }

    // Require each submission to receive the minimum number of reviews.
    for (itSubmission = netSubs.begin(); itSubmission != netSubs.end(); ++itSubmission) {
        MaxFlow::addResidualEdge(itSubmission->second, sink, minReviewsPerSubmission);
    }
}

/**Caller to LoadFiles*/
bool CentralManager::loadFiles(const std::string &filename) {
    Parser parser(*this);
    const bool success = parser.loadData(filename);
    if (!success) {
        std::cerr<<"Error: CentralManager Failed to Load Data from: "<< filename<<"\n"<<std::endl;
    }
    //std::cout << "loadFiles() passed.\n"; // correto. erro não é aqui
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

//Clears All Data to prepare for new Data File for a new Graph
void CentralManager::clearData() {
    // 1. Clear the maps holding the pointers
    submissions.clear();
    reviewers.clear();

    // 2. Reset the assignment state variables
    lastAssignments.clear();
    lastMissing.clear();
    hasAssignmentRun = false;

    // 3. Clear the main graph
    graph.clear();
}


// Executa o fluxo completo sem interação: carregar, atribuir e exportar.
void CentralManager::runBatchMode(const std::string &input_file, const std::string &output_file) {
    std::cout << "Batch Mode\n";
    //Removi a imposição da pasta input, em batch mode.
    if (!loadFiles(input_file)) {
        return;
    }

    //Ignorar os output files dentro dos .csv, o commando é rei e senhor!
    if (!output_file.empty()) {
        outputFilename = output_file;
    }


    // Corre primeiro o assignment principal antes de qualquer análise extra.
    const bool feasible = runPrimaryOnlyAssigment();

    if (GenerateAssignments != 0 || !feasible) {
        //Por estar a usar modo append é preciso limpar o ficheiro, se o ficheiro já existir
        std::ofstream clearFile("./output/"+ outputFilename);
        clearFile.close();

        if (writeAssignementOutput(outputFilename)) {
           std::cout << "Assignment output written to " << outputFilename << "\n";
        }
    }

    // A análise de risco só é feita para o caso K = 1.
    if (RiskAnalysis == 1) {
        std::vector<int> risky = evaluateRiskone();
        if (writeRiskOutput(outputFilename, risky)) {
            std::cout << "Risk analysis written to " << outputFilename << "\n";
    }
}
    std::cout << "Done\n";

}
// teste

// Menu simples para testar manualmente as operações principais do projeto.
void CentralManager::runInteractiveMenu() {
    int op;
    bool running = true;
    bool isFileLoaded = false;
    std::string filename;
    while (running) {
        std::cout << "------------------------------------\n";
        std::cout << "Choose an option by number:\n";
        std::cout << "------------------------------------\n";
        if (!isFileLoaded) {
            std::cout << "\t1 - Load input file\t\n";
        }
        else {
            std::cout << "\t1 - Reload a new input file, \""<< filename <<"\" is loaded.\t\n";
        }
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

        // Cada opção do menu chama diretamente uma funcionalidade do gestor central.
        switch(op) {
            case 0:
                running = false;
                break;

            case 1:
                std::cout << "Select input filename: ";
                std::cin >> filename;
                if (!loadFiles("./input/" + filename)) {
                    std::cout << "Error: Could not load file: \"" << filename << "\"!\n";
                    std::cout << "Make sure the .csv file is in inside the input folder.\n";
                }
                else {
                    isFileLoaded=true;
                    std::cout<<"File: \"" << filename<<"\" loaded successfully.\n\n";
                }
                break;

            case 2:
                showSubmissions();
                break;

            case 3:
                showReviewers();
                break;

            case 4:
                showParameters();
                break;

            // Executa o assignment e mostra se foi possível satisfazer todas as submissões.
            case 5:
                if (runPrimaryOnlyAssigment()) {
                    std::cout << "Assignment is feasible. Total matches: " << lastAssignments.size() << "\n";
                }
                else {
                    std::cout << "Assignment is not feasible.\n";
                    for (std::size_t i = 0; i < lastMissing.size(); ++i) {
                        std::cout << "Submission " << std::get<0>(lastMissing[i])
                                  << " needs " << std::get<2>(lastMissing[i])
                                  << " more review(s) in domain " << std::get<1>(lastMissing[i])
                                  << "\n";
                    }
                }
                break;

            // Mostra os reviewers críticos apenas quando RiskAnalysis = 1.
            case 6:
                if (RiskAnalysis == 0) {
                    std::cout << "RiskAnalysis is set to 0 in the current dataset.\n";
                }
                else if (RiskAnalysis == 1) {
                    std::vector<int> risky = evaluateRiskone();
                    std::cout << "Risky reviewers: ";
                    if (risky.empty()) std::cout << "none";
                    else std::cout << joinReviewerIds(risky);
                    std::cout << "\n";
                }
                else {
                    std::cout << "Only RiskAnalysis = 1 is implemented.\n";
                }
                break;

            // Exporta o último resultado calculado para ficheiro.
            case 7: {
                if (!hasAssignmentRun) {
                    std::cout << "Run the assignment algorithm first.\n";
                }
                else {
                    std::string outName;

                    // Clear the leftover newline from the buffer
                    std::cin.ignore(10000, '\n');

                    std::cout << "Output filename (press Enter for default): ";
                    std::getline(std::cin, outName);

                    // outName will naturally be an empty string if you just press Enter
                    if (writeAssignementOutput(outName)) {
                        std::cout << "Assignment results exported successfully.\n";
                    }

                    if (RiskAnalysis == 1) {
                        std::vector<int> risky = evaluateRiskone();
                        if (writeRiskOutput(outName, risky)) {
                            std::cout << "Risk analysis written to " << outName << "\n";
                        }
                    }
                }
                break;
            }

            default:
                std::cout << "Operation unavailable.\n";
                break;
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

// Mostra todas as submissões carregadas de forma ordenada por ID.
void CentralManager::showSubmissions() const {
    if (submissions.empty()) {
        std::cout << "No submissions loaded.\n";
        return;
    }

    std::vector<int> ids;
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator it;
    // Recolhe os IDs para imprimir sempre numa ordem estável.
    for (it = submissions.begin(); it != submissions.end(); ++it) {
        ids.push_back(it->first);
    }
    std::sort(ids.begin(), ids.end());

    std::cout << "\nSubmissions\n";
    std::cout << "-----------\n";
    for (std::size_t i = 0; i < ids.size(); ++i) {
        const NodeInfo info = submissions.at(ids[i])->getInfo();
        std::cout << "Id: " << info.id
                  << " | Title: " << info.title
                  << " | Authors: " << info.authors
                  << " | Email: " << info.email
                  << " | Primary: " << info.primaryDomain
                  << " | Secondary: ";
        if (info.secondaryDomain >= 0) std::cout << info.secondaryDomain;
        else std::cout << "-";
        std::cout << "\n";
    }
}

// Mostra todos os reviewers carregados, ordenados por ID.
void CentralManager::showReviewers() const {
    if (reviewers.empty()) {
        std::cout << "No reviewers loaded.\n";
        return;
    }

    std::vector<int> ids;
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator it;
    for (it = reviewers.begin(); it != reviewers.end(); ++it) {
        ids.push_back(it->first);
    }
    std::sort(ids.begin(), ids.end());

    std::cout << "\nReviewers\n";
    std::cout << "---------\n";
    for (std::size_t i = 0; i < ids.size(); ++i) {
        const NodeInfo info = reviewers.at(ids[i])->getInfo();
        std::cout << "Id: " << info.id
                  << " | Name: " << info.reviewerName
                  << " | Email: " << info.email
                  << " | Primary: " << info.primaryDomain
                  << " | Secondary: ";
        if (info.secondaryDomain >= 0) std::cout << info.secondaryDomain;
        else std::cout << "-";
        std::cout << "\n";
    }
}

// Mostra os parâmetros lidos do ficheiro de input.
void CentralManager::showParameters() const {
    std::cout << "\nParameters\n";
    std::cout << "----------\n";
    std::cout << "MinReviewsPerSubmission: " << minReviewsPerSubmission << "\n";
    std::cout << "MaxReviewsPerReviewer: " << maxReviewsPerReviewer << "\n";
    std::cout << "PrimaryReviewerExpertise: " << primaryReviewerExpertise << "\n";
    std::cout << "SecondaryReviewerExpertise: " << secondaryReviewerExpertise << "\n";
    std::cout << "PrimarySubmissionDomain: " << primarySubmissionDomain << "\n";
    std::cout << "SecondarySubmissionDomain: " << secondarySubmissionDomain << "\n";
    std::cout << "GenerateAssignments: " << GenerateAssignments << "\n";
    std::cout << "RiskAnalysis: " << RiskAnalysis << "\n";
    std::cout << "OutputFileName: " << outputFilename << "\n";
}

// Percorre as arestas reviewer -> submission e guarda as que receberam fluxo.
std::vector<std::tuple<int,int,int>> CentralManager::extractAssignment(
    const std::unordered_map<int, Vertex<NodeInfo>*>& netRevs
) const {
    std::vector<std::tuple<int,int,int>> result;

    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator it;
    for (it = netRevs.begin(); it != netRevs.end(); ++it) {
        int reviewerId = it->first;
        Vertex<NodeInfo>* reviewerVertex = it->second;
        std::vector<Edge<NodeInfo>*> edges = reviewerVertex->getAdj();

        for (std::size_t i = 0; i < edges.size(); ++i) {
            Edge<NodeInfo>* edge = edges[i];
            NodeInfo destination = edge->getDest()->getInfo();
            if (destination.type == NodeType::submission && edge->getFlow() > 0) {
                // Se houve fluxo nesta aresta, então esta review foi atribuída.
                int matchDomain = getMatchDomain(reviewerVertex->getInfo(),destination);
                result.push_back(std::make_tuple(destination.id, reviewerId, matchDomain));
            }
        }
    }
    // Ordena para garantir output consistente e fácil de comparar/testar.
    std::sort(result.begin(), result.end());
    return result;
}

// Calcula quantas reviews faltam a cada submissão com base no fluxo até ao sink.
std::vector<std::tuple<int,int,int>> CentralManager::extractMissingReviews(
    const std::unordered_map<int, Vertex<NodeInfo>*>& netSubs
) const {
    std::vector<std::tuple<int,int,int>> result;
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator it;

    // A aresta submission -> sink indica quantas reviews foram efetivamente atribuídas.
    for (it = netSubs.begin(); it != netSubs.end(); ++it) {
        Vertex<NodeInfo>* submissionVertex = it->second;
        std::vector<Edge<NodeInfo>*> edges = submissionVertex->getAdj();
        int assignedReviews = 0;

        for (std::size_t i = 0; i < edges.size(); ++i) {
            Edge<NodeInfo>* edge = edges[i];
            NodeInfo destination = edge->getDest()->getInfo();

            if (destination.type == NodeType::sink) {
                assignedReviews = static_cast<int>(edge->getFlow());
                break;
            }
        }

        int missing = minReviewsPerSubmission - assignedReviews;
        // Só guarda submissões que ficaram abaixo do mínimo exigido.
        if (missing > 0) {
            NodeInfo info = submissionVertex->getInfo();
            result.push_back(std::make_tuple(info.id, info.primaryDomain, missing));
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

// Constrói a rede, corre Edmonds-Karp e guarda o resultado do assignment.
bool CentralManager::runPrimaryOnlyAssigment() {
    // Limpa o estado anterior para evitar misturar execuções.
    hasAssignmentRun = true;
    lastAssignments.clear();
    lastMissing.clear();

    if (submissions.empty() || reviewers.empty()) {
        lastRunFeasible = false;
        return false;
    }

    // Usa uma rede temporária para calcular o fluxo desta execução.
    Graph<NodeInfo> network;
    std::unordered_map<int, Vertex<NodeInfo>*> netSubs;
    std::unordered_map<int, Vertex<NodeInfo>*> netRevs;
    Vertex<NodeInfo>* source = nullptr;
    Vertex<NodeInfo>* sink = nullptr;

    buildPrimaryOnlyNetwork(network, netSubs, netRevs, source, sink);

    // O fluxo máximo representa o número total de reviews atribuídas.
    const int maxFlow = MaxFlow::edmondsKarp(network, source, sink);
    const int totalRequired = static_cast<int>(submissions.size()) * minReviewsPerSubmission;

    // O assignment só é viável se o fluxo cobrir todas as reviews exigidas.
    lastAssignments = extractAssignment(netRevs);
    lastMissing = extractMissingReviews(netSubs);
    lastRunFeasible = (maxFlow == totalRequired);

    return lastRunFeasible;
}
// Um reviewer é de risco se, ao removê-lo, o assignment deixar de ser viável.
std::vector<int> CentralManager::evaluateRiskone() const {
    std::vector<int> risky;

    if (submissions.empty() || reviewers.empty()) {
        return risky;
    }

    const int totalRequired = static_cast<int>(submissions.size()) * minReviewsPerSubmission;

    std::vector<int> reviewerIds;
    std::unordered_map<int, Vertex<NodeInfo>*>::const_iterator it;

    // Testa cada reviewer removendo-o temporariamente da rede.
    for (it = reviewers.begin(); it != reviewers.end(); ++it) {
        reviewerIds.push_back(it->first);
    }
    std::sort(reviewerIds.begin(), reviewerIds.end());

    for (std::size_t i = 0; i < reviewerIds.size(); ++i) {
        Graph<NodeInfo> network;
        std::unordered_map<int, Vertex<NodeInfo>*> netSubs;
        std::unordered_map<int, Vertex<NodeInfo>*> netRevs;
        Vertex<NodeInfo>* source = nullptr;
        Vertex<NodeInfo>* sink = nullptr;

        buildPrimaryOnlyNetwork(network, netSubs, netRevs, source, sink, reviewerIds[i]);

        const int maxFlow = MaxFlow::edmondsKarp(network, source, sink);

        // Se o fluxo desce abaixo do necessário, este reviewer é crítico.
        if (maxFlow < totalRequired) {
            risky.push_back(reviewerIds[i]);
        }
    }

    return risky;
}

// Escreve o resultado do último assignment em formato compatível com o enunciado.
bool CentralManager::writeAssignementOutput(const std::string& filename) const {
    if (!hasAssignmentRun) {
        std::cerr << "Error: no assignment has been run yet.\n";
        return false;
    }

    const std::string baseName = filename.empty() ? outputFilename : filename;
    const std::string finalName = "./output/" + baseName;
    std::ofstream out(finalName.c_str());
    if (!out.is_open()) {
        std::cerr << "Error: could not open output file " << finalName << "\n";
        return false;
    }

    // Se a atribuição foi viável, exporta os matches por submissão e por reviewer.
    //if (lastRunFeasible) {
        std::vector<std::tuple<int,int,int>> bySubmission = lastAssignments;
        std::sort(bySubmission.begin(), bySubmission.end());

        // Reorganiza os mesmos dados para facilitar leitura do ponto de vista do reviewer.
        std::vector<std::tuple<int,int,int>> byReviewer;
        for (std::size_t i = 0; i < lastAssignments.size(); ++i) {
            byReviewer.push_back(std::make_tuple(
                std::get<1>(lastAssignments[i]),
                std::get<0>(lastAssignments[i]),
                std::get<2>(lastAssignments[i])
            ));
        }
        std::sort(byReviewer.begin(), byReviewer.end());

        out << "#SubmissionId,ReviewerId,Match\n";
        for (std::size_t i = 0; i < bySubmission.size(); ++i) {
            out << std::get<0>(bySubmission[i]) << ", "
                << std::get<1>(bySubmission[i]) << ", "
                << std::get<2>(bySubmission[i]) << "\n";
        }

        out << "#ReviewerId,SubmissionId,Match\n";
        for (std::size_t i = 0; i < byReviewer.size(); ++i) {
            out << std::get<0>(byReviewer[i]) << ", "
                << std::get<1>(byReviewer[i]) << ", "
                << std::get<2>(byReviewer[i]) << "\n";
        }

        out << "#Total: " << bySubmission.size() << "\n";
    //}

    // Caso contrário, exporta apenas as submissões que ficaram com reviews em falta.
    if (!lastRunFeasible) {
        std::vector<std::tuple<int,int,int>> missing = lastMissing;
        std::sort(missing.begin(), missing.end());

        out << "#SubmissionId,Domain,MissingReviews\n";
        for (std::size_t i = 0; i < missing.size(); ++i) {
            out << std::get<0>(missing[i]) << ", "
                << std::get<1>(missing[i]) << ", "
                << std::get<2>(missing[i]) << "\n";
        }
    }

    return true;
}

// Escreve os reviewers cuja remoção compromete a viabilidade do assignment.
bool CentralManager::writeRiskOutput(const std::string& filename, const std::vector<int>& risky) const {

    const std::string baseName = filename.empty() ? "risk.csv" : filename;
    const std::string finalName = "./output/" + baseName;

    std::ofstream out(finalName.c_str(), std::ios::app);//mode append
    if (!out.is_open()) {
        std::cerr << "Error: could not open risk output file " << finalName << "\n";
        return false;
    }

    out << "#Risk Analysis: " << RiskAnalysis << "\n";
    out << joinReviewerIds(risky) << "\n";
    return true;
}

// Mantém compatibilidade com uma interface antiga para gerar o output.
void CentralManager::generateOutputCSV(const std::string& filename) {
    writeAssignementOutput(filename);
}