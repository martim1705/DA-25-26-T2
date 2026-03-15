#pragma once

#include "Graph.h"
#include "DataEntities.h"
#include <unordered_map>
#include <string>
#include <iostream>

class CentralManager {
private:

    /*
    Creates a source node, a sink node, reviewer vertices, and submission vertices.
    Adds edges from source to reviewers, from reviewers to compatible submissions,
    and from submissions to sink
    */
    void buildPrimaryOnlyNetwork(
        Graph<NodeInfo>& network,
        std::unordered_map<int, Vertex<NodeInfo>*>& netSubs,
        std::unordered_map<int, Vertex<NodeInfo>*>& netRevs,
        Vertex<NodeInfo>*& source,
        Vertex<NodeInfo>*& sink,
        int excludeReviewerId = -1
    ) const;

    /*
    Reads the flow values on reviewer-to-submission edges and keeps the matches
    with positive flow.
    */
    std::vector<std::tuple<int,int,int>> extractAssignement(
        const std::unordered_map<int, Vertex<NodeInfo>*>& netRevs
    ) const;

    /*
    Compares the flow reaching the sink with the required minimum number of reviews
    for each submission.
    */
    std::vector<std::tuple<int,int,int>> extractMissingReviews(
        const std::unordered_map<int, Vertex<NodeInfo>*>& netSubs
    ) const;

    std::vector <std::tuple<int,int,int>> lastAssignments; // submissionId, reviewerId, matchDomain
    std::vector <std::tuple<int,int,int>> lastMissing; // submissionId, domain, missingReviews

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
    bool loadFiles(const std::string& filename);
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

    void showSubmissions() const;
    void showReviewers() const;
    void showParameters() const;

    /*
    Builds the flow network, computes the maximum flow, and stores the resulting
    assignments or missing reviews.
    */
    bool runPrimaryOnlyAssigment();

    /*
    A reviewer is considered risky if removing them from the network makes the
    assignment infeasible.
    */
    std::vector<int> evaluateRiskone() const;

    /*
    Writes either the successful assignment lists or the list of submissions with
    missing reviews, depending on the latest execution result.
    */
    bool writeAssignementOutput(const std::string& filename) const;

    //Writes the result of the risk analysis to an output file.
    bool writeRiskOutput(const std::string& filename, const std::vector<int>& risky) const;

    // Tarefa 1.1 - tipos de menus
    void runInteractiveMenu();
    void runBatchMode(const std::string & string, const std::string & risk_file);
};

