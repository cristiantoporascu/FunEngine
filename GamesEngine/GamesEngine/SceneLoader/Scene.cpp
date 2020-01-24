#include "Scene.h"
#include "../Utils/NetworkEngine.h"
#include <iostream>
#include <sstream>

Scene::Scene(int activeLevel) {
	this->SetCurrentLevel(activeLevel);
	lights.push_back(new Light(LightType::Directional, glm::vec3(-0.5f, -1.0f, 0), glm::vec3(1.0f), glm::vec3(0.2f), glm::vec3(1.0f)));
}

Scene::~Scene() {
	this->ClearCurrentData();
	for (unsigned int i = 0; i < this->lights.size(); i++) {
		delete this->lights[i];
	}
	this->lights.clear();
}

void Scene::ExportDataParser(int levelParsed, const std::vector<GameObject*>& gameObjectsList) {
	std::string buildLevelName = "Resources/LevelData/Level" + std::to_string(levelParsed) + ".xml";

	// Create the document and set the main declaration
	tinyxml2::XMLDocument levelData;
	tinyxml2::XMLNode* levelDeclaration = levelData.NewDeclaration("xml version=\"1.0\"");
	levelData.InsertEndChild(levelDeclaration);

	// Create the root node of the level file
	tinyxml2::XMLNode* rootNode = levelData.NewElement("LEVEL");
	levelData.InsertEndChild(rootNode);

	// Set the version of the level
	tinyxml2::XMLNode* levelVersionNode = levelData.NewElement("VERSION");
	levelVersionNode->ToElement()->SetText(std::to_string(levelParsed).c_str());
	rootNode->InsertEndChild(levelVersionNode);

	// Create the main node that stores the list of gameObjects
	tinyxml2::XMLNode* levelGameObjectsNode = levelData.NewElement("GAMEOBJECTS");
	rootNode->InsertEndChild(levelGameObjectsNode);

	for (unsigned int i = 0; i < gameObjectsList.size(); i++) {
		// For each object in the list create and attach an object to the list
		tinyxml2::XMLNode* objectNode = levelData.NewElement("OBJECT");
		levelGameObjectsNode->InsertEndChild(objectNode);

		// Set the movement state of the object
		tinyxml2::XMLNode* objectMovingStateNode = levelData.NewElement("MOVESTATE");
		objectMovingStateNode->ToElement()->SetText(gameObjectsList[i]->GetMeshById(0)->GetMovementState() == GL_STATIC_DRAW ? "STATIC" : "DYNAMIC");
		objectNode->InsertEndChild(objectMovingStateNode);

		// Set the state of the game object
		tinyxml2::XMLNode* objectActiveState = levelData.NewElement("ACTIVE");
		objectActiveState->ToElement()->SetText(gameObjectsList[i]->GetIsActive() ? "TRUE" : "FALSE");
		objectNode->InsertEndChild(objectActiveState);

		// Set the type state of the game object
		tinyxml2::XMLNode* objectTypeNode = levelData.NewElement("OBJECTTYPE");
		objectTypeNode->ToElement()->SetText(gameObjectsList[i]->GetObjectType() == ObjectType::PLAYER ? "PLAYER" : "COMMON");
		objectNode->InsertEndChild(objectTypeNode);

		// Set the mass of the game object
		tinyxml2::XMLNode* objectMassNode = levelData.NewElement("OBJECTMASS");
		objectMassNode->ToElement()->SetText(gameObjectsList[i]->GetRigidBody()->getMass());
		objectNode->InsertEndChild(objectMassNode);

		// Set the physics state of the game object
		tinyxml2::XMLNode* objectPhysicsState = levelData.NewElement("PHYSICSSTATE");
		objectPhysicsState->ToElement()->SetText(
			gameObjectsList[i]->GetCollisionBox() != nullptr || gameObjectsList[i]->GetCollisionSphere() != nullptr
			? "TRUE" : "FALSE");
		objectNode->InsertEndChild(objectPhysicsState);

		// Create the main transform node that stores all the transform of the objects ( pos/rot/scale )
		tinyxml2::XMLNode* objectTransformNode = levelData.NewElement("TRANSFORMS");
		objectNode->InsertEndChild(objectTransformNode);

		// Set the position transform of the object
		tinyxml2::XMLNode* objectTransformPositionNode = levelData.NewElement("TRANSFORM-POSITION");
		objectTransformPositionNode->ToElement()->SetAttribute("AXIS-X", gameObjectsList[i]->GetTransform()->GetPos().x);
		objectTransformPositionNode->ToElement()->SetAttribute("AXIS-Y", gameObjectsList[i]->GetTransform()->GetPos().y);
		objectTransformPositionNode->ToElement()->SetAttribute("AXIS-Z", gameObjectsList[i]->GetTransform()->GetPos().z);
		objectTransformNode->InsertEndChild(objectTransformPositionNode);

		// Set the rotation transform of the object
		tinyxml2::XMLNode* objectTransformRotationNode = levelData.NewElement("TRANSFORM-ROTATION");
		objectTransformRotationNode->ToElement()->SetAttribute("AXIS-X", gameObjectsList[i]->GetTransform()->GetRot().x);
		objectTransformRotationNode->ToElement()->SetAttribute("AXIS-Y", gameObjectsList[i]->GetTransform()->GetRot().y);
		objectTransformRotationNode->ToElement()->SetAttribute("AXIS-Z", gameObjectsList[i]->GetTransform()->GetRot().z);
		objectTransformNode->InsertEndChild(objectTransformRotationNode);

		// Set the scale transform of the object
		tinyxml2::XMLNode* objectTransformScaleNode = levelData.NewElement("TRANSFORM-SCALE");
		objectTransformScaleNode->ToElement()->SetAttribute("AXIS-X", gameObjectsList[i]->GetTransform()->GetScale().x);
		objectTransformScaleNode->ToElement()->SetAttribute("AXIS-Y", gameObjectsList[i]->GetTransform()->GetScale().y);
		objectTransformScaleNode->ToElement()->SetAttribute("AXIS-Z", gameObjectsList[i]->GetTransform()->GetScale().z);
		objectTransformNode->InsertEndChild(objectTransformScaleNode);

		// Set the file path name of the imported 3D model
		tinyxml2::XMLNode* objectFileNameNode = levelData.NewElement("FILENAME");
		objectFileNameNode->ToElement()->SetText(gameObjectsList[i]->GetFileName().c_str());
		objectNode->InsertEndChild(objectFileNameNode);

		// Store a referrence to the vertices of the new/existing mesh
		std::vector<Mesh*> tempReferrence = gameObjectsList[i]->GetMeshes();
		if (gameObjectsList[i]->GetIsImported()) {

			// Set the colour of the new imported 3D object
			tinyxml2::XMLNode* objectColourNode = levelData.NewElement("COLOUR");
			objectColourNode->ToElement()->SetAttribute("R", tempReferrence[0]->GetVertices()[0].colour.r * 255);
			objectColourNode->ToElement()->SetAttribute("G", tempReferrence[0]->GetVertices()[0].colour.g * 255);
			objectColourNode->ToElement()->SetAttribute("B", tempReferrence[0]->GetVertices()[0].colour.b * 255);
			objectNode->InsertEndChild(objectColourNode);
		}
		else {

			// Bind a mesh list to the object in the iteration
			tinyxml2::XMLNode* meshListNode = levelData.NewElement("MESHLIST");
			objectNode->InsertEndChild(meshListNode);

			// For each mesh that belong to the game object save all the properties
			for (unsigned int m = 0; m < tempReferrence.size(); m++) {

				// Set bind a mesh node for each mesh in the list
				tinyxml2::XMLNode* meshNode = levelData.NewElement("MESH");
				meshListNode->InsertEndChild(meshNode);

				// Set the material for the specific bound mesh
				tinyxml2::XMLNode* meshMaterialNode = levelData.NewElement("MATERIAL");
				meshNode->InsertEndChild(meshMaterialNode);

				// Set the type of the shader used to bind the material
				Material* tempRefferenceForMaterial = tempReferrence[m]->GetMeshMaterial();
				tinyxml2::XMLNode* shaderMeshMaterialNode = levelData.NewElement("SHADERTYPE");

				switch (tempRefferenceForMaterial->GetShader()->GetShaderType()) {
				case ShaderType::EMPTY:
					shaderMeshMaterialNode->ToElement()->SetText("EMPTY");
					break;
				case ShaderType::FLAT:
					shaderMeshMaterialNode->ToElement()->SetText("FLAT");
					break;
				case ShaderType::PHONG:
					shaderMeshMaterialNode->ToElement()->SetText("PHONG");
					break;
				}
				meshMaterialNode->InsertEndChild(shaderMeshMaterialNode);

				// Set a texture list node for the material
				std::vector<Texture*> tempRefferenceOfMaterialTexture = tempRefferenceForMaterial->GetTextures();
				tinyxml2::XMLNode* textureMeshMaterialNode = levelData.NewElement("TEXTURELIST");
				meshMaterialNode->InsertEndChild(textureMeshMaterialNode);

				// Set each texture of the material to the specific node
				for (unsigned int t = 0; t < tempRefferenceOfMaterialTexture.size(); t++) {
					tinyxml2::XMLNode* singleTextureMaterialNode = levelData.NewElement("TEXTURE");
					textureMeshMaterialNode->InsertEndChild(singleTextureMaterialNode);

					// Set the texture's type
					tinyxml2::XMLNode* textureTypeNode = levelData.NewElement("TEXTURETYPE");

					switch (tempRefferenceOfMaterialTexture[t]->GetTextureType()){
					case TextureType::DIFFUSE:
						textureTypeNode->ToElement()->SetText("DIFFUSE");
						break;
					case TextureType::SPECULAR:
						textureTypeNode->ToElement()->SetText("SPECULAR");
						break;
					case TextureType::NORMAL:
						textureTypeNode->ToElement()->SetText("NORMAL");
						break;
					case TextureType::ROUGHNESS:
						textureTypeNode->ToElement()->SetText("ROUGHNESS");
						break;
					case TextureType::EMISSIVE:
						textureTypeNode->ToElement()->SetText("EMISSIVE");
						break;
					case TextureType::BUMP:
						textureTypeNode->ToElement()->SetText("BUMP");
						break;
					}
					singleTextureMaterialNode->InsertEndChild(textureTypeNode);

					// Set the path that the texture has been loaded from
					tinyxml2::XMLNode* textureFileNameNode = levelData.NewElement("TEXTUREFILE");
					textureFileNameNode->ToElement()->SetText(tempRefferenceOfMaterialTexture[t]->GetTextureName().c_str());
					singleTextureMaterialNode->InsertEndChild(textureFileNameNode);

					// Set the size of the loaded texture
					tinyxml2::XMLNode* textureFileSizeNode = levelData.NewElement("TEXTURESIZE");
					textureFileSizeNode->ToElement()->SetText(std::to_string(tempRefferenceOfMaterialTexture[t]->GetTextureSize()).c_str());
					singleTextureMaterialNode->InsertEndChild(textureFileSizeNode);
				}

				// Set each colour type to the file ( DIFFUSE, SPECULAR, EMISSIVE)
				tinyxml2::XMLNode* meshMaterialDiffuseNode = levelData.NewElement("DIFFUSE");
				meshMaterialDiffuseNode->ToElement()->SetAttribute("AXIS-X", tempRefferenceForMaterial->GetDiffuseColour().x * 255);
				meshMaterialDiffuseNode->ToElement()->SetAttribute("AXIS-Y", tempRefferenceForMaterial->GetDiffuseColour().y * 255);
				meshMaterialDiffuseNode->ToElement()->SetAttribute("AXIS-Z", tempRefferenceForMaterial->GetDiffuseColour().z * 255);
				meshMaterialNode->InsertEndChild(meshMaterialDiffuseNode);

				tinyxml2::XMLNode* meshMaterialSpecularNode = levelData.NewElement("SPECULAR");
				meshMaterialSpecularNode->ToElement()->SetAttribute("AXIS-X", tempRefferenceForMaterial->GetSpecularColour().x * 255);
				meshMaterialSpecularNode->ToElement()->SetAttribute("AXIS-Y", tempRefferenceForMaterial->GetSpecularColour().y * 255);
				meshMaterialSpecularNode->ToElement()->SetAttribute("AXIS-Z", tempRefferenceForMaterial->GetSpecularColour().z * 255);
				meshMaterialNode->InsertEndChild(meshMaterialSpecularNode);

				tinyxml2::XMLNode* meshMaterialEmissionNode = levelData.NewElement("EMISSION");
				meshMaterialEmissionNode->ToElement()->SetAttribute("AXIS-X", tempRefferenceForMaterial->GetEmissionColour().x * 255);
				meshMaterialEmissionNode->ToElement()->SetAttribute("AXIS-Y", tempRefferenceForMaterial->GetEmissionColour().y * 255);
				meshMaterialEmissionNode->ToElement()->SetAttribute("AXIS-Z", tempRefferenceForMaterial->GetEmissionColour().z * 255);
				meshMaterialNode->InsertEndChild(meshMaterialEmissionNode);

				// Set the shininess of the object
				tinyxml2::XMLNode* meshMaterialShininessNode = levelData.NewElement("SHININESS");
				meshMaterialShininessNode->ToElement()->SetText(std::to_string(tempRefferenceForMaterial->GetShininess()).c_str());
				meshMaterialNode->InsertEndChild(meshMaterialShininessNode);

				// Create the triangle node for the vertices and indices of the object
				tinyxml2::XMLNode* meshTrianglesNode = levelData.NewElement("TRIANGLES");
				meshNode->InsertEndChild(meshTrianglesNode);

				// Create the main node for each vertex
				tinyxml2::XMLNode* meshTriangleVerticesNode = levelData.NewElement("TRIANGLE-VERTICES");
				meshTrianglesNode->InsertEndChild(meshTriangleVerticesNode);

				std::vector<Vertex> tempReferrenceVertices = tempReferrence[m]->GetVertices();
				for (unsigned int v = 0; v < tempReferrenceVertices.size(); v++) {

					// Create the vertex node that stores the data about each vertex in the triangle
					tinyxml2::XMLNode* meshVertexNode = levelData.NewElement("VERTEX");
					meshTriangleVerticesNode->InsertEndChild(meshVertexNode);

					// Set the position of the vertex
					tinyxml2::XMLNode* meshVertexPositionNode = levelData.NewElement("VERTEX-POSITION");
					meshVertexPositionNode->ToElement()->SetAttribute("AXIS-X", tempReferrenceVertices[v].pos.x);
					meshVertexPositionNode->ToElement()->SetAttribute("AXIS-Y", tempReferrenceVertices[v].pos.y);
					meshVertexPositionNode->ToElement()->SetAttribute("AXIS-Z", tempReferrenceVertices[v].pos.z);
					meshVertexNode->InsertEndChild(meshVertexPositionNode);

					// Set the colour of the vertex
					tinyxml2::XMLNode* meshVertexColourNode = levelData.NewElement("VERTEX-COLOUR");
					meshVertexColourNode->ToElement()->SetAttribute("AXIS-X", tempReferrenceVertices[v].colour.r * 255);
					meshVertexColourNode->ToElement()->SetAttribute("AXIS-Y", tempReferrenceVertices[v].colour.g * 255);
					meshVertexColourNode->ToElement()->SetAttribute("AXIS-Z", tempReferrenceVertices[v].colour.b * 255);
					meshVertexNode->InsertEndChild(meshVertexColourNode);

					// Set the normals of the vertex
					tinyxml2::XMLNode* meshVertexNormalsNode = levelData.NewElement("VERTEX-NORMALS");
					meshVertexNormalsNode->ToElement()->SetAttribute("AXIS-X", tempReferrenceVertices[v].normals.x);
					meshVertexNormalsNode->ToElement()->SetAttribute("AXIS-Y", tempReferrenceVertices[v].normals.y);
					meshVertexNormalsNode->ToElement()->SetAttribute("AXIS-Z", tempReferrenceVertices[v].normals.z);
					meshVertexNode->InsertEndChild(meshVertexNormalsNode);

					// Set the texture coord of the vertex
					if (tempReferrenceVertices[v].textureCoord.x != 0.0f || tempReferrenceVertices[v].textureCoord.y != 0.0f) {
						tinyxml2::XMLNode* meshVertexTextureNode = levelData.NewElement("VERTEX-TEXTURE");
						meshVertexTextureNode->ToElement()->SetAttribute("AXIS-X", tempReferrenceVertices[v].textureCoord.x);
						meshVertexTextureNode->ToElement()->SetAttribute("AXIS-Y", tempReferrenceVertices[v].textureCoord.y);
						meshVertexNode->InsertEndChild(meshVertexTextureNode);
					}
				}

				// Export and initialise the attribute for the indices node
				std::string buildIndicesAttribute = "";
				std::vector<unsigned int> tempIndices = tempReferrence[m]->GetIndices();
				for (unsigned int j = 0; j < tempIndices.size(); j++) {
					buildIndicesAttribute += std::to_string(tempIndices[j]);
					if (j < tempIndices.size() - 1)
						buildIndicesAttribute += " ";
				}

				// Set the indices attribute in the triangle indices node
				tinyxml2::XMLNode* meshTriangleIndicesNode = levelData.NewElement("TRIANGLE-INDICES");
				meshTriangleIndicesNode->ToElement()->SetAttribute("INDICES-LIST", buildIndicesAttribute.c_str());
				meshTrianglesNode->InsertEndChild(meshTriangleIndicesNode);
			}
		}
	}

	// Save the exported file
	levelData.SaveFile(buildLevelName.c_str());

	if (levelData.ErrorID() != NULL) {
		std::cout << "ERROR: Level - " << levelParsed << " could not be saved or other problems occured." << std::endl;
		return;
	}
}

