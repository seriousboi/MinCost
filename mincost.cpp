#include "graph.cpp"



bool checkGraph(Graph & graph);  /* retourne un booléen qui indique si les flots sont valides */



int main(int nbArgs,char **values)
{
  Graph testGraph(4);
  testGraph.addArc(0,1,10,4);
  testGraph.addArc(0,1,3,2);
  testGraph.print();
  return 0;
}
