/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

//#define VALIDATOR // CONDITIONAL COMPILE: VALIDATOR (Comment out not to use validator)



Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}


bool Backtrack::PrintAllMatches(const Graph &data, const Graph &query,
                                const CandidateSet &cs, bool newRank) {

    Embedding embedding;
    time_t start = clock();

    if (newRank) {
        std::cout << "t " << query.GetNumVertices() << "\n";
    }

    //Array for visited checking - initialization
    visited.clear();
    visited.resize(data.GetNumVertices());
    std::fill(visited.begin(), visited.end(), false);

    size_t queryVertexNum = query.GetNumVertices();

    //rank value computation. rank value is used on topological order of queue, to improve bactracking by giving efficient order.
    //Idea was from DAG ordering.
    std::vector<std::pair<double, Vertex>> rank;
    for (size_t i = 0; i < queryVertexNum; i++) {
        rank.push_back(std::make_pair(cs.GetCandidateSize(i) / (0.01 * queryVertexNum / data.GetNumLabels() + query.GetDegree(i)), i));
    }

    std::sort(rank.begin(), rank.end());
    
    //To make searching order such that searches nodes that is a connected component of previous queue embedding
    if (newRank) {
        std::vector<std::pair<double, Vertex>> Rank;
        Rank.push_back(rank.at(0));
        rank.erase(rank.begin());
        int i = 0;
        
        while (!rank.empty()) {
            bool b = false;
            int j = 0;
        
            while (b == false && j < Rank.size()) {
                b = query.IsNeighbor(Rank.at(j).second, rank.at(i).second);
                j++;
            }
            if (b == true) {
                Rank.push_back(rank.at(i));
                rank.erase(rank.begin() + i);
                i = 0;
            }
            else {
                i ++;
            }
        }
        rank = Rank;
    }
    

    std::vector<std::pair<Vertex, Vertex>> backStack; // Stack for backtracking. Storing pair of <qVertex, dVertexId>

    Vertex qVertexId = 0;
    Vertex dVertex = 0;

    int total = 0;
    
    // Push root node to stack
    for (size_t i = 0; i < cs.GetCandidateSize(rank.at(qVertexId).second); i++){ // for each candidate v
        dVertex = cs.GetCandidate(rank.at(qVertexId).second, i);
        backStack.push_back(std::make_pair(qVertexId, dVertex));
    }

    // DFS backtrack using stack
    while(!backStack.empty()) {
        
        // In case if the progaram has found 1e-5 solutions.
        if (total == 100000) {
            return true;
        }
        
        // If first trial has failed, choose second method
        if (newRank && total == 0 && (double)(clock() - start)/CLOCKS_PER_SEC  >= 20) {
            return false;
        }
        
        qVertexId = backStack.back().first;
        dVertex = backStack.back().second;
        embedding.push_back(std::make_pair(rank.at(qVertexId).second, dVertex));
        // conditional branch(1): if |M| = |V(q)|
        if (qVertexId == queryVertexNum - 1) {

#ifdef VALIDATOR
            if (validate(data, query, embedding, rank)) {
                printEmbedding(embedding);
            }
            else {
                std::cout << "Validation failed!" << std::endl;
            }
#else
            printEmbedding(embedding);
#endif

            backStack.pop_back();
            embedding.pop_back();
            total ++;

            while (backStack.back().first < qVertexId) {
                qVertexId = backStack.back().first;
                visited.at(backStack.back().second) = false;
                embedding.pop_back();
                backStack.pop_back();
            }

        }
        else {
            visited.at(dVertex) = true;
            // conditional branch(2) and (3)
            size_t numCandidates = cs.GetCandidateSize(rank.at(qVertexId + 1).second); // C(r) : Number of candidates for root vertex

            for (size_t i = 0; i < numCandidates; i++){ // for each candidate v
                dVertex = cs.GetCandidate(rank.at(qVertexId + 1).second, i);

                // Injective Mapping Condition
                if (!visited.at(dVertex)) {
                    if (checkEdgeConnection(data, query, embedding, dVertex, rank.at(qVertexId + 1).second)) {
                        backStack.push_back(std::make_pair(qVertexId + 1, dVertex));
                    }
                }
            }

            // Dead end, none pushed to stack
            if (qVertexId == backStack.back().first) {
                visited.at(backStack.back().second) = false;
                backStack.pop_back();
                embedding.pop_back();

                while (backStack.back().first < qVertexId) {
                    qVertexId = backStack.back().first;
                    visited.at(backStack.back().second) = false;
                    embedding.pop_back();
                    backStack.pop_back();
                }

            }

        }
    }
    return true;
}

// return true is choosen vertex of Graph is well connected.
bool Backtrack::checkEdgeConnection(const Graph &data, const Graph &query, const Embedding &embedding, Vertex dVertex, Vertex qVertex) {

    bool edgeMatches = true;

    // for all vertices in advance of vertex(queryVertex)
    for (Embedding ::const_iterator iter = embedding.begin(); iter != embedding.end(); iter++) {
        if (!data.IsNeighbor(dVertex, iter->second) && query.IsNeighbor(qVertex, iter->first)) {
            edgeMatches = false;
            return edgeMatches;
        }
    }

    return edgeMatches;
}

// print the embedding solution
void Backtrack::printEmbedding(const Embedding &embedding) {
    std::cout << "a";
    for (Embedding ::const_iterator iter = embedding.begin(); iter != embedding.end(); iter++) {
        std::cout << " " << iter->second;
    }
    std::cout << std::endl;
}

// validator
bool Backtrack::validate(const Graph &data, const Graph &query, const Embedding &embedding, std::vector<std::pair<double, Vertex>> &rank) {

    // Things to check
    // (1) number of verticies
    // (2) lines in query / lines in embedding
    // (3) labels in query / labels in embedding
    size_t numQueryVertices = query.GetNumVertices();
    if (numQueryVertices != embedding.size()) {
        std::cout << "Number of vertices does not match!" << std::endl;
        return false;
    }

    // Loop through every verticies in query
    Vertex qVertex1, qVertex2, dVertex1, dVertex2;

    for (size_t i = 0; i < numQueryVertices; i++) {

        qVertex1 = rank.at(i).second;
        dVertex1 = embedding.at(i).second;


        // check label match
        if (query.GetLabel(qVertex1) != data.GetLabel(dVertex1)) {
            std::cout << "Label does not match!" << std::endl;
            return false;
        }

        // check line match
        for (size_t j = i+1; j < numQueryVertices; j++) {

            qVertex2 = rank.at(j).second;
            dVertex2 = embedding.at(j).second;

            if (query.IsNeighbor(qVertex1, qVertex2) && !data.IsNeighbor(dVertex1, dVertex2)) {
                std::cout << "Line does not match!" << std::endl;
                return false;
            }
        }

    }
    return true;


}



/*
 * Basic PSEUDO CODE FOR BACKTRACKING
 * REFERENCE: Efficient Subgraph Matching:
 *            Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together
 * Backtrack(q, q_D, CS, M)
 * if |M| = |V(q)| then
 *      Report M;
 * else if |M| = 0 then
 *      foreach v in C(r) do
 *      M <- {(r, v)};
 *      Mark v as visited;
 *      Backtrack(q, q_D, CS, M);
 *      Mark v as unvisited;
 * else
 *      u <- extentable vertex with min weight w_m(u);
 *      foreach v in C_M(u) do
 *          if v is unvisited then
 *              M' <- M \union {(u, v}};
 *              Mark v as visited;
 *              Backtrack(q, q_D, CS, M');
 *              Mark v as unvisited;
 * return
 */
