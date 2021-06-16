#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "defines.h"

namespace gl
{
    using ModelId = unsigned int;
    using Transform3dId = unsigned int;
    using MeshId = unsigned int;
    using ShaderId = unsigned int;
    using CameraId = unsigned int;

    class Model
    {
    public:
        void Draw(CameraId id = 0) const;
        const glm::mat4& GetModelMatrix() const;
        Transform3dId GetTransform() const;
        std::vector<ShaderId> GetShaderIds() const;
    private:
        friend class ResourceManager;

        ModelId id_ = DEFAULT_ID;
        std::vector<MeshId> meshes_ = {};
        Transform3dId transform_ = DEFAULT_ID;
    };
}//!gl