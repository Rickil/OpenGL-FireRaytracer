#include "KDTree.hh"
#include <iostream>

//recursive constructor finds median of a set using the median of three algorithm
KDTree::KDTree(int _depth, std::vector<Particle*> _particles)
{
    //intialise median to centre of set
    int median = _particles.size()/2;
    //if there are 3 or more particles in the set find the median using median of three algorithm
    if (_particles.size() >= 3)
    {
        int mid;
        //depending on the depth, split by a different axis
        switch(_depth % 3)
        {
            case 1:
                //median of three algorithm
                mid = FindMedian(_particles.front()->position.x, _particles[median]->position.x, _particles.back()->position.x);
                //switch the middle value to the centre of the set
                switch (mid)
                {
                    case 1:
                        Swap(_particles,0,median);
                        break;
                    case 3:
                        Swap(_particles,_particles.size()-1,median);
                        break;
                }
                break;
            case 2:
                mid = FindMedian(_particles.front()->position.y, _particles[median]->position.y, _particles.back()->position.y);
                switch (mid)
                {
                    case 1:
                        Swap(_particles,0,median);
                        break;
                    case 3:
                        Swap(_particles,_particles.size()-1,median);
                        break;
                }
                break;
            case 0:
                mid = FindMedian(_particles.front()->position.z, _particles[median]->position.z, _particles.back()->position.z);
                switch (mid)
                {
                    case 1:
                        Swap(_particles,0,median);
                        break;
                    case 3:
                        Swap(_particles,_particles.size()-1,median);
                        break;
                }
                break;
        }
    }
    //assign the point to the median of the set
    m_point = _particles[median];
    m_left = nullptr;
    m_right = nullptr;
    m_depth = _depth;
    std::vector<Particle*> leftSubSet;
    std::vector<Particle*> rightSubSet;

    //create subsets out of the left over particles
    for (int i = 0; i < median; i++)
    {
        leftSubSet.push_back(_particles[i]);
    }

    for (int i = median+1; i < _particles.size(); i++)
    {
        rightSubSet.push_back(_particles[i]);
    }

    //if the subsets are larger than 0 recursively create a new tree node
    if (rightSubSet.size() > 0)
    {
        m_right = new KDTree(_depth+1, rightSubSet);
    }
    if (leftSubSet.size() > 0)
    {
        m_left = new KDTree(_depth+1, leftSubSet);
    }
}


int KDTree::FindMedian(float _start, float _middle, float _end)
{
    //median of three algorithm, return 1, 2 or 3 depending on if the median value is the start, middle or end of the set respectively
    if (_start > _end)
    {
        if (_middle > _start)
        {
            return 1;
        }
        if (_end > _middle)
        {
            return 3;
        }
        return 2;
    }
    else
    {
        if (_start > _middle)
        {
            return 1;
        }
        if (_middle > _end)
        {
            return 3;
        }
        return 2;
    }
}

//simple function to swap the given values within a vector
void KDTree::Swap(std::vector<Particle*> _particles, int _index1, int _index2)
{
    Particle* temp = _particles[_index1];
    _particles[_index1] = _particles[_index2];
    _particles[_index2] = temp;
}

//recursively go down the tree and delete nodes on the way back up
void KDTree::DestroyTree()
{
    if(m_right != nullptr)
    {
        m_right->DestroyTree();
        delete m_right;
    }
    if(m_left != nullptr)
    {
        m_left->DestroyTree();
        delete m_left;
    }
}

//print function for debugging
void KDTree::PrintTree()
{
    //print the tree in order from below the node that called the function
    if (m_left != nullptr)
    {
        m_left->PrintTree();
    }

    std::cout << m_point->position.x << " " << m_point->position.y << " " << m_point->position.z << " : " << m_depth << std::endl;

    if (m_right != nullptr)
    {
        m_right->PrintTree();
    }
}

//recursive function to find all the particles within a range of a particle
void KDTree::FindParticlesInRange(float _squaredRange, Vector3 _position, std::vector<Particle*> &_particlesInRange, float* _largestDistanceInRange)
{
    ///set the distance between the node being searched and the particle
    float distance = Vector3::Magnitude(_position - m_point->position);
    //if the distance is smaller than the smallest distance which is greater than the range checked so far, continue with checks
    if (distance < *_largestDistanceInRange)
    {
        //if the distance is within the range, add this node's particle to the vector
        if (distance <= _squaredRange)
        {
            _particlesInRange.push_back(m_point);
        }
            //otherwise lower the smallest distance which is greater than the range
        else
        {
            *_largestDistanceInRange = distance;
        }
        //if there are nodes beneath the one being checked, call the function in those nodes
        if (m_left != nullptr)
        {
            m_left->FindParticlesInRange(_squaredRange,_position, _particlesInRange, _largestDistanceInRange);
        }
        if (m_right != nullptr)
        {
            m_right->FindParticlesInRange(_squaredRange,_position, _particlesInRange, _largestDistanceInRange);
        }
    }
    //else discard this node being checked and it's children
}