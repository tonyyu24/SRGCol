import sys
from sage.graphs.graph_list import from_graph6
from sage.graphs.line_graph import line_graph

def lineg(filename):
  with open(filename, 'r') as f:
    L = f.readlines()
  f.close()
  l = from_graph6(L)
  pos = filename.find('/')
  outname = "Lineg" + filename[pos:(len(filename) - 3)] + "LGadj.txt"
  sys.stdout = open(outname, 'w')
  count = 1
  triangleset = set()
  for G in l:
    print("Graph " + str(count))
    lg = G.line_graph()
    print("Adjacency Matrix:\n" + str(lg.adjacency_matrix()), flush = True)
    print("Cubed Adjacency Matrix:\n" + str((lg.adjacency_matrix()^3)), flush = True)
    lgt = lg.triangles_count()
    print("Triangles: " + str(lgt) + "\n", flush = True)
    triangleset.add(lgt)
    count = count + 1
  print(triangleset, flush = True)