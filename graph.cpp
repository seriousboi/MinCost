#include <unordered_map>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;



class Arc
{
  public:
    float flow;
    float cost;
    float capacity;
    float residualCapacity;

    Arc(float capacity,float cost);
};



class Graph
{
  public:
    int nbVertices;
    vector<int> productions;
    vector<unordered_map<int,vector<Arc>>> vertices;

    Graph(int nbVertices);
    Graph(string filename);
    void addArc(int start,int end,float capacity,float cost);
    void print();
};



Arc::Arc(float capacity,float cost)
{
  this->capacity = capacity;
  this->cost = cost;
  flow = 0;
  residualCapacity = capacity;
}



Graph::Graph(string filename)
{
  std::ifstream dataStream(filename);
  if(!dataStream)
  {
    std::cout << "oof, cannot open file " << filename << std::endl;
    return;
  }
  string lineIdentifier;
  dataStream >> lineIdentifier;
  while(not dataStream.eof())
  {
    if(lineIdentifier=="p")
    {
      dataStream >> lineIdentifier; /* on saute le "min" */
      dataStream >> nbVertices;
      vertices = vector<unordered_map<int,vector<Arc>>>(nbVertices);
      productions = vector<int>(nbVertices);
      int nbEdges;
      dataStream >> lineIdentifier; /* on saute le nombre d'arcs */
    }
    else if(lineIdentifier=="n")
    {
      int vertexIndex;
      dataStream >> vertexIndex;
      vertexIndex--; /* l'indexage commence à 1 dans les fichiers */
      dataStream >> productions[vertexIndex];
    }
    else if(lineIdentifier=="a")
    {
      int startIndex,endIndex;
      dataStream >> startIndex;
      dataStream >> endIndex;
      startIndex--;
      endIndex--;
      dataStream >> lineIdentifier; /* on saute mincap */
      float capacity,cost;
      dataStream >> capacity;
      dataStream >> cost;
      addArc(startIndex,endIndex,capacity,cost);
    }
    dataStream >> lineIdentifier;
    cout<<lineIdentifier<<endl;
  }
}



Graph::Graph(int nbVertices)
{
  this->nbVertices = nbVertices;
  vertices = std::vector<std::unordered_map<int,vector<Arc>>>(nbVertices);
}



void Graph::addArc(int start,int end,float capacity,float cost) /* ajoute l'arc du sommet start vers le sommet end */
{
  if(not(start<nbVertices and end<nbVertices and start>=0 and end>=0))
  {
    cout << "Cannot add invalid edge to graph, yikes\n";
    return;
  }
  vertices[start][end].push_back(Arc(capacity,cost));
}



void Graph::print() /* affiche les listes d'adjacences sous forme <sommet i: (voisin1,coût1) (voisin2,coût2)> */
{
  std::cout << "graph with " << nbVertices << " vertices\n";
  for(int vertex=0;vertex<nbVertices;++vertex)
  {
    std::cout << "vertex " << vertex << ": ";

    for(auto &[neighbor,arcs] : vertices[vertex])
    {
      for(Arc & arc : arcs)
      {
        std::cout << "(" << neighbor << "," << arc.capacity << ") ";
      }
    }
    std::cout << "\n";
  }
}