void Scene::LevelDataParser(int levelParsed, std::vector<GameObject*>& gameObjectsList) {
	std::string buildLevelName = "Resources/LevelData/Level" + std::to_string(levelParsed) + ".xml";

	tinyxml2::XMLDocument levelData;
	levelData.LoadFile(buildLevelName.c_str());

	if (levelData.ErrorID() != NULL) {
		std::cout << "ERROR: Level - " << levelParsed << " could not be loaded or doesn't exist." << std::endl;
		return;
	}

	tinyxml2::XMLNode* rootNode = levelData.RootElement();

	/* VERSION */
	tinyxml2::XMLNode* levelVersionNode = rootNode->FirstChildElement("VERSION");
	if (std::atoi(levelVersionNode->FirstChild()->Value()) == levelParsed) {

		/* GAME OBJECTS LIST */
		tinyxml2::XMLNode* gameObjectsListNode = rootNode->FirstChildElement("GAMEOBJECTS");
		
		/* EACH OBJECT */
		tinyxml2::XMLNode* objectNode = gameObjectsListNode->FirstChildElement("OBJECT");
		if (objectNode != nullptr) {
			while (objectNode != nullptr) {
				/* ---------- Common Properties for NORMAL and 3D ---------- */

				/* IS ACTIVE */
				tinyxml2::XMLNode* isActiveNode = objectNode->FirstChildElement("ACTIVE");
				bool activeState = std::string(isActiveNode->FirstChild()->Value()) == "TRUE" ? true : false;

				/* OBJECT TYPE */
				tinyxml2::XMLNode* objectTypeNode = objectNode->FirstChildElement("OBJECTTYPE");
				ObjectType objectTypeState = std::string(objectTypeNode->FirstChild()->Value()) == "PLAYER" ? ObjectType::PLAYER : ObjectType::COMMON;

				/* OBJECT MASS */
				tinyxml2::XMLNode* objectMassNode = objectNode->FirstChildElement("OBJECTMASS");
				float objectMass = std::stof(objectMassNode->FirstChild()->Value());

				/* OBJECT PHYSICS STATE */
				tinyxml2::XMLNode* physicsStateNode = objectNode->FirstChildElement("PHYSICSSTATE");
				bool physicsState = std::string(physicsStateNode->FirstChild()->Value()) == "TRUE" ? true : false;

				/* TRANSFORM */
				Transform* newTransform = new Transform();
				tinyxml2::XMLNode* transformNode = objectNode->FirstChildElement("TRANSFORMS");

				/* -- TRANSFORM POSITION */
				tinyxml2::XMLNode* transformPositionNode = transformNode->FirstChildElement("TRANSFORM-POSITION");
				newTransform->SetPos(
					glm::vec3(
						std::stof(transformPositionNode->ToElement()->FindAttribute("AXIS-X")->Value()),
						std::stof(transformPositionNode->ToElement()->FindAttribute("AXIS-Y")->Value()),
						std::stof(transformPositionNode->ToElement()->FindAttribute("AXIS-Z")->Value())
					)
				);

				/* -- TRANSFORM ROTATION */
				tinyxml2::XMLNode* transformRotationNode = transformNode->FirstChildElement("TRANSFORM-ROTATION");
				newTransform->SetRot(
					glm::vec3(
						std::stof(transformRotationNode->ToElement()->FindAttribute("AXIS-X")->Value()),
						std::stof(transformRotationNode->ToElement()->FindAttribute("AXIS-Y")->Value()),
						std::stof(transformRotationNode->ToElement()->FindAttribute("AXIS-Z")->Value())
					)
				);

				/* -- TRANSFORM SCALE */
				tinyxml2::XMLNode* transformScaleNode = transformNode->FirstChildElement("TRANSFORM-SCALE");
				newTransform->SetScale(
					glm::vec3(
						std::stof(transformScaleNode->ToElement()->FindAttribute("AXIS-X")->Value()),
						std::stof(transformScaleNode->ToElement()->FindAttribute("AXIS-Y")->Value()),
						std::stof(transformScaleNode->ToElement()->FindAttribute("AXIS-Z")->Value())
					)
				);

				/* MOVEMENT STATE */
				GLenum moveState;
				moveState = std::string(objectNode->FirstChildElement("MOVESTATE")->FirstChild()->Value()) == "STATIC" ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;

				/* FILE NAME */
				tinyxml2::XMLNode* fileNameNode = objectNode->FirstChildElement("FILENAME");
				std::string meshImportFileName = "";
				if (fileNameNode->FirstChild() != nullptr) {
					meshImportFileName = std::string(fileNameNode->FirstChild()->Value());
				}

				if (meshImportFileName == "") {
					tinyxml2::XMLNode* meshListNode = objectNode->FirstChildElement("MESHLIST");

					std::vector<Mesh*> objectMeshes;
					tinyxml2::XMLNode* meshNode = meshListNode->FirstChildElement("MESH");
					if (meshNode != nullptr) {
						while (meshNode != nullptr) {

							/* MATERIAL */
							tinyxml2::XMLNode* materialNode = meshNode->FirstChildElement("MATERIAL");
							tinyxml2::XMLNode* shaderMaterialNode = materialNode->FirstChildElement("SHADERTYPE");

							/* -- SHADER */
							Shader* meshShader;
							if (std::string(shaderMaterialNode->FirstChild()->Value()) == "EMPTY")
								meshShader = new Shader(ShaderType::EMPTY);
							else if (std::string(shaderMaterialNode->FirstChild()->Value()) == "FLAT")
								meshShader = new Shader(ShaderType::FLAT);
							else if (std::string(shaderMaterialNode->FirstChild()->Value()) == "PHONG")
								meshShader = new Shader(ShaderType::PHONG);
							else
								meshShader = new Shader(ShaderType::EMPTY);

							/* -- TEXTURE LIST */
							tinyxml2::XMLNode* textureListNode = materialNode->FirstChildElement("TEXTURELIST");

							std::vector<Texture*> meshTextures = { AssetManager::s_assetManager->CheckTextureLoaded("Default.jpg", TextureType::DIFFUSE, 1, 1) };
							tinyxml2::XMLNode* textureNode = textureListNode->FirstChildElement("TEXTURE");
							if (textureNode != nullptr) {
								meshTextures.clear();

								while (textureNode != nullptr) {
									tinyxml2::XMLNode* textureTypeNode = textureNode->FirstChildElement("TEXTURETYPE");

									TextureType textureType;
									if (std::string(textureTypeNode->FirstChild()->Value()) == "DIFFUSE")
										textureType = TextureType::DIFFUSE;
									else if (std::string(textureTypeNode->FirstChild()->Value()) == "SPECULAR")
										textureType = TextureType::SPECULAR;
									else if (std::string(textureTypeNode->FirstChild()->Value()) == "NORMAL")
										textureType = TextureType::NORMAL;
									else if (std::string(textureTypeNode->FirstChild()->Value()) == "BUMP")
										textureType = TextureType::BUMP;
									else if (std::string(textureTypeNode->FirstChild()->Value()) == "EMISSIVE")
										textureType = TextureType::EMISSIVE;
									else if (std::string(textureTypeNode->FirstChild()->Value()) == "ROUGHNESS")
										textureType = TextureType::ROUGHNESS;
									else
										textureType = TextureType::UNKNOWN;

									tinyxml2::XMLNode* textureFilePathNode = textureNode->FirstChildElement("TEXTUREFILE");
									std::string filePath = std::string(textureFilePathNode->FirstChild()->Value());

									tinyxml2::XMLNode* textureSizeNode = textureNode->FirstChildElement("TEXTURESIZE");
									int textureSize = std::stoi(textureSizeNode->FirstChild()->Value());

									Texture* newTexture = AssetManager::s_assetManager->CheckTextureLoaded(filePath, textureType, textureSize, textureSize);

									meshTextures.push_back(newTexture);
									textureNode = textureNode->NextSibling();
								}
							}

							/* -- TYPES OF COLOUR*/
							/* ---- DIFFUSE */
							tinyxml2::XMLNode* diffuseColourNode = materialNode->FirstChildElement("DIFFUSE");
							glm::vec3 newDiffuse(
								std::stof(diffuseColourNode->ToElement()->FindAttribute("AXIS-X")->Value()) / 255,
								std::stof(diffuseColourNode->ToElement()->FindAttribute("AXIS-Y")->Value()) / 255,
								std::stof(diffuseColourNode->ToElement()->FindAttribute("AXIS-Z")->Value()) / 255
							);

							/* ---- SPECULAR */
							tinyxml2::XMLNode* specularColourNode = materialNode->FirstChildElement("SPECULAR");
							glm::vec3 newSpecular(
								std::stof(specularColourNode->ToElement()->FindAttribute("AXIS-X")->Value()) / 255,
								std::stof(specularColourNode->ToElement()->FindAttribute("AXIS-Y")->Value()) / 255,
								std::stof(specularColourNode->ToElement()->FindAttribute("AXIS-Z")->Value()) / 255
							);

							/* ---- EMISIVE */
							tinyxml2::XMLNode* emissionColourNode = materialNode->FirstChildElement("EMISSION");
							glm::vec3 newEmission(
								std::stof(emissionColourNode->ToElement()->FindAttribute("AXIS-X")->Value()) / 255,
								std::stof(emissionColourNode->ToElement()->FindAttribute("AXIS-Y")->Value()) / 255,
								std::stof(emissionColourNode->ToElement()->FindAttribute("AXIS-Z")->Value()) / 255
							);

							/* ---- EMISIVE */
							tinyxml2::XMLNode* shininessColourNode = materialNode->FirstChildElement("SHININESS");
							float newShininess = std::stof(shininessColourNode->FirstChild()->Value());

							Material* newMeshMaterial = new Material(meshTextures, meshShader);

							/* TRIANGLES */
							tinyxml2::XMLNode* trianglesListNode = meshNode->FirstChildElement("TRIANGLES");

							/* -- TRIANGLE VERTICES */
							std::vector<Vertex> vertices;
							tinyxml2::XMLNode* vertexNodeData = trianglesListNode->FirstChildElement("TRIANGLE-VERTICES")->FirstChildElement("VERTEX");
							while (vertexNodeData != nullptr) {
								Vertex newVertex;

								tinyxml2::XMLNode* vertexPos = vertexNodeData->FirstChildElement("VERTEX-POSITION");
								newVertex.pos = glm::vec3(
									std::stof(vertexPos->ToElement()->FindAttribute("AXIS-X")->Value()),
									std::stof(vertexPos->ToElement()->FindAttribute("AXIS-Y")->Value()),
									std::stof(vertexPos->ToElement()->FindAttribute("AXIS-Z")->Value())
								);

								tinyxml2::XMLNode* vertexColour = vertexNodeData->FirstChildElement("VERTEX-COLOUR");
								newVertex.colour = glm::vec3(
									std::stof(vertexColour->ToElement()->FindAttribute("AXIS-X")->Value()) / 255,
									std::stof(vertexColour->ToElement()->FindAttribute("AXIS-Y")->Value()) / 255,
									std::stof(vertexColour->ToElement()->FindAttribute("AXIS-Z")->Value()) / 255
								);

								tinyxml2::XMLNode* vertexNormals = vertexNodeData->FirstChildElement("VERTEX-NORMALS");
								newVertex.normals = glm::vec3(
									std::stof(vertexNormals->ToElement()->FindAttribute("AXIS-X")->Value()),
									std::stof(vertexNormals->ToElement()->FindAttribute("AXIS-Y")->Value()),
									std::stof(vertexNormals->ToElement()->FindAttribute("AXIS-Z")->Value())
								);

								tinyxml2::XMLNode* vertexTexture = vertexNodeData->FirstChildElement("VERTEX-TEXTURE");
								if (vertexTexture != nullptr)
									newVertex.textureCoord = glm::vec2(
										std::stof(vertexTexture->ToElement()->FindAttribute("AXIS-X")->Value()),
										std::stof(vertexTexture->ToElement()->FindAttribute("AXIS-Y")->Value())
									);

								vertices.push_back(newVertex);
								vertexNodeData = vertexNodeData->NextSibling();
							}

							/* -- TRIANGLE INDICES */
							std::vector<unsigned int> indices;
							tinyxml2::XMLNode* indicesNodeData = trianglesListNode->FirstChildElement("TRIANGLE-INDICES");

							std::string indicesString = indicesNodeData->ToElement()->FindAttribute("INDICES-LIST")->Value();
							std::stringstream ss(indicesString);
							int temp;
							while (ss >> temp)
								indices.push_back(temp);

							objectMeshes.push_back(new Mesh(newMeshMaterial, vertices, indices, moveState));

							meshNode = meshNode->NextSibling();
						}
					}

					gameObjectsList.push_back(new GameObject(newTransform, objectMeshes, physicsState, objectMass, objectTypeState));
				}
				else {
					/* COLOUR */
					tinyxml2::XMLNode* colour = objectNode->FirstChildElement("COLOUR");

					gameObjectsList.push_back(
						new GameObject(
							newTransform, 
							meshImportFileName, 
							moveState, 
							glm::vec3(
								std::stof(colour->ToElement()->FindAttribute("R")->Value()) / 255,
								std::stof(colour->ToElement()->FindAttribute("G")->Value()) / 255,
								std::stof(colour->ToElement()->FindAttribute("B")->Value()) / 255
							), physicsState, objectMass, objectTypeState
						) 
					);
				}

				objectNode = objectNode->NextSibling();
			}
		}
		else {
			std::cout << "ERROR: No objects present in the scene." << std::endl;
		}
	}
	else {
		std::cout << "ERROR: Level name doesn't match level version." << std::endl;
	}
}

