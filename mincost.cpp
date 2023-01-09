#include "graph.cpp"



bool checkGraph(Graph & graph);  /* retourne un booléen qui indique si les flots sont valides */



int main(int nbArgs,char **values)
{
  Graph testGraph("instances/example.dat");
  testGraph.print();
  return 0;
}
