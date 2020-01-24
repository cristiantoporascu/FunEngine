#include "Texture.h"
#include "AssetManager.h"
#include <iostream>

Texture::Texture(const std::string& fileName, const TextureType& textureType, int width, int height)
{
	this->m_textureName = fileName;
	this->m_textureWidth = width;
	this->m_textureHeight = height;
	this->m_textureType = textureType;

	glGenTextures(1, &this->m_textureId);
	AssetManager::s_assetManager->LoadTexture(fileName, this->m_textureId, this->m_textureWidth, this->m_textureHeight);
	AssetManager::s_assetManager->PushLoadedAsset(AssetType::TEXTURE, (unsigned int*)this);
}

Texture::~Texture() {}

void Texture::BindTexture(unsigned int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, this->m_textureId);
}

void Texture::UnbindTexture(unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, 0);
}