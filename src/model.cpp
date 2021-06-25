#include "model.h"

#include "resource_manager.h"

void gl::Model::Create(const std::string_view path)
{
    Model::Definition modef;
    for (size_t m = 0; m < objData.size(); m++)
    {
        if (!(objData[m].positions.size() == objData[m].texCoords.size() && objData[m].texCoords.size() == objData[m].normals.size() && objData[m].normals.size() == objData[m].tangents.size())) EngineError("Inconsistent number of vertex components!");
        if (objData[m].positions.size() < 3 || objData[m].positions.size() % 3 != 0) EngineError("Unexpected number of vertices!"); // % 3 since we're working with triangles.

        // Load vertex data.
        VertexBuffer::Definition vbdef;
        Material::Definition localmatdef;
        Mesh::Definition mdef;

        // Collapse all the arrays of vertex data into a single array.
        const size_t sizeOfVerticesData = objData[m].positions.size() * 3 * 3 + objData[m].texCoords.size() * 2;
        const size_t stride = 11;
        std::vector<float> verticesData = std::vector<float>(sizeOfVerticesData);
        for (size_t startOfVertex = 0; startOfVertex < sizeOfVerticesData; startOfVertex += stride)
        {
            // TODO: u sure about this?
            // Positions.
            verticesData[startOfVertex + 0] = objData[m].positions[startOfVertex / stride].x; // TODO: check we're filling things out properly
            verticesData[startOfVertex + 1] = objData[m].positions[startOfVertex / stride].y;
            verticesData[startOfVertex + 2] = objData[m].positions[startOfVertex / stride].z;
            // Texcoords.
            verticesData[startOfVertex + 3] = objData[m].texCoords[startOfVertex / stride].x;
            verticesData[startOfVertex + 4] = objData[m].texCoords[startOfVertex / stride].y;
            // Normals.
            verticesData[startOfVertex + 5] = objData[m].normals[startOfVertex / stride].x;
            verticesData[startOfVertex + 6] = objData[m].normals[startOfVertex / stride].y;
            verticesData[startOfVertex + 7] = objData[m].normals[startOfVertex / stride].z;
            // Tangents.
            verticesData[startOfVertex + 8] = objData[m].tangents[startOfVertex / stride].x;
            verticesData[startOfVertex + 9] = objData[m].tangents[startOfVertex / stride].y;
            verticesData[startOfVertex + 10] = objData[m].tangents[startOfVertex / stride].z;
        }
        vbdef.data = verticesData;
        vbdef.dataLayout =
        {
            3, // pos
            2, // uv
            3, // normals
            3  // tangents
        };

        if (matdef.vertexPath.empty()) // No material manually specified.
        {
            localmatdef.correctGamma = correctGamma;
            localmatdef.flipImages = flipImages;
            localmatdef.useHdr = false;
            localmatdef.vertexPath = ILLUM2_SHADER[0];
            localmatdef.fragmentPath = ILLUM2_SHADER[1];
            if (!objData[m].material.ambientMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.ambientMap, Texture2D::Type::AMBIENT });
                localmatdef.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
            }
            if (!objData[m].material.alphaMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.alphaMap, Texture2D::Type::ALPHA });
                localmatdef.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
            }
            if (!objData[m].material.diffuseMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.diffuseMap, Texture2D::Type::DIFFUSE });
                localmatdef.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
            }
            if (!objData[m].material.specularMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.specularMap, Texture2D::Type::SPECULAR });
                localmatdef.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
            }
            if (!objData[m].material.normalMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.normalMap, Texture2D::Type::NORMALMAP });
                localmatdef.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
            }
            if (!objData[m].material.roughnessMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.roughnessMap, Texture2D::Type::ROUGHNESS });
                localmatdef.staticInts.insert({ ROUGHNESS_SAMPLER_NAME, ROUGHNESS_TEXTURE_UNIT });
            }
            if (!objData[m].material.metallicMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.metallicMap, Texture2D::Type::METALLIC });
                localmatdef.staticInts.insert({ METALLIC_SAMPLER_NAME, METALLIC_TEXTURE_UNIT });
            }
            if (!objData[m].material.sheenMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.sheenMap, Texture2D::Type::SHEEN });
                localmatdef.staticInts.insert({ SHEEN_SAMPLER_NAME, SHEEN_TEXTURE_UNIT });
            }
            if (!objData[m].material.emissiveMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.emissiveMap, Texture2D::Type::EMISSIVE });
                localmatdef.staticInts.insert({ EMISSIVE_SAMPLER_NAME, EMISSIVE_TEXTURE_UNIT });
            }
            localmatdef.staticFloats.insert({ SHININESS_NAME, objData[m].material.shininess });
            localmatdef.staticFloats.insert({ IOR_NAME, objData[m].material.ior });
            localmatdef.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            localmatdef.dynamicMat4s.insert({ VIEW_MARIX_NAME, cameras_[0].GetViewMatrixPtr() }); // Note: this makes all shaders dependant on the main camera!
            localmatdef.dynamicVec3s.insert({ VIEW_POSITION_NAME, cameras_[0].GetPositionPtr() });

            mdef.vertexBuffer = CreateResource(vbdef);
            mdef.material = CreateResource(localmatdef);
            modef.meshes.push_back(CreateResource(mdef));
        }
        else // A material has been specified by the user.
        {
            mdef.vertexBuffer = CreateResource(vbdef);
            mdef.material = CreateResource(matdef);
            modef.meshes.push_back(CreateResource(mdef));
        }
    }
    modef.modelMatrices = modelMatrices;

    return CreateResource(modef);
}

