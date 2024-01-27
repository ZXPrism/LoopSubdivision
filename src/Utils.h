#pragma once

#include <cstddef>

struct Vertex
{
    // helper functions:
    Vertex operator+(const Vertex &rhs);
    void operator+=(const Vertex &rhs);
    friend Vertex operator*(float scalar, const Vertex &rhs);

    float _data[3];
};

struct Edge
{
    unsigned _vertIndex[2];
    unsigned _faceIndex[2];
    unsigned _vertOppositeIndex[2];
};

struct Face
{
    unsigned _edgeIndex[3];
};

struct EdgeHash
{
    std::size_t operator()(const Edge &edge) const;
};

struct EdgeEqual
{
    bool operator()(const Edge &lhs, const Edge &rhs) const;
};
