#include "Model.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

constexpr float PI = 3.141592653f;
constexpr unsigned UNKNOWN = UINT32_MAX;

Model::Model(const std::string &ObjFilePath) // TODO: add annotations..
{
    std::string src;
    std::stringstream fileSsm;
    std::ifstream fin(ObjFilePath);

    if (!fin)
    {
        std::cout << "Could not load .obj file!" << std::endl;
        return;
    }

    fileSsm << fin.rdbuf();
    src = fileSsm.str();
    fin.close();

    std::string output, line;
    std::stringstream ssm(src);

    std::unordered_set<Edge, EdgeHash, EdgeEqual> edgeSet;

    _faces = std::make_shared<std::vector<Face>>();
    _edges = std::make_shared<std::vector<Edge>>();

    while (std::getline(ssm, line))
    {
        if (ssm.eof())
            break;

        if (line[0] == 'v' && line[1] == ' ')
        {
            std::stringstream vertSsm(line.substr(2));
            _vertices.emplace_back();
            for (int i = 0; i < 3; i++)
            {
                vertSsm >> _vertices.back()._data[i];
            }
        }
        else if (line[0] == 'f')
        {
            unsigned curFaceNo = _faces->size();
            _faces->emplace_back();

            std::stringstream faceSsm(line.substr(2));
            std::string tmp;
            unsigned tmpVert[3];

            for (int i = 0; i < 3; i++)
            {
                faceSsm >> tmp;
                auto slashPos = tmp.find('/');
                if (slashPos == std::string::npos)
                {
                    slashPos = tmp.size();
                }
                tmpVert[i] = std::stoul(tmp.substr(0, slashPos)) - 1;
            }

            for (int i = 0; i < 3; i++)
            {
                Edge tmpEdge{{tmpVert[i], tmpVert[(i + 1) % 3]},
                             {curFaceNo, UNKNOWN},
                             {tmpVert[(i + 2) % 3], UNKNOWN}};
                auto iter = edgeSet.find(tmpEdge);
                if (iter == edgeSet.end())
                {
                    edgeSet.insert(tmpEdge);
                    // printf("Added new edge with faceNo %u\n", curFaceNo);
                }
                else
                {
                    tmpEdge._faceIndex[1] = iter->_faceIndex[0];
                    tmpEdge._vertOppositeIndex[1] = iter->_vertOppositeIndex[0];
                    edgeSet.erase(iter);
                    edgeSet.insert(tmpEdge);
                    // printf("Found existing edge <%u, %u>\n", curFaceNo, tmpEdge._faceIndex[1]);
                }
            }
        }
    }

    printf("Total face num: %u\n", _faces->size());
    printf("Total edge num: %u\n", edgeSet.size());

    std::vector<int> faceEdgeIndex(_faces->size());

    for (auto &edge : edgeSet)
    {
        for (int i = 0; i < 2; i++)
        {
            auto faceIdx = edge._faceIndex[i];
            if (faceIdx == UNKNOWN) // special case: this edge only belongs to ONE face
                continue;
            // printf("faceIdx: %u\n", faceIdx);
            // printf("faceEdgeIndex[faceIdx]: %u\n", faceEdgeIndex[faceIdx]);
            (*_faces)[faceIdx]._edgeIndex[faceEdgeIndex[faceIdx]++] = _edges->size();
        }
        // printf("Adj face <%u, %u>\n", edge._faceIndex[0], edge._faceIndex[1]);

        _edges->push_back(edge);
    }
}

bool Model::isUpdated() const
{
    return _updateFlag;
}

void Model::ClearUpdFlag()
{
    _updateFlag = false;
}

