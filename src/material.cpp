#include "material.h"

#include "resource_manager.h"

void gl::Material::Create(Definition def)
{
    if (shader_.GetPROGRAM() != 0)
    {
        EngineError("Calling Create() a second time...");
    }

    for (const auto& pair : def.texturePathsAndTypes)
    {
        assert(!pair.first.empty() && (int)pair.second < (int)Texture::Type::INVALID);
    }

    for (size_t i = 0; i < def.texturePathsAndTypes.size(); i++)
    {
        Texture tex;
        tex.Create(def.texturePathsAndTypes[i].second, def.texturePathsAndTypes[i].first);
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
	for (const auto& texture : textures_)
	{
		texture.Unbind();
	}
    shader_.Unbind();
}