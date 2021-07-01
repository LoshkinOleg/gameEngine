#include "material.h"

#include "resource_manager.h"

void gl::Material::Create(Definition def)
{
    if (shader_.GetPROGRAM() != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    // assert(def.texturePathsAndTypes.size() > 0); // We might actually want to create materials without textures. Like blank meshes
    for (const auto& pair : def.texturePathsAndTypes)
    {
        // We don't want framebuffers to have materials.
        assert(!pair.first.empty() && (int)pair.second < (int)Texture::Type::INVALID);
    }

    for (size_t i = 0; i < def.texturePathsAndTypes.size(); i++)
    {
        Texture tex;
        if (def.texturePathsAndTypes[i].second != Texture::Type::CUBEMAP)
        {
            tex.Create(def.texturePathsAndTypes[i].second, def.texturePathsAndTypes[i].first, def.flipImages, def.correctGamma, true);
        }
        else
        {
            assert(def.texturePathsAndTypes.size() >= i + 6); // A cubemap needs 6 textures.
            std::array<std::string_view, 6> paths =
            {
                def.texturePathsAndTypes[i].first,
                def.texturePathsAndTypes[i + 1].first,
                def.texturePathsAndTypes[i + 2].first,
                def.texturePathsAndTypes[i + 3].first,
                def.texturePathsAndTypes[i + 4].first,
                def.texturePathsAndTypes[i + 5].first
            };
            tex.Create(paths, def.flipImages, def.correctGamma);
            i += 5;
        }
        textures_.push_back(tex);
    }

    shader_.Create(def.shader);
}

void gl::Material::Bind()
{
	for (const auto& texture : textures_)
	{
		texture.Bind();
	}
    shader_.Bind();
}
void gl::Material::Unbind()
{
	// for (const auto& texture : textures_)
	// {
	// 	texture.Unbind();
	// }
    // shader_.Unbind();
}