void Model::LoopSubdivision()
{
    // We assume that the total number of edges won't exceed 2^32
    unsigned vertNum = _vertices.size();
    unsigned edgeNum = _edges->size();
    unsigned faceNum = _faces->size();

    std::vector<Vertex> adjVertexSum(vertNum);
    std::vector<int> adjVertexNum(vertNum);

    // Step 1: Compute new edge vertices
    // printf("Step 1 starts!\n");
    for (auto &edge : *_edges)
    {
        auto &v0 = _vertices[edge._vertOppositeIndex[0]];
        auto &v1 = _vertices[edge._vertIndex[0]];
        auto &v2 = _vertices[edge._vertOppositeIndex[1]];
        auto &v3 = _vertices[edge._vertIndex[1]];

        // printf("%u %u %u %u\n", edge._vertOppositeIndex[0], edge._vertIndex[0], edge._vertOppositeIndex[1],
        // edge._vertIndex[1]);

        // Each edge vertex corresponds to an edge.
        // These edge vertices are stored in _vertives[vertNum], _vertives[vertNum + 1],
        // _vertives[vertNum + 2], ..., _vertices[vertNum + edgeNum - 1].
        if (edge._vertOppositeIndex[0] == UNKNOWN || edge._vertOppositeIndex[1] == UNKNOWN)
        {
            _vertices.push_back(0.5f * (v1 + v3));
        }
        else
        {
            _vertices.push_back(0.125f * (v0 + v2) + 0.375f * (v1 + v3));
        }

        // Collect adjacent vertex information for Step 2
        ++adjVertexNum[edge._vertIndex[0]];
        ++adjVertexNum[edge._vertIndex[1]];
        adjVertexSum[edge._vertIndex[0]] += _vertices.back();
        adjVertexSum[edge._vertIndex[1]] += _vertices.back();
    }

    // Step 2: Adjust original vertices (excluding newly added edge vertices)
    // printf("Step 2 starts!\n");
    auto square = [](float x) { return x * x; };

    for (int i = 0; i < vertNum; i++)
    {
        auto n = adjVertexNum[i];
        auto beta = 1.0f / n * (0.625f - square((0.375f + 0.25f * std::cosf(2 * PI / adjVertexNum[i]))));
        _vertices[i] = (1 - n * beta) * _vertices[i] + beta * adjVertexSum[i];
    }

    // Step 3: Update edges & faces, preparing for next iteration
    // Key: 1. Each original face is divided into 4 sub faces
    // 2. Each original edge is divided into 2 sub edges
    // printf("Step 3 starts!\n");

    auto newEdges = std::make_shared<std::vector<Edge>>();
    auto newFaces = std::make_shared<std::vector<Face>>();

    newEdges->reserve(edgeNum * 2 + faceNum * 3);
    newFaces->reserve(faceNum * 4);

    std::unordered_set<Edge, EdgeHash, EdgeEqual> edgeSet;
    auto InsertEdge = [&](Edge &edge) {
        // printf("Insert edge:\n\tfaceIndex: %u %u\n\tvertIndex: %u %u\n\tvertOppositeIndex: %u %u\n",
        //  edge._faceIndex[0], edge._faceIndex[1], edge._vertIndex[0], edge._vertIndex[1],
        //  edge._vertOppositeIndex[0], edge._vertOppositeIndex[1]);

        auto iter = edgeSet.find(edge);
        if (iter == edgeSet.end())
        {
            edgeSet.insert(edge);
        }
        else
        {
            edge._faceIndex[1] = iter->_faceIndex[0];
            edge._vertOppositeIndex[1] = iter->_vertOppositeIndex[0];
            edgeSet.erase(iter);
            edgeSet.insert(edge);

            // printf("Complete edge:\n\tfaceIndex: %u %u\n\tvertIndex: %u %u\n\tvertOppositeIndex: %u %u\n",
            // edge._faceIndex[0], edge._faceIndex[1], edge._vertIndex[0], edge._vertIndex[1],
            // edge._vertOppositeIndex[0], edge._vertOppositeIndex[1]);
        }
        // printf("edgeSet Size: %u\n", edgeSet.size());
    };

    for (auto &face : *_faces)
    {
        // central sub face
        unsigned centralSubFaceID = newFaces->size();
        newFaces->emplace_back();

        // for each edge ID (face.edgeIndex[i]) of the current face (face)
        for (int i = 0; i < 3; i++)
        {
            // corresponding edge point ID: vertNum + face._edgeIndex[i]
            Edge centralSubFaceEdge{{vertNum + face._edgeIndex[i], vertNum + face._edgeIndex[(i + 1) % 3]},
                                    {centralSubFaceID, centralSubFaceID + i + 1},
                                    {vertNum + face._edgeIndex[(i + 2) % 3], UNKNOWN}};
            // 2 sub edge
            auto &edge = (*_edges)[face._edgeIndex[i]];
            Edge subEdge1{{edge._vertIndex[0], vertNum + face._edgeIndex[i]},
                          {centralSubFaceID + i + 1, UNKNOWN},
                          {vertNum + face._edgeIndex[(i + 1) % 3], UNKNOWN}};

            Edge subEdge2{{edge._vertIndex[1], vertNum + face._edgeIndex[i]},
                          {centralSubFaceID + (i + 2) % 3 + 1, UNKNOWN},
                          {vertNum + face._edgeIndex[(i + 2) % 3], UNKNOWN}};
            auto &oppositeEdge = (*_edges)[face._edgeIndex[(i + 2) % 3]];
            if (edge._vertIndex[0] != oppositeEdge._vertOppositeIndex[0] &&
                edge._vertIndex[0] != oppositeEdge._vertOppositeIndex[1])
            {
                std::swap(subEdge1._vertIndex[0], subEdge2._vertIndex[0]);
                std::swap(subEdge1._faceIndex[0], subEdge2._faceIndex[0]);
            }

            centralSubFaceEdge._vertOppositeIndex[1] = subEdge1._vertIndex[0];
            edgeSet.insert(centralSubFaceEdge);

            InsertEdge(subEdge1);
            InsertEdge(subEdge2);
        }

        // the other 3 sub faces
        newFaces->emplace_back();
        newFaces->emplace_back();
        newFaces->emplace_back();
    }

    std::vector<std::uint8_t> faceEdgeIndex(newFaces->size());

    for (auto &edge : edgeSet)
    {
        for (int i = 0; i < 2; i++)
        {
            auto faceIdx = edge._faceIndex[i];
            if (faceIdx == UNKNOWN) // special case: this edge only belongs to ONE face
                continue;
            (*newFaces)[faceIdx]._edgeIndex[faceEdgeIndex[faceIdx]++] = newEdges->size();
        }
        newEdges->push_back(edge);
    }

    _edges = newEdges;
    _faces = newFaces;

    _updateFlag = true;

    // printf("edgeSet.size(): %u\n", edgeSet.size());
    // printf("_edges.size(): %u\n", _edges->size());
    // printf("One iteration ends!\n\n");
}
