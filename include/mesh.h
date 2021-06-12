#pragma once

#include "defines.h"

namespace gl {

    using VertexBufferId = unsigned int;
    using MaterialId = unsigned int;
    using MeshId = unsigned int;

    class Mesh
    {
    public:
        void Draw() const;
    private:
        friend class ResourceManager;

        MeshId id_ = DEFAULT_ID;
        VertexBufferId vertexBuffer_ = DEFAULT_ID;
        MaterialId material_ = DEFAULT_ID;
    };


//struct Vertex
//{
//    glm::vec3 pos = glm::vec3(0.0f);
//    glm::vec3 normal = glm::vec3(0.0f);
//    glm::vec2 texCoord = glm::vec2(0.0f);
//};
//
//class Mesh
//{
//public:
//    Mesh() {};
//    void Init(const std::string& modelDir, tinyobj::ObjReader reader, unsigned int shapeIndex, const std::string& shaderName, std::function<void(Shader, Mesh)> shaderOnInit, std::function<void(Shader, Mesh)> shaderOnDraw)
//    {
//        // Init shader.
//        shader_ = Shader("../data/shaders/" + shaderName + ".vert", "../data/shaders/" + shaderName + ".frag");
//        shaderOnDraw_ = shaderOnDraw;
//
//        auto& attrib = reader.GetAttrib();
//        auto& shapes = reader.GetShapes();
//        auto& materials = reader.GetMaterials();
//
//        // Load materials.
//        const int matId = shapes[shapeIndex].mesh.material_ids[0]; // An obj should have 1 material per mesh. Therefore shapes[shapeIndex].mesh.material_ids values should all be the same.
//        if (!materials[matId].ambient_texname.empty())
//        {
//            textures_[0] = Texture(modelDir + materials[matId].ambient_texname);
//        }
//        else // If no texture is found, load a small blank texture for the shader to use.
//        {
//            textures_[0] = Texture("../data/textures/blank2x2.png");
//            std::cout << "WARNING: loaded obj has no ambient map. Loading blank2x2.png instead." << std::endl;
//        }
//        if (!materials[matId].diffuse_texname.empty())
//        {
//            textures_[1] = Texture(modelDir + materials[matId].diffuse_texname);
//        }
//        else
//        {
//            textures_[1] = Texture("../data/textures/blank2x2.png");
//            std::cout << "WARNING: loaded obj has no diffuse map. Loading blank2x2.png instead." << std::endl;
//        }
//        if (!materials[matId].specular_texname.empty())
//        {
//            textures_[2] = Texture(modelDir + materials[matId].specular_texname);
//        }
//        else
//        {
//            textures_[2] = Texture("../data/textures/blank2x2.png");
//            std::cout << "WARNING: loaded obj has no specular map. Loading blank2x2.png instead." << std::endl;
//        }
//        ambientColor_ = glm::vec3(materials[matId].ambient[0], materials[matId].ambient[1], materials[matId].ambient[2]);
//        diffuseColor_ = glm::vec3(materials[matId].diffuse[0], materials[matId].diffuse[1], materials[matId].diffuse[2]);
//        specularColor_ = glm::vec3(materials[matId].specular[0], materials[matId].specular[1], materials[matId].specular[2]);
//        shininess_ = materials[matId].shininess;
//
//        // Load vertices.
//        const size_t numOfTriangles = shapes[shapeIndex].mesh.num_face_vertices.size();
//        std::vector<Vertex> vertices;
//        for (size_t f = 0; f < numOfTriangles * 3; f += 3) // Traverse the index array 3 spots at a time since we're working with triangles.
//        {
//            for (size_t v = 0; v < 3; v++)
//            {
//                tinyobj::index_t indices = shapes[shapeIndex].mesh.indices[f + v];
//
//                glm::vec3 points = glm::vec3(0.0f, 0.0f, 0.0f);
//                glm::vec3 normals = glm::vec3(0.0f, 0.0f, 0.0f);
//                glm::vec2 texCoords = glm::vec2(0.0f, 0.0f);
//
//                if (indices.vertex_index > -1)
//                {
//                    points.x = attrib.vertices[3 * size_t(indices.vertex_index)];
//                    points.y = attrib.vertices[3 * size_t(indices.vertex_index) + 1];
//                    points.z = attrib.vertices[3 * size_t(indices.vertex_index) + 2];
//                }
//                else
//                {
//                    std::cerr << "ERROR: Mesh does not contain position data!" << std::endl;
//                    abort();
//                }
//                if (indices.normal_index > -1)
//                {
//                    normals.x = attrib.normals[3 * size_t(indices.normal_index)];
//                    normals.y = attrib.normals[3 * size_t(indices.normal_index) + 1];
//                    normals.z = attrib.normals[3 * size_t(indices.normal_index) + 2];
//                }
//                else
//                {
//                    std::cout << "WARNING: Mesh does not contain normals data! Replacing with empty vector." << std::endl;
//                }
//                if (indices.texcoord_index > -1)
//                {
//                    texCoords.x = attrib.texcoords[2 * size_t(indices.texcoord_index)];
//                    texCoords.y = attrib.texcoords[2 * size_t(indices.texcoord_index) + 1];
//                }
//                else
//                {
//                    std::cout << "WARNING: Mesh does not contain texCoord data! Replacing with empty vector." << std::endl;
//                }
//                vertices.push_back(Vertex(points, normals, texCoords));
//            }
//        }
//        verticesCount_ = (unsigned int)vertices.size();
//
//        // Transfer vertices to GPU.
//        glGenVertexArrays(1, &VAO_);
//        glBindVertexArray(VAO_);
//        glGenBuffers(1, &VBO_);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
//        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
//        glEnableVertexAttribArray(2);
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//
//        shader_.Bind();
//        try
//        {
//            shaderOnInit(shader_, *this);
//        }
//        catch (const std::exception& e)
//        {
//            std::cerr << "ERROR: Exception from invoking shaderOnInit in Mesh::Init(): " << e.what() << std::endl;
//            abort();
//        }
//        Shader::UnBind();
//    }
//    void Draw(const Camera& camera)
//    {
//        // Bind everything.
//        shader_.Bind();
//        try
//        {
//            shaderOnDraw_(shader_, *this);
//        }
//        catch (const std::exception& e)
//        {
//            std::cerr << "ERROR: Exception from invoking shaderOnDraw in Mesh::Draw(): " << e.what() << std::endl;
//            abort();
//        }
//        glBindVertexArray(VAO_);
//        glBindBuffer(GL_ARRAY_BUFFER, VBO_);
//        for (size_t i = 0; i < 3; i++)
//        {
//            textures_[i].Bind((unsigned int)i);
//        }
//
//        // Send draw call.
//        glDrawArrays(GL_TRIANGLES, 0, verticesCount_);
//
//        // Unbind everything.
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        glBindVertexArray(0);
//        Texture::UnBind();
//        Shader::UnBind();
//    }
//    void Destroy()
//    {
//        for (size_t i = 0; i < 3; i++)
//        {
//            textures_[i].Destroy();
//        }
//        shader_.Destroy();
//        glDeleteBuffers(1, &VBO_);
//        glDeleteVertexArrays(1, &VAO_);
//    }
//    glm::vec3 GetAmbientColor() const
//    {
//        return ambientColor_;
//    }
//    glm::vec3 GetDiffuseColor() const
//    {
//        return diffuseColor_;
//    }
//    glm::vec3 GetSpecularColor() const
//    {
//        return specularColor_;
//    }
//    float GetShininess() const
//    {
//        return shininess_;
//    }
//    Shader& GetShader()
//    {
//        return shader_;
//    }
//private:
//    unsigned int VAO_ = 0, VBO_ = 0;
//    unsigned int verticesCount_ = 0;
//    glm::vec3 ambientColor_ = glm::vec3(0.0f);
//    glm::vec3 diffuseColor_ = glm::vec3(0.0f);
//    glm::vec3 specularColor_ = glm::vec3(0.0f);
//    float shininess_ = 0.0f;
//    Texture textures_[3] = { Texture(), Texture(), Texture() };
//    Shader shader_ = {};
//    std::function<void(Shader, Mesh)> shaderOnDraw_ = nullptr;
//};
//
//class Model
//{
//public:
//    Model() = default;
//    /*
//    @modelDir: Must end with a '/'
//    @modelName: Don't include the extension.
//    @shaderDir: Must end with a '/'
//    @shaderName: Don't include the extension.
//    */
//    void Init(const std::string& modelName, const std::string& shaderName, std::function<void(Shader, Mesh)> shaderOnInit, std::function<void(Shader, Mesh)> shaderOnDraw)
//    {
//        // Init meshes.
//        tinyobj::ObjReader reader;
//        tinyobj::ObjReaderConfig config;
//        config.mtl_search_path = "../data/models/" + modelName + "/";
//        config.triangulate = true;
//
//        if (!reader.ParseFromFile("../data/models/" + modelName + "/" + modelName + ".obj", config))
//        {
//            std::cerr << "ERROR: tinyObj could not open file!" << std::endl;
//            if (!reader.Error().empty())
//            {
//                std::cerr << "objLoader: " << reader.Error() << std::endl;
//                abort();
//            }
//            std::cerr << "objLoader: reader.Error() is empty." << std::endl;
//            throw;
//        }
//        if (!reader.Warning().empty())
//        {
//            std::cout << "WARNING: tinyObj: " << reader.Warning() << std::endl;
//        }
//
//        meshes_.resize(reader.GetShapes().size());
//
//        for (size_t i = 0; i < meshes_.size(); i++)
//        {
//            meshes_[i].Init("../data/models/" + modelName + "/", reader, (unsigned int)i, shaderName, shaderOnInit, shaderOnDraw);
//        }
//    }
//    void Draw(const Camera& camera)
//    {
//        for (size_t i = 0; i < meshes_.size(); i++)
//        {
//            meshes_[i].Draw(camera);
//        }
//    }
//    void Destroy()
//    {
//        for (size_t i = 0; i < meshes_.size(); i++)
//        {
//            meshes_[i].Destroy();
//            
//        }
//    }
//    Shader& GetShader(int meshIndex)
//    {
//        return meshes_[meshIndex].GetShader();
//    }
//private:
//    std::vector<Mesh> meshes_ = {};
//};

} // End namespace gl.