void gl::Model::Create(Definition def)
{
    

    // Accumulate relevant data into a string for hashing.
    std::string accumulatedData = "";
    for (const auto& id : def.meshes)
    {
        accumulatedData += std::to_string(id);
    }
    // Note: we don't take into account the transform models.

    const XXH32_hash_t hash = XXH32(accumulatedData.c_str(), sizeof(char) * accumulatedData.size(), HASHING_SEED);
    assert(hash != DEFAULT_ID); // We aren't handling this issue...

    if (IsModelIdValid(hash))
    {
        EngineWarning("WARNING: attempting to create a new Model with data identical to an existing one. Returning the id of the existing one instead.");
        return hash;
    }

    gl::Model model;
    model.meshes_ = def.meshes;
    model.modelMatrices_ = def.modelMatrices;

    // Create a vbo for the model matrices.
    glGenBuffers(1, &model.modelMatricesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, model.modelMatricesVBO_);
    glBufferData(GL_ARRAY_BUFFER, model.modelMatrices_.size() * sizeof(glm::mat4), &model.modelMatrices_[0][0][0], GL_DYNAMIC_DRAW);

    const auto& meshes = GetMeshes(model.meshes_);
    for (size_t i = 0; i < meshes.size(); i++)
    {
        const VertexBuffer& vb = GetVertexBuffer(meshes[i].vertexBuffer_);
        glBindVertexArray(vb.VAO_);
        CheckGlError();
        const size_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 1);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)vec4Size);
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 2);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
        glEnableVertexAttribArray(MODEL_MATRIX_LOCATION + 3);
        glVertexAttribPointer(MODEL_MATRIX_LOCATION + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));
        CheckGlError();
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 1, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 2, 1);
        glVertexAttribDivisor(MODEL_MATRIX_LOCATION + 3, 1);
        CheckGlError();
        glBindVertexArray(0);
        CheckGlError();
    }

    models_.insert({ hash, model });

    return hash;
}

