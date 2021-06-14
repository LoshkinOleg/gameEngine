#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "defines.h"

namespace gl
{
    class Camera;
    using ModelId = unsigned int;
    using Transform3dId = unsigned int;
    using MeshId = unsigned int;
    using ShaderId = unsigned int;
    using FramebufferId = unsigned int;

    class Model
    {
    public:
        void Draw(const Camera& camera, FramebufferId fb = DEFAULT_ID) const;
        const glm::mat4& GetModelMatrix() const;
        std::vector<ShaderId> GetShaderIds() const;
    private:
        friend class ResourceManager;

        ModelId id_ = DEFAULT_ID;
        std::vector<MeshId> meshes_ = {};
        Transform3dId transform_ = DEFAULT_ID;
    };
}//!gl