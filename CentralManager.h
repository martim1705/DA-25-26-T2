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
    * @brief Constrói a rede de fluxo usada no problema de atribuição de revisões.
    *
    * Cria os vértices da fonte, do sumidouro, dos revisores e das submissões.
    * Adiciona arestas da fonte para os revisores, dos revisores para as submissões
    * compatíveis, e das submissões para o sumidouro.
    *
     * A compatibilidade entre revisores e submissões é determinada pela função
    * canMatch(), tendo em conta os parâmetros de controlo atualmente carregados.
    *
     * @param network Grafo de fluxo a construir.
     * @param netSubs Mapa que associa cada ID de submissão ao respetivo vértice na rede.
    * @param netRevs Mapa que associa cada ID de revisor ao respetivo vértice na rede.
    * @param source Ponteiro de saída para o vértice fonte.
    * @param sink Ponteiro de saída para o vértice sumidouro.
    * @param excludeReviewerId ID de um revisor a excluir da rede, usado na análise de risco.
    *
    * @note Complexidade temporal: O(R + S + R*S), onde R é o número de revisores
    * e S o número de submissões.
    */
    void buildNetwork(
        Graph<NodeInfo>& network,
        std::unordered_map<int, Vertex<NodeInfo>*>& netSubs,
        std::unordered_map<int, Vertex<NodeInfo>*>& netRevs,
        Vertex<NodeInfo>*& source,
        Vertex<NodeInfo>*& sink,
        int excludeReviewerId = -1
    ) const;

    /**
    * @brief Determina os domínios de submissão que podem ser usados no emparelhamento.
    *
    * A lista devolvida depende do valor de GenerateAssignments e, no caso geral,
    * dos parâmetros booleanos que indicam se os domínios primário e secundário
    * da submissão podem ser considerados.
    *
    * @param sub Informação da submissão a analisar.
    * @return Vetor com os domínios da submissão elegíveis para matching.
    *
     * @note O vetor não contém duplicados nem valores inválidos.
    */
    std::vector<int> allowedSubmissionDomains(const NodeInfo& sub) const;
    
    /**
    * @brief Determina os domínios de expertise do revisor que podem ser usados no emparelhamento.
    *
    * A lista devolvida depende do valor de GenerateAssignments e, no caso geral,
    * dos parâmetros booleanos que indicam se os domínios primário e secundário
    * do revisor podem ser considerados.
    *
    * @param rev Informação do revisor a analisar.
    * @return Vetor com os domínios do revisor elegíveis para matching.
    *
    * @note O vetor não contém duplicados nem valores inválidos.
    */
    std::vector<int> allowedReviewerDomains(const NodeInfo& rev) const;
    
    /**
    * @brief Verifica se uma submissão e um revisor são compatíveis segundo os domínios permitidos.
    *
    * Compara os domínios elegíveis da submissão com os domínios elegíveis do revisor.
    * Se existir interseção, o método devolve true e escreve em @p matchDomain o domínio
    * pelo qual a correspondência foi estabelecida.
    *
    * @param sub Informação da submissão.
    * @param rev Informação do revisor.
    * @param matchDomain Parâmetro de saída onde é guardado o domínio usado no matching.
    * @return true se a submissão e o revisor puderem ser emparelhados, false caso contrário.
    */
    bool canMatch(const NodeInfo& sub, const NodeInfo& rev, int& matchDomain) const;

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
    * @brief Parâmetro de controlo que define quais os domínios permitidos no matching.
    *
    * O significado exato do valor depende da especificação do projeto.
    */
    int GenerateAssignments = 0;

    /**
    * @brief Parâmetro de controlo que define o tipo de análise de risco a executar.
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

    /**
    * @brief Define o modo de geração de atribuições.
    *
    * Este parâmetro controla que domínios podem ser considerados no matching
    * entre revisores e submissões.
    *
    * @param gen Valor do modo GenerateAssignments lido do ficheiro de input.
    */
    void setGenerateAssignments(int gen);

    /**
    * @brief Define o modo de análise de risco a executar.
    *
    * @param risk Valor do parâmetro RiskAnalysis lido do ficheiro de input.
    */
    void setRiskAnalysis(int risk);

    /**
    * @brief Define o nome do ficheiro de output.
    *
    * @param name Nome do ficheiro onde os resultados devem ser escritos.
    */
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

    /**
    * @brief Escreve o resultado da última execução do algoritmo de atribuição.
    *
    * Se a última execução tiver sido viável, escreve as atribuições ordenadas por
    * submissão e por revisor, bem como o número total de revisões atribuídas.
    * Caso contrário, escreve as submissões com revisões em falta.
    *
    * @param filename Nome do ficheiro de output a gerar.
    * @return true se o ficheiro tiver sido escrito com sucesso, false caso contrário.
    */
    bool writeAssignmentOutput(const std::string& filename) const;


    /**
    * @brief Acrescenta ao ficheiro de output o resultado da análise de risco.
    *
    * O método escreve a secção "#Risk Analysis" e a lista de IDs dos revisores
    * considerados críticos. Se não existirem revisores de risco, a linha seguinte
    * fica vazia.
    *
    * @param filename Nome do ficheiro onde a informação deve ser escrita.
    * @param risky Vetor com os IDs dos revisores classificados como de risco.
    * @return true se a escrita tiver sido concluída com sucesso, false caso contrário.
    */
    bool writeRiskOutput(const std::string& filename, const std::vector<int>& risky) const;

    /**
    * @brief Executa o menu interativo da aplicação.
    *
    * Permite carregar ficheiros, visualizar dados, executar o algoritmo de atribuição,
    * correr a análise de risco e exportar resultados através de um menu textual.
    */
    void runInteractiveMenu();
    
    /**
    * @brief Executa o programa em modo batch.
    *
    * Carrega o ficheiro de input, executa o algoritmo de atribuição e, se aplicável,
    * escreve o resultado da atribuição e da análise de risco para o ficheiro de output.
    *
    * @param input_file Caminho para o ficheiro CSV de entrada.
    * @param output_file Nome do ficheiro de output a gerar. Se estiver vazio,
    * é usado o nome configurado nos dados de entrada.
    */
    void runBatchMode(const std::string & input_file, const std::string & risk_file);

    /// @brief Imprime a estrutura da rede na consola para fins de depuração (debug).
    void printNetwork(Graph<NodeInfo>& network) const;

    /// @brief Método de compatibilidade para gerar o ficheiro CSV de output final.
    void generateOutputCSV(const std::string& filename);
};

