#include <algorithm>
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
#include <time.h>
#include <tuple>
#include <unistd.h>
#include <utility>
#include <vector>

//Thanks to StackOverflow
int exec(const char* cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if(!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return std::stoi(result);
}

int main(int argc, char *argv[]) {

  //The program takes in the inputs ./. -vet filename col
  //and runs all cnfs associated with the given parameters

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
        fprintf(stderr, "Usage: %s [-vet] filename col\n", argv[0]);
        exit(-1);
    }
  }
  if(optind == 1) {
    fprintf(stderr, "Usage: %s [-vet] filename col \n", argv[0]);
    exit(-1);
  }

  std::string filename = argv[optind];
  std::string col = argv[optind + 1];

  std::string searchname = filename + "*" + col + ".cnf";

  int c = exec(("find CNF/ -name " + searchname + " | wc -l").c_str());

  int sat = 0;

  for(int i = 1; i < c + 1; i++) {
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

    sourcename = sourcename + "col" + col + ".cnf";
    outname = outname + "col" + col + ".out";
    std::system(("./cadical-master/build/cadical -q -w " + outname + " " + sourcename).c_str());
    
    std::ifstream c(outname.c_str());
    std::stringstream out;
    out << c.rdbuf();
    if (out.str().find("s SATISFIABLE") != std::string::npos) sat++;
    else std::cout << "GRAPH " << i << " UNSATSIFIABLE" << std::endl;
  }

  std::string statout = "CNF/" + filename + "col" + col + "stat.txt";
  
  freopen(statout.c_str(), "w", stdout);
  std::cout << "SATISFIABLE: " << sat << "/" << c;
}