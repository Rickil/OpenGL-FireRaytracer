#include "KDTree.hh"
#include <algorithm>



KDTree::KDTree(int depth, std::vector<Particle*> particles)
{
    // Check if the particles list is empty
    if (particles.empty())
    {
        center = nullptr;
        left = nullptr;
        right = nullptr;
        return;
    }

    // Calculate the axis to split on based on the current depth
    int axis = depth % 3;

    // Sort the particles based on the selected axis
    std::sort(particles.begin(), particles.end(), [axis](Particle* a, Particle* b) {
        return a->position.toFloatArray()[axis] < b->position.toFloatArray()[axis];
    });

    // Find the median index
    int median = particles.size() / 2;

    // Assign the particle at the median index as the current node's point
    center = particles[median];
    left = nullptr;
    right = nullptr;
    depth = depth;

    std::vector<Particle*> leftSubSet(particles.begin(), particles.begin() + median);
    std::vector<Particle*> rightSubSet(particles.begin() + median + 1, particles.end());

    // Recursively create left and right child nodes
    if (!leftSubSet.empty())
    {
        left = new KDTree(depth + 1, leftSubSet);
    }
    if (!rightSubSet.empty())
    {
        right = new KDTree(depth + 1, rightSubSet);
    }
}

//recursively go down the tree and delete nodes on the way back up
void KDTree::DestroyTree()
{
    if(right != nullptr)
    {
        right->DestroyTree();
        delete right;
    }
    if(left != nullptr)
    {
        left->DestroyTree();
        delete left;
    }
}

//recursive function to find all the particles within a range of a particle
void KDTree::getNeighboursParticles(float squaredRange, Vector3 position, std::vector<Particle*> &neighbours, float* largestDistanceInRange)
{
    ///set the distance between the node being searched and the particle
    float distance = Vector3::Magnitude(position - center->position);

    if (distance < *largestDistanceInRange)
    {

        if (distance <= squaredRange)
        {
            neighbours.push_back(center);
        }

        else
        {
            *largestDistanceInRange = distance;
        }

        if (left != nullptr)
        {
            left->getNeighboursParticles(squaredRange,position, neighbours, largestDistanceInRange);
        }
        if (right != nullptr)
        {
            right->getNeighboursParticles(squaredRange,position, neighbours, largestDistanceInRange);
        }
    }
}