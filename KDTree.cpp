#include "KDTree.hh"
#include <algorithm>

KDTree::KDTree(int maxVerticesPerSphere, const std::vector<float> vertices) : root(nullptr)
{
    //build vec3 vector from float array
    std::vector<Vector3> array;
    for (unsigned int i=0; i<vertices.size(); i+=3){
        array.emplace_back(vertices[i],vertices[i+1],vertices[i+2]);
    }

    // Build the KDTree
    root = BuildTree(array, maxVerticesPerSphere, depth+1);
}

Node* KDTree::BuildTree(std::vector<Vector3>& vertices, int maxVerticesPerSphere, int depth)
{
    Node* node = nullptr;
    if (vertices.empty()) {
        return nullptr;
    }

    if (vertices.size() <= maxVerticesPerSphere) {
        // Create a leaf node
        node = CreateLeafNode(vertices);
        return node;
    }

    // Calculate the axis to split on
    int axis = depth%3;

    // Sort the vertices based on the selected axis
    std::sort(vertices.begin(), vertices.end(), [axis](Vector3 a, Vector3 b) {
        return a.toFloatArray()[axis] < b.toFloatArray()[axis];
    });

    // Create an internal node
    node = CreateInternalNode(vertices, axis);

    // Split the vertices into left and right subsets
    std::vector<Vector3> leftSubSet(vertices.begin(), vertices.begin() + vertices.size() / 2);
    std::vector<Vector3> rightSubSet(vertices.begin() + vertices.size() / 2, vertices.end());

    // Recursively build the left and right child nodes
    node->left = BuildTree(leftSubSet, maxVerticesPerSphere, depth+1);
    node->right = BuildTree(rightSubSet, maxVerticesPerSphere, depth+1);

    return node;
}

void KDTree::CalculateBoundingSphere(Node* node, std::vector<Vector3>& vertices)
{
    Vector3 minPoint = vertices[0];
    Vector3 maxPoint = vertices[0];

    // Find the minimum and maximum points in the vertices
    for (auto& vertex : vertices) {
        minPoint = Vector3::Min(minPoint, vertex);
        maxPoint = Vector3::Max(maxPoint, vertex);
    }

    // Calculate the center and radius of the bounding sphere
    node->center = (minPoint + maxPoint) * 0.5f;
    node->radius = Vector3::Magnitude(maxPoint - node->center);
}

Node* KDTree::CreateLeafNode(std::vector<Vector3>& vertices)
{
    Node* leafNode = new Node();
    CalculateBoundingSphere(leafNode, vertices);
    leafNode->vertices = vertices;
    return leafNode;
}

Node* KDTree::CreateInternalNode(std::vector<Vector3>& vertices, int axis)
{
    Node* internalNode = new Node();
    CalculateBoundingSphere(internalNode, vertices);
    internalNode->vertices = vertices;
    return internalNode;
}

void KDTree::DestroyTree(Node* node)
{
    if (node == nullptr) {
        return;
    }

    DestroyTree(node->left);
    DestroyTree(node->right);
    delete node;
}

void KDTree::fillSmart(std::vector<float>& smart_vertex_buffer)
{
    std::vector<Node*> leafNodes;
    std::vector<Node*> stack;
    stack.push_back(root);

    // Perform depth-first traversal to collect leaf nodes
    while (!stack.empty()) {
        Node* node = stack.back();
        stack.pop_back();

        if (node != nullptr) {
            if (node->left == nullptr && node->right == nullptr) {
                // Leaf node, add its data to the smart_vertex_buffer
                smart_vertex_buffer.push_back(node->center.x);
                smart_vertex_buffer.push_back(node->center.y);
                smart_vertex_buffer.push_back(node->center.z);
                smart_vertex_buffer.push_back(node->radius);
                smart_vertex_buffer.push_back(static_cast<float>(node->vertices.size()*3));

                for (const auto& vertex : node->vertices) {
                    smart_vertex_buffer.push_back(vertex.x);
                    smart_vertex_buffer.push_back(vertex.y);
                    smart_vertex_buffer.push_back(vertex.z);
                }
            } else {
                // Internal node, push its children to the stack
                stack.push_back(node->right);
                stack.push_back(node->left);
            }
        }
    }
}

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