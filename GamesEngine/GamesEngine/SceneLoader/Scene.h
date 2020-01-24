#pragma once
#include "../Objects/GameObject.h"
#include "../Shaders/Light.h"
#include <tinyxml2/tinyxml2.h>
#include <string>
#include <regex>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

/**
 * Level XML Architecture
 * ( Example: @nodePurpose					nodeName(extraInfo) )
 * @root									LEVEL
 * --@levelData								VERSION
 * --@soundSamples							SOUNDS			----- TODO
 * --|-@sound								SOUND			----- TODO
 * --|-|-@soundName							SOUNDNAME		----- TODO
 * --|-|-@soundType							SOUNDCHANNEL	----- TODO
 * --|-|-@soundEvent						SOUNDACTION		----- TODO
 * --@sceneLighting							SCENELIGHTS		----- TODO
 * --|-@specificLight						LIGHT			----- TODO
 * --|-|-@lightType							LIGHTTYPE		----- TODO
 * --@gameObjectIterator					GAMEOBJECTS
 * --|-@object								OBJECT
 * --|-|-@isActive							ACTIVE
 * --|-|-@objectPhysicsState				PHYSICSSTATE
 * --|-|-@objectType						OBJECTTYPE
 * --|-|-@objectMass						OBJECTMASS
 * --|-|-@fileName							FILENAME ( If empty ( "" ) it will be counted as not imported )
 * --|-|-@movementState						MOVESTATE(STATIC / DYNAMIC)
 * --|-|-@transformsNode					TRANSFORMS
 * --|-|-|-@positionFromCenter				TRANSFORM-POSITION ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-@rotationFromCenter				TRANSFORM-ROTATION ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-@scaleFromCenter					TRANSFORM-SCALE ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-@(ifFileNameEmpty)
 * --|-|-|-@colourChange					COLOUR (Attributes: R, G, B -- Range(1,255))
 * --|-|-@(ELSE)
 * --|-|-|-@meshList						MESHLIST
 * --|-|-|-|-@mesh							MESH
 * --|-|-|-|-|-@material					MATERIAL
 * --|-|-|-|-|-|-@shaderType				SHADERTYPE ( EMPTY/FLAT/PHONG )
 * --|-|-|-|-|-|-@texturesList				TEXTURELIST
 * --|-|-|-|-|-|-|-@textures				TEXTURE
 * --|-|-|-|-|-|-|-|-@textureType			TEXTURETYPE ( DIFFUSE, SPECULAR, BUMP )
 * --|-|-|-|-|-|-|-|-@textureFilePath		TEXTUREFILE
 * --|-|-|-|-|-|-|-|-@textureSize			TEXTURESIZE
 * --|-|-|-|-|-|-@diffuseColour				DIFFUSE ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-@specularColour			SPECULAR ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-@emissionColour			EMISSION ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-@shininessColour			SHININESS (FLOAT)
 * --|-|-|-|-|-@triangleIterator			TRIANGLES
 * --|-|-|-|-|-|-@triangleVertices			TRIANGLE-VERTICES
 * --|-|-|-|-|-|-|-@actualVerticesData		VERTEX
 * --|-|-|-|-|-|-|-|-@vertexPos				VERTEX-POSITION ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-|-|-@vertexTexture			VERTEX-TEXTURE (!!OPTIONAL!! uses default unless specified) ( Attributes: AXIS-X, AXIS-Y )
 * --|-|-|-|-|-|-|-|-@vertexColour			VERTEX-COLOUR ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-|-|-@vertexNormals			VERTEX-NORMALS ( Attributes: AXIS-X, AXIS-Y, AXIS-Z )
 * --|-|-|-|-|-|-@triangleIndices			TRIANGLE-INDICES ( Attributes; INDICES-LIST => Comma separated )
 */

class Scene {
private:
	// GameObjects that are currently existing in the scene
	std::vector<GameObject*> gameObjects;
	std::vector<Light*> lights;

	// Scene current level
	int m_currentLevel;

public:
	Scene() {}
	Scene(int activeLevel);
	~Scene();

	/**
	 * Interprets the objects information and the level information and converts it into
	 * the necesarry information based on the above arhitecture so that the levels can be rewritten in XML
	 * @param levelParsed						Number of the parsed level ( From xml file )
	 * @param gameObjectsList					List of the game objects where the objects will be created
	 */
	void ExportDataParser(int levelParsed, const std::vector<GameObject*>& gameObjectsList);

	/**
	 * Interprets all the fields presented in the above arhitecture
	 * and creates the necesary gameObjects in order for the scene to run
	 * as well as setting level specific information like ( Audio, background, spheres )
	 * @param levelParsed						Number of the parsed level ( From xml file )
	 * @param gameObjectsList					List of the game objects where the objects will be created
	 */
	void LevelDataParser(int levelParsed, std::vector<GameObject*>& gameObjectsList);

	/**
	 * Deletes the specific selected level and clears the .xml file that was hosting
	 * previously the data of that specific level
	 * @param levelParsed						Number of the parsed level ( From xml file )
	 */
	void DeleteDataParser(int levelParsed) {
		std::string buildLevelName = "Resources/LevelData/Level" + std::to_string(levelParsed) + ".xml";
		if (remove(buildLevelName.c_str()) == 0)
			std::cout << "SUCCESS: File has been deleted" << std::endl;
		else
			std::cout << "ERROR: Unable to delete the file" << std::endl;
	}

	/**
	 * Clear the data of the wanted gameObject parsed list
	 * @param gameObjectsList					List of the game objects where the objects will be created
	 */
	void ClearDataParsed(std::vector<GameObject*>& gameObjectsList) {
		for (int i = 0; i < gameObjectsList.size(); i++) {
			delete gameObjectsList[i];
		}
		gameObjectsList.clear();
	}

	/**
	 * Clear the data of the level that this scene belongs to
	 */
	void ClearCurrentData() {
		for (int i = 0; i < this->gameObjects.size(); i++) {
			delete this->gameObjects[i];
		}
		this->gameObjects.clear();
	}

	/**
	 * Draw the meshes that are existing in the current scene
	 * as well as calculating the uniforms for the specific lights present in the scene
	 * @param builderActive					If the scene builder is active then disable
	 *										The update of the physics simulation
	 */
	void DrawScene(const bool& builderActive);

	/**
	 * This method is desined to handle the events from the game objects that
	 * are at the moment of the update initialised inside the scene
	 * @param e								Event that has been queued inside this frame
	 */
	void HandleEvents(Event& e);

	/**
	 * Getters and setters
	 */
public:
	inline void AddObjectToScene(GameObject* newGO) { this->gameObjects.push_back(newGO); }
	inline void RemoveObjectById(const int& id) {
		// Clears the memory of the specific object instance
		delete this->gameObjects[id];
		// Erases the object from the array
		this->gameObjects.erase(this->gameObjects.begin() + id);
	}

	inline const std::vector<GameObject*>& GetSceneParsedObjects() const { return this->gameObjects; }
	inline GameObject* GetParsedObject(const int& indexParsedObject) const { return this->gameObjects[indexParsedObject]; }
	inline GameObject* GetScenePlayer() const {
		for (unsigned int i = 0; i < this->gameObjects.size(); i++) {
			if (this->gameObjects[i]->GetObjectType() == ObjectType::PLAYER) {
				return this->gameObjects[i];
			}
		}
	}
	inline GameObject* GetSceneFinish() const {
		for (unsigned int i = 0; i < this->gameObjects.size(); i++) {
			if (this->gameObjects[i]->GetFileName() == "Finish.obj") {
				return this->gameObjects[i];
			}
		}
		return nullptr;
	}
	inline const int& GetCurrentLevel() const { return this->m_currentLevel; }
	inline const std::vector<Light*>& GetSceneLights() const { return this->lights; }
	inline const Light* GetSceneLightById(const int& id) const { return this->lights[id]; }

	inline void SetCurrentLevel(const int& newLevel) { 
		this->m_currentLevel = newLevel;
		this->ClearDataParsed(this->gameObjects);
		this->LevelDataParser(newLevel, this->gameObjects);
	}

	/**
	 * Iterate through all the files inside the resource folder
	 * and check how many levels are there
	 * @return std::vector<std::string>				returns a list with all
	 *												the .xml files that are under LevelData
	 */
	static inline std::vector<std::string> GetLevelFiles()
	{
		namespace fs = std::experimental::filesystem;

		std::vector<std::string> output;
		std::string path = fs::current_path().generic_u8string() + "/Resources/LevelData";
		for (const auto& entry : fs::directory_iterator(path)) {
			output.push_back(std::regex_replace(entry.path().generic_u8string(), std::regex(path+"/"), ""));
		}

		return output;
	}
};