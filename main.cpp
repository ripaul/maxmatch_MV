/*
This file is intended to serve as a usage example.
Call with:
./main [filename]
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <sstream>


#include "Graph.h"
#include "graph_simple.h"
#include "read_table.h"

//for time measurement
#define dt(b,a) ( (double) (a.tv_usec - b.tv_usec) / 1000000 + (double) (a.tv_sec - b.tv_sec))


void print (MVGraph g) {
        std::cerr << "start" << std::endl;
        for(nodeid j = 0; j < g.nodes.size(); j++) {
                MVNode& n = g.nodes[j];
                
                for(unsigned int k=0;k<n.degree;k++) {
                        nodeid i = g.edges[n.edges_idx + k];
                        
                        std::cerr << " " << j << " " << i << std::endl;
                }
        }
        std::cerr << "end" << std::endl;
}


int main(int argc, char** argv){
        std::string matching_path;
        std::string graph_path;
        
        std::vector< std::pair< unsigned long, unsigned long > > matching;
        
        if (argc > 2) {
                matching_path = argv[2];
                std::ifstream m_in(matching_path);
                std::string line; 
                // count lines in i
                unsigned long i = 0, source, target;
                
                while (std::getline(m_in, line)) {
                        std::istringstream iss(line);
                        iss >> source >> target;
                        matching.push_back({source, target});
                }
        }
        
        if (argc > 1) {
                graph_path = std::string(argv[1]);
        } 
        
        if (argc <= 1) {
                // at least a graph file has to be passed
                std::cerr << "too few arguments";
                return EXIT_FAILURE;
        }
        
        std::ifstream in (graph_path);
        
        unsigned int mode, source, target;
        std::vector<unsigned int> e1, e2;
        std::unordered_map<unsigned int, unsigned int> ids;
        std::string line;
        
        while (std::getline(in, line)) {
                // skip lines starting with comment symbol
                if (line[0] == '#') continue;
                
                std::istringstream iss(line);
                // each line in graph file consists of update mode, source, target.
                // since we are considering static graphs here, mode may be ignored.
                iss >> source >> target;
                
                // save forward and backward edges
                e1.push_back(source);
                e2.push_back(target);
                
                e1.push_back(target);
                e2.push_back(source);
        }
        
        // create graph from edge list
        graph g1;
        g1.create_graph_vectors(e1, e2, &ids);

        MVGraph g(g1);
        
        //~ print(g);
        
        g.init(matching);
        //~ g.greedy_init();
	
	std::cout << g.matchnum << std::endl;
        
        g.max_match();
        
        return 0;
}
