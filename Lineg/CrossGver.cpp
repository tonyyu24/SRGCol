#include <algorithm>
#include <cctype>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <new>
#include <set>
#include <sstream>
#include <string>
#include <string.h>
#include <time.h>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <vector>

#if DEBUG == 1
  std::ofstream log("log.txt");
#endif

struct pair_cmp {
  bool operator() (const std::pair<int, int> &a, const std::pair<int, int> &b) const {
    if(a.first == b.first) return a.second < b.second;
    else return a.first < b.first;
  }
};

struct tupl_cmp {
  bool operator() (const std::tuple<int, int, int> &a, const std::tuple<int, int, int> &b) const {
    if(std::get<0>(a) == std::get<0>(b)) {
      if(std::get<1>(a) == std::get<1>(b)) return std::get<2>(a) < std::get<2>(b);
      else return std::get<1>(a) < std::get<1>(b);
    }
    else return std::get<0>(a) < std::get<0>(b);
  }
};

class Graph {
  private:
    int  order_v;
    std::list<int> *adjlist;
    std::set<std::pair<int, int>, pair_cmp> edge;
    std::set<std::tuple<int, int, int>, tupl_cmp> triangle;
  public:
    Graph(int);
    ~Graph();
    void adj_insert(int, int);
    std::pair<int, int> access_edge(int);
    void color(int ,int);
    void edges();
    int  edgesize();
    inline int order();
    void print_adj();
    void print_edges();
    void triangles();
    int  trianglesize();

    friend void verify(Graph*, std::string, std::string, int, bool, bool, bool);
};

Graph::Graph(int o) {
  order_v = o;
  adjlist = new std::list<int>[o];
}

Graph::~Graph() {
  delete [] adjlist;
}

void Graph::adj_insert(int ind, int d) {
  adjlist[ind].insert(adjlist[ind].end(), d);
#if DEBUG == 1
  std::streambuf *coutbuf = std::cout.rdbuf(); 
  std::cout.rdbuf(log.rdbuf()); 
  std::cout << "Inserted edge (" << ind + 1 << ", " << d + 1 << ")\n"; 
  std::cout.rdbuf(coutbuf);
#endif
}

void Graph::edges() {
  for(int i = 0; i < order_v; i++) {
    std::list<int>::iterator j = adjlist[i].begin();
    for(int k = 0; k < i; k++) {
      edge.insert(edge.end(), std::make_pair(std::min(i, *j), std::max(i, *j)));
      j++;
      if(j == adjlist[i].end()) break;
    }
  }
#if DEBUG == 1
  std::streambuf *coutbuf = std::cout.rdbuf(); 
  std::cout.rdbuf(log.rdbuf()); 
  print_edges(); 
  std::cout.rdbuf(coutbuf);
#endif
}

int Graph::edgesize() {
  return this->edge.size();
}

void Graph::triangles() {
  for(auto const &p : edge) { 
    for(auto const &r : edge) {
      if(p == r) ;
      else {
        if(p.first == r.first) {
          if(edge.find(std::make_pair(p.second, r.second)) != edge.end()) {
            auto tri = std::make_tuple(p.first, p.second, r.second);
            triangle.insert(triangle.end(), tri);
          }
        }
      }
    }
  }
}

int Graph::trianglesize() {
  return this->triangle.size();
}

void Graph::print_adj() {
  for(int i = 0; i < order_v; i++) {
    std::cout << i << ": ";
    for(std::list<int>::iterator j = adjlist[i].begin(); j != adjlist[i].end(); j++) {
      if(j == --adjlist[i].end()) std::cout << *j << "\n";
      else std::cout << *j << ", ";
    }
  }
}

void Graph::print_edges() {
  int count = 0;
  for(auto const &p : edge) {
    std::cout << "(" << p.first << ", " << p.second << ")\n";
    count++;
  }
  std::cout << "Edges: " << count << "\n";
}

inline int Graph::order() {
  return this->order_v;
}

/*
*
*   As documented in cnfgen.cpp
*
*/

inline int vindex(int j, int k) {
  //Returns first variable encoding data for vertex j
  return 2 * j * k + 1;
}

inline int eindex(int j, int k, int vert) {
  //Returns first variable encoding data for edge j 
  //Edges are lexicographically ordered by std::set
  return 2 * j * k + 2 * (vert - 1) * k + 1;
}

inline int sindex(int j, int k, int counter) {
  return counter + k + j;
}

//We need the graphs to parse the witnesses
int parse() {

  //Initial parsing of the showg output to find the graph order
  std::string first;
  std::getline(std::cin, first);
  std::getline(std::cin, first);

  size_t num = first.find_last_not_of("0123456789");

  std::string ord = first.substr(num, std::string::npos);
  
  int res = std::stoi(ord);

  return res;

}

