#include "resource_manager.h"

#include <string>
#include <numeric>

#include <glad/glad.h>
#ifndef XXH_INLINE_ALL
#define XXH_INLINE_ALL
#endif // !XXH_INLINE_ALL
#include "xxhash.h"
#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif // !TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "material.h"
#include "defines.h"

gl::ResourceManager::~ResourceManager()
{
    EngineWarning("Destroying an instance of ResourceManager. This should only happen at the end of the program's lifetime.");
}

void gl::ResourceManager::Shutdown() const
{
    for (const auto& pair : PROGRAMs_)
    {
        glDeleteProgram(pair.second);
    }
    for (const auto& pair : TEXs_)
    {
        glDeleteTextures(1, &pair.second);
    }
    for (const auto& pair : VBOs_)
    {
        glDeleteBuffers(1, &pair.second);
    }
    for (const auto& pair : VAOs_)
    {
        glDeleteVertexArrays(1, &pair.second);
    }
}

GLuint gl::ResourceManager::RequestVAO(XXH32_hash_t hash) const
{
    const auto match = VAOs_.find(hash);
    if (match != VAOs_.end()) // Such a VAO exists already, return it'd gpu name.
    {
        EngineWarning("VAO hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0; // No VAO with such data exists, let the caller create a new VBO.
}

void gl::ResourceManager::AppendNewVAO(unsigned int gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = (unsigned int)VAOs_.size();
    assert(VAOs_.find(hash) == VAOs_.end());

    VAOs_.insert({ hash, gpuName });
}

GLuint gl::ResourceManager::RequestVBO(XXH32_hash_t hash) const
{
    const auto match = VBOs_.find(hash);
    if (match != VBOs_.end())
    {
        EngineWarning("VBO hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewVBO(unsigned int gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = (unsigned int)VBOs_.size();
    assert(VBOs_.find(hash) == VBOs_.end());

    VBOs_.insert({ hash, gpuName });
}

GLuint gl::ResourceManager::RequestTEX(XXH32_hash_t hash) const
{
    const auto match = TEXs_.find(hash);
    if (match != TEXs_.end())
    {
        // EngineWarning("TEX hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewTEX(unsigned int gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = (unsigned int)TEXs_.size();
    assert(TEXs_.find(hash) == TEXs_.end());

    TEXs_.insert({ hash, gpuName });
}

unsigned int gl::ResourceManager::RequestPROGRAM(XXH32_hash_t hash) const
{
    const auto match = PROGRAMs_.find(hash);
    if (match != PROGRAMs_.end())
    {
        EngineWarning("PROGRAM hash already in the map. Returning existing gpu name.");
        return match->second;
    }
    else return 0;
}

void gl::ResourceManager::AppendNewPROGRAM(unsigned int gpuName, XXH32_hash_t hash)
{
    if (hash == 0) hash = (unsigned int)PROGRAMs_.size();
    assert(PROGRAMs_.find(hash) == PROGRAMs_.end());

    PROGRAMs_.insert({ hash, gpuName });
}

void gl::ResourceManager::DeleteVAO(unsigned int gpuName)
{
    for (const auto& pair : VAOs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteVertexArrays(1, &gpuName);
            VAOs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent VAO!");
}

void gl::ResourceManager::DeleteVBO(unsigned int gpuName)
{
    for (const auto& pair : VBOs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteBuffers(1, &gpuName);
            VBOs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent VBO!");
}

void gl::ResourceManager::DeleteTEX(unsigned int gpuName)
{
    for (const auto& pair : TEXs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteTextures(1, &gpuName);
            TEXs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent TEX!");
}

void gl::ResourceManager::DeletePROGRAM(unsigned int gpuName)
{
    for (const auto& pair : PROGRAMs_)
    {
        if (pair.second == gpuName)
        {
            glDeleteProgram(gpuName);
            PROGRAMs_.erase(pair.first);
            return;
        }
    }
    EngineError("Trying to delete a non existent PROGRAMs_!");
}

gl::Camera& gl::ResourceManager::GetCamera()
{
    return camera_;
}

std::vector<gl::ResourceManager::ObjData> gl::ResourceManager::ReadObj(std::string_view path, bool generateOwnNormals, bool flipNormals, bool reverseWindingOrder)
{
    std::vector<ObjData> returnVal;

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

    for (size_t shape = 0; shape < shapes.size(); shape++)
    {
        size_t index_offset = 0;
        std::vector<glm::vec3> positions, normals, tangents;
        std::vector<glm::vec2> texcoords;

        const auto& mesh = shapes[shape].mesh;
        for (size_t face = 0; face < mesh.num_face_vertices.size(); face++)
        {
            // Process vertices.
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
            assert(deltaUv0 != glm::vec2(0.0f) && deltaUv1 != glm::vec2(0.0f));

            const float F = 1.0f / (deltaUv0.x * deltaUv1.y - deltaUv1.x * deltaUv0.y);
            const glm::vec3 tangent = glm::normalize(glm::vec3
            (
                F * (deltaUv1.y * deltaPos0.x - deltaUv0.y * deltaPos1.x),
                F * (deltaUv1.y * deltaPos0.y - deltaUv0.y * deltaPos1.y),
                F * (deltaUv1.y * deltaPos0.z - deltaUv0.y * deltaPos1.z)
            ));

            positions.push_back(reverseWindingOrder ? pos2 : pos0);
            positions.push_back(pos1);
            positions.push_back(reverseWindingOrder ? pos0 : pos2);
            texcoords.push_back(reverseWindingOrder ? uv2 : uv0);
            texcoords.push_back(uv1);
            texcoords.push_back(reverseWindingOrder ? uv0 : uv2);
            tangents.push_back(tangent);
            tangents.push_back(tangent);
            tangents.push_back(tangent);

            if (generateOwnNormals)
            {
                const glm::vec3 normal = glm::normalize(glm::cross(deltaPos0, deltaPos1));
                normals.push_back(flipNormals ? -normal : normal);
                normals.push_back(flipNormals ? -normal : normal);
                normals.push_back(flipNormals ? -normal : normal);
            }
            else // Load obj normals.
            {
                // Make sure obj has normals data if we're not generating our own.
                assert(idx0.normal_index > -1 && idx1.normal_index > -1 && idx2.normal_index > -1);

                const glm::vec3 normal0 =
                {
                    attrib.normals[3 * size_t(idx0.normal_index) + 0],
                    attrib.normals[3 * size_t(idx0.normal_index) + 1],
                    attrib.normals[3 * size_t(idx0.normal_index) + 2]
                };
                const glm::vec3 normal1 =
                {
                    attrib.normals[3 * size_t(idx1.normal_index) + 0],
                    attrib.normals[3 * size_t(idx1.normal_index) + 1],
                    attrib.normals[3 * size_t(idx1.normal_index) + 2]
                };
                const glm::vec3 normal2 =
                {
                    attrib.normals[3 * size_t(idx2.normal_index) + 0],
                    attrib.normals[3 * size_t(idx2.normal_index) + 1],
                    attrib.normals[3 * size_t(idx2.normal_index) + 2]
                };
                normals.push_back(flipNormals ? -normal0 : normal0);
                normals.push_back(flipNormals ? -normal1 : normal1);
                normals.push_back(flipNormals ? -normal2 : normal2);
            }
        }

        // Process material.
        assert(std::accumulate(mesh.material_ids.begin(), mesh.material_ids.end(), 0) / mesh.material_ids.size() == mesh.material_ids[0]); // All vertices should have the same material.
        const int matId = mesh.material_ids[0]; // Only 1 material per mesh, so just take the one that's at index 0.

        std::string alphaMap = materials[matId].alpha_texname; // map_d
        std::string normalMap = materials[matId].bump_texname; // map_Bump
        std::string diffuseMap = materials[matId].diffuse_texname; // map_Kd
        std::string specularMap = materials[matId].specular_texname; // map_Ks

        float shininess = materials[matId].shininess;

        returnVal.push_back(
            {
                positions,
                texcoords,
                normals,
                tangents,
                dir,
                alphaMap,
                normalMap,
                diffuseMap,
                specularMap,
                shininess
            });
    }

    return returnVal;
}

std::vector<gl::Material::Definition> gl::ResourceManager::PreprocessMaterialData(const std::vector<gl::ResourceManager::ObjData> objData)
{
    std::vector<gl::Material::Definition> returnVal = std::vector<gl::Material::Definition>(objData.size(), gl::Material::Definition());

    for (size_t mesh = 0; mesh < objData.size(); mesh++)
    {
        std::string dir = objData[mesh].dir.data();
        std::string path = dir;
        path += objData[mesh].alphaMap;
        if (!objData[mesh].alphaMap.empty())
        {
            returnVal[mesh].texturePathsAndTypes.push_back({path, gl::Texture::Type::ALPHA});
        }
        path = dir;
        path += objData[mesh].normalMap;
        if (!objData[mesh].normalMap.empty())
        {
            returnVal[mesh].texturePathsAndTypes.push_back({ path, gl::Texture::Type::NORMALMAP });
        }
        path = dir;
        path += objData[mesh].diffuseMap;
        if (!objData[mesh].diffuseMap.empty())
        {
            returnVal[mesh].texturePathsAndTypes.push_back({ path, gl::Texture::Type::DIFFUSE_OR_ROUGHNESS });
        }
        path = dir;
        path += objData[mesh].specularMap;
        if (!objData[mesh].specularMap.empty())
        {
            returnVal[mesh].texturePathsAndTypes.push_back({ path, gl::Texture::Type::SPECULAR_OR_METALLIC });
        }
    }
    return returnVal;
}

std::vector<gl::Shader::Definition> gl::ResourceManager::PreprocessShaderData(const std::vector<ObjData> objData)
{
    std::vector<gl::Shader::Definition> returnVal = std::vector<gl::Shader::Definition>(objData.size(), gl::Shader::Definition());

    for (size_t mesh = 0; mesh < objData.size(); mesh++)
    {
        std::string dir = objData[mesh].dir.data();
        std::string path = dir;
        path += objData[mesh].alphaMap;
        if (!objData[mesh].alphaMap.empty())
        {
            returnVal[mesh].staticInts.insert({ gl::ALPHA_SAMPLER_NAME, gl::ALPHA_TEXTURE_UNIT });
        }
        path = dir;
        path += objData[mesh].normalMap;
        if (!objData[mesh].normalMap.empty())
        {
            returnVal[mesh].staticInts.insert({ gl::NORMALMAP_SAMPLER_NAME, gl::NORMALMAP_TEXTURE_UNIT });
        }
        path = dir;
        path += objData[mesh].diffuseMap;
        if (!objData[mesh].diffuseMap.empty())
        {
            returnVal[mesh].staticInts.insert({ gl::DIFFUSE_SAMPLER_NAME, gl::DIFFUSE_TEXTURE_UNIT });
        }
        path = dir;
        path += objData[mesh].specularMap;
        if (!objData[mesh].specularMap.empty())
        {
            returnVal[mesh].staticInts.insert({ gl::SPECULAR_SAMPLER_NAME, gl::SPECULAR_TEXTURE_UNIT });
        }
        if (objData[mesh].shininess > 1.0f)
        {
            returnVal[mesh].staticFloats.insert({ gl::SHININESS_NAME, objData[mesh].shininess });
        }
    }
    return returnVal;
}
