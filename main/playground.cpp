#include <limits>
#include <assert.h>
#include <utility>
#include <algorithm>
#include <numeric>
#include <functional>

#include <glad/glad.h>
#include "imgui.h"
#include <glm/gtc/quaternion.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "engine.h"
#include "shader.h"
#include "PerlinNoise.h"

namespace gl
{
    constexpr const int NR_OF_PARTICLES_FOR_PROJECTILE = 64;
    constexpr const float PROJECTILE_EXPLOSION_RADIUS_MULTIPLIER = 2.0f;

    class DrawableVector
    {
    public:
        void Init(const glm::vec3 color, const glm::mat4& view)
        {
            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/drawableVector.vert";
            sdef.fragmentPath = "../data/shaders/drawableVector.frag";
            sdef.staticMat4s.insert({"PROJECTION", ORTHO});
            sdef.dynamicMat4s.insert({ "view", &view });
            sdef.staticVec3s.insert({"COLOR", GREEN});
            shader_.Create(sdef);

            CheckGlError();
            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);
            glBindVertexArray(VAO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            std::array<glm::vec2, 5> vertices_ =
            {
                glm::vec2(0.0f, 0.0f),
                glm::vec2(1.0f, 0.0f),
                glm::vec2(0.8f, 0.2f),
                glm::vec2(0.8f, -0.2f),
                glm::vec2(1.0f, 0.0f)
            };
            glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertices_.size(), vertices_.data(), GL_STATIC_DRAW);
            CheckGlError();
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
            CheckGlError();
        }
        void Destroy()
        {
            glDeleteBuffers(1, &VBO_);
            glDeleteVertexArrays(1, &VAO_);
            glDeleteProgram(shader_.GetPROGRAM());
            CheckGlError();
        }
        void Update(const glm::vec2 newOrigin, const glm::vec2 newEnd)
        {
            const float SCALE_FACTOR = 50.0f;

            glm::vec2 vec = newEnd - newOrigin;
            const float scale = glm::length(vec);
            vec = glm::normalize(vec);
            const float rotation = (vec.x > 0) ? glm::atan(vec.y / vec.x) : glm::atan(vec.y / vec.x) + PI;

            glm::mat4 model = glm::translate(IDENTITY_MAT4, ToVec3(newOrigin));
            model = glm::rotate(model, rotation, FRONT_VEC3);
            model = glm::scale(model, ONE_VEC3 * scale * SCALE_FACTOR);

            shader_.Bind();
            shader_.SetMat4({ "model" , model });
            CheckGlError();
            glBindVertexArray(VAO_);
            glDrawArrays(GL_LINE_STRIP, 0, 5);
            CheckGlError();
        }
    private:
        unsigned int VAO_ = 0, VBO_ = 0;
        Shader shader_;
    };

    struct Rectangle
    {
        glm::vec2 GetFullExtents() const
        {
            return topRight - bottomLeft;
        }
        glm::vec2 GetHalfExtents() const
        {
            return GetFullExtents() * 0.5f;
        }
        glm::vec2 GetCenter() const
        {
            return (bottomLeft + topRight) * 0.5f;
        }
        bool Intersect(const glm::vec2 point) const
        {
            const auto relBottomLeft = bottomLeft - point;
            const auto relTopRight = topRight - point;
            const float projOnX_bottomLeft = glm::dot(ToVec3(relBottomLeft), RIGHT_VEC3);
            const float projOnY_bottomLeft = glm::dot(ToVec3(relBottomLeft), UP_VEC3);
            const float projOnX_topRight = glm::dot(ToVec3(relTopRight), RIGHT_VEC3);
            const float projOnY_topRight = glm::dot(ToVec3(relTopRight), UP_VEC3);

            if ((projOnX_bottomLeft > 0.0f && projOnX_topRight > 0.0f) ||
                (projOnX_bottomLeft < 0.0f && projOnX_topRight < 0.0f))
            {
                return false;
            }
            if ((projOnY_bottomLeft > 0.0f && projOnY_topRight > 0.0f) ||
                (projOnY_bottomLeft < 0.0f && projOnY_topRight < 0.0f))
            {
                return false;
            }
            return true;
        }

        glm::vec2 bottomLeft = ZERO_VEC3;
        glm::vec2 topRight = ZERO_VEC3;
    };

    class Map
    {
    private:
        constexpr static const size_t CHUNK_RESOLUTION_ = 64;
        constexpr static const float MAP_TILE_MULTIPLIER_ = 20.0f; // Needs to be big enough for the player not to see the chunks updating.
        constexpr static const float TEXTURE_QUAD_SIDE_LEN_ = 2.0f; // Data in the quad VBO ranges from -1 to 1. Must take this into account when using the map.

        enum ChunkLocation : int
        {
            BOTTOM_LEFT = 0,
            BOTTOM_MIDDLE = 1,
            BOTTOM_RIGHT = 2,
            MIDDLE_LEFT = 3,
            MIDDLE_MIDDLE = 4,
            MIDDLE_RIGHT = 5,
            TOP_LEFT = 6,
            TOP_MIDDLE = 7,
            TOP_RIGHT = 8
        };

        struct MapChunk_
        {
            void Generate(const siv::BasicPerlinNoise<float>& perlinNoiseGenerator)
            {
                constexpr const float INCREMENT_ = 1.0f / (float)CHUNK_RESOLUTION_;
                float fx = 0.0f, fy = 0.0f;
                for (size_t y = 0; y < CHUNK_RESOLUTION_; y++)
                {
                    fx = 0.0f;
                    for (size_t x = 0; x < CHUNK_RESOLUTION_; x++)
                    {
                        const float generated = perlinNoiseGenerator.noise2D_0_1(fx + (offset.x / (MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_)), fy + (offset.y / (MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_)));
                        data[y][x] = generated;
                        fx += INCREMENT_;
                    }
                    fy += INCREMENT_;
                }
            }
            glm::vec2 WorldPos()
            {
                return glm::vec2(offset.x, offset.y);
            }

            std::array<std::array<float, CHUNK_RESOLUTION_>, CHUNK_RESOLUTION_> data = { {0} };
            glm::ivec2 offset = { 0,0 };
        };

    public:
        glm::vec2 ComputeTerrainIncline(const glm::vec2 tankPos) const
        {
            constexpr const int DISTANCE_BETWEEN_SAMPLES_ = CHUNK_RESOLUTION_ / CHUNK_RESOLUTION_;

            // Find chunk closest to player.
            std::array<std::pair<int, glm::vec2>, 9> relChunkPos;
            for (size_t i = 0; i < 9; i++)
            {
                relChunkPos[i] = { i, ToVec2(chunks_[i].offset) - tankPos };
            }
            std::sort(relChunkPos.begin(), relChunkPos.end(), [](const std::pair<int, glm::vec2> v, const std::pair<int, glm::vec2> u) {return ManhattanCompare(v.second, u.second); });

            // Determine the chunk's data's indices. The data at these indices correspond to the area the player is standing on.
            const ChunkLocation closestChunkLocation = (ChunkLocation)relChunkPos[0].first;
            const MapChunk_ *const closestChunk = &chunks_[closestChunkLocation];
            const glm::vec2 relTankPos = -relChunkPos[0].second;
            std::array<std::array<glm::ivec2, 3>, 3> chunkDataToSample;
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    chunkDataToSample[y + 1][x + 1].x = (int)RemapToRange(
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * -0.5f,
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ *  0.5f,
                        0.0f,
                        (float)CHUNK_RESOLUTION_,
                        relTankPos.x + float(x * DISTANCE_BETWEEN_SAMPLES_));
                    chunkDataToSample[y + 1][x + 1].y = (int)RemapToRange(
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * -0.5f,
                        MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ * 0.5f,
                        0.0f,
                        (float)CHUNK_RESOLUTION_,
                        relTankPos.y + float(y * DISTANCE_BETWEEN_SAMPLES_));
                }
            }

            // Access the chunk's data to compute the local average incline.
            glm::vec2 accumulatedIncline = ZERO_VEC3;
            const float middleHeight = 1.0f - closestChunk->data[chunkDataToSample[1][1].y][chunkDataToSample[1][1].x];
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    if (x == 0 && y == 0) continue; // Middle chunk height - middle chunk height = 0, so ignore it.

                    glm::ivec2 dataToSample = chunkDataToSample[y + 1][x + 1];
                    const MapChunk_* chunkToSample = closestChunk;

                    const bool negX = dataToSample.x < 0;
                    const bool posX = dataToSample.x >= CHUNK_RESOLUTION_;
                    const bool negY = dataToSample.y < 0;
                    const bool posY = dataToSample.y >= CHUNK_RESOLUTION_;

                    if (negX)
                    {
                        dataToSample.x += CHUNK_RESOLUTION_;
                        if (negY) // Sample from bottom left neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert // Sanity check.
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 4];
                        }
                        else if (posY) // Sample from top left neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 2];
                        }
                        else // Sample from left neighbor.
                        {
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 1];
                        }
                    }
                    else if (posX)
                    {
                        dataToSample.x -= CHUNK_RESOLUTION_;
                        if (negY) // Sample from bottom right neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 2];
                        }
                        else if (posY) // Sample from top right neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_LEFT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 4];
                        }
                        else // Sample from right neighbor.
                        {
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT &&
                                closestChunkLocation != ChunkLocation::MIDDLE_RIGHT &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 1];
                        }
                    }
                    else
                    {
                        if (negY) // Sample from bottom neighbor.
                        {
                            dataToSample.y += CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::BOTTOM_LEFT &&
                                closestChunkLocation != ChunkLocation::BOTTOM_MIDDLE &&
                                closestChunkLocation != ChunkLocation::BOTTOM_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation - 3];
                        }
                        else if (posY) // Sample from top neighbor.
                        {
                            dataToSample.y -= CHUNK_RESOLUTION_;
                            assert
                            (
                                closestChunkLocation != ChunkLocation::TOP_LEFT &&
                                closestChunkLocation != ChunkLocation::TOP_MIDDLE &&
                                closestChunkLocation != ChunkLocation::TOP_RIGHT
                            );
                            chunkToSample = &chunks_[closestChunkLocation + 3];
                        }
                    }

                    accumulatedIncline += -glm::normalize(glm::vec2(x, y)) * (( 1.0f - chunkToSample->data[dataToSample.y][dataToSample.x]) -  middleHeight);
                }
            }

            return accumulatedIncline / 8.0f;
        }

        void Update(const glm::vec2 playerPos, const unsigned int VAO)
        {
            glBindVertexArray(VAO);

            // Must be > 1 + (SQRT_OF_TWO / 2) to avoid updates fighting each other. Defines the radius of the circle the player can navigate in without triggering map updates.
            constexpr const float TOLERABLE_PLAYER_OFFSET = 1.0f + SQRT_OF_TWO * 0.5f;
            constexpr const float UPDATE_MAP_THRESHOLD = TOLERABLE_PLAYER_OFFSET * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_;

            const bool updateRight = glm::length(chunks_[3].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateLeft = glm::length(chunks_[5].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateUp = glm::length(chunks_[1].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;
            const bool updateDown = glm::length(chunks_[7].WorldPos() - playerPos) > UPDATE_MAP_THRESHOLD;

            if (updateRight) // Moving right.
            {
                chunks_[0] = chunks_[1];
                chunks_[1] = chunks_[2];
                chunks_[3] = chunks_[4];
                chunks_[4] = chunks_[5];
                chunks_[6] = chunks_[7];
                chunks_[7] = chunks_[8];
                chunks_[2].offset = chunks_[1].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[5].offset = chunks_[4].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[8].offset = chunks_[7].offset + glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[2].Generate(perlinGenerator_);
                chunks_[5].Generate(perlinGenerator_);
                chunks_[8].Generate(perlinGenerator_);
            }
            if (updateLeft) // Moving left.
            {
                chunks_[2] = chunks_[1];
                chunks_[1] = chunks_[0];
                chunks_[5] = chunks_[4];
                chunks_[4] = chunks_[3];
                chunks_[8] = chunks_[7];
                chunks_[7] = chunks_[6];
                chunks_[0].offset = chunks_[1].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[3].offset = chunks_[4].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[6].offset = chunks_[7].offset - glm::ivec2(MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, 0);
                chunks_[0].Generate(perlinGenerator_);
                chunks_[3].Generate(perlinGenerator_);
                chunks_[6].Generate(perlinGenerator_);
            }
            if (updateUp) // Moving up.
            {
                chunks_[0] = chunks_[3];
                chunks_[3] = chunks_[6];
                chunks_[1] = chunks_[4];
                chunks_[4] = chunks_[7];
                chunks_[2] = chunks_[5];
                chunks_[5] = chunks_[8];
                chunks_[6].offset = chunks_[3].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[7].offset = chunks_[4].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[8].offset = chunks_[5].offset + glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[6].Generate(perlinGenerator_);
                chunks_[7].Generate(perlinGenerator_);
                chunks_[8].Generate(perlinGenerator_);
            }
            if (updateDown) // Moving down.
            {
                chunks_[6] = chunks_[3];
                chunks_[3] = chunks_[0];
                chunks_[7] = chunks_[4];
                chunks_[4] = chunks_[1];
                chunks_[8] = chunks_[5];
                chunks_[5] = chunks_[2];
                chunks_[0].offset = chunks_[3].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[1].offset = chunks_[4].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[2].offset = chunks_[5].offset - glm::ivec2(0, MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_);
                chunks_[0].Generate(perlinGenerator_);
                chunks_[1].Generate(perlinGenerator_);
                chunks_[2].Generate(perlinGenerator_);
            }

            if (updateRight || updateLeft || updateDown || updateUp)
            {
                for (int i = 0; i < 9; i++)
                {
                    glBindTexture(GL_TEXTURE_2D, TEXs_[i]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNK_RESOLUTION_, CHUNK_RESOLUTION_, 0, GL_RED, GL_FLOAT, &(chunks_[i].data)[0][0]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }

            // Draw map.
            shader_.Bind();
            for (int i = 0; i < 9; i++)
            {
                glm::mat4 model = glm::translate(IDENTITY_MAT4, glm::vec3(chunks_[i].offset.x, chunks_[i].offset.y, 0.0f));
                model = glm::scale(model, ONE_VEC3 * MAP_TILE_MULTIPLIER_);
                shader_.SetMat4({ "model", model });
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TEXs_[i]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        void Init(const glm::mat4& view)
        {
            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/sprite.vert";
            sdef.fragmentPath = "../data/shaders/sprite_RED.frag";
            sdef.staticInts.insert({ "ALBEDO", 0 });
            sdef.dynamicMat4s.insert({ "view", &view });
            sdef.staticMat4s.insert({ "PROJECTION", ORTHO });
            shader_.Create(sdef);

            chunks_.resize(9);
            glGenTextures(9, TEXs_);
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    chunks_[(y + 1) * 3 + (x + 1)].offset = { x * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_, y * MAP_TILE_MULTIPLIER_ * TEXTURE_QUAD_SIDE_LEN_ };
                    chunks_[(y + 1) * 3 + (x + 1)].Generate(perlinGenerator_);

                    glBindTexture(GL_TEXTURE_2D, TEXs_[(y + 1) * 3 + (x + 1)]);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNK_RESOLUTION_, CHUNK_RESOLUTION_, 0, GL_RED, GL_FLOAT, &(chunks_[(y + 1) * 3 + (x + 1)].data)[0][0]);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
            }
            CheckGlError();
        }
        void Destroy()
        {
            glDeleteTextures(9, TEXs_);
            glDeleteProgram(shader_.GetPROGRAM());
        }

    private:
        unsigned int TEXs_[9] = {0};
        std::vector<MapChunk_> chunks_; // 0: LB, 1: MB, 2: RB, 3: LM, 4: MM, 5: RM, 6: LT, 7: MT, 8: RT 
        siv::BasicPerlinNoise<float> perlinGenerator_;
        Shader shader_;
    };

    class Projectile
    {
    public:
        bool InFlight() const
        {
            return timer_ > -PARTICLES_STATE_LIFETIME_;
        }
        void Init(const glm::vec3 color)
        {
            color_ = color;
            glGenVertexArrays(1, &VAO_);
            glBindVertexArray(VAO_);
            glGenBuffers(1, &VBO_);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, positions_.size() * sizeof(glm::vec2), positions_.data(), GL_DYNAMIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        }
        void Launch(const glm::vec2 firingTankPos, const float firingTankGunPos)
        {
            // Launch projectile.
            timer_ = PROJECTILE_STATE_LIFETIME_;
            avgPos_ = ZERO_VEC3;
            for (size_t i = 0; i < NR_OF_PARTICLES_FOR_PROJECTILE; i++)
            {
                const float x = glm::cos((float)i);
                const float y = glm::sin((float)i);
                positions_[i] = firingTankPos + glm::vec2(x, y) * PROJECTILE_STATE_PIXELS_SPREAD_;
                avgPos_ += positions_[i];
            }
            avgPos_ /= NR_OF_PARTICLES_FOR_PROJECTILE;
            dir_ = glm::normalize(glm::vec2(glm::cos(firingTankGunPos), glm::sin(firingTankGunPos))); // TODO: is this normalize necessary?
        }
        void Update(const Rectangle enemyHitbox, const float dt, std::function<void()> onHit, Shader& shader)
        {
            if (InFlight())
            {
                if (timer_ > 0.0f) // Projectile is in it's projectile state.
                {
                    const bool contact = enemyHitbox.Intersect(avgPos_); // Check for collision with an enemy tank.

                    avgPos_ = ZERO_VEC3;
                    for (size_t i = 0; i < NR_OF_PARTICLES_FOR_PROJECTILE; i++)
                    {
                        positions_[i] += dir_ * dt * PROJECTILE_MOV_SPEED_;
                        avgPos_ += positions_[i];
                    }
                    avgPos_ /= NR_OF_PARTICLES_FOR_PROJECTILE;

                    if (contact)
                    {
                        timer_ = 0.0f; // Put projectile into it's particles state immediately.
                        onHit();
                    }
                }

                // Upload to gpu and issue draw call.
                shader.Bind();
                shader.SetFloat({"timer", timer_});
                shader.SetVec3({"color", color_});
                glBindVertexArray(VAO_);
                glBindBuffer(GL_ARRAY_BUFFER, VBO_);
                glBufferSubData(GL_ARRAY_BUFFER, 0, positions_.size() * sizeof(glm::vec2), positions_.data());
                glDrawArrays(GL_POINTS, 0, (GLsizei)positions_.size());

                timer_ -= dt; // Must be at end of Draw for dir_ to be generated.
            }
        }
        void Destroy()
        {
            glDeleteBuffers(1, &VBO_);
            glDeleteVertexArrays(1, &VAO_);
        }
    private:
        constexpr static const float PROJECTILE_MOV_SPEED_ = 20.0f;
        constexpr static const float PROJECTILE_STATE_LIFETIME_ = 1.0f;
        constexpr static const float PROJECTILE_STATE_PIXELS_SPREAD_ = 0.033f;

        constexpr static const float PARTICLES_STATE_LIFETIME_ = 0.5f;

        unsigned int VAO_ = 0, VBO_ = 0;
        std::array<glm::vec2, NR_OF_PARTICLES_FOR_PROJECTILE> positions_ = {};
        glm::vec2 dir_ = ZERO_VEC3;
        glm::vec2 avgPos_ = ZERO_VEC3;
        glm::vec3 color_ = ONE_VEC3;

        // Uniforms.
        float timer_ = -PARTICLES_STATE_LIFETIME_ - 0.1f; // ----PARTICLE--- 0 ++++++PROJECTILE++++
    };

    class A_Tank
    {
    public:
        Rectangle GetHitBox() const
        {
            return hitbox_;
        }
        glm::vec2 GetPos() const
        {
            return pos_;
        }
    protected:
        constexpr static const glm::vec3 GUN_SCALE_ = glm::vec3(1.5f, 0.5f, 1.0f);
        constexpr static const float TURN_MULT_ = 1.0f;
        constexpr static const float TANK_SPEED_ = 5.0f;

        glm::vec2 pos_ = ZERO_VEC3;
        float bodyRot_ = 0.0f;
        float gunRot_ = 0.0f;
        Rectangle hitbox_ = {};
        glm::vec3 color_ = WHITE;
    };

    class AiTank : public A_Tank
    {
    public:
        void Kill()
        {
            isDead_ = true;
        }
        void Init(const glm::vec2 startingPos, const glm::vec3 color, A_Tank* playerTank)
        {
            playerTank_ = playerTank;
            pos_ = startingPos;
            color_ = color;
            hitbox_.bottomLeft = startingPos - ToVec2(ONE_VEC3); // ONE because texture quad goes from -1;-1 to 1;1
            hitbox_.topRight = startingPos + ToVec2(ONE_VEC3);
            projectile_.Init(color);
        }
        void Update(const float dt, const glm::vec2 playerPos, const unsigned int VAO, Shader& tankShader, Shader& projectleShader, const unsigned int TEXs[2])
        {
            if (!isDead_)
            {
                reloadTimer_ -= dt;

                if (reloadTimer_ < 0.0f)
                {
                    reloadTimer_ = RELOAD_TIME_;
                    projectile_.Launch(pos_, gunRot_);
                }
                projectile_.Update
                (
                    playerTank_->GetHitBox(),
                    dt,
                    []() {},
                    projectleShader
                );

                // Rotate tank.
                bodyRot_ += PI * dt * TURN_MULT_ * 0.5f;

                // Move tank.
                glm::vec2 dir = glm::vec2(glm::cos(bodyRot_), glm::sin(bodyRot_));
                pos_ += dir * dt * TANK_SPEED_;
                hitbox_.bottomLeft = pos_ - ToVec2(ONE_VEC3);
                hitbox_.topRight = pos_ + ToVec2(ONE_VEC3);

                // Rotate tank's gun.
                const glm::vec2 relPlayerPos = playerPos - pos_;
                gunRot_ = glm::atan(relPlayerPos.y / relPlayerPos.x);
                if (relPlayerPos.x < 0.0f) gunRot_ += PI;

                glBindVertexArray(VAO);

                tankShader.Bind();
                glActiveTexture(GL_TEXTURE0);

                // Draw tank body.
                glm::mat4 model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
                model = glm::rotate(model, bodyRot_, FRONT_VEC3);
                tankShader.SetMat4({ "model", model });
                tankShader.SetVec3({ "color", color_ });
                glBindTexture(GL_TEXTURE_2D, TEXs[0]);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                // Draw tank gun.
                model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
                model = glm::rotate(model, gunRot_, FRONT_VEC3);
                model = glm::scale(model, GUN_SCALE_);
                tankShader.SetMat4({ "model", model });
                glBindTexture(GL_TEXTURE_2D, TEXs[1]);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        void Destroy()
        {
            projectile_.Destroy();
        }
    private:

        constexpr static const float RELOAD_TIME_ = 1.5f;

        Projectile projectile_;
        float reloadTimer_ = RELOAD_TIME_;
        bool isDead_ = false;
        A_Tank* playerTank_ = nullptr;
    };

    class PlayerTank : public A_Tank
    {
    public:
        void Init(const glm::vec2 startingPos, const glm::vec3 color, std::vector<AiTank*> enemies, const glm::mat4& view)
        {
            enemies_ = enemies;
            pos_ = startingPos;
            color_ = color;
            hitbox_.bottomLeft = startingPos - ToVec2(ONE_VEC3); // ONE because texture quad goes from -1;-1 to 1;1
            hitbox_.topRight = startingPos + ToVec2(ONE_VEC3);
            for (auto& projectile : projectilePool_)
            {
                projectile.Init(color);
            }
            movementVector_.Init(GREEN, view);
        }
        void Update(const bool d, const bool w, const bool a, const bool s, const bool lmb, const float dt, const glm::vec2 relMousePos, const unsigned int VAO, unsigned int TEXs[2], Shader& tankShader, Shader& projectileShader, const Map& map)
        {
            // Rotate tank.
            if (d != a)
            {
                float rotation = PI * dt * TURN_MULT_;
                if (d) rotation = -rotation;
                if ((a || d) && s) rotation = -rotation;
                bodyRot_ += rotation;
            }

            // Move tank.
            glm::vec2 movementVec = ZERO_VEC2;
            if (w != s)
            {
                glm::vec2 dir = glm::vec2(glm::cos(bodyRot_), glm::sin(bodyRot_));
                if (s) dir = -dir;
                constexpr const float INCLINE_EFFECT_MULTIPLIER = 100.0f;
                movementVec = (dir * dt * TANK_SPEED_) + (map.ComputeTerrainIncline(pos_) * dt * INCLINE_EFFECT_MULTIPLIER);
                pos_ += movementVec; // (dir * dt * TANK_SPEED_) + (map.ComputeTerrainIncline(pos_) * dt * INCLINE_EFFECT_MULTIPLIER);
                hitbox_.bottomLeft = pos_ - ToVec2(ONE_VEC3);
                hitbox_.topRight = pos_ + ToVec2(ONE_VEC3);
            }
            movementVector_.Update(pos_, pos_ + movementVec);

            // Rotate gun.
            gunRot_ = glm::atan(relMousePos.y / relMousePos.x);
            if (relMousePos.x < 0.0f) gunRot_ += PI;

            // Launch a projectile.
            bool projectileLaunched = false;
            for (auto& projectile : projectilePool_)
            {
                if (lmb && !projectileLaunched)
                {
                    if (!projectile.InFlight())
                    {
                        projectile.Launch(pos_, gunRot_);
                        projectileLaunched = true;
                    }
                }
                projectile.Update
                (
                    enemies_.front()->GetHitBox(),
                    dt,
                    std::bind(&AiTank::Kill, enemies_.front()),
                    projectileShader
                );
            }

            glBindVertexArray(VAO);

            tankShader.Bind();
            glActiveTexture(GL_TEXTURE0);

            // Draw tank body.
            glm::mat4 model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
            model = glm::rotate(model, bodyRot_, FRONT_VEC3);
            tankShader.SetMat4({ "model", model });
            tankShader.SetVec3({"color", color_});
            glBindTexture(GL_TEXTURE_2D, TEXs[0]);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            // Draw tank gun.
            model = glm::translate(IDENTITY_MAT4, ToVec3(pos_));
            model = glm::rotate(model, gunRot_, FRONT_VEC3);
            model = glm::scale(model, GUN_SCALE_);
            tankShader.SetMat4({ "model", model });
            glBindTexture(GL_TEXTURE_2D, TEXs[1]);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        void Destroy()
        {
            for (auto& projectile : projectilePool_)
            {
                projectile.Destroy();
            }
            movementVector_.Destroy();
        }

    private:
        std::vector<Projectile> projectilePool_ = std::vector<Projectile>(5);
        std::vector<AiTank*> enemies_;
        DrawableVector movementVector_;
    };

    class InputManager
    {
    public:
        static InputManager& Get()
        {
            static gl::InputManager instance;
            return instance;
        }

        void UpdateButtons()
        {
            keyboardButtons_['d'].last = keyboardButtons_['d'].current;
            keyboardButtons_['w'].last = keyboardButtons_['w'].current;
            keyboardButtons_['a'].last = keyboardButtons_['a'].current;
            keyboardButtons_['s'].last = keyboardButtons_['s'].current;
            mouseButtons_[0].last = mouseButtons_[0].current;
            mouseButtons_[1].last = mouseButtons_[1].current;
        }
        void ProcessEvent(SDL_Event& event)
        {
            switch (event.type)
            {
                case SDL_KEYDOWN:
                    {
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_d:
                                {
                                    keyboardButtons_['d'].current = true;
                                }break;
                            case SDLK_w:
                                {
                                    keyboardButtons_['w'].current = true;
                                }break;
                            case SDLK_a:
                                {
                                    keyboardButtons_['a'].current = true;
                                }break;
                            case SDLK_s:
                                {
                                    keyboardButtons_['s'].current = true;
                                }break;

                            default:
                                break;
                        }
                    }break;

                case SDL_KEYUP:
                    {
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_d:
                                {
                                    keyboardButtons_['d'].current = false;
                                }break;
                            case SDLK_w:
                                {
                                    keyboardButtons_['w'].current = false;
                                }break;
                            case SDLK_a:
                                {
                                    keyboardButtons_['a'].current = false;
                                }break;
                            case SDLK_s:
                                {
                                    keyboardButtons_['s'].current = false;
                                }break;

                            default:
                                break;
                        }
                    }break;

                case SDL_MOUSEMOTION:
                    {
                        lastMousePos_ = mousePos_;
                        mousePos_.x = (float)event.motion.x;
                        mousePos_.y = (float)event.motion.y;
                    }break;

                case SDL_MOUSEBUTTONDOWN:
                    {
                        if (event.button.button == SDL_BUTTON_LEFT) // lmb
                        {
                            mouseButtons_[0].current = true;
                        }
                        else if (event.button.button == SDL_BUTTON_RIGHT) // rmb
                        {
                            mouseButtons_[1].current = true;
                        }
                    }break;

                case SDL_MOUSEBUTTONUP:
                    {
                        if (event.button.button == SDL_BUTTON_LEFT)
                        {
                            mouseButtons_[0].current = false;
                        }
                        else if (event.button.button == SDL_BUTTON_RIGHT)
                        {
                            mouseButtons_[1].current = false;
                        }
                    }break;
                default:
                    break;
            }
        }

        bool IsDown(const char* button) const
        {
            bool current = false;
            if (button[1] == '\0') // Accessing a keyboard button.
            {
                current = keyboardButtons_.at(button[0]).current;
            }
            else
            {
                if (button == "lmb")
                {
                    current = mouseButtons_[0].current;
                }
                else if ("rmb")
                {
                    current = mouseButtons_[1].current;
                }
            }
            return current;
        }
        bool JustPressed(const char* button) const
        {
            bool current = false;
            bool last = false;
            if (button[1] == '\0') // Accessing a keyboard button.
            {
                current = keyboardButtons_.at(button[0]).current;
                last = keyboardButtons_.at(button[0]).last;
            }
            else
            {
                if (button == "lmb")
                {
                    current = mouseButtons_[0].current;
                    last = mouseButtons_[0].last;
                }
                else if ("rmb")
                {
                    current = mouseButtons_[1].current;
                    last = mouseButtons_[1].last;
                }
            }
            // EngineMessage(std::to_string(last).c_str());
            return (last == false) && (current == true);
        }
        bool JustReleased(const char* button) const
        {
            bool current = false;
            bool last = false;
            if (button[1] == '\0') // Accessing a keyboard button.
            {
                current = keyboardButtons_.at(button[0]).current;
                last = keyboardButtons_.at(button[0]).last;
            }
            else
            {
                if (button == "lmb")
                {
                    current = mouseButtons_[0].current;
                    last = mouseButtons_[0].last;
                }
                else if ("rmb")
                {
                    current = mouseButtons_[1].current;
                    last = mouseButtons_[1].last;
                }
            }
            return last == true && current == false;
        }
        bool IsHeld(const char* button) const
        {
            bool current = false;
            bool last = false;
            if (button[1] == '\0') // Accessing a keyboard button.
            {
                current = keyboardButtons_.at(button[0]).current;
                last = keyboardButtons_.at(button[0]).last;
            }
            else
            {
                if (button == "lmb")
                {
                    current = mouseButtons_[0].current;
                    last = mouseButtons_[0].last;
                }
                else if ("rmb")
                {
                    current = mouseButtons_[1].current;
                    last = mouseButtons_[1].last;
                }
            }
            return last == true && current == true;
        }
        bool MouseIsMoving() const
        {
            return mousePos_ != lastMousePos_;
        }

        glm::vec2 GetMousePosWorldSpace(const float SCREEN_RESOLUTION[2], const glm::mat4& projection, const glm::mat4& view) const
        {
            return NdcToWorld(ScreenToNdc(mousePos_, SCREEN_RESOLUTION), projection, view);
        }
        glm::vec2 GetMousePosNdc(const float SCREEN_RESOLUTION[2]) const
        {
            return ScreenToNdc(mousePos_, SCREEN_RESOLUTION);
        }
        glm::vec2 GetMousePosScreenSpace() const
        {
            return mousePos_;
        }
    private:
        struct KeyboardButton
        {
            SDL_KeyCode keycode = SDLK_UNKNOWN;
            bool current = false;
            bool last = false;
        };
        struct MouseButton
        {
            bool current = false;
            bool last = false;
        };

        std::map<const char, KeyboardButton> keyboardButtons_ =
        {
            {'d', {SDLK_d, false, false}},
            {'w', {SDLK_w, false, false}},
            {'a', {SDLK_a, false, false}},
            {'s', {SDLK_s, false, false}}
        };
        std::array<MouseButton, 2> mouseButtons_ = {};
        glm::vec2 mousePos_ = glm::vec2(0.0f, 0.0f);
        glm::vec2 lastMousePos_ = glm::vec2(0.0f, 0.0f);
    };

    class Playground : public Program
    {
    public:
        void Init() override
        {
            glEnable(GL_CULL_FACE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glPointSize(PIXEL_SIZE_);

            {
                std::vector<float> data = QUAD_POSITIONS;
                glGenVertexArrays(1, &quadVAO_);
                glBindVertexArray(quadVAO_);
                glGenBuffers(1, &quadVBO_);
                glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
                glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
                CheckGlError();
            }

            int width, height, nrOfChannels;
            unsigned char* imgData = stbi_load("../data/tank_body.png", &width, &height, &nrOfChannels, 0);
            assert(imgData != nullptr && width > 0 && height > 0 && nrOfChannels == 4);
            glGenTextures(2, tankTextures_);
            glBindTexture(GL_TEXTURE_2D, tankTextures_[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            stbi_image_free(imgData);
            imgData = stbi_load("../data/tank_gun.png", &width, &height, &nrOfChannels, 0);
            assert(imgData != nullptr && width > 0 && height > 0 && nrOfChannels == 4);
            glBindTexture(GL_TEXTURE_2D, tankTextures_[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            Shader::Definition sdef;
            sdef.vertexPath = "../data/shaders/sprite.vert";
            sdef.fragmentPath = "../data/shaders/sprite_RGBA.frag";
            sdef.staticInts.insert({ "ALBEDO", 0 });
            sdef.dynamicMat4s.insert({ "view", &view_ });
            sdef.staticMat4s.insert({ "PROJECTION", ORTHO });
            tankShader_.Create(sdef);

            sdef = {};
            sdef.vertexPath = "../data/shaders/projectile.vert";
            sdef.fragmentPath = "../data/shaders/projectile.frag";
            sdef.dynamicMat4s.insert({ "view", &view_ });
            sdef.staticMat4s.insert({ "PROJECTION", ORTHO });
            sdef.staticFloats.insert({ "NR_OF_VERTICES", NR_OF_PARTICLES_FOR_PROJECTILE });
            sdef.staticFloats.insert({ "EXPLOSION_RADIUS_MULTIPLIER", PROJECTILE_EXPLOSION_RADIUS_MULTIPLIER });
            projectileShader_.Create(sdef);

            map_.Init(view_);
            playerTank_.Init(ZERO_VEC3, RED + BLUE + GREEN * 0.35f, {&enemyTank_}, view_);
            enemyTank_.Init(ONE_VEC3 * 5.0f, RED, reinterpret_cast<A_Tank*>(&playerTank_));
        }

        void Update(seconds dt) override
        {
            lastDt_ = dt_;
            dt_ = dt.count();
            timer_ += dt_;
            const glm::vec2 playerPos = playerTank_.GetPos();
            view_ = glm::lookAt(ToVec3(playerPos) + FRONT_VEC3, ToVec3(playerPos), UP_VEC3);

            glClearColor(CLEAR_SCREEN_COLOR[0], CLEAR_SCREEN_COLOR[1], CLEAR_SCREEN_COLOR[2], CLEAR_SCREEN_COLOR[3]);
            glClear(GL_COLOR_BUFFER_BIT /* | GL_DEPTH_BUFFER_BIT*/);

            map_.Update(playerPos, quadVAO_);

            // TODO: make all particles be drawn on top of all tanks.
            enemyTank_.Update(
                dt_,
                playerPos,
                quadVAO_,
                tankShader_,
                projectileShader_,
                tankTextures_);

            playerTank_.Update(
                inputManager_.IsDown("d"),
                inputManager_.IsDown("w"),
                inputManager_.IsDown("a"),
                inputManager_.IsDown("s"),
                inputManager_.JustPressed("lmb"),
                dt_,
                inputManager_.GetMousePosWorldSpace(SCREEN_RESOLUTION, ORTHO, view_) - playerPos,
                quadVAO_,
                tankTextures_,
                tankShader_,
                projectileShader_,
                map_);

            inputManager_.UpdateButtons(); // TODO: wierd as fuck to put it down here... needs to be here for the InputManager's Just...() functions to work, look into it.
        }
        void Destroy() override
        {
            // ResourceManager::Get().Shutdown();
            glDeleteVertexArrays(1, &quadVAO_);
            glDeleteBuffers(1, &quadVBO_);
            
            glDeleteTextures(2, tankTextures_);

            glDeleteProgram(tankShader_.GetPROGRAM());
            glDeleteProgram(projectileShader_.GetPROGRAM());

            playerTank_.Destroy();
            enemyTank_.Destroy();
            map_.Destroy();
        }
        void OnEvent(SDL_Event& event) override
        {
            if (event.type == SDL_QUIT)
            {
                exit(0);
            }
            else
            {
                inputManager_.ProcessEvent(event);
            }
        }
        void DrawImGui() override
        {

        }

    private:
        // ResourceManager& resourceManager_ = ResourceManager::Get();
        InputManager& inputManager_ = InputManager::Get();
        float timer_ = 0.0f;
        float dt_ = 0.0f;
        float lastDt_ = 0.0f;
        
        unsigned int quadVAO_ = 0, quadVBO_ = 0;
        glm::mat4 view_ = IDENTITY_MAT4; // Uniform.

        constexpr static const float PIXEL_SIZE_ = 2.0f;
        PlayerTank playerTank_;
        AiTank enemyTank_;
        Shader tankShader_, projectileShader_;
        unsigned int tankTextures_[2] = {0};

        Map map_;
    };

} // End namespace gl.

int main(int argc, char** argv)
{
    gl::Playground program;
    gl::Engine engine(program);
    engine.Run();
    return EXIT_SUCCESS;
}
