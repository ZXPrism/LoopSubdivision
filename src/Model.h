#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Utils.h"

class Model
{
    friend class Renderer;

public:
    Model(const std::string &objFilePath);

    bool isUpdated() const;
    void ClearUpdFlag();

    void LoopSubdivision();

private:
    bool _updateFlag = true;

    std::vector<Vertex> _vertices;
    std::shared_ptr<std::vector<Edge>> _edges;
    std::shared_ptr<std::vector<Face>> _faces;
};
