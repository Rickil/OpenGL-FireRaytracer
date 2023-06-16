//
// Created by yanis on 16/06/2023.
//

#ifndef POGL_KDTREE_HH
#define POGL_KDTREE_HH

#include "Particle.hh"
#include <vector>

class KDTree {
private:
    int m_depth;
    Particle* m_point;
    KDTree* m_left;
    KDTree* m_right;
public:
    KDTree(int _depth, std::vector<Particle*> _particles);
    void DestroyTree();
    void PrintTree(); //for debug only
    int FindMedian(float _start, float _middle, float _end);
    void Swap(std::vector<Particle*> _particles, int _index1, int _index2);
    //magnitude function simply qorks out the squared magnitude of a vector
    void FindParticlesInRange(float _range, Vector3 _position, std::vector<Particle*> &_particlesInRange, float* _largestDistanceInRange);
};


#endif //POGL_KDTREE_HH
