#include "Parser.h"
#include "DataEntities.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bits/locale_facets_nonio.h>

#include "CentralManager.h"

namespace {
    // Remove espaços exteriores e aspas, útil para nomes de ficheiro no input.
    std::string stripQuotesCopy(const std::string& text) {
        std::string result = text;
        std::size_t begin = result.find_first_not_of(" \n\t\r");
        if (begin == std::string::npos) return "";
        std::size_t end = result.find_last_not_of(" \n\t\r");
        result = result.substr(begin, end - begin + 1);

        if (result.size() >= 2 && result.front() == '"' && result.back() == '"') {
            result = result.substr(1, result.size() - 2);
        }
        return result;
    }
}

void Parser::trim(std::string& str) {
    if (str.empty()) {return;}
    str.erase(0, str.find_first_not_of(" \n\t\r")); //Erase before useful data
    if (!str.empty()) { str.erase(str.find_last_not_of(" \n\t\r") + 1); } //Erase after useful data

}

// Lê o ficheiro CSV por secções e envia cada linha para o parser adequado.
bool Parser::loadData(const std::string& filename) {
    //testa tipo de ficheiro == csv ?
    if (filename.length()<4||filename.substr(filename.length()-4)!=".csv") {
        std::cerr<<"Error: filename must be a valid csv file"<<std::endl;
        return false;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr<<"Error: could not open file "<<filename<<std::endl;
        return false;
    }

    //Limpa possíveis dados antigos
    manager.clearData();

    std::string line;

    // section indica se estamos a ler submissions, reviewers, parameters ou control.
    int section=0;
    while (std::getline(file, line)) {

        trim(line);
        if (line.empty()) continue;

        //Section Change
        if (line.find("#Submissions") == 0) { section=1; continue;}
        if (line.find("#Reviewers") == 0) { section=2; continue;}
        if (line.find("#Parameters") == 0) { section=3; continue;}
        if (line.find("#Control") == 0) { section=4; continue;}

        /**Dúvida vamos ter que testar comentários a meio das linhas?*/

        //Ignore all after #
        if (line.find('#')!=std::string::npos){ continue;}

        if (section==0) {continue;}
        if (section==1) {parseSubmissionLine(line);}
        else if (section==2) {parseReviewerLine(line);}
        else if (section==3) {parseParameterLine(line);}
        else {parseControlLine(line);}
    }
    file.close();
    //std::cout << "finished loadData\n";
    return true;
}

// Converte uma linha CSV numa submissão e adiciona-a ao manager.
void Parser::parseSubmissionLine(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    NodeInfo info;
    info.type= NodeType::submission;


    if (!std::getline(ss, token, ',')) return;//Empty should I throw an error?

    //Entradas no info
    info.id= std::stoi(token);

    // O ID da submissão tem de ser inteiro e único.
    if (manager.hasSubmission(info.id)) {
        std::cerr<<"Error: Repeated ID in submission: "<<info.id<<". Skipping Line."<<std::endl;
        return;
    }

    std::getline(ss,info.title, ',');
    trim(info.title);

    std::getline(ss, info.authors, ',');
    trim(info.authors);

    std::getline(ss, info.email, ',');
    trim(info.email);

    //Primary Domain (Mandatory)
    if (std::getline(ss, token, ',')) {
        trim(token);
        if (token.empty()) {
            std::cerr << "Error: Primary domain missing for submission"<<info.id<<"\n";
            return;
        }
        info.primaryDomain=std::stoi(token);
    }
    else {//nothing was read
        std::cerr << "Error: Primary domain missing for submission"<<info.id<<"\n";
        return;
    }
    //Secondary Domain(Optional)
    if (std::getline(ss, token, ',')) {
        trim(token);
        if (token.empty()) {
            info.secondaryDomain=-1;
        }
        else
        info.secondaryDomain=std::stoi(token);
    }
    else { //nothing was read
        info.secondaryDomain=-1;
    }

    manager.addSubmission(info.id,info);

}

// Converte uma linha CSV num reviewer e adiciona-o ao manager.
void Parser::parseReviewerLine(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    NodeInfo info;
    info.type= NodeType::reviewer;


    if (!std::getline(ss, token, ',')) return;//Empty should I throw an error?

    //Entradas no info
    info.id= std::stoi(token);

    if (manager.hasReviewer(info.id)) {
        std::cerr<<"Error: Repeated ID in reviewer: "<<info.id<<". Skipping Line."<<std::endl;
        return;
    }

    std::getline(ss,info.reviewerName, ',');
    trim(info.reviewerName);

    std::getline(ss, info.email, ',');
    trim(info.email);

    //Primary Domain (Mandatory)
    if (std::getline(ss, token, ',')) {
        trim(token);
        if (token.empty()) {
            std::cerr << "Error: Primary domain missing for Reviewer: "<<info.id<<"\n";
            return;
        }
        info.primaryDomain=std::stoi(token);
    }
    else {//Nothing was read
        std::cerr << "Error: Primary domain missing for Reviewer: "<<info.id<<"\n";
        return;
    }
    //Secondary Domain(Optional)
    if (std::getline(ss, token, ',')) {
        trim(token);
        if (token.empty()) {
            info.secondaryDomain=-1;
        }
        else
            info.secondaryDomain=std::stoi(token);
    }
    else { //nothing was read
        info.secondaryDomain=-1;
    }

    manager.addReviewer(info.id,info);
}

// Lê parâmetros gerais que influenciam a construção da rede e as restrições.
void Parser::parseParameterLine(const std::string& line) {
    std::stringstream ss(line);
    std::string key, valueStr;

    if (std::getline(ss, key, ',') && std::getline(ss, valueStr, ',')) {
        trim(key);
        trim(valueStr);
    }
    if (valueStr.empty()) {return;}

    int value=std::stoi(valueStr);

    // Cada chave do input atualiza diretamente o estado do CentralManager.
    if (key=="MinReviewsPerSubmission") {
        manager.setMinReviewsPerSubmission(value);
    }
    else if (key=="MaxReviewsPerReviewer") {
        manager.setMaxReviewsPerReviewer(value);
    }
    else if (key=="PrimaryReviewerExpertise") {
        manager.setPrimaryReviewerExpertise(value);
    }
    else if (key=="SecondaryReviewerExpertise") {
        manager.setSecondaryReviewerExpertise(value);
    }
    else if (key=="PrimarySubmissionDomain") {
        manager.setPrimarySubmissionDomain(value);
    }
    else if (key=="SecondarySubmissionDomain") {
        manager.setSecondarySubmissionDomain(value);
    }

}

// Lê opções de execução, como geração de output e análise de risco.
void Parser::parseControlLine(const std::string &line) {
    std::stringstream ss(line);
    std::string key,valueStr;

    if (std::getline(ss,key,',') && std::getline(ss, valueStr, ',')) {
        trim(key);
        trim(valueStr);
    }

    if (valueStr.empty()) {return;}

    if (key=="GenerateAssignments") {
        const int value=std::stoi(valueStr);
        manager.setGenerateAssignments(value);
    }
    
    if (key=="RiskAnalysis") {
        const int value=std::stoi(valueStr);
        manager.setRiskAnalysis(value);
    }

    // Remove aspas para evitar criar ficheiros com nome inválido.
    if (key=="OutputFileName") {
        manager.setOutputFilename(stripQuotesCopy(valueStr));
    }
}