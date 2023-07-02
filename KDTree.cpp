#include "KDTree.hh"
#include <algorithm>

KDTree::KDTree(int maxVerticesPerSphere, const std::vector<float> vertices, const std::vector<float> uv) : root(nullptr)
{
    //build vec3 vector from float array
    std::vector<VertexData> data;
    for (unsigned int i=0; i<vertices.size(); i+=3){
        Vector3 vertex = {vertices[i],vertices[i+1],vertices[i+2]};
        Vector3 uvCoord = {uv[i],uv[i+1],uv[i+2]};
        data.push_back({vertex, uvCoord});
    }

    // Build the KDTree
    root = BuildTree(data, maxVerticesPerSphere, depth+1);
}

Node* KDTree::BuildTree(std::vector<VertexData> data, int maxVerticesPerSphere, int depth)
{
    Node* node = nullptr;
    if (data.empty()) {
        return nullptr;
    }

    if (data.size() <= maxVerticesPerSphere) {
        // Create a leaf node
        node = CreateLeafNode(data);
        return node;
    }

    // Calculate the axis to split on
    int axis = depth%3;

    // Sort the vertices based on the selected axis
    std::sort(data.begin(), data.end(), [axis](VertexData a, VertexData b) {
        return a.vertex.toFloatArray()[axis] < b.vertex.toFloatArray()[axis];
    });
    //need to sort the uv buffer also

    // Create an internal node
    node = CreateInternalNode(data, axis);

    // Split the vertices into left and right subsets
    std::vector<VertexData> leftSubSet(data.begin(), data.begin() + data.size() / 2);
    std::vector<VertexData> rightSubSet(data.begin() + data.size() / 2, data.end());

    // Recursively build the left and right child nodes
    node->left = BuildTree(leftSubSet,  maxVerticesPerSphere, depth+1);
    node->right = BuildTree(rightSubSet,  maxVerticesPerSphere, depth+1);

    return node;
}

void KDTree::CalculateBoundingSphere(Node* node, std::vector<VertexData> data)
{
    Vector3 minPoint = data[0].vertex;
    Vector3 maxPoint = data[0].vertex;

    // Find the minimum and maximum points in the vertices
    for (auto& vertex_data : data) {
        minPoint = Vector3::Min(minPoint, vertex_data.vertex);
        maxPoint = Vector3::Max(maxPoint, vertex_data.vertex);
    }

    // Calculate the center and radius of the bounding sphere
    node->center = (minPoint + maxPoint) * 0.5f;
    node->radius = Vector3::Magnitude(maxPoint - node->center);
}

Node* KDTree::CreateLeafNode(std::vector<VertexData> data)
{
    Node* leafNode = new Node();
    CalculateBoundingSphere(leafNode, data);
    leafNode->data = data;
    return leafNode;
}

Node* KDTree::CreateInternalNode(std::vector<VertexData> data, int axis)
{
    Node* internalNode = new Node();
    CalculateBoundingSphere(internalNode, data);
    internalNode->data = data;
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
                smart_vertex_buffer.push_back(static_cast<float>(node->data.size()*6));

                for (const auto& vertex_data : node->data) {
                    smart_vertex_buffer.push_back(vertex_data.vertex.x);
                    smart_vertex_buffer.push_back(vertex_data.vertex.y);
                    smart_vertex_buffer.push_back(vertex_data.vertex.z);
                    smart_vertex_buffer.push_back(vertex_data.uv.x);
                    smart_vertex_buffer.push_back(vertex_data.uv.y);
                    smart_vertex_buffer.push_back(vertex_data.uv.z);
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