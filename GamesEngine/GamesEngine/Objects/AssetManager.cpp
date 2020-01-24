#include "AssetManager.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <imgLoader/stb_image.h>

AssetManager* AssetManager::s_assetManager = new AssetManager();

std::vector<Mesh*> AssetManager::LoadMesh(
	const std::string& filePath, 
	const bool& importTexture, 
	const bool& importAnimation, 
	const GLenum& movementState, 
	const glm::vec3& importedColour
) {
	this->generateTextures = importTexture;
	this->generateAnimation = importAnimation;

	std::vector<MeshData> meshDataOutput;

	std::string build3DModelPath = "Resources/3DModels/" + filePath;
	Assimp::Importer meshImporter;
	const aiScene* scene = meshImporter.ReadFile(build3DModelPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR: Mesh could not be loaded using Assimp. Error name: " << meshImporter.GetErrorString() << std::endl;
		return std::vector<Mesh*> {};
	}

	if (importTexture) {
		// TODO: BUILD THE TEXTURE LOADER
	}
	if (importAnimation)
	{
		// TODO: BUILD THE ANIMATION LOADER
	}

	this->ProcessMeshScene(scene, meshDataOutput);

	std::vector<Mesh*> finalMeshOutput;

	for (MeshData mD : meshDataOutput) {
		Shader* newShader = new Shader(ShaderType::PHONG);

		std::vector<Texture*> meshTextureSet = mD.textureData;

		Material* newMaterial = new Material(meshTextureSet, newShader);

		for (unsigned int i = 0; i < mD.verticesData.size(); i++) {
			mD.verticesData[i].colour = importedColour;
		}

		finalMeshOutput.push_back(new Mesh(newMaterial, mD.verticesData, mD.indicesData, movementState));
	}

	return finalMeshOutput;
}

void AssetManager::ProcessMeshScene(const aiScene* scene, std::vector<MeshData>& meshDataInput) {
	aiNode* sceneNode = scene->mRootNode;

	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < sceneNode->mNumChildren; i++) {
		aiNode* meshSceneNodeChildren = sceneNode->mChildren[i];

		// Process each mesh located at the current node
		for (unsigned int i = 0; i < meshSceneNodeChildren->mNumMeshes; i++)
		{
			// The node object only contains indices to index the actual objects in the scene. 
			// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
			aiMesh* mesh = scene->mMeshes[meshSceneNodeChildren->mMeshes[i]];
			MeshData tempMesh = this->ProcessMeshData(scene, mesh);
			meshDataInput.push_back(tempMesh);
		}
	}
}

