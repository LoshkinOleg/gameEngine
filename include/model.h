#pragma once
#include <vector>

#include <glm/glm.hpp>

#include "defines.h"

namespace gl
{
    class Camera;
    using ModelId = unsigned int;
    using ShaderId = unsigned int;
    using Transform3dId = unsigned int;
    using MeshId = unsigned int;

    class Model // Note: current limitation is of 1 material/shader per model.
    {
    public:
        void Draw(const Camera& camera) const;
        const glm::mat4& GetModelMatrix() const;
    private:
        friend class ResourceManager;

        ModelId id_ = DEFAULT_ID;
        std::vector<MeshId> meshes_ = {};
        ShaderId shader_ = DEFAULT_ID; // TODO: make a model be able to handle multiple shaders since different meshes might have different materials, therefore needing different shaders!
        Transform3dId transform_ = DEFAULT_ID;
    };
}//!gl