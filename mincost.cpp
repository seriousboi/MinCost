#include <unordered_set>
#include <limits.h>
#include <chrono>
#include "graph.cpp"



bool checkGraph(Graph & graph);  /* retourne un booléen qui indique si les flots sont valides et affiche le cout */
int cost_sol(Graph & graph);   /* retourne le coût de la solution retournée */
void pccs(Graph & graph);       /* affècte un flot de cout minimal avec l'algorithme de plus courts chemins augmentants */
vector<vector<int>>  dijkstra(Graph & graph,int start,vector<int> potentials);  /* retourne les vecteur des distances et des prédécésseurs des plus courts chemins depuis start */
int findClosestTempVertex(vector<int> distances,unordered_set<int> temporarilyTagged); /* fonction pour dijkstra, trouve le sommet avec une étiquette temporaire le plus proche du départ */
int findMaxFlowAlongPath(Graph & graph,int start,int end,vector<int> predecessors,vector<int> potentials);
void increaseAlongPath(Graph & graph,int start,int end,vector<int> predecessors,int increase);
void runTests(); /* teste le PCCS sur toutes les instances et affiche les resultats */



int main(int nbArgs,char **values)
{
  runTests();
  return 0;
}



void runTests()
{
  vector<string> filenames = {
  "instances/example.dat","instances/example_small_1.dat","instances/example_small_2.dat",
  "instances/example_medium_1.dat","instances/example_medium_2.dat","instances/example_medium_3.dat","instances/example_medium_4.dat",
  "instances/example_large_1.dat","instances/example_large_2.dat"};

  chrono::steady_clock::time_point startTime;
  chrono::steady_clock::time_point endTime;

  for(string filename:filenames) /* boucle pour parcourir les fichiers */
  {
    cout << filename << endl;
    Graph graph(filename);

    startTime = chrono::steady_clock::now();
    pccs(graph);
    endTime = chrono::steady_clock::now();

    int minCost = cost_sol(graph);
    cout << "Cost: " << minCost << endl;

    cout << "Solution found in " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " ms\n";

    if(checkGraph(graph)){cout << "Valid solution" << endl;}
    else{cout << "Invalid Solution" << endl;}

    cout << endl;
  }
}



void pccs(Graph & graph)
{
  graph.addSuperSourceSink(); /* ajout d'une super source et d'un super puit */
  graph.symmetrisation(); /* on symmétrise pour avoir les arcs inverse */

  vector<int> potentials(graph.nbVertices,0); /* on initialise les différentes variables pour l'algorithme */
  int excess = graph.productions[graph.sourceIndex]; /* on ne regarde que un excès durant l'algorithme grace à la super source */

  while(excess > 0)
  {
    vector<vector<int>> results = dijkstra(graph,graph.sourceIndex,potentials); /* on fait toujours dijkstra depuis la supersource */
    vector<int> predecessors = results[0];
    vector<int> distances = results[1];

    if(predecessors[graph.sinkIndex] == -1) /* on vérifie qu'un chemin existe entre source et sink */
    {
      cout << "Unfeasable" << endl; /* si non, alors le problème n'est pas réalisable */
      return;
    }

    int flowIncrease = min(excess,findMaxFlowAlongPath(graph,graph.sourceIndex,graph.sinkIndex,predecessors,potentials));
    increaseAlongPath(graph,graph.sourceIndex,graph.sinkIndex,predecessors,flowIncrease);
    excess -= flowIncrease;

    for(int index=0;index<graph.nbVertices;++index)
    {
      potentials[index] -= distances[index]; /* mise à jour des potentiels */
    }
  }
}



vector<vector<int>>  dijkstra(Graph & graph,int start,vector<int> potentials)
{ /* dijkstra naif adapté pour faire un parcour dans le graphe résiduel */
  vector<vector<int>> results(2); /* initialisation des vecteurs de retour */
  vector<int> & predecessors = results[0];
  vector<int> & distances = results[1];
  predecessors.resize(graph.nbVertices,-1);
  distances.resize(graph.nbVertices,INT_MAX);

  distances[start] = 0; /* on initialise les variables de l'algorithme */
  predecessors[start] = start;
  unordered_set<int> temporarilyTagged;
  for(int vertex=0;vertex<graph.nbVertices;++vertex){temporarilyTagged.insert(vertex);}

  int currentVertex;
  while(not temporarilyTagged.empty())
  {
    currentVertex = findClosestTempVertex(distances,temporarilyTagged);
    temporarilyTagged.erase(currentVertex);

    for(auto & [neighbor,arcs] : graph.vertices[currentVertex])
    {
      for(Arc arc:arcs)
      {
        if(arc.residualCapacity > 0) /* on fait dijkstra sur les arc du graphe résiduel qui ont des capacités positives */
        {
          int cost = arc.cost - potentials[currentVertex] + potentials[neighbor]; /* on se sert des coûts réduits */
          if(distances[neighbor] > distances[currentVertex] + cost)
          {
            distances[neighbor] = distances[currentVertex] + cost;
            predecessors[neighbor] = currentVertex;
          }
        }
      }
    }
  }
  return results;
}



