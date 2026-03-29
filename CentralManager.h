#pragma once

#include "Graph.h"
#include "DataEntities.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <tuple>

///@brief Gere o fluxo de execução, armazena as entidades de dados e gere os algoritmos de Fluxo Máximo.
class CentralManager {
private:

    /**
     * @brief Cria um nó fonte, um sumidouro, vértices de revisores e submissões.
     * Adiciona arestas da fonte para revisores, de revisores para submissões compatíveis, e de submissões para o sumidouro.
     * @param network O grafo a construir.
     * @param netSubs Mapa que liga IDs de submissão aos seus vértices.
     * @param netRevs Mapa que liga IDs de revisor aos seus vértices.
     * @param source Ponteiro para o vértice fonte gerado.
     * @param sink Ponteiro para o vértice sumidouro gerado.
     * @param excludeReviewerId ID de um revisor a excluir (usado na análise de risco).
     * @note Complexidade Temporal: O(V + E) onde V é o número de submissões e revisores, e E o número de correspondências.
     */
    void buildPrimaryOnlyNetwork(
        Graph<NodeInfo>& network,
        std::unordered_map<int, Vertex<NodeInfo>*>& netSubs,
        std::unordered_map<int, Vertex<NodeInfo>*>& netRevs,
        Vertex<NodeInfo>*& source,
        Vertex<NodeInfo>*& sink,
        int excludeReviewerId = -1
    ) const;


    /** @brief Lê os valores de fluxo nas arestas revisor->submissão e guarda as correspondências com fluxo positivo.
     * @param netRevs Mapa que liga IDs de revisor aos seus vértices.
     * @return std::vector<std::tuple<int,int,int>> Vetor de tuplos (submissionId, reviewerId, primaryDomain)
     */
    std::vector<std::tuple<int,int,int>> extractAssignment(
        const std::unordered_map<int, Vertex<NodeInfo>*>& netRevs
    ) const;


    /**
     * @brief Compara o fluxo que chega ao sink com o número mínimo de revisões exigido para cada submissão.
     * @param netSubs Mapa que liga IDs de submissão aos seus vértices.
     * @return std::vector<std::tuple<int,int,int>> Vetor de tuplos (idSubmissao, dominioPrimario, revisoesEmFalta),
     * ordenados por ordem crescente
     */
    std::vector<std::tuple<int,int,int>> extractMissingReviews(
        const std::unordered_map<int, Vertex<NodeInfo>*>& netSubs
    ) const;

    std::vector <std::tuple<int,int,int>> lastAssignments; // submissionId, reviewerId, matchDomain
    std::vector <std::tuple<int,int,int>> lastMissing; // submissionId, domain, missingReviews
    bool hasAssignmentRun = false;
    bool lastRunFeasible = false;

    //Grafos e Conjuntos.
    Graph<NodeInfo> graph;

    //Usar como atalho para iterar só sobre subgrupos
    //ESCOLHI USAR UNMAPPED PARA SER MAIS RAPIDA A CONSTRUÇÃO DO GRAPH,
    //MAS OBRIGA A USAR UM SORT NO MENU AO EXPOR
    //(Escolhi assim porque acaba por ser mais eficiente em batch, mas podemos usar map)
    std::unordered_map<int,Vertex<NodeInfo>*> submissions;
    std::unordered_map<int,Vertex<NodeInfo>*> reviewers;

    //Parameters
    int minReviewsPerSubmission = 0;
    int maxReviewsPerReviewer = 0;
    bool primaryReviewerExpertise = false; //Connect using primaryReviewerExpertise?
    bool secondaryReviewerExpertise = false; //Connect using secondaryReviewerExpertise?
    bool primarySubmissionDomain = false; //Connect using primarySubmissionDomain?
    bool secondarySubmissionDomain = false; //Connect using secondarySubmissionDomain?

    //Control
    int GenerateAssignments = 0; //parece redundante
    int RiskAnalysis = 0; //how many interviewers to remove per cycle in risk analysis
    std::string outputFilename;

public:
    /// @brief Carrega os dados a partir do ficheiro especificado.
    bool loadFiles(const std::string& filename);
    /// @brief Limpa os dados atuais em memória.
    void clearData();


/**Subsets*/
    void addSubmission(int id, const NodeInfo& info);
    void addReviewer(int id, const NodeInfo& info);
    bool hasSubmission(int id) const;
    bool hasReviewer(int id) const;
    const std::unordered_map<int, Vertex<NodeInfo>*>& getSubmissions() const;
    const std::unordered_map<int, Vertex<NodeInfo>*>& getReviewers() const;

/**Parametros*/
    void setMinReviewsPerSubmission(int value);
    int getMinReviewsPerSubmission() const;

    void setMaxReviewsPerReviewer(int value);
    int getMaxReviewsPerReviewer() const;

/*Este quatro parecem-me redundantes para lógica***TIRAR DÚVIDA*/
    void setPrimaryReviewerExpertise(bool value);
//    bool getPrimaryReviewerExpertise() const;

    void setSecondaryReviewerExpertise(bool value);
//    bool getSecondaryReviewerExpertise() const;

    void setPrimarySubmissionDomain(bool value);
//    bool getPrimarySubmissionDomain() const;

    void setSecondarySubmissionDomain(bool value);
//    bool getSecondarySubmissionDomain() const;

    /**Parametros de Controlo*/
    //Em princípio não precisam de getters (posso estar errado claro)
    //porque são utilizados apenas pelo CentralManager
    void setGenerateAssignments(int gen);
    void setRiskAnalysis(int risk);
    void setOutputFilename(const std::string& name);

    void showSubmissions() const;
    void showReviewers() const;
    void showParameters() const;


    /**
    * @brief Constrói a rede de fluxo, calcula o fluxo máximo e armazena os resultados.
    * @return Verdadeiro se a atribuição for viável, falso caso existam revisões em falta.
    * @note Complexidade Temporal: Dominada pelo algoritmo de Edmonds-Karp O(V * E^2).
    */
    bool runPrimaryOnlyAssigment();


    /**
     * @brief Avalia quais os revisores que são críticos para a viabilidade da atribuição.
     * Um revisor é considerado de risco se a sua remoção tornar a atribuição inviável.
     * @return Um vetor com os IDs dos revisores de risco.
     * @note Complexidade Temporal: O(R * V * E^2) onde R é o número de revisores.
     */
    std::vector<int> evaluateRiskone() const;

    /// @brief Escreve as listas de atribuição com sucesso ou as submissões com revisões em falta.
    bool writeAssignementOutput(const std::string& filename) const;

    //Writes the result of the risk analysis to an output file.
    /// @brief Escreve o resultado da análise de risco num ficheiro de output.
    bool writeRiskOutput(const std::string& filename, const std::vector<int>& risky) const;

    // Tarefa 1.1 - tipos de menus
    void runInteractiveMenu();
    /// @brief Executa o programa em batch Mode
    void runBatchMode(const std::string & input_file, const std::string & risk_file);

    /// @brief Imprime a estrutura da rede na consola para fins de depuração (debug).
    void printNetwork(Graph<NodeInfo>& network) const;

    /// @brief Metodo de compatibilidade para gerar o ficheiro CSV de output final.
    void generateOutputCSV(const std::string& filename);
};

