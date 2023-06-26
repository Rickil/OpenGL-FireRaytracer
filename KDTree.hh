//
// Created by yanis on 16/06/2023.
//

#ifndef POGL_KDTREE_HH
#define POGL_KDTREE_HH

#include "Particle.hh"
#include <vector>

class KDTree {
private:
    int depth;
    Particle* center;
    KDTree* left;
    KDTree* right;
public:
    KDTree(int depth, std::vector<Particle*> particles);
    void DestroyTree();
    void getNeighboursParticles(float range, Vector3 position, std::vector<Particle*> &neighbours, float* largestDistanceInRange);
};


#endif //POGL_KDTREE_HH
