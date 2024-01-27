#include "Utils.h"

std::size_t EdgeHash::operator()(const Edge &edge) const
{
    return (edge._vertIndex[0] ^ edge._vertIndex[1]) + (edge._vertIndex[0] & edge._vertIndex[1]);
}

bool EdgeEqual::operator()(const Edge &lhs, const Edge &rhs) const
{
    return (lhs._vertIndex[0] == rhs._vertIndex[0] && lhs._vertIndex[1] == rhs._vertIndex[1]) ||
           (lhs._vertIndex[0] == rhs._vertIndex[1] && lhs._vertIndex[1] == rhs._vertIndex[0]);
}

Vertex Vertex::operator+(const Vertex &rhs)
{
    return {_data[0] + rhs._data[0], _data[1] + rhs._data[1], _data[2] + rhs._data[2]};
}

void Vertex::operator+=(const Vertex &rhs)
{
    _data[0] += rhs._data[0];
    _data[1] += rhs._data[1];
    _data[2] += rhs._data[2];
}

Vertex operator*(float scalar, const Vertex &rhs)
{
    return {scalar * rhs._data[0], scalar * rhs._data[1], scalar * rhs._data[2]};
}
