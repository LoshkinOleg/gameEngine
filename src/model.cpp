#include "model.h"

#include <glad/glad.h>
#include "tiny_obj_loader.h"

#include "resource_manager.h"

void gl::Model::Create(const std::string_view path, std::vector<glm::mat4> modelMatrices, Material::Definition material)
{
    // TODO: make following Create functions use Defintions instead of objects.

    if (modelMatricesVBO_ != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    tinyobj::ObjReaderConfig reader_config;
    std::string dir = std::string(path.begin(), path.begin() + path.find_last_of('/') + 1);
    reader_config.mtl_search_path = dir;
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(path.data(), reader_config))
    {
        if (!reader.Error().empty())
        {
            EngineError(reader.Error().c_str());
        }
        else
        {
            // TODO: refactor EngineError func to avoid this kind of horror...
            std::string msg = "Failed to load file at path: ";
            msg += path.data();
            msg += ", at directory: ";
            msg += dir.c_str();
            EngineError(msg.c_str());
        }
    }

    if (!reader.Warning().empty())
    {
        EngineError(reader.Warning().c_str());
    }
    
    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    modelMatrices_ = modelMatrices;
    glGenBuffers(1, &modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices_.size(), &modelMatrices[0][0][0], GL_DYNAMIC_DRAW);
    ResourceManager::Get().AppendNewTransformModelVBO(modelMatricesVBO_);

    for (size_t shape = 0; shape < shapes.size(); shape++)
    {
        size_t index_offset = 0;
        std::vector<glm::vec3> positions, normals, tangents;
        std::vector<glm::vec2> texcoords;

        const auto& mesh = shapes[shape].mesh;
        for (size_t face = 0; face < mesh.num_face_vertices.size(); face++)
        {
            tinyobj::index_t idx0 = mesh.indices[index_offset + 0];
            tinyobj::index_t idx1 = mesh.indices[index_offset + 1];
            tinyobj::index_t idx2 = mesh.indices[index_offset + 2];
            index_offset += 3;

            const glm::vec3 pos0 =
            {
                attrib.vertices[3 * size_t(idx0.vertex_index) + 0],
                attrib.vertices[3 * size_t(idx0.vertex_index) + 1],
                attrib.vertices[3 * size_t(idx0.vertex_index) + 2]
            };
            const glm::vec3 pos1 =
            {
                attrib.vertices[3 * size_t(idx1.vertex_index) + 0],
                attrib.vertices[3 * size_t(idx1.vertex_index) + 1],
                attrib.vertices[3 * size_t(idx1.vertex_index) + 2]
            };
            const glm::vec3 pos2 =
            {
                attrib.vertices[3 * size_t(idx2.vertex_index) + 0],
                attrib.vertices[3 * size_t(idx2.vertex_index) + 1],
                attrib.vertices[3 * size_t(idx2.vertex_index) + 2]
            };
            const glm::vec3 deltaPos0 = pos1 - pos0;
            const glm::vec3 deltaPos1 = pos2 - pos1;

            const glm::vec2 uv0 =
            {
                attrib.texcoords[2 * size_t(idx0.texcoord_index) + 0],
                attrib.texcoords[2 * size_t(idx0.texcoord_index) + 1]
            };
            const glm::vec2 uv1 =
            {
                attrib.texcoords[2 * size_t(idx1.texcoord_index) + 0],
                attrib.texcoords[2 * size_t(idx1.texcoord_index) + 1]
            };
            const glm::vec2 uv2 =
            {
                attrib.texcoords[2 * size_t(idx2.texcoord_index) + 0],
                attrib.texcoords[2 * size_t(idx2.texcoord_index) + 1]
            };
            const glm::vec2 deltaUv0 = uv1 - uv0;
            const glm::vec2 deltaUv1 = uv2 - uv1;

            const float F = 1.0f / (deltaUv0.x * deltaUv1.y - deltaUv1.x * deltaUv0.y);
            const glm::vec3 tangent = glm::normalize(glm::vec3
            (
                F* (deltaUv1.y * deltaPos0.x - deltaUv0.y * deltaPos1.x),
                F* (deltaUv1.y * deltaPos0.y - deltaUv0.y * deltaPos1.y),
                F* (deltaUv1.y * deltaPos0.z - deltaUv0.y * deltaPos1.z)
            ));

            const glm::vec3 normal = glm::normalize(glm::cross(deltaPos0, deltaPos1));

            positions.push_back(pos0);
            positions.push_back(pos1);
            positions.push_back(pos2);
            texcoords.push_back(uv0);
            texcoords.push_back(uv1);
            texcoords.push_back(uv2);
            normals.push_back(normal);
            normals.push_back(normal);
            normals.push_back(normal);
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);
        }

        VertexBuffer::Definition vbdef;
        vbdef.data.resize(positions.size() * 3 * 3 + texcoords.size() * 2);
        for (size_t vertexStart = 0; vertexStart < positions.size(); vertexStart++)
        {
            vbdef.data[vertexStart * 11 + 0] = positions[vertexStart].x; // 11 is the stride.
            vbdef.data[vertexStart * 11 + 1] = positions[vertexStart].y;
            vbdef.data[vertexStart * 11 + 2] = positions[vertexStart].z;
            vbdef.data[vertexStart * 11 + 3] = texcoords[vertexStart].x;
            vbdef.data[vertexStart * 11 + 4] = texcoords[vertexStart].y;
            vbdef.data[vertexStart * 11 + 5] = normals[vertexStart].x;
            vbdef.data[vertexStart * 11 + 6] = normals[vertexStart].y;
            vbdef.data[vertexStart * 11 + 7] = normals[vertexStart].z;
            vbdef.data[vertexStart * 11 + 8] = tangents[vertexStart].x;
            vbdef.data[vertexStart * 11 + 9] = tangents[vertexStart].y;
            vbdef.data[vertexStart * 11 + 10] = tangents[vertexStart].z;
        }
        vbdef.dataLayout =
        {
            3, // pos
            2, // uv
            3, // normal
            3 // tan
        };
        VertexBuffer vb;
        vb.Create(vbdef);
        glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
        const auto& vaoAndVbo = vb.GetVAOandVBO();
        glBindVertexArray(vaoAndVbo[0]);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 1);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 2);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 3);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 1, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 2, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 3, 1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        Material::Definition matdef;
        Material mat;
        if (material.shader.vertexPath.empty()) // Use default shader.
        {
            matdef.shader.vertexPath = ILLUM2_SHADER[0];
            matdef.shader.fragmentPath = ILLUM2_SHADER[1];
            if (!materials[mesh.material_ids[0]].ambient_texname.empty())
            {
                matdef.texturePathsAndTypes.push_back({ dir + materials[mesh.material_ids[0]].ambient_texname, Texture::Type::AMBIENT });
                matdef.shader.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
            }
            if (!materials[mesh.material_ids[0]].alpha_texname.empty())
            {
                matdef.texturePathsAndTypes.push_back({ dir + materials[mesh.material_ids[0]].alpha_texname, Texture::Type::ALPHA });
                matdef.shader.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
            }
            if (!materials[mesh.material_ids[0]].diffuse_texname.empty())
            {
                matdef.texturePathsAndTypes.push_back({ dir + materials[mesh.material_ids[0]].diffuse_texname, Texture::Type::DIFFUSE });
                matdef.shader.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
            }
            if (!materials[mesh.material_ids[0]].specular_texname.empty())
            {
                matdef.texturePathsAndTypes.push_back({ dir + materials[mesh.material_ids[0]].specular_texname, Texture::Type::SPECULAR });
                matdef.shader.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
            }
            if (!materials[mesh.material_ids[0]].bump_texname.empty())
            {
                matdef.texturePathsAndTypes.push_back({ dir + materials[mesh.material_ids[0]].bump_texname, Texture::Type::NORMALMAP });
                matdef.shader.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
            }
            matdef.shader.staticFloats.insert({ SHININESS_NAME, materials[mesh.material_ids[0]].shininess });
            matdef.shader.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            matdef.shader.dynamicMat4s.insert({ VIEW_MARIX_NAME, ResourceManager::Get().GetCamera().GetViewMatrixPtr() });
            matdef.shader.dynamicVec3s.insert({ VIEW_POSITION_NAME, ResourceManager::Get().GetCamera().GetPositionPtr() });
            // TODO: add pbr textures and ior
            mat.Create(matdef);
        }
        else
        {
            mat.Create(material);
        }

        meshes_.push_back(Mesh());
        meshes_.back().Create(vb, mat);
    }
}

