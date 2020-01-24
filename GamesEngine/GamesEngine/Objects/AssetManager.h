#pragma once
#include "Mesh.h"
#include "../Utils/SoundEngine.h"
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum AssetType {
	MESH,
	TEXTURE,
	AUDIO
};

struct MeshData {
	std::vector<Vertex> verticesData;
	std::vector<unsigned int> indicesData;
	std::vector<Texture*> textureData;
};

class AssetManager {
private:
	std::vector<AssetType> m_loadedAssetsTypes;
	std::vector<unsigned int*> m_loadedAssets;

	bool generateTextures = false;
	bool generateAnimation = false;

public:
	/**
	 * Singletone for the assetmanager to be accesable from everywhere
	 */
	static AssetManager* s_assetManager;

	static aiTextureType TextureTypeToAssimp(TextureType texType)
	{
		switch (texType)
		{
		case TextureType::DIFFUSE:		return aiTextureType_DIFFUSE;
		case TextureType::SPECULAR:		return aiTextureType_SPECULAR;
		case TextureType::NORMAL:		return aiTextureType_NORMALS;
		case TextureType::BUMP:			return aiTextureType_HEIGHT;
		case TextureType::EMISSIVE:		return aiTextureType_EMISSIVE;
		case TextureType::ROUGHNESS:	return aiTextureType_DIFFUSE_ROUGHNESS;

		default:
			return aiTextureType_UNKNOWN;
		}
	}

	static TextureType AssimpToTextureType(aiTextureType texType)
	{
		switch (texType)
		{
		case aiTextureType_DIFFUSE:					return TextureType::DIFFUSE;
		case aiTextureType_SPECULAR:				return TextureType::SPECULAR;
		case aiTextureType_NORMALS:					return TextureType::NORMAL;
		case aiTextureType_HEIGHT:					return TextureType::BUMP;
		case aiTextureType_EMISSIVE:				return TextureType::EMISSIVE;
		case aiTextureType_DIFFUSE_ROUGHNESS:		return TextureType::ROUGHNESS;

		default:
			return TextureType::UNKNOWN;
		}
	}

	AssetManager() {};
	~AssetManager() {
		for (unsigned int i = 0; i < this->m_loadedAssets.size(); i++) {
			switch (this->m_loadedAssetsTypes[i]) {
			case AssetType::TEXTURE:
				delete ((Texture*)this->m_loadedAssets[i]);
				break;
			case AssetType::MESH:
				delete ((Mesh*)this->m_loadedAssets[i]);
				break;
			}
		}
	};

	/**
	 * Load the mesh using ::assimp:: and gets the Mesh data exporting it into the object
	 * @param filePath							The path to the mesh that needs importing
	 * @param importTexture						Whether the texture shoudlbe imported or not
	 * @param importAnimation					Whether the animation should be imported or not
	 * @param movementState						Movement state of the imported mesh
	 * @param importedColour					Mesh colour for each vertex
	 * @return vector<Mesh*>					Data formated in the mesh properties of a mesh
	 *											so that it can be passed to the mesh
	 */
	std::vector<Mesh*> LoadMesh(
		const std::string& filePath, 
		const bool& importTexture,
		const bool& importAnimation, 
		const GLenum& movementState, 
		const glm::vec3& importedColour = glm::vec3(1.0f, 1.0f, 1.0f));

	/**
	 * With the data loaded by the mesh loader through ::assimp:: process and it create the necesary MeshData
	 * @param scene								The scene of the loaded file
	 * @param meshData							Refference to the mesh data from the mesh constructor
	 *											in order to export the data loaded by the AM
	 */
	void ProcessMeshScene(const aiScene* scene, std::vector<MeshData>& meshDataInput);

	/**
	 * With the data loaded by the mesh loader through ::assimp:: process and it create the necesary MeshData
	 * @param scene								The scene of the loaded file
	 * @param meshDataScene						Mesh data loaded by the ::assimp::loader
	 * @return MeshData							Result of parsing the sceneMesh into the meshData we need
	 */
	MeshData ProcessMeshData(const aiScene* scene, aiMesh* meshDataScene);

	/**
	 * Load the material that was stores as refference by the ::assimp:: scene loader
	 * @param material							Material that was found by the scene loader
	 * @param textureType						Type of the texture that the material should be bound to
	 * @return vector<Texture*>					Vector of textures to be bound to the material
	 */
	std::vector<Texture*> MeshMaterialLoader(aiMaterial* material, const TextureType& textureType);

	/**
	 * Load the mesh using ::assimp:: and gets the Mesh data exporting it into the object
	 * @param fileName							The path to the mesh that needs importing
	 * @param textureId							The generated texture based on it's object instance and
	 *											and instantiation
	 * @param width								The width that the map should be converted to
	 * @param height							The height that the map should be converted to
	 */
	void LoadTexture(const std::string& fileName, GLuint& textureId, int& width, int& height);

	/**
	 * With the data that was loaded generate the texture and then return the id
	 * @param textureId							The generated texture based on it's object instance and
	 *											and instantiation (passed by refference for easy access)
	 * @param loadedImageData					The loaded data of the texture map
	 * @param width								The width that the map should be converted to
	 * @param height							The height that the map should be converted to
	 */
	void ProcessTexture(const unsigned char* loadedImageData, GLuint& textureId, int width, int height);

	/**
	 * Check if the texture that follows to be loaded already exists inside the resource manager
	 * @param fileName							The path to the mesh that needs importing
	 * @param textureType						The type of the texture that will be loaded
	 * @param width								The width that the map should be converted to
	 * @param height							The height that the map should be converted to
	 * @return Texture*							Pointer to the already existing texture
	 */
	Texture* CheckTextureLoaded(const std::string& fileName, const TextureType& textureType, int width, int height);

	/**
	 * Load the sound with the specified filenames and initialise the sound in the sound engine
	 * @param fileName							The path to the specific sound sample
	 * @param fModMode							Specific aditional options that can be passed for 
	 *											a specific sound sample
	 */
	void LoadSound(const std::string& fileName, FMOD_MODE fModMode);

	/**
	 * Check if the sound that is passed at this point has been already loaded by the resource manager
	 * @param fileName							The path to the specific sound sample that needs testing
	 * @return FMOD::Sound*						Return pointer to the already existing sample
	 */
	FMOD::Sound* CheckSoundLoaded(const std::string& fileName);


	/**
	 * Getters and setters
	 */
public:
	inline std::vector<unsigned int*> GetLoadedAssets() { return this->m_loadedAssets; }
	inline unsigned int* GetLoadedAssetsById(const int& id) { return this->m_loadedAssets[id]; }
	inline std::vector<AssetType> GetLoadedAssetsTypes() { return this->m_loadedAssetsTypes; }
	inline const AssetType& GetLoadedAssetsTypesById(const int& id) { return this->m_loadedAssetsTypes[id]; }

	inline void PushLoadedAsset(AssetType newAssetType, unsigned int* newAsset) { 
		this->m_loadedAssets.push_back(newAsset); 
		this->m_loadedAssetsTypes.push_back(newAssetType);
	}
};