#pragma once

#include "Graph.h"
#include "DataEntities.h"
#include <unordered_map>
#include <string>
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

    /**
    * @brief Vetor com as últimas atribuições calculadas, no formato
    * (submissionId, reviewerId, matchDomain)
     */
    std::vector <std::tuple<int,int,int>> lastAssignments;
    /**
     * @brief vetor com uma atribuição mal-sucedida, no formato
     * (submissionId, domain, missingReviews)
     */
    std::vector <std::tuple<int,int,int>> lastMissing;

    /**
     * @brief Variável booleana que é verdadeira se o algoritmo de atribuição foi corrido
     */
    bool hasAssignmentRun = false;

    /**
     * @brief Variável booleana que é verdadeira se a última atribuição foi viável
     */
    bool lastRunFeasible = false;

    /**
     * @brief Grafo composto por NodeInfo's
     */
    Graph<NodeInfo> graph;

    /**
     * @brief Mapa desordenado composto por elementos (int, Vertex<NodeInfo>), onde os vértices correspondem às submissões
     */
    std::unordered_map<int,Vertex<NodeInfo>*> submissions;

    /**
     * @brief Mapa desordenado composto por elementos (int, Vertex<NodeInfo>), onde os vértices correspondem aos revisores
     */
    std::unordered_map<int,Vertex<NodeInfo>*> reviewers;

    /**
     * @brief Variável inteira indica o número mínimo de revisores por submissão
     */
    int minReviewsPerSubmission = 0;

    /**
     * @brief Variável inteira indica o número máximo de revisões por revisor
     */
    int maxReviewsPerReviewer = 0;
    bool primaryReviewerExpertise = false;
    bool secondaryReviewerExpertise = false;
    bool primarySubmissionDomain = false;
    bool secondarySubmissionDomain = false;



    /**
     * @brief parâmetro numérico que controla a geração de atribuições
     */
    int GenerateAssignments = 0;

    /**
     * @brief parâmtro numérico que determina o tipo de análise de risco
     */
    int RiskAnalysis = 0;

    /**
     * @brief Nome do ficheiro de output a gerar
     */
    std::string outputFilename;

public:
    /// @brief Carrega os dados a partir do ficheiro especificado.
    bool loadFiles(const std::string& filename);
    /// @brief Limpa os dados atuais em memória.
    void clearData();


/**Subsets*/
    /**
     *@brief Adiciona a informação da submissão identificada com um id ao grafo
     * @param id identificador da submissão
     * @param info informação acerca da submissão
     */
    void addSubmission(int id, const NodeInfo& info);

    /**
     *@brief Adiciona a informação do revisor identificada com um id ao grafo
     * @param id identificador do revisor
     * @param info informação acerca do revisor
     */
    void addReviewer(int id, const NodeInfo& info);

    /**
     * @brief Verifica se existe uma submissão com o ID especificado.
     *
     * @param id identificador de submissão
     * @return bool retorna verdadeiro se a submissão estiver presente no unordered_map submissions
     */
    bool hasSubmission(int id) const;
    /**
     *
     * @brief Verifica se existe um revisor com o ID especificado.
     *
     * @param id identificador de revisor
     * @return bool retorna verdadeiro se o revisor estiver presente no unordered_map reviewers
     */
    bool hasReviewer(int id) const;

    /**
    * @brief Obtém o mapa de submissões.
    * @return Referência constante para o mapa de submissões.
    */
    const std::unordered_map<int, Vertex<NodeInfo>*>& getSubmissions() const;

    /**
    * @brief Obtém o mapa de revisores.
    * @return Referência constante para o mapa de revisores.
    */
    const std::unordered_map<int, Vertex<NodeInfo>*>& getReviewers() const;

