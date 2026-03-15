# DA-25-26-T2
Projeto de Desenho de Algoritmos 25/26

O CentralManager parece estar a crescer bem,
se calhar é melhor ter o menu fora deste, num ficheiro à parte.
Menu.h e Menu.cpp

Code/
├── main.cpp
├── CentralManager.h
├── CentralManager.cpp
├── Parser.h
├── Parser.cpp
├── MaxFlow.h
├── MaxFlow.cpp
├── DataEntities.h
└── Graph.h

Detalhe das Funções por Ficheiro

main.cpp

    int main(int argc, char* argv[]): Lê os argumentos da linha de comandos. Se detetar -b input.csv risk.csv, instancia a ferramenta em modo batch. Caso contrário, arranca o menu interativo (T1.1).

DataEntities.h

    enum NodeType: Define se o nó é SUBMISSION, REVIEWER, SOURCE ou SINK.

    struct NodeInfo: Estrutura com ID, domínios, nome/título, autores e email. Inclui a sobrecarga do operator==.

Graph.h

    Ficheiro original das TP, inalterado nas suas funções nucleares (addVertex, addEdge, addBidirectionalEdge, findVertex, etc.).

Parser.h / Parser.cpp (Tarefa 1.2)

    bool loadData(const std::string& filename): Abre o ficheiro CSV e gere a leitura.

    void parseSubmissions(std::ifstream& file): Lê o bloco #Submissions e instancia a NodeInfo.

    void parseReviewers(std::ifstream& file): Lê o bloco #Reviewers e instancia a NodeInfo.

    void parseParameters(std::ifstream& file): Lê e guarda o MinReviewsPerSubmission, MaxReviewsPerReviewer, etc.

    void parseControl(std::ifstream& file): Lê e guarda o GenerateAssignments, RiskAnalysis e OutputFileName.

MaxFlow.h / MaxFlow.cpp (Tarefas 2.1 e 2.2)

    void buildNetwork(Graph<NodeInfo>& graph, const std::unordered_map<int, Vertex<NodeInfo>*>& subs, const std::unordered_map<int, Vertex<NodeInfo>*>& revs, int mode): Constrói a Origem, o Destino e as arestas com base no GenerateAssignments (0, 1, 2 ou 3) e nos parâmetros de capacidade.

    void edmondsKarp(Graph<NodeInfo>& graph, Vertex<NodeInfo>* source, Vertex<NodeInfo>* sink): Implementação do algoritmo base de fluxo máximo.

    std::vector<int> evaluateRiskOne(Graph<NodeInfo>& graph): Executa sistematicamente o fluxo máximo desativando um revisor de cada vez, retornando a lista de revisores cuja ausência inviabiliza as revisões mínimas.

CentralManager.h / CentralManager.cpp (Tarefas 1.1 e Escrita de Output)

    void runInteractiveMenu(): Apresenta as opções na consola e processa o input do utilizador.

    void runBatchMode(const std::string& inputFile, const std::string& riskFile): Executa o fluxo completo do programa sem interação humana.

    void generateOutputCSV(const std::string& filename): Filtra as arestas do grafo onde o fluxo é igual a 1 e escreve o ficheiro com os blocos #SubmissionId,ReviewerId,Match e #ReviewerId,SubmissionId,Match.

    void generateUnsuccessfulReport(): Imprime as submissões que não atingiram o mínimo de revisões exigidas.

    void generateRiskOutput(const std::string& filename, const std::vector<int>& riskyReviewers): Escreve o ficheiro de análise de risco.