int findClosestTempVertex(vector<int> distances,unordered_set<int> temporarilyTagged)
{ /* fonction pour dijkstra, trouve le sommet avec une étiquette temporaire le plus proche du départ */
  int mininmum = INT_MAX;
  int closestVertex = -1;
  for(int vertex:temporarilyTagged)
  {
    if(distances[vertex] <= mininmum)
    {
      closestVertex = vertex;
      mininmum = distances[vertex];
    }
  }
  return closestVertex;
}



void increaseAlongPath(Graph & graph,int start,int end,vector<int> predecessors,int increase)
{
  int currentVertex = end;
  int nextVertex = predecessors[end];

  while(currentVertex != start)
  {
    graph.increaseFlow(increase,nextVertex,currentVertex);
    currentVertex = nextVertex;
    nextVertex = predecessors[currentVertex];
  }
}



int findMaxFlowAlongPath(Graph & graph,int start,int end,vector<int> predecessors,vector<int> potentials)
{
  int minimum = INT_MAX;
  int currentVertex = end;
  int nextVertex = predecessors[end];

  while(currentVertex != start)
  {

    /* ce truc est super moche, il vaudrait mieux que dijkstra garde en mémoire le sous indice des arcs du chemin */
    int minEdgeCost = INT_MAX; /* on cherche le sous indice d'un arc de capacité résiduelle strictement positive et de coût minimal parmis les arcs entre les deux sommet du chemin */
    int subEdgeIndex = -1;
    for(int subArcIndex=0;subArcIndex<graph.vertices[nextVertex][currentVertex].size();subArcIndex++)
    {
      Arc arc = graph.vertices[nextVertex][currentVertex][subArcIndex];
      if(arc.residualCapacity > 0)
      {
        int cost = arc.cost - potentials[nextVertex] + potentials[currentVertex];
        if(cost < minEdgeCost)
        {
          minEdgeCost = cost;
          subEdgeIndex = subArcIndex;
        }
      }
    }

    if(graph.vertices[nextVertex].at(currentVertex)[subEdgeIndex].residualCapacity < minimum)
    {
      minimum = graph.vertices[nextVertex].at(currentVertex)[subEdgeIndex].residualCapacity;
    }

    currentVertex = nextVertex;
    nextVertex = predecessors[currentVertex];
  }
  return minimum;
}



bool checkGraph(Graph & graph){
  vector<int> transitingFlows(graph.nbVertices,0);

  for(int vertex=0; vertex< graph.nbVertices; ++vertex)
  {
    for(auto &[neighbor,edges]:graph.vertices[vertex])
    {
      for(int subEdgeIndex=0;subEdgeIndex<edges.size();++subEdgeIndex)
      {
        Arc edge = edges[subEdgeIndex];
        if(edge.flow > edge.capacity) /* contrainte de capacité */
        {
          cout << "Overflow on arc (" << vertex << "," << neighbor << ")[" << subEdgeIndex << "]\n";
          edge.print();
          return false;
        }
        transitingFlows[neighbor] += edge.flow;
        transitingFlows[vertex] -= edge.flow;
      }
    }
  }
  for(int vertex=0; vertex< graph.nbVertices; ++vertex)
  {
    if(transitingFlows[vertex] + graph.productions[vertex] != 0) /* contrainte d'équilibre */
    {
      cout << "Imbalance on vertex " << vertex << endl;
      cout << "Transiting " << transitingFlows[vertex]  << ", produced " << graph.productions[vertex] << endl;
      return false;
    }
  }
  return true;
}



int cost_sol(Graph & graph){
  int cost_tot = 0;
  for (int i =0; i< graph.nbVertices; ++i) {        // parcours du vecteur des noeuds départs
    for(auto &[k, v] :graph.vertices[i]) {          // parcours de la map des noeuds destinations et des arcs associés
      for (int i=0; i< v.size(); ++i){
        cost_tot = cost_tot + v[i].cost*v[i].flow;     //on ajoute les couts
      }
    }
  }
  return cost_tot;
}