/**Parametros*/

    /**
     * @brief Define o número mínimo de revisões por submissão.
     *
     * @param value Número mínimo de revisões.
     */
    void setMinReviewsPerSubmission(int value);

    /**
     * @brief Obtém o número mínimo de revisões por submissão
     * @return int Número mínimo de revisões
     */
    int getMinReviewsPerSubmission() const;

    /**
     * @brief Define o número máximo de revisões por revisor.
     *
     * @param value Número máximo de revisões.
     */
    void setMaxReviewsPerReviewer(int value);

    /**
     * @brief Obtém o número máximo de revisões por revisor.
     *
     * @return Número máximo de revisões.
     */
    int getMaxReviewsPerReviewer() const;

    /**
     * @brief Define se a correspondência deve considerar o domínio primário do revisor.
     *
     * @param value true para ativar a correspondência pelo domínio primário do revisor,
     * false caso contrário.
     */
    void setPrimaryReviewerExpertise(bool value);


    /**
     * @brief Define se a correspondência deve considerar o domínio secundário do revisor.
     *
     * @param value true para ativar a correspondência pelo domínio secundário do revisor,
     * false caso contrário.
     */
    void setSecondaryReviewerExpertise(bool value);


    /**
     * @brief Define se a correspondência deve considerar o domínio primário da submissão.
     *
     * @param value true para ativar a correspondência pelo domínio primário da submissão,
     * false caso contrário.
     */
    void setPrimarySubmissionDomain(bool value);


    /**
     * @brief Define se a correspondência deve considerar o domínio secundário da submissão.
     *
     * @param value true para ativar a correspondência pelo domínio secundário da submissão,
     * false caso contrário.
     */
    void setSecondarySubmissionDomain(bool value);

    /**Parametros de Controlo*/
    void setGenerateAssignments(int gen);
    void setRiskAnalysis(int risk);
    void setOutputFilename(const std::string& name);

    /**
     * @brief Imprime todas as submissões ordenadas por ID
     *
     * Extrai os IDs da submissão de um mapa interno para um vetor interno, ordena-os, e imprime as informações das submissões
     *
     * Domínios secundários nulos são representados por "-".
     *
     * Se nenhuma submissão estiver disponível, uma mensagem de aviso é impressa.
     */
    void showSubmissions() const;

    /**
     * @brief Imprime todas os revisores ordenados por ID
     *
     * Extrai os IDs do revisor de um mapa interno para um vetor interno, ordena-os, e imprime as informações dos revisores
     *
     * Domínios secundários nulos são representados por "-".
     *
     * Se nenhum revisor estiver disponível, uma mensagem de aviso é impressa.
     */
    void showReviewers() const;


    /**
     *
     * @brief Imprime os valores de variáveis internas à classe CentralManager que correspondem a parâmetros
     */
    void showParameters() const;


    /**
    * @brief Constrói a rede de fluxo, calcula o fluxo máximo e armazena os resultados.
    * @return Verdadeiro se a atribuição for viável, falso caso existam revisões em falta.
    * @note Complexidade Temporal: Dominada pelo algoritmo de Edmonds-Karp O(V * E^2).
    */
    bool runPrimaryOnlyAssignment();


    /**
     * @brief Avalia quais os revisores que são críticos para a viabilidade da atribuição.
     * Um revisor é considerado de risco se a sua remoção tornar a atribuição inviável.
     * @return Um vetor com os IDs dos revisores de risco.
     * @note Complexidade Temporal: O(R * V * E^2) onde R é o número de revisores.
     */
    std::vector<int> evaluateRiskOne() const;

    /// @brief Escreve as listas de atribuição com sucesso ou as submissões com revisões em falta.
    bool writeAssignmentOutput(const std::string& filename) const;


    /// @brief Escreve o resultado da análise de risco num ficheiro de output.
    bool writeRiskOutput(const std::string& filename, const std::vector<int>& risky) const;


    void runInteractiveMenu();
    /// @brief Executa o programa em batch mode
    void runBatchMode(const std::string & input_file, const std::string & risk_file);

    /// @brief Imprime a estrutura da rede na consola para fins de depuração (debug).
    void printNetwork(Graph<NodeInfo>& network) const;

    /// @brief Método de compatibilidade para gerar o ficheiro CSV de output final.
    void generateOutputCSV(const std::string& filename);
};

