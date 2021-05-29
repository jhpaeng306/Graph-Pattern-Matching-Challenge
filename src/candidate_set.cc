/**
 * @file candidate_set.cc
 *
 */

#include "candidate_set.h"
//#define SORT_CANDIDATE_SET

<<<<<<< HEAD
CandidateSet::CandidateSet(const std::string& filename, const Graph &data, const bool &sortCS) {
=======
CandidateSet::CandidateSet(const std::string& filename, const Graph &data, bool useCSrank) {
>>>>>>> 40e7383354249ce7959e1d0c8d920e47882f565b
  std::ifstream fin(filename);

  if (!fin.is_open()) {
    std::cout << "Candidate set file " << filename << " not found!\n";
    exit(EXIT_FAILURE);
  }

  char type;
  size_t num_query_vertices;

  fin >> type >> num_query_vertices;

  cs_.resize(num_query_vertices);

  while (fin >> type) {
    if (type == 'c') {
      Vertex id;
      size_t candidate_set_size;

      fin >> id >> candidate_set_size;

      cs_[id].resize(candidate_set_size);

      for (size_t i = 0; i < candidate_set_size; ++i) {
        Vertex data_vertex;
        fin >> data_vertex;
        cs_[id][i] = data_vertex;

<<<<<<< HEAD
      }

      if (sortCS) {

          // code for sorting CS in degree size order
          auto compare = [&data](Vertex u, Vertex v) {
              return (data.GetDegree(u) < data.GetDegree(v));
          };
          std::sort(cs_[id].begin(), cs_[id].end(), compare);

      }
=======
      }
        
        if (useCSrank) {
            // code for sorting CS in degree size order
           auto compare = [&data](Vertex u, Vertex v) {
               return (data.GetDegree(u) < data.GetDegree(v));
           };
           std::sort(cs_[id].begin(), cs_[id].end(), compare);
        }
        
>>>>>>> 40e7383354249ce7959e1d0c8d920e47882f565b

    }
  }

  fin.close();
}

CandidateSet::~CandidateSet() {}