void Scene::DrawScene(const bool& builderActive)
{
	for (std::vector<GameObject*>::size_type i = 0; i != this->gameObjects.size(); i++) {
		for (std::vector<Light*>::size_type j = 0; j != this->lights.size() && this->gameObjects[i]->GetMeshes().size() > 0; j++)
		{
			this->lights[j]->SetUniforms(this->gameObjects[i]->GetMeshById(0)->GetMeshMaterial()->GetShader());
		}

		// If networking type is server don't draw the player entity nor update it
		if (this->gameObjects[i]->GetObjectType() == ObjectType::PLAYER
			&& NetworkEngine::s_networkEngine->GetNetworkType() == NetworkType::SERVER
			&& !builderActive) {
			continue;
		}
		else {
			this->gameObjects[i]->DrawMesh();
		}

		if (!builderActive) {
			this->gameObjects[i]->Update();
		}

		if (this->gameObjects[i]->GetObjectType() == ObjectType::PLAYER 
			&& this->GetSceneFinish() != nullptr) {
			if (this->GetSceneFinish()->GetTransform()->GetPos().z - 30.0f
				<= this->gameObjects[i]->GetTransform()->GetPos().z) {
				EventQueue::s_eventQueue
					->AddEventToQueue(new Event(EventTypes::PLAYER_FINISH, 0));
			}
		}
	}
}

void Scene::HandleEvents(Event& e) {
	for (std::vector<GameObject*>::size_type i = 0; i != this->gameObjects.size(); i++) {
		this->gameObjects[i]->OnEvent(e);
	}
}