void gl::Model::Draw() const
{
    ResourceManager& resourceManager = ResourceManager::Get();
    const std::vector<Mesh> meshes = resourceManager.GetMeshes(meshes_);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw((int)modelMatrices_.size());
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

const std::vector<gl::MeshId> gl::Model::GetMesheIds() const
{
    return meshes_;
}

std::vector<gl::ResourceManager::ObjData> gl::Model::ReadObj(const std::string_view path)
{
    Model::Definition modef;
    for (size_t m = 0; m < objData.size(); m++)
    {
        if (!(objData[m].positions.size() == objData[m].texCoords.size() && objData[m].texCoords.size() == objData[m].normals.size() && objData[m].normals.size() == objData[m].tangents.size())) EngineError("Inconsistent number of vertex components!");
        if (objData[m].positions.size() < 3 || objData[m].positions.size() % 3 != 0) EngineError("Unexpected number of vertices!"); // % 3 since we're working with triangles.

        // Load vertex data.
        VertexBuffer::Definition vbdef;
        Material::Definition localmatdef;
        Mesh::Definition mdef;

        // Collapse all the arrays of vertex data into a single array.
        const size_t sizeOfVerticesData = objData[m].positions.size() * 3 * 3 + objData[m].texCoords.size() * 2;
        const size_t stride = 11;
        std::vector<float> verticesData = std::vector<float>(sizeOfVerticesData);
        for (size_t startOfVertex = 0; startOfVertex < sizeOfVerticesData; startOfVertex += stride)
        {
            // TODO: u sure about this?
            // Positions.
            verticesData[startOfVertex + 0] = objData[m].positions[startOfVertex / stride].x; // TODO: check we're filling things out properly
            verticesData[startOfVertex + 1] = objData[m].positions[startOfVertex / stride].y;
            verticesData[startOfVertex + 2] = objData[m].positions[startOfVertex / stride].z;
            // Texcoords.
            verticesData[startOfVertex + 3] = objData[m].texCoords[startOfVertex / stride].x;
            verticesData[startOfVertex + 4] = objData[m].texCoords[startOfVertex / stride].y;
            // Normals.
            verticesData[startOfVertex + 5] = objData[m].normals[startOfVertex / stride].x;
            verticesData[startOfVertex + 6] = objData[m].normals[startOfVertex / stride].y;
            verticesData[startOfVertex + 7] = objData[m].normals[startOfVertex / stride].z;
            // Tangents.
            verticesData[startOfVertex + 8] = objData[m].tangents[startOfVertex / stride].x;
            verticesData[startOfVertex + 9] = objData[m].tangents[startOfVertex / stride].y;
            verticesData[startOfVertex + 10] = objData[m].tangents[startOfVertex / stride].z;
        }
        vbdef.data = verticesData;
        vbdef.dataLayout =
        {
            3, // pos
            2, // uv
            3, // normals
            3  // tangents
        };

        if (matdef.vertexPath.empty()) // No material manually specified.
        {
            localmatdef.correctGamma = correctGamma;
            localmatdef.flipImages = flipImages;
            localmatdef.useHdr = false;
            localmatdef.vertexPath = ILLUM2_SHADER[0];
            localmatdef.fragmentPath = ILLUM2_SHADER[1];
            if (!objData[m].material.ambientMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.ambientMap, Texture2D::Type::AMBIENT });
                localmatdef.staticInts.insert({ AMBIENT_SAMPLER_NAME, AMBIENT_TEXTURE_UNIT });
            }
            if (!objData[m].material.alphaMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.alphaMap, Texture2D::Type::ALPHA });
                localmatdef.staticInts.insert({ ALPHA_SAMPLER_NAME, ALPHA_TEXTURE_UNIT });
            }
            if (!objData[m].material.diffuseMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.diffuseMap, Texture2D::Type::DIFFUSE });
                localmatdef.staticInts.insert({ DIFFUSE_SAMPLER_NAME, DIFFUSE_TEXTURE_UNIT });
            }
            if (!objData[m].material.specularMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.specularMap, Texture2D::Type::SPECULAR });
                localmatdef.staticInts.insert({ SPECULAR_SAMPLER_NAME, SPECULAR_TEXTURE_UNIT });
            }
            if (!objData[m].material.normalMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.normalMap, Texture2D::Type::NORMALMAP });
                localmatdef.staticInts.insert({ NORMALMAP_SAMPLER_NAME, NORMALMAP_TEXTURE_UNIT });
            }
            if (!objData[m].material.roughnessMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.roughnessMap, Texture2D::Type::ROUGHNESS });
                localmatdef.staticInts.insert({ ROUGHNESS_SAMPLER_NAME, ROUGHNESS_TEXTURE_UNIT });
            }
            if (!objData[m].material.metallicMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.metallicMap, Texture2D::Type::METALLIC });
                localmatdef.staticInts.insert({ METALLIC_SAMPLER_NAME, METALLIC_TEXTURE_UNIT });
            }
            if (!objData[m].material.sheenMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.sheenMap, Texture2D::Type::SHEEN });
                localmatdef.staticInts.insert({ SHEEN_SAMPLER_NAME, SHEEN_TEXTURE_UNIT });
            }
            if (!objData[m].material.emissiveMap.empty())
            {
                localmatdef.texturePathsAndTypes.push_back({ objData[m].material.emissiveMap, Texture2D::Type::EMISSIVE });
                localmatdef.staticInts.insert({ EMISSIVE_SAMPLER_NAME, EMISSIVE_TEXTURE_UNIT });
            }
            localmatdef.staticFloats.insert({ SHININESS_NAME, objData[m].material.shininess });
            localmatdef.staticFloats.insert({ IOR_NAME, objData[m].material.ior });
            localmatdef.staticMat4s.insert({ PROJECTION_MARIX_NAME, PERSPECTIVE });
            localmatdef.dynamicMat4s.insert({ VIEW_MARIX_NAME, cameras_[0].GetViewMatrixPtr() }); // Note: this makes all shaders dependant on the main camera!
            localmatdef.dynamicVec3s.insert({ VIEW_POSITION_NAME, cameras_[0].GetPositionPtr() });

            mdef.vertexBuffer = CreateResource(vbdef);
            mdef.material = CreateResource(localmatdef);
            modef.meshes.push_back(CreateResource(mdef));
        }
        else // A material has been specified by the user.
        {
            mdef.vertexBuffer = CreateResource(vbdef);
            mdef.material = CreateResource(matdef);
            modef.meshes.push_back(CreateResource(mdef));
        }
    }
    modef.modelMatrices = modelMatrices;

    return CreateResource(modef);
}
