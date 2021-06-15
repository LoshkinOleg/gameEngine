#include "skybox.h"

#include <glad/glad.h>

#include "vertex_buffer.h"
#include "texture.h"
#include "shader.h"
#include "resource_manager.h"

void gl::Skybox::Draw(CameraId id)
{
    ResourceManager resourceManager = ResourceManager::Get();
    const VertexBuffer& vertexBuffer = resourceManager.GetVertexBuffer(vertexBuffer_);
    const Texture& texture = resourceManager.GetTexture(texture_);
    Shader& shader = resourceManager.GetShader(shader_);

    glDepthFunc(GL_LEQUAL);
    shader.Bind();
    shader.OnDraw(Model{}, resourceManager.GetCamera(id)); // NOTE: currently, a skybox isn't linked to a Model.
    texture.Bind();
    vertexBuffer.Draw();
    texture.Unbind();
    shader.Unbind();
    glDepthFunc(GL_LESS);
}