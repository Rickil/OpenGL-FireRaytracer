//
// Created by yanis on 16/06/2023.
//

#ifndef POGL_KDTREE_HH
#define POGL_KDTREE_HH

#include "Particle.hh"
#include <vector>

/*class Sphere {
public:
    static Sphere CalculateFromPoints(std::vector<Vector3>& points)
    {
        Sphere boundingSphere;

        // Calculate the center as the average of all points
        for (Vector3& point : points) {
            boundingSphere.center += point;
        }
        boundingSphere.center /= points.size();

        // Calculate the radius as the maximum distance from the center
        for (Vector3& point : points) {
            float distance = Vector3::Magnitude(point - boundingSphere.center);
            boundingSphere.radius = std::max(boundingSphere.radius, distance);
        }

        return boundingSphere;
    }

    Vector3 center;
    float radius;
};*/

class TriangleData
{
public:
    Vector3 vertex1;
    Vector3 uv1;
    Vector3 vertex2;
    Vector3 uv2;
    Vector3 vertex3;
    Vector3 uv3;

    TriangleData(Vector3 vertex1, Vector3 uv1, Vector3 vertex2, Vector3 uv2, Vector3 vertex3, Vector3 uv3){
        this->vertex1 = vertex1;
        this->uv1 = uv1;
        this->vertex2 = vertex2;
        this->uv2 = uv2;
        this->vertex3 = vertex3;
        this->uv3 = uv3;
    }

    Vector3 getCentroid(){
        return (vertex1+vertex2+vertex3)/3.0;
    }
};

class Node {
public:
    Vector3 center;
    float radius;
    Node* left = nullptr;
    Node* right = nullptr;
    std::vector<TriangleData> data;
};

class KDTree {
public:

    int depth = 0;
    Node* root;
    Particle* center;
    KDTree* left;
    KDTree* right;
    KDTree(int depth, std::vector<Particle*> particles);
    KDTree(int maxVerticesPerSphere, const std::vector<float> vertices, const std::vector<float> uv);
    void DestroyTree();
    void getNeighboursParticles(float range, Vector3 position, std::vector<Particle*> &neighbours, float* largestDistanceInRange);

    Node* BuildTree(std::vector<TriangleData> data, int maxTrianglesPerSphere, int depth);
    void CalculateBoundingSphere(Node* node, std::vector<TriangleData> data);
    Node* CreateLeafNode(std::vector<TriangleData> data);
    Node* CreateInternalNode(std::vector<TriangleData> data);
    void DestroyTree(Node* node);
    void fillSmart(Node* node, std::vector<float>& smart_vertex_buffer);
};


#endif //POGL_KDTREE_HH