MeshData AssetManager::ProcessMeshData(const aiScene* scene, aiMesh* meshDataScene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::vector<Texture*> textures;
	//vector<Texture> textures;
	//vector<VertexBoneData> bones_id_weights_for_each_vertex;

	vertices.reserve(meshDataScene->mNumVertices);
	indices.reserve(meshDataScene->mNumVertices);

	//bones_id_weights_for_each_vertex.resize(mesh->mNumVertices);

	// Vertices
	for (unsigned int i = 0; i < meshDataScene->mNumVertices; i++)
	{
		Vertex tempVertex;
		glm::vec3 tempVector;
		tempVector.x = meshDataScene->mVertices[i].x;
		tempVector.y = meshDataScene->mVertices[i].y;
		tempVector.z = meshDataScene->mVertices[i].z;
		tempVertex.pos = tempVector;

		if (meshDataScene->mNormals != NULL)
		{
			tempVector.x = meshDataScene->mNormals[i].x;
			tempVector.y = meshDataScene->mNormals[i].y;
			tempVector.z = meshDataScene->mNormals[i].z;
			tempVertex.normals = tempVector;
		}
		else
		{
			tempVertex.normals = glm::vec3(0.0f, 0.0f, 0.0f);
		}


		// In assimp model can have 8 different texture coordinates
		// We only care about the first set of texture coordinates
		if (meshDataScene->mTextureCoords[0])
		{
			glm::vec2 tempTexture;
			tempTexture.x = meshDataScene->mTextureCoords[0][i].x;
			tempTexture.y = meshDataScene->mTextureCoords[0][i].y;
			tempVertex.textureCoord = tempTexture;
		}
		else
		{
			tempVertex.textureCoord = glm::vec2(0.0f, 0.0f);
		}

		if (meshDataScene->mTangents != NULL) {
			tempVector.x = meshDataScene->mTangents->x;
			tempVector.y = meshDataScene->mTangents->y;
			tempVector.z = meshDataScene->mTangents->z;
			tempVertex.tangent = tempVector;
		}
		else
		{
			tempVertex.tangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		if (meshDataScene->mBitangents != NULL) {
			tempVector.x = meshDataScene->mBitangents->x;
			tempVector.y = meshDataScene->mBitangents->y;
			tempVector.z = meshDataScene->mBitangents->z;

			tempVertex.biTangent = tempVector;
		}
		else
		{
			tempVertex.biTangent = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		vertices.push_back(tempVertex);
	}

	// Indices
	for (unsigned int i = 0; i < meshDataScene->mNumFaces; i++)
	{
		aiFace face = meshDataScene->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	// Material
	if (this->generateTextures) {
		aiMaterial* material = scene->mMaterials[meshDataScene->mMaterialIndex];

		// Diffuse Map
		std::vector<Texture*> diffuseMaps = this->MeshMaterialLoader(material, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// Specular Map
		std::vector<Texture*> specularMaps = this->MeshMaterialLoader(material, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// Normal Map
		std::vector<Texture*> normalMaps = this->MeshMaterialLoader(material, TextureType::BUMP);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	MeshData outputMeshData;
	outputMeshData.indicesData = indices;
	outputMeshData.verticesData = vertices;
	outputMeshData.textureData = textures;

	return outputMeshData;

#if 0
	// load bones
	/*for (uint i = 0; i < mesh->mNumBones; i++)
	{
		uint bone_index = 0;
		string bone_name(mesh->mBones[i]->mName.data);

		cout << mesh->mBones[i]->mName.data << endl;

		if (m_bone_mapping.find(bone_name) == m_bone_mapping.end())
		{
			// Allocate an index for a new bone
			bone_index = m_num_bones;
			m_num_bones++;
			BoneMatrix bi;
			m_bone_matrices.push_back(bi);
			m_bone_matrices[bone_index].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
			m_bone_mapping[bone_name] = bone_index;

			//cout << "bone_name: " << bone_name << "			 bone_index: " << bone_index << endl;
		}
		else
		{
			bone_index = m_bone_mapping[bone_name];
		}

		for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			uint vertex_id = mesh->mBones[i]->mWeights[j].mVertexId;
			float weight = mesh->mBones[i]->mWeights[j].mWeight;
			bones_id_weights_for_each_vertex[vertex_id].addBoneData(bone_index, weight);


			//cout << " vertex_id: " << vertex_id << "	bone_index: " << bone_index << "		weight: " << weight << endl;
		}
	}*/
#endif
}

std::vector<Texture*> AssetManager::MeshMaterialLoader(aiMaterial* material, const TextureType& textureType) {
	std::vector<Texture*> textures;

	for (unsigned int i = 0; i < material->GetTextureCount(TextureTypeToAssimp(textureType)); i++) {
		aiString filePath;
		material->GetTexture(TextureTypeToAssimp(textureType), i, &filePath);

		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		std::string filename = std::string(filePath.C_Str());
		Texture* texture = this->CheckTextureLoaded(filename, textureType, 1000, 1000);

		textures.push_back(texture);
	}

	// If no materials have been found, use the default map
	if (textureType == TextureType::DIFFUSE && textures.size() <= 0) {
		textures.push_back(this->CheckTextureLoaded("Default.jpg", TextureType::DIFFUSE, 1, 1));
	}

	return textures;
}

void AssetManager::LoadTexture(const std::string& fileName, GLuint& textureId, int& width, int& height) {
	stbi_set_flip_vertically_on_load(0);

	std::string buildName = "Resources/Textures/" + fileName;
	int bits; // BitsPerPixels
	unsigned char* image = stbi_load(buildName.c_str(), &width, &height, &bits, 4);

	if (image == NULL) {
		std::cout << "ERROR: Texture failed to load!" << std::endl;
	}

	this->ProcessTexture(image, textureId, width, height);
}

void AssetManager::ProcessTexture(const unsigned char* loadedImageData, GLuint& textureId, int width, int height) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, loadedImageData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free((void*)loadedImageData);
}

Texture* AssetManager::CheckTextureLoaded(const std::string& fileName, const TextureType& textureType, int width, int height)
{
	for (unsigned int i = 0; i < this->m_loadedAssets.size(); i++)
	{
		if (this->m_loadedAssetsTypes[i] == AssetType::TEXTURE) {
			if (std::strcmp(((Texture*)this->m_loadedAssets[i])->GetTextureName().c_str(), fileName.c_str()) == 0
				&& textureType == ((Texture*)this->m_loadedAssets[i])->GetTextureType())
			{
				return ((Texture*)this->m_loadedAssets[i]);
			}
		}
	}

	return new Texture(fileName, textureType, width, height);
}

void AssetManager::LoadSound(const std::string& fileName, FMOD_MODE fModMode) {
	FMOD::Sound* newSound = nullptr;

	std::string buildFileName = "Resources/AudioSamples/" + fileName;

	newSound = this->CheckSoundLoaded(buildFileName);

	if (newSound == nullptr) {
		SoundEngine::s_soundEngine
			->ErrorCheck(SoundEngine::s_soundEngine->GetLowerSystem()->createSound(buildFileName.c_str(), fModMode, NULL, &newSound));

		SoundEngine::s_soundEngine->InitialiseNewSound(fileName, newSound);
		this->PushLoadedAsset(AssetType::AUDIO, (unsigned int*)newSound);
	}
}

FMOD::Sound* AssetManager::CheckSoundLoaded(const std::string& fileName) {
	SoundMap tempMap = SoundEngine::s_soundEngine->GetEngineSoundMap();

	for (unsigned int i = 0; i < tempMap.size(); i++)
	{
		if (tempMap.count(fileName) > 0)
		{
			return SoundEngine::s_soundEngine->GetFromMap<FMOD::Sound, SoundMap>(fileName, tempMap);
		}
	}

	return nullptr;
}
