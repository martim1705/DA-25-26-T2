#pragma once

#include "Graph.h"
#include "DataEntities.h"
#include <unordered_map>
#include <string>

class CentralManager {
private:

    //Graph and Sets.
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

/**Subsets*/
    void addSubmission(int id, const NodeInfo& info);
    void addReviewer(int id, const NodeInfo& info);
    const std::unordered_map<int, Vertex<NodeInfo>*>& getSubmissions() const;
    const std::unordered_map<int, Vertex<NodeInfo>*>& getReviewers() const;

/**Parametros*/
    void setMinReviewsPerSubmission(int value);
    int getMinReviewsPerSubmission() const;

    void setMaxReviewsPerReviewer(int value);
    int getMaxReviewsPerReviewer() const;

/**Este quatro parecem-me redundantes para lógica***TIRAR DÚVIDA****/
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
};