void verify(Graph *G, std::string sourcename, std::string outname, int col, bool vertex, bool edge, bool triangle) {
  freopen(sourcename.c_str(), "r", stdin);
  freopen(outname.c_str(), "w", stdout);
  
  int vert = G->order();
  int edgesize = G->edgesize();

  int vertc, edgec;

  if(vertex) {
    vertc = vert;
  } else {
    vertc = 0;
  }
  if(edge) {
    edgec = edgesize;
  } else {
    edgec = 0;
  }

  char curr[256];
  int vecounter = 0;

  std::vector<std::pair<int, int> > edges;
  std::copy(G->edge.begin(), G->edge.end(), std::back_inserter(edges));
  assert(edges.size() == G->edgesize());

  std::vector<std::tuple<int, int, int> > triangles;
  std::copy(G->triangle.begin(), G->triangle.end(), std::back_inserter(triangles));
  assert(triangles.size() == G->trianglesize());

  std::vector<std::vector<int> > color_classes(vert);

  while(!feof(stdin)) {
    scanf("%s", curr);
    if(std::isdigit(curr[0])) {
      int x = std::stoi(curr);
      int y = x - (2 * vecounter * col);
      if(1 <= y && y <= col) {
        if(vecounter < vertc) {
          color_classes[vecounter].push_back(y);
          for(const int j : G->adjlist[vecounter]) {
            std::vector<int>::iterator begin = color_classes[j].begin();
            std::vector<int>::iterator end = color_classes[j].end();
            if(std::find(begin, end, y) != end) throw 1;
          } 
          std::cout << "Vertex " << vecounter << ": Color " << y << std::endl;
        }
        else if (vecounter - vertc < edgec){
          int curredge = vecounter - vertc;
          std::pair<int, int> cpair = edges[curredge];
          int first = cpair.first;
          int second = cpair.second;
          if(std::find(color_classes[first].begin(), color_classes[first].end(), y) != color_classes[first].end()) throw 1;
          if(std::find(color_classes[second].begin(), color_classes[second].end(), y) != color_classes[second].end()) throw 1;
          color_classes[first].push_back(y);
          color_classes[second].push_back(y);
          std::cout << "Edge (" << first << ", " << second << "): Color " << y << std::endl;
        }
        else {
          int currtri = vecounter - edgesize - vertc;
          std::tuple<int, int, int> ctri = triangles[currtri];
          int first  = std::get<0>(ctri);
          int second = std::get<1>(ctri);
          int third  = std::get<2>(ctri);
          if(std::find(color_classes[first].begin(), color_classes[first].end(), y) != color_classes[first].end()) throw 1;
          if(std::find(color_classes[second].begin(), color_classes[second].end(), y) != color_classes[second].end()) throw 1;
          if(std::find(color_classes[third].begin(), color_classes[third].end(), y) != color_classes[third].end()) throw 1;
          color_classes[first].push_back(y);
          color_classes[second].push_back(y);
          color_classes[third].push_back(y);
          std::cout << "Triangle (" << first << ", " << second << ", " << third << "): Color " << y << std::endl;
        }
        vecounter++;
      }
    }
  }
}

int main(int argc, char *argv[]) {

  //The program takes in the inputs ./. -vet filename c order
  //and translates all witnesses associated with the 
  //given parameters

  bool vertex, edge, triangle;

  //Thanks to StackOverflow
  int opt;
  while((opt = getopt(argc, argv, "vet")) != -1) {
    switch(opt) {
      case 'v':
        vertex = true;
        break;
      case 'e':
        edge = true;
        break;
      case 't':
        triangle = true;
        break;
      default:
        fprintf(stderr, "Usage: %s [-vet] filename c order\n", argv[0]);
        exit(-1);
    }
  }
  if(optind == 1) {
    fprintf(stderr, "Usage: %s [-vet] filename c order\n", argv[0]);
    exit(-1);
  }

  std::string filename = argv[optind];
  std::string col = argv[optind + 1];
  int c = std::stoi(col);

  std::string graphsource = "Lineg/" + filename + ".txt";

  freopen(graphsource.c_str(), "r", stdin);
  int order = parse();

  std::ios::sync_with_stdio(false);

  std::vector<Graph*> graphs;
  while(!std::cin.eof() && std::cin.good()) {
    Graph *graph = new Graph(order);
    graphs.insert(graphs.end(), graph);
    for(int i = 0; i < order; i++) {
      for(int j = 0; j < order; j++) {
        int x;
        std::cin >> x;
        if(x) graph->adj_insert(i, j);
      }
    }
#if DDEBUG == 1
  graphs->print_adj();
#endif
    graph->edges();
    graph->triangles();

    //This trick is unique to showg, will generalize later
    std::string skip;
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
    std::getline(std::cin, skip);
  }

  int i = 1;

  for(Graph *j : graphs) {
    std::string sourcename = "CNF/" + filename + "G" + std::to_string(i);

    std::string outname = "CNF/" + filename + "G" + std::to_string(i);

    if(vertex) {
      sourcename.append("v");
      outname.append("v");
    }
    if(edge) {
      sourcename.append("e");
      outname.append("e");
    }
    if(triangle) {
      sourcename.append("t");
      outname.append("e");
    }
    sourcename = sourcename + "col" + col + ".out";
    outname = outname + "col" + col + ".txt";

    verify(j, sourcename, outname, c, vertex, edge, triangle);
    i++;
  }
}