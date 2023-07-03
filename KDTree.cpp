#include "KDTree.hh"
#include <algorithm>

/*KDTree::KDTree(int maxVerticesPerSphere, const std::vector<float> vertices, const std::vector<float> uv) : root(nullptr)
{
    //build vec3 vector from float array
    std::vector<TriangleData> data;
    for (unsigned int i=0; i<vertices.size(); i+=9){
        Vector3 vertex1 = {vertices[i],vertices[i+1],vertices[i+2]};
        Vector3 uv1 = {uv[i],uv[i+1],uv[i+2]};
        Vector3 vertex2 = {vertices[i+3],vertices[i+4],vertices[i+5]};
        Vector3 uv2 = {uv[i+3],uv[i+4],uv[i+5]};
        Vector3 vertex3 = {vertices[i+6],vertices[i+7],vertices[i+8]};
        Vector3 uv3 = {uv[i+6],uv[i+7],uv[i+8]};
        data.push_back({vertex1, uv1,vertex2, uv2,vertex3, uv3});
    }

    // Build the KDTree
    root = BuildTree(data, maxVerticesPerSphere, depth+1);
}

Node* KDTree::BuildTree(std::vector<TriangleData> data, int maxTrianglesPerSphere, int depth)
{
    Node* node = nullptr;
    if (data.empty()) {
        return nullptr;
    }

    if (data.size() <= maxTrianglesPerSphere) {
        // Create a leaf node
        node = CreateLeafNode(data);
        return node;
    }

    // Calculate the axis to split on
    int axis = depth%3;

    // Sort the vertices based on the selected axis
    std::sort(data.begin(), data.end(), [axis](TriangleData a, TriangleData b) {
        float centroidA = a.getCentroid().toFloatArray()[axis];
        float centroidB = b.getCentroid().toFloatArray()[axis];
        return centroidA < centroidB;
    });
    //need to sort the uv buffer also

    // Create an internal node
    node = CreateInternalNode(data);

    // Split the vertices into left and right subsets
    std::vector<TriangleData> leftSubSet(data.begin(), data.begin() + data.size() / 2);
    std::vector<TriangleData> rightSubSet(data.begin() + data.size() / 2, data.end());

    // Recursively build the left and right child nodes
    node->left = BuildTree(leftSubSet,  maxTrianglesPerSphere, depth+1);
    node->right = BuildTree(rightSubSet,  maxTrianglesPerSphere, depth+1);

    return node;
}

void KDTree::CalculateBoundingSphere(Node* node, std::vector<TriangleData> data)
{
    Vector3 minPoint = data[0].getCentroid();
    Vector3 maxPoint = data[0].getCentroid();

    // Find the minimum and maximum points in the vertices
    for (auto& vertex_data : data) {
        minPoint = Vector3::Min(minPoint, vertex_data.getCentroid());
        maxPoint = Vector3::Max(maxPoint, vertex_data.getCentroid());
    }

    // Calculate the center and radius of the bounding sphere
    node->center = (minPoint + maxPoint) * 0.5f;
    node->radius = Vector3::Magnitude(maxPoint - node->center);
}

Node* KDTree::CreateLeafNode(std::vector<TriangleData> data)
{
    Node* leafNode = new Node();
    CalculateBoundingSphere(leafNode, data);
    leafNode->data = data;
    return leafNode;
}

Node* KDTree::CreateInternalNode(std::vector<TriangleData> data)
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

void KDTree::fillSmart(Node* node, std::vector<float>& smart_vertex_buffer)
{
    if (node->left == nullptr && node->right == nullptr){
        smart_vertex_buffer.push_back(node->center.x);
        smart_vertex_buffer.push_back(node->center.y);
        smart_vertex_buffer.push_back(node->center.z);
        smart_vertex_buffer.push_back(node->radius);
        smart_vertex_buffer.push_back(static_cast<float>(node->data.size()*18));

        for (const auto& vertex_data : node->data) {
            //first vertex
            smart_vertex_buffer.push_back(vertex_data.vertex1.x);
            smart_vertex_buffer.push_back(vertex_data.vertex1.y);
            smart_vertex_buffer.push_back(vertex_data.vertex1.z);
            smart_vertex_buffer.push_back(vertex_data.uv1.x);
            smart_vertex_buffer.push_back(vertex_data.uv1.y);
            smart_vertex_buffer.push_back(vertex_data.uv1.z);

            //second vertex
            smart_vertex_buffer.push_back(vertex_data.vertex2.x);
            smart_vertex_buffer.push_back(vertex_data.vertex2.y);
            smart_vertex_buffer.push_back(vertex_data.vertex2.z);
            smart_vertex_buffer.push_back(vertex_data.uv2.x);
            smart_vertex_buffer.push_back(vertex_data.uv2.y);
            smart_vertex_buffer.push_back(vertex_data.uv2.z);

            //third vertex
            smart_vertex_buffer.push_back(vertex_data.vertex3.x);
            smart_vertex_buffer.push_back(vertex_data.vertex3.y);
            smart_vertex_buffer.push_back(vertex_data.vertex3.z);
            smart_vertex_buffer.push_back(vertex_data.uv3.x);
            smart_vertex_buffer.push_back(vertex_data.uv3.y);
            smart_vertex_buffer.push_back(vertex_data.uv3.z);

        }
    }else{
        if (node->left){
            fillSmart(node->left, smart_vertex_buffer);
        }if (node->right){
            fillSmart(node->right, smart_vertex_buffer);
        }
    }
}*/

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