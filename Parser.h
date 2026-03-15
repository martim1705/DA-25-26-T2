#ifndef PARSER_H
#define PARSER_H

class CentralManager;

#include <string>

class Parser {
    private:
    CentralManager& manager;
    static void trim(std::string& str);

    void parseSubmissionLine(const std::string& line);//manda info ao manager para instanciar vertices Submission
    void parseReviewerLine(const std::string& line);//manda info ao manager para instanciar vertices Reviewers
    void parseParameterLine(const std::string& line);//(Pesos das edges da source para submissions e peso das edges dos reviewers para sink).
    void parseControlLine(const std::string& line);//guarda Control Parameters

    public:
    explicit Parser(CentralManager& m) : manager(m) {}

    //Abre o ficheiro e gere leitura
    bool loadData(const std::string& filename);
};

#endif
