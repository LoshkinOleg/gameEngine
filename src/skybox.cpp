#include "skybox.h"

#include <glad/glad.h>

#include "mesh.h"
#include "resource_manager.h"

void gl::Skybox::Draw() const
{
    glDepthFunc(GL_LEQUAL);
    ResourceManager::Get().GetMesh(mesh_).Draw();
    glDepthFunc(GL_LESS);
}