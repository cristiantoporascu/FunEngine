#pragma once
#include <string>
#include <glad/glad.h>

// Forward declarations
class AssetManager;

enum TextureType {
	UNKNOWN, DIFFUSE, SPECULAR, NORMAL, BUMP, EMISSIVE, ROUGHNESS
};

class Texture
{
private:
	std::string m_textureName;
	int m_textureWidth;
	int m_textureHeight;

	GLuint m_textureId;

	TextureType m_textureType;

public:
	static TextureType ConvertIntToType(const int& id) {
		switch (id) {
		case 0:
			return TextureType::UNKNOWN;
		case 1:
			return TextureType::DIFFUSE;
		case 2:
			return TextureType::SPECULAR;
		case 3:
			return TextureType::NORMAL;
		case 4:
			return TextureType::BUMP;
		case 5:
			return TextureType::EMISSIVE;
		case 6:
			return TextureType::ROUGHNESS;

		}
	}

	Texture(const std::string& fileName, const TextureType& textureType, int width, int height);
	~Texture();

	/**
	 * Bind specific texture to the VAO while computing a certain object/s
	 * @param unit				The offset of the specific material ( in case if you have multiple )
	 */
	void BindTexture(unsigned int unit);

	/**
	 * Unbind specific texture from the VAO
	 * @param unit				The offset of the specific material ( in case if you have multiple )
	 */
	void UnbindTexture(unsigned int unit);

	/**
	 * Getters and setters
	 */
public:
	inline const std::string& GetTextureName() const { return this->m_textureName; }
	inline const TextureType& GetTextureType() const { return this->m_textureType; }
	inline const int& GetTextureSize() const { return this->m_textureWidth; }

	inline void SetTextureName(const std::string& newTexturePath) { this->m_textureName = newTexturePath; }
	inline void SetTextureType(const TextureType& newTextureType) { this->m_textureType = newTextureType; }
	inline void SetTextureSize(const int& newTextureSize) { 
		this->m_textureWidth = newTextureSize;
		this->m_textureHeight = newTextureSize;
	}
};