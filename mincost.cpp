#include <unordered_set>
#include <limits.h>
#include "graph.cpp"



bool checkGraph(Graph & graph);  /* retourne un booléen qui indique si les flots sont valides et affiche le cout */
int cost_sol(Graph & graph);   //retourne le cout de la solution retournée
void pccs(Graph & graph);       /* affècte un flot de cout minimal avec l'algorithme de plus courts chemins augmentants */
vector<vector<int>>  dijkstra(Graph & graph,int start,vector<int> potentials);  /* retourne les vecteur des distances et des prédécésseurs des plus courts chemins depuis start */
int findClosestTempVertex(vector<int> distances,unordered_set<int> temporarilyTagged); /* fonction pour dijkstra, trouve le sommet avec une étiquette temporaire le plus proche du départ */
int findMaxFlowAlongPath(Graph & graph,int start,int end,vector<int> predecessors,vector<int> potentials);
void increaseAlongPath(Graph & graph,int start,int end,vector<int> predecessors,int increase);


int main(int nbArgs,char **values)
{
  Graph test("instances/example.dat");
  pccs(test);
  cout << cost_sol(test) << endl;
  //for(int i=0;i<test.nbVertices;++i){std::cout << i << " dist "<< results[1][i]<< " pred " << results[0][i]<<'\n';}
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



int findClosestTempVertex(std::vector<int> distances,std::unordered_set<int> temporarilyTagged)
{
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
  int flot_entrant = 0;
  int flot_sortant = 0;

  for (int i =0; i< graph.nbVertices; ++i) {        // parcours du vecteur des noeuds départs
    for(auto &[k, v] :graph.vertices[i]) {          // parcours de la map des noeuds destinations et des arcs associés
      for (int i=0; i< v.size(); ++i){       // parcours du vecteur des arcs destinations (si il y a plusieurs éléments dans ce vecteur c'est que le graphe est un multigraphe)
        if ( v[i].capacity<=v[i].flow){       // le flot des arcs doit être  inférieur à leur capacité (2ème contraintes)
          return false;
        }

        flot_sortant = flot_sortant + v[i].capacity;    // flot sortant du noeud i
        for (auto &[l, w] :graph.vertices[k]){                // parcours des arcs symétriques au noeud sortant de i,  i.e. les noeuds entrants en i (pour que cela marche il faut que le graphe soit symmétrique )
          for (int j=0; j< w.size(); ++j){              // parcours du vecteur des arcs de ces noeuds
            flot_entrant = flot_entrant + w[j].flow;
          }
        }
      }
    }
  if( flot_entrant != flot_sortant){
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



// Graph pccs(Graph & graph, vector<int> consommation){
//   vector<int> potentiel (graph.nbVertices,0);
//   symmetrisation(graph);
//   while (consommation[source])!=0){                             // tant qu'on a du flot à envoyer b(s) je sais pas comment on l'obtient
//     vector<int> distance = dijkstra(s,graph);   // On calucle la distance entre s et chaque noeud


//     //mise à jour des potentiels
//     for (int i=0; i< distance.size(); ++i){
//       potentiel[i] = potentiel[i] - distance[i];
//     }


//     //calcul de la valeur du flow

//     int delta = b(s);
//     if (b(p)<delta)){
//       delta = b(p);
//     }
//     for (int i =0; i< graph.nbVertices; ++i) {        // parcours du vecteur des noeuds départs
//       for(auto &[k, v] :graph.vertices[i]) {          // parcours de la map des noeuds destinations et des arcs associés
//         for (int i=0; i< v.size(); ++i){
//           if(v[i].residualCapacity<delta){
//             delta = v[i].residualCapacity<delta
//           }
//         }
//       }
//     }
//   }
// }
