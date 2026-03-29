#ifndef PARSER_H
#define PARSER_H

class CentralManager;

#include <string>

///@brief  Lê o ficheiro .CSV e escreve no CentralManager
class Parser {
    private:
    CentralManager& manager;
    ///@brief Remove espaços em branco no início e no fim duma string.
    static void trim(std::string& str);

    ///@brief Envia info ao manager para instanciar vertices Submission.
    void parseSubmissionLine(const std::string& line);
    ///@brief Envia info ao manager para instanciar vertices Reviewers.
    void parseReviewerLine(const std::string& line);
    ///@brief Regista pesos das edges da source para submissions e pesos das edges dos reviewers para sink.
    void parseParameterLine(const std::string& line);
    ///@brief Guarda Control Parameters
    void parseControlLine(const std::string& line);

    public:
    explicit Parser(CentralManager& m) : manager(m) {}

    /// @brief Abre o ficheiro e gere leitura delegando as linhas a funções separadas.
    bool loadData(const std::string& filename);
};

#endif