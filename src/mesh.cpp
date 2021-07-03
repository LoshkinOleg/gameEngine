#include "mesh.h"

void gl::Mesh::Create(VertexBuffer vb, Material mat)
{
    if (vb_.GetVAOandVBO()[0] != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    vb_ = vb;
    material_ = mat;
}

void gl::Mesh::Draw(int nrOfInstances)
{
    material_.Bind();
    vb_.Draw(nrOfInstances);
    material_.Unbind();
}

void gl::Mesh::DrawSingle()
{
    material_.Bind();
    vb_.DrawSingle();
    material_.Unbind();
}

void gl::Mesh::DrawUsingShader(Shader& shader, int nrOfInstances)
{
    material_.Bind();
    shader.Bind();
    vb_.Draw(nrOfInstances);
    shader.Unbind();
    material_.Unbind();
}
