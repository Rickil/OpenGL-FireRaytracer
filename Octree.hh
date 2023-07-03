//
// Created by yanis on 03/07/2023.
//

#ifndef POGL_OCTREE_HH
#define POGL_OCTREE_HH

#include "Vector3.hh"
#include <vector>
#include <algorithm>
#include <array>

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

class AABB {
public:
    Vector3 min;
    Vector3 max;

    AABB(){}

    AABB(Vector3 minPoint, Vector3 maxPoint) : min(minPoint), max(maxPoint) {}

    bool intersects(TriangleData triangle){
        /*// Check if the AABB intersects with the triangle
        Vector3 triangleMin = Vector3(
                std::min({triangle.vertex1.x, triangle.vertex2.x, triangle.vertex3.x}),
                std::min({triangle.vertex1.y, triangle.vertex2.y, triangle.vertex3.y}),
                std::min({triangle.vertex1.z, triangle.vertex2.z, triangle.vertex3.z})
        );

        Vector3 triangleMax = Vector3(
                std::max({triangle.vertex1.x, triangle.vertex2.x, triangle.vertex3.x}),
                std::max({triangle.vertex1.y, triangle.vertex2.y, triangle.vertex3.y}),
                std::max({triangle.vertex1.z, triangle.vertex2.z, triangle.vertex3.z})
        );

        // Check for no overlap along each axis
        if (triangleMin.x > max.x || triangleMax.x < min.x)
            return false;
        if (triangleMin.y > max.y || triangleMax.y < min.y)
            return false;
        if (triangleMin.z > max.z || triangleMax.z < min.z)
            return false;

        // Check if the triangle intersects the AABB face
        Vector3 triangleNormal = cross(triangle.vertex2 - triangle.vertex1, triangle.vertex3 - triangle.vertex1).normalize();

        if (!triangleIntersectsAABBFace(triangle.vertex1, triangle.vertex2, triangle.vertex3, triangleNormal))
            return false;

        // Check if the AABB intersects the triangle face
        Vector3 aabbCenter = (min + max) * 0.5;
        Vector3 aabbExtent = (max - min) * 0.5;

        return triangleIntersectsAABBFace(aabbCenter + aabbExtent, aabbCenter + Vector3(aabbExtent.x, aabbExtent.y, -aabbExtent.z), aabbCenter + Vector3(-aabbExtent.x, aabbExtent.y, aabbExtent.z), Vector3(0, 0, 1)) ||
               triangleIntersectsAABBFace(aabbCenter + aabbExtent, aabbCenter + Vector3(aabbExtent.x, -aabbExtent.y, aabbExtent.z), aabbCenter + Vector3(-aabbExtent.x, aabbExtent.y, aabbExtent.z), Vector3(0, -1, 0)) ||
               triangleIntersectsAABBFace(aabbCenter + aabbExtent, aabbCenter + Vector3(-aabbExtent.x, aabbExtent.y, aabbExtent.z), aabbCenter + Vector3(-aabbExtent.x, -aabbExtent.y, -aabbExtent.z), Vector3(1, 0, 0)) ||
               triangleIntersectsAABBFace(aabbCenter + aabbExtent, aabbCenter + Vector3(aabbExtent.x, aabbExtent.y, -aabbExtent.z), aabbCenter + Vector3(aabbExtent.x, -aabbExtent.y, aabbExtent.z), Vector3(0, 1, 0)) ||
               triangleIntersectsAABBFace(aabbCenter - aabbExtent, aabbCenter + Vector3(-aabbExtent.x, aabbExtent.y, -aabbExtent.z), aabbCenter + Vector3(aabbExtent.x, -aabbExtent.y, -aabbExtent.z), Vector3(0, 0, -1)) ||
               triangleIntersectsAABBFace(aabbCenter - aabbExtent, aabbCenter + Vector3(-aabbExtent.x, -aabbExtent.y, -aabbExtent.z), aabbCenter + Vector3(aabbExtent.x, -aabbExtent.y, aabbExtent.z), Vector3(0, -1, 0));*/
        Vector3 triangleCenter = (triangle.vertex1 + triangle.vertex2 + triangle.vertex3) * (1.0f / 3.0f);
        return (triangleCenter.x >= min.x && triangleCenter.y >= min.y &&triangleCenter.z >= min.z &&
                triangleCenter.x <= max.x && triangleCenter.y <= max.y &&triangleCenter.z <= max.z);
    }

private:
    Vector3 cross(Vector3 a, Vector3 b){
        return Vector3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
        );
    }

    bool triangleIntersectsAABBFace(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 faceNormal) {
        Vector3 triangleCenter = (v1 + v2 + v3) * (1.0f / 3.0f);
        /*Vector3 faceToPoint = triangleCenter - min;

        if (dot(faceToPoint, faceNormal) > 0)
            return false;

        Vector3 edge1 = v2 - v1;
        Vector3 edge2 = v3 - v2;
        Vector3 edge3 = v1 - v3;

        Vector3 faceEdge1 = cross(faceNormal, edge1);
        Vector3 faceEdge2 = cross(faceNormal, edge2);
        Vector3 faceEdge3 = cross(faceNormal, edge3);

        return
                dot(faceToPoint, faceEdge1) <= 0 &&
                dot(faceToPoint, faceEdge2) <= 0 &&
                dot(faceToPoint, faceEdge3) <= 0;*/
        return (triangleCenter.x >= min.x && triangleCenter.y >= min.y &&triangleCenter.z >= min.z &&
                triangleCenter.x <= max.x && triangleCenter.y <= max.y &&triangleCenter.z <= max.z);
    }

    float dot(Vector3 a, Vector3 b){
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
};

class OctreeNode {
public:
    std::vector<TriangleData> triangles;
    AABB boundingBox;
    std::array<OctreeNode*, 8> children;

    OctreeNode(AABB box) : boundingBox(box) {
        for (int i = 0; i < 8; ++i) {
            children[i] = nullptr;
        }
    }

    ~OctreeNode() {
        for (int i = 0; i < 8; ++i) {
            delete children[i];
        }
    }

    bool isLeafNode() const {
        return children[0] == nullptr;
    }
};

class Octree {
public:
    OctreeNode *root;
    int maximumTrianglesPerCube;

    Octree(int maxTriangles) : maximumTrianglesPerCube(maxTriangles) {
        // Define the bounding box for the root node
        AABB rootBox(Vector3(-100, -100, -100), Vector3(100, 100, 100));
        root = new OctreeNode(rootBox);
    }

    ~Octree() {
        delete root;
    }

    void build(std::vector<float> &vertex_buffer, std::vector<float> &uv_buffer) {
        std::vector<TriangleData> triangles;
        for (unsigned int i = 0; i < vertex_buffer.size(); i += 9) {
            Vector3 vertex1 = {vertex_buffer[i], vertex_buffer[i + 1], vertex_buffer[i + 2]};
            Vector3 uv1 = {uv_buffer[i], uv_buffer[i + 1], uv_buffer[i + 2]};
            Vector3 vertex2 = {vertex_buffer[i + 3], vertex_buffer[i + 4], vertex_buffer[i + 5]};
            Vector3 uv2 = {uv_buffer[i + 3], uv_buffer[i + 4], uv_buffer[i + 5]};
            Vector3 vertex3 = {vertex_buffer[i + 6], vertex_buffer[i + 7], vertex_buffer[i + 8]};
            Vector3 uv3 = {uv_buffer[i + 6], uv_buffer[i + 7], uv_buffer[i + 8]};
            triangles.push_back({vertex1, uv1, vertex2, uv2, vertex3, uv3});
        }
        root->triangles = triangles;
        buildOctree(root);
    }

    void buildOctree(OctreeNode *node) {
        if (node->triangles.size() <= maximumTrianglesPerCube) {
            // The node has fewer triangles than the maximum limit, no need to split further
            return;
        }

        // Calculate the center of the node's bounding box
        Vector3 center = (node->boundingBox.min + node->boundingBox.max) * 0.5f;

        // Create the eight children nodes
        for (int i = 0; i < 8; ++i) {
            Vector3 childMin, childMax;
            getChildBoundingBox(node->boundingBox, i, childMin, childMax);
            AABB childBox(childMin, childMax);
            node->children[i] = new OctreeNode(childBox);
        }

        // Distribute the triangles among the children nodes
        for (TriangleData &triangle: node->triangles) {
            for (int i = 0; i < 8; ++i) {
                if (isInCube(triangle, node->children[i]->boundingBox)) {
                    node->children[i]->triangles.push_back(triangle);
                }
            }
        }

        // Recursively build the octree for each child node
        for (int i = 0; i < 8; ++i) {
            buildOctree(node->children[i]);
        }

        // Clear the triangle list for the current node since triangles are distributed to children
        node->triangles.clear();
    }

    bool isInCube(TriangleData& triangle, AABB box) {
        Vector3 minPoint = box.min;
        Vector3 maxPoint = box.max;
        Vector3 triangleCenter = triangle.getCentroid();

        /*return (triangleCenter.x >= minPoint.x && triangleCenter.x <= maxPoint.x &&
                triangleCenter.y >= minPoint.y && triangleCenter.y <= maxPoint.y &&
                triangleCenter.z >= minPoint.z && triangleCenter.z <= maxPoint.z);*/

        return (triangleCenter.x >= std::min(minPoint.x, maxPoint.x) && triangleCenter.x <= std::max(minPoint.x, maxPoint.x) &&
                triangleCenter.y >= std::min(minPoint.y, maxPoint.y) && triangleCenter.y <= std::max(minPoint.y, maxPoint.y) &&
                triangleCenter.z >= std::min(minPoint.z, maxPoint.z) && triangleCenter.z <= std::max(minPoint.z, maxPoint.z));
    }