void gl::Model::Create(VertexBuffer::Definition vb, Material::Definition mat, std::vector<glm::mat4> modelMatrices)
{
    modelMatrices_ = modelMatrices;

    Material material;
    VertexBuffer vertbuff;
    vertbuff.Create(vb);
    CheckGlError();
    material.Create(mat);
    CheckGlError();

    glGenBuffers(1, &modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * modelMatrices_.size(), &modelMatrices[0][0][0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    const auto& vaoAndVbo = vertbuff.GetVAOandVBO();
    glBindVertexArray(vaoAndVbo[0]);
    glEnableVertexAttribArray(MODEL_MATRIX_LOCATION);
    glVertexAttribPointer(MODEL_MATRIX_LOCATION, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 1);
    glVertexAttribPointer(MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 2);
    glVertexAttribPointer(MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(2 * 4 * sizeof(float)));
    glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 3);
    glVertexAttribPointer(MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(3 * 4 * sizeof(float)));
    glVertexAttribDivisor(MODEL_MATRIX_LOCATION, 1);
    glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 1, 1);
    glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 2, 1);
    glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 3, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    CheckGlError();

    meshes_.push_back(Mesh());
    meshes_.back().Create(vertbuff, material);
    CheckGlError();
}

void gl::Model::Draw()
{
    // TODO: fix the issue of inappropriate use of aModel in shader when there's more than 1 mesh in a model.
    // TODO: add a way to check if the models need updating, no need to update the transformModels every frame...
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    for (size_t i = 0; i < meshes_.size(); i++)
    {
        meshes_[i].Draw((int)modelMatrices_.size());
    }
}

void gl::Model::DrawSingle()
{
    meshes_[0].DrawSingle();
}

void gl::Model::DrawUsingShader(Shader& shader)
{
    glBindBuffer(GL_ARRAY_BUFFER, modelMatricesVBO_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4) * modelMatrices_.size(), (void*)&modelMatrices_[0][0]);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    for (size_t i = 0; i < meshes_.size(); i++)
    {
        meshes_[i].DrawUsingShader(shader, (int)modelMatrices_.size());
    }
}

void gl::Model::Translate(glm::vec3 v, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::translate(modelMatrices_[modelMatrixIndex], v);
}

void gl::Model::Rotate(glm::vec3 cardinalRotation, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.x, RIGHT_VEC3);
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.y, UP_VEC3);
    modelMatrices_[modelMatrixIndex] = glm::rotate(modelMatrices_[modelMatrixIndex], cardinalRotation.z, FRONT_VEC3);
}

void gl::Model::Scale(glm::vec3 v, size_t modelMatrixIndex)
{
    modelMatrices_[modelMatrixIndex] = glm::scale(modelMatrices_[modelMatrixIndex], v);
}

std::vector<glm::mat4>& gl::Model::GetModelMatrices()
{
    return modelMatrices_;
}
