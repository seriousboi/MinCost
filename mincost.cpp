#include "graph.cpp"



bool checkGraph(Graph & graph);  /* retourne un booléen qui indique si les flots sont valides et affiche le cout */
float cost_sol(Graph & graph);   //retourne le cout de la solution retournée
Graph pccs(Graph & graph);       // retourne un graphe avec un graphe de cout min



int main(int nbArgs,char **values)
{
  Graph test("instances/example.dat");
  test.print();
  test.addSuperSourceSink();
  test.print();
}



bool checkGraph(Graph & graph){
  float flot_entrant = 0;
  float flot_sortant = 0;

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



float cost_sol(Graph & graph){
  float cost_tot = 0;
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

//     float delta = b(s);
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
