#include "Graph.h"


void MVGraph::max_match() {
	//for once set the levels by hand, next time reset does this
	for(int i=0;i<nodes.size();i++) {
		if(nodes[i].match == UNMATCHED) {
			add_to_level(0,i);
			nodes[i].set_min_level(0);
		}
	}
	
	bool found = max_match_phase();
	while(nodes.size() / 2 > matchnum && found){
		//~ debug("===================================");
		reset();
		found = max_match_phase();
		//~ debug("===================================");
	}
}

bool MVGraph::max_match_phase() {
	bool found = false;
	int i;

  
	for(int i=0;i<nodes.size()/2+1 && (!found);i++){
		//printf(":: %i %i \n",g->todonum ,g->bridgenum);
		if(todonum<=0 && bridgenum<=0){
			return false;
		}
		MIN(i);
		found = MAX(i);
    }
	return found;
}


inline void MVGraph::step_to(nodeid to, nodeid from, int level) {
	level++; 
	int tl = nodes[to].min_level;
	if(tl >= level) {
	    if(tl != level){
			add_to_level(level,to);
			nodes[to].set_min_level(level);
	    }
	    nodes[to].preds.push_back(from);
	    nodes[to].number_preds++;
	    
	    nodes[from].pred_to.push_back(MVNodePos(to,nodes[to].preds.size()-1));
	}
	else {
	    //found bridge
	    int ten = tenacity(to,from);
	    if(ten == -1) {
			//hanging bridge
			nodes[to].hanging_bridges.push_back(from);
			nodes[from].hanging_bridges.push_back(to);
	    }
	    else add_to_bridges((ten-1)/2,to,from);
	}
}


void MVGraph::MIN(int i) {
	if(levels.size() <= i) {
		//~ fprintf(stderr,"MVGraph::MIN(): no nodes on level %d!\n",i);
		return;
	}
	for(int& current : levels[i]) {
		todonum--;
		MVNode& n = nodes[current];
		if(i%2==0) {
			for(int j=0;j<n.degree;j++) {
				int edge = edges[n.edges_idx + j];
				if(edge != n.match) step_to(edge,current,i);
			}
		}
		else if(n.match != UNMATCHED) step_to(n.match,current,i);
	}
}


//~ int last_n1 = UNSET,last_n2 = UNSET;
bool MVGraph::MAX(int i) {
	bool found = false;
	if(bridges.size() <= i) {
		//~ fprintf(stderr,"MVGraph::MAX(): no bridges on level %d!\n",i);
		return false;
	}
	for(const MVBridge& current : bridges[i]) {
		bridgenum--;
		nodeid n1 = current.first;
		nodeid n2 = current.second;
		if(nodes[n1].deleted || nodes[n2].deleted) continue;
		int result = DDFS(n1,n2);
		if(result == DDFS_ERROR) { /* ??? */ }
		if(result == DDFS_EMPTY) continue;
		if(result == DDFS_PATH) {
			find_path(n1,n2);
			//~ debug("+++++++");
			augment_path();
			if(nodes.size() / 2 <= matchnum) return true; //perfect matching
			remove_path();
			found = true;
		}
		else {
			/* Found a petal */
			nodeid b = last_ddfs.bottleneck;
			int current_ten = i*2+1;
			for(nodeid& itt : last_ddfs.nodes_seen) {
				nodes[itt].bud = b; /* set the bbud for all nodes */
				/* set the maxlevel */
				nodes[itt].set_max_level(current_ten - nodes[itt].min_level);
				add_to_level(nodes[itt].max_level,itt);
				for(nodeid& hanging : nodes[itt].hanging_bridges) {
					int hanging_ten = tenacity(itt,hanging);
					if(hanging_ten != -1) add_to_bridges((hanging_ten-1)/2,itt,hanging);
				}
			}
		}
	}
	return found;
}