    void getChildBoundingBox(AABB& parentBox, int childIndex, Vector3& childMin, Vector3& childMax) {
        Vector3 parentCenter = (parentBox.min + parentBox.max) * 0.5f;
        std::vector<Vector3> cubePoints(8);

        cubePoints[0] = parentBox.min; // Front-bottom-left
        cubePoints[1] = Vector3(parentBox.max.x, parentBox.min.y, parentBox.min.z); // Front-bottom-right
        cubePoints[2] = Vector3(parentBox.min.x, parentBox.max.y, parentBox.min.z); // Front-top-left
        cubePoints[3] = Vector3(parentBox.max.x, parentBox.max.y, parentBox.min.z); // Front-top-right
        cubePoints[4] = Vector3(parentBox.min.x, parentBox.min.y, parentBox.max.z); // Back-bottom-left
        cubePoints[5] = Vector3(parentBox.max.x, parentBox.min.y, parentBox.max.z); // Back-bottom-right
        cubePoints[6] = Vector3(parentBox.min.x, parentBox.max.y, parentBox.max.z); // Back-top-left
        cubePoints[7] = parentBox.max; // Back-top-right
        /*Vector3 parentExtent = (parentBox.max - parentBox.min) * 0.5f;

        // Determine the position offset for the child node
        Vector3 offset;
        offset.x = (childIndex & 1) ? parentExtent.x : -parentExtent.x;
        offset.y = (childIndex & 2) ? parentExtent.y : -parentExtent.y;
        offset.z = (childIndex & 4) ? parentExtent.z : -parentExtent.z;

        // Calculate the min and max points for the child node
        childMin = parentCenter + offset;
        childMax = parentCenter + offset + parentExtent;*/
        childMin = parentCenter;
        childMax = cubePoints[childIndex];
    }

    bool triangleIntersectsAABB(TriangleData triangle, AABB box) {
        // Check if the triangle intersects with the AABB
        return box.intersects(triangle);
    }

    void fillSmart(OctreeNode *node, std::vector<float> &smart_vertex_buffer) {
        if (node->isLeafNode() && !node->triangles.empty()) {
            smart_vertex_buffer.push_back(node->boundingBox.min.x);
            smart_vertex_buffer.push_back(node->boundingBox.min.y);
            smart_vertex_buffer.push_back(node->boundingBox.min.z);
            smart_vertex_buffer.push_back(node->boundingBox.max.x);
            smart_vertex_buffer.push_back(node->boundingBox.max.y);
            smart_vertex_buffer.push_back(node->boundingBox.max.z);
            smart_vertex_buffer.push_back(static_cast<float>(node->triangles.size() * 18));

            for (const auto &vertex_data: node->triangles) {
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
        } else {
            for (auto child: node->children) {
                if (child) {
                    fillSmart(child, smart_vertex_buffer);
                }
            }
        }
    }

    void destroyTree(OctreeNode *node) {
        if (node->isLeafNode()) {
            delete node;
        } else {
            for (auto child: node->children) {
                destroyTree(child);
            }
        }
    }
};

/*private:
    void insertTriangle(OctreeNode* node, TriangleData triangle) {
        if (node->isLeafNode()) {
            if (node->triangles.size() < maximumTrianglesPerCube) {
                node->triangles.push_back(triangle);
            } else {
                subdivideNode(node);
                insertTriangleInChildren(node, triangle);
            }
        } else {
            insertTriangleInChildren(node, triangle);
        }
    }

    void subdivideNode(OctreeNode* node) {
        Vector3 minPoint = node->boundingBox.min;
        Vector3 maxPoint = node->boundingBox.max;
        Vector3 midPoint = (minPoint + maxPoint) * 0.5f;

        std::array<AABB, 8> childBoxes;
        for (int i = 0; i < 8; ++i) {
            int x = i & 1;
            int y = (i >> 1) & 1;
            int z = (i >> 2) & 1;
            Vector3 childMin = Vector3(x ? midPoint.x : minPoint.x,
                                              y ? midPoint.y : minPoint.y,
                                              z ? midPoint.z : minPoint.z);
            Vector3 childMax = Vector3(x ? maxPoint.x : midPoint.x,
                                              y ? maxPoint.y : midPoint.y,
                                              z ? maxPoint.z : midPoint.z);
            childBoxes[i] = AABB(childMin, childMax);
        }

        for (int i = 0; i < 8; ++i) {
            node->children[i] = new OctreeNode(childBoxes[i]);
        }
    }

    void insertTriangleInChildren(OctreeNode* node, TriangleData triangle) {
        for (int i = 0; i < 8; ++i) {
            if (node->children[i]->boundingBox.intersects(triangle)) {
                insertTriangle(node->children[i], triangle);
            }
        }
    }
};*/


#endif //POGL_OCTREE_HH
