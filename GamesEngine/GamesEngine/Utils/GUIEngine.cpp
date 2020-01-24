#include "GUIEngine.h"
#include "WindowDisplay.h"
#include "../SceneLoader/Scene.h"
#include <ImGUI/examples/imgui_impl_opengl3.cpp>
#include <ImGUI/examples/imgui_impl_glfw.cpp>
#include <iostream>

GUIEngine::GUIEngine(WindowDisplay* graphicEngine) {
	// Initialise the graphic entity
	this->m_graphicEngineEntity = graphicEngine;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(graphicEngine->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 430");

	// Fetch the first version of the files
	this->GUI_LevelBuilder_DataRefresh();
}

GUIEngine::~GUIEngine() {
	this->GUI_LevelBuilder_ClearData();
	ImGui_ImplGlfw_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

void GUIEngine::DrawGUI(bool& displayGUI) {
	if (displayGUI) {
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Menus rendered
		this->GUI_WindowManager(displayGUI);

		// WM Entities
		this->GUI_LevelBuilder();
		this->GUI_ProfilerWindow();

		// -- Hide the entity of the demo window as is not required
		// -- in the actual gameplay
		// this->GUI_DemoWindow();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

void GUIEngine::GUI_WindowManager(bool& displayGUI) {
	ImGui::Begin("WindowManager", &displayGUI);

	/* Helper */
	ImGui::Text("Description:");
	ImGui::Text("\tIn order to close the WindowManager and play the game press \"TAB\"");
	ImGui::Text("\tIf you check one of the boxes below it will disaply another set of windows");
	ImGui::Text("\tAdditional information can be found within those windows");
	ImGui::NewLine();

	ImGui::Checkbox("Profiler window", &this->WM_ProfilerActive);
	ImGui::Checkbox("Level builder window", &this->WM_LevelBuilderActive);
	ImGui::NewLine();
	ImGui::NewLine();
	// -- Hide the entity of the demo window as is not required
	// -- in the actual gameplay
	// ImGui::Checkbox("Demo window", &this->WM_DemoWindowActive);

	ImGui::Text("Main menu:");
	ImGui::Text("\tIn order to be able to start the game you first need to decide whether this");
	ImGui::Text("\tentity will be the client or the host of the current instance of the game.");
	ImGui::Text("\tYou can do that by pressing one of the buttons below.\n\n");
	ImGui::Text("\tIf you want to be the host, you will also need to select the level you want to load from the list");
	ImGui::NewLine();
	ImGui::Combo("Level to be parsed for gameplay", &this->m_currentlySelectedGame, this->LB.LevelListSet.c_str());
	ImGui::Button("Select parsed level", ImVec2(ImGui::GetWindowWidth(), 40.0f));
	if (ImGui::IsItemClicked()) {
		this->m_graphicEngineEntity->SetSceneManager(this->LB.LevelData[m_currentlySelectedGame]);
	}
	ImGui::NewLine();
	ImGui::NewLine();

	ImGui::Button("Create host server", ImVec2(ImGui::GetWindowWidth(), 40.0f));
	if (this->m_graphicEngineEntity->GetSceneManager() != nullptr) {
		if (ImGui::IsItemClicked()) {
			NetworkEngine::s_networkEngine->InitialiseNetworkAdapter(NetworkType::SERVER);
		}
	}

	ImGui::NewLine();
	ImGui::Button("Connect as client to host", ImVec2(ImGui::GetWindowWidth(), 40.0f));
	if (ImGui::IsItemClicked()) {
		NetworkEngine::s_networkEngine->InitialiseNetworkAdapter(NetworkType::CLIENTS);
	}

	ImGui::End();
}

void GUIEngine::GUI_ProfilerWindow() {
	/* Helper */
	if (this->WM_ProfilerActive) {
		ImGui::Begin("Profiler window");

		ImGui::Text("Description:");
		ImGui::Text("\tIn this window it can be viewed the time that each subsystem taxes to run the");
		ImGui::Text("\tprocesses on that specific system.");
		ImGui::Text("\tAlong side the processes for each subsystem, we have the FPS displayed over the");
		ImGui::Text("\ttime that the application runs.");
		ImGui::NewLine();
		ImGui::NewLine();

		ImGui::Checkbox("Pause profiling", &this->PF.PauseProfiling);
		ImGui::NewLine();

		for (WM_SubsystemProfiling system : this->PF.SubSystems) {
			std::string buildLabel = system.SubsystemName
				+ "\n(min: 0 - max: " + std::to_string(system.maxSize) + ")";
			ImGui::PlotHistogram(
				buildLabel.c_str(),
				system.DataSet.data(),
				system.DataSet.size(),
				0, NULL, 0.0f, system.maxSize, ImVec2(0, 80));
			ImGui::NewLine();
		}

		ImGui::End();
	}
}

void GUIEngine::GUI_LevelBuilder() {
	if (this->WM_LevelBuilderActive) {
		ImGui::Begin("Level builder");
		
		/* Helper */
		ImGui::Text("Instructions:");
		ImGui::Text("\tThe level editor is focused on building new levels or changing existing ones");
		ImGui::Text("\tby using dropdowns to create specific objects in specific positions with");
		ImGui::Text("\tspecific properties and previewing the outcome of the created scene and objects");
		ImGui::NewLine();

		ImGui::Text("\tIf you want to fly around the scene and see the outcome, disable the level builder window");
		ImGui::NewLine();

		ImGui::Text("\t*When you delete a level, is important to know that all the other changes that were made");
		ImGui::Text("\tto other level data will be saved and exported");
		ImGui::NewLine();

		/* Level Properties */
		//  Currently not any, attempting to have background sound
		//  And others TO DO

		if (ImGui::BeginTabBar("##LevelEditorTab")) {
			//  Tab that allows the building of the existing levels
			if (ImGui::BeginTabItem("Current levels")) {
				ImGui::NewLine();
				// Button action for the data refresh button
				ImGui::Button("Refresh level data");
				if (ImGui::IsItemClicked()) {
					this->GUI_LevelBuilder_DataRefresh();
				}
				ImGui::NewLine();

				// List of different levels
				ImGui::Combo("Level to edit selection", &this->LB.CurrentLevelEdited, this->LB.LevelListSet.c_str());

				this->LB.ObjectListSet = "";
				//  Sanity check to see whether after the refresh the current level exists
				if (this->LB.CurrentLevelEdited > this->LB.LevelData.size() - 1) {
					this->LB.CurrentLevelEdited = 0;
				}
				// Build op the list of objects from the specific level
				for (int i = 0; i < this->LB.LevelData[this->LB.CurrentLevelEdited]->GetSceneParsedObjects().size(); i++) {
					char tempString[128];
					sprintf(tempString, "Object %d", i + 1);
					this->LB.ObjectListSet += tempString;
					this->LB.ObjectListSet += '\0';
				}
				// Dropdown of available levels in the file
				ImGui::Combo("Edited object in current level", &this->LB.CurrentEditedObject, this->LB.ObjectListSet.c_str());
				ImGui::NewLine();

				// Button action for adding a new object to the currently selected level
				ImGui::Button("Add new object");
				if (ImGui::IsItemClicked()) {
					this->GUI_LevelBuilder_AddObject();
				}

				ImGui::SameLine(); ImGui::Button("Create a new level");
				if (ImGui::IsItemClicked()) {
					this->GUI_LevelBuilder_AddLevel();
				}
				ImGui::NewLine();

				//  Sanity check to see whether after the refresh the current object exists
				if (this->LB.CurrentEditedObject > this->LB.LevelData[this->LB.CurrentLevelEdited]->GetSceneParsedObjects().size() - 1) {
					this->LB.CurrentEditedObject = 0;
				}

				ImGui::Indent();
				// If the level data is empty, don't pass anything to the modifier getting all the default values
				if(this->LB.LevelData[this->LB.CurrentLevelEdited]->GetSceneParsedObjects().size() > 0)
					this->GUI_LevelBuilder_ObjectModifier(this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject));
				ImGui::Unindent();
				ImGui::NewLine();

				// Button action to parse and export the level back into XML
				ImGui::Button("Export changed level data");
				if (ImGui::IsItemClicked()) {
					this->GUI_LevelBuilder_ExportData();
				}

				ImGui::SameLine(); ImGui::Button("Delete currently selected level");
				if (ImGui::IsItemClicked()) {
					this->GUI_LevelBuilder_DeleteLevel();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

void GUIEngine::GUI_LevelBuilder_ScenePreview() {
	// Run update for all in the current selected level game objects.
	this->LB.LevelData[this->LB.CurrentLevelEdited]->DrawScene(this->GetIsBuilderActive());
}

void GUIEngine::GUI_LevelBuilder_DataRefresh() {
	// Revert the changes on data refresh
	this->GUI_LevelBuilder_RevertObject();

	// Clean the data that currently was fetched
	this->GUI_LevelBuilder_ClearData();

	// Get the number of existing levels for builder
	std::string levelSet;
	std::vector<std::string> levelFilePath = Scene::GetLevelFiles();
	for (int i = 0; i < levelFilePath.size(); i++) {
		Scene* newScene =  new Scene(i + 1);

		levelSet += levelFilePath[i];
		levelSet += '\0';
		this->LB.LevelData.push_back(newScene);
	}

	this->LB.LevelListSet = levelSet;
}

void GUIEngine::GUI_LevelBuilder_ExportData(bool exportAll) {
	if (exportAll) {
		for (unsigned int i = 0; i < this->LB.LevelData.size(); i++) {
			this->LB.LevelData[i]->ExportDataParser(i + 1, this->LB.LevelData[i]->GetSceneParsedObjects());
		}
	}
	else {
		this->LB.LevelData[this->LB.CurrentLevelEdited]
			->ExportDataParser(this->LB.CurrentLevelEdited + 1, this->LB.LevelData[this->LB.CurrentLevelEdited]->GetSceneParsedObjects());
	}
}

void GUIEngine::GUI_LevelBuilder_AddLevel() {
	Scene* emptyLevelData = new Scene();
	this->LB.LevelData.push_back(emptyLevelData);

	this->LB.LevelListSet += "Level" + std::to_string(this->LB.LevelData.size()) + ".xml";
	this->LB.LevelListSet += '\0';
}

void GUIEngine::GUI_LevelBuilder_DeleteLevel() {
	this->CheckSelectedWindowLevel();

	// Temporary referrence the same level to be able to rearrange the data
	Scene* tempLevel = this->LB.LevelData[this->LB.CurrentLevelEdited];

	// Erases the object from the array
	this->LB.LevelData.erase(this->LB.LevelData.begin() + this->LB.CurrentLevelEdited);
	this->LB.LevelData.push_back(tempLevel);

	// Regenerate all the levels to rearange the data
	this->GUI_LevelBuilder_ExportData(true);

	// Completly delete the level file
	this->LB.LevelData[this->LB.LevelData.size() - 1]
		->DeleteDataParser(this->LB.LevelData.size());

	// Refresh the data so the deleted level will be removed
	this->GUI_LevelBuilder_DataRefresh();

	// Reverts the object instance back to normal
	this->LB.CurrentLevelEdited = 0;
	this->GUI_LevelBuilder_RevertObject();
}

void GUIEngine::GUI_LevelBuilder_ClearData() {
	this->CheckSelectedWindowLevel();

	for (int i = 0; i < this->LB.LevelData.size(); i++) {
		delete this->LB.LevelData[i];
	}
	this->LB.LevelData.clear();
}

void GUIEngine::GUI_LevelBuilder_AddObject() {
	std::vector<Vertex> newVert;
	std::vector<unsigned int> newInd;

	Transform* newTransform = new Transform();

	Shader* newShader = new Shader(ShaderType::EMPTY);
	Material* newMaterial = new Material(newShader);

	std::vector<Mesh*> newMesh = { new Mesh(newMaterial, newVert, newInd, GL_STATIC_DRAW) };

	// After the initialisation of all the properties, pushes a new default object to the data
	this->LB.LevelData[this->LB.CurrentLevelEdited]->AddObjectToScene(new GameObject(newTransform, newMesh));
}

void GUIEngine::GUI_LevelBuilder_SaveObject() {
	// Set the active state of the object
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetActiveState(this->LB.ObjectState);

	// Set the mass of the object
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetBodyMass(this->LB.ObjectMass);

	// Set the object type state of the object
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetObjectType(GameObject::ConvertIntToType(this->LB.ObjectType));

	// Set the imported file path to the new changed one
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)
		->SetFilePath(this->LB.Imported3DFilePath);
	
	// Set the new position to the transform of the currently saved object
	glm::vec3 newPosition(this->LB.ObjectPosition[0], this->LB.ObjectPosition[1], this->LB.ObjectPosition[2]);
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)
		->GetTransform()->SetPos(newPosition);

	// Set the new rotation to the transform of the currently saved object
	glm::vec3 newRotation(glm::radians(this->LB.ObjectRotation[0]), glm::radians(this->LB.ObjectRotation[1]), glm::radians(this->LB.ObjectRotation[2]));
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)
		->GetTransform()->SetRot(newRotation);

	// Set the new scale to the transform of the currently saved object
	glm::vec3 newScale(this->LB.ObjectScale[0], this->LB.ObjectScale[1], this->LB.ObjectScale[2]);
	this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)
		->GetTransform()->SetScale(newScale);

	if (this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetIsImported()) {
		for (unsigned int i = 0; i < this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetMeshes().size(); i++) {
			delete this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetMeshById(i);
		}

		// Set the colour to the new imported object
		glm::vec3 newColour(this->LB.ObjectColour.x, this->LB.ObjectColour.y, this->LB.ObjectColour.z);
		this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)
			->SetMeshes(AssetManager::s_assetManager->LoadMesh(
				this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetFileName(),
				true, false,
				this->LB.ObjectMovementState == 0 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW,
				newColour));

		// Set the physics enabled state of the object
		this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetPhysicsEnabled(this->LB.ObjectPhysicsState);
	}
	else {
		for (unsigned int i = 0; i < this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetMeshes().size(); i++) {
			delete this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->GetMeshById(i);
		}

		std::vector<Mesh*> newMeshes;
		for (unsigned int i = 0; i < this->LB.NumberOfMeshes; i++) {
			// Initialise the new shader
			Shader* newShader = new Shader(Shader::ConvertIntToType(this->LB.ShaderType[i]));

			// Set and unset the textures from the material
			std::vector<Texture*> newTextures;
			for (unsigned int j = 0; j < this->LB.NumberOfTextures[i]; j++) {
				newTextures.push_back(AssetManager::s_assetManager
					->CheckTextureLoaded(this->LB.TextureFilePath[i][j], Texture::ConvertIntToType(this->LB.TextureType[i][j]), this->LB.TextureSize[i][j], this->LB.TextureSize[i][j]));
			}

			// Set DIFFUSE/SPECULAR/EMISSION/SHININESS of the material
			glm::vec3 newDiffuse(this->LB.MaterialDiffuse[i][0], this->LB.MaterialDiffuse[i][1], this->LB.MaterialDiffuse[i][2]);
			glm::vec3 newSpecular(this->LB.MaterialSpecular[i][0], this->LB.MaterialSpecular[i][1], this->LB.MaterialSpecular[i][2]);
			glm::vec3 newEmission(this->LB.MaterialEmission[i][0], this->LB.MaterialEmission[i][1], this->LB.MaterialEmission[i][2]);
			float newShininess = this->LB.MaterialShininess[i];

			std::vector<Vertex> newVertices;

			// Set the new vertices for the changed model
			for (int j = 0; j < this->LB.NumberOfVertices[i]; j++) {
				glm::vec3 pos(this->LB.VertexPos[i][j][0], this->LB.VertexPos[i][j][1], this->LB.VertexPos[i][j][2]);
				glm::vec3 col(this->LB.VertexColour[i][j][0], this->LB.VertexColour[i][j][1], this->LB.VertexColour[i][j][2]);
				glm::vec3 norm(this->LB.VertexNormal[i][j][0], this->LB.VertexNormal[i][j][1], this->LB.VertexNormal[i][j][2]);
				glm::vec2 uv(this->LB.VertexTexture[i][j][0], this->LB.VertexTexture[i][j][1]);

				newVertices.push_back(Vertex(pos, uv, norm, col));
			}

			std::vector<unsigned int> newIndices;

			// Load the new indices based on the input field
			std::stringstream ss(this->LB.VertexIndices[i]);
			int temp;
			while (ss >> temp)
				newIndices.push_back(temp);

			Material* newMaterial = new Material(newTextures, newShader);
			newMaterial->SetDiffuseColour(newDiffuse);
			newMaterial->SetSpecularColour(newSpecular);
			newMaterial->SetEmissionColour(newEmission);
			newMaterial->SetShininess(newShininess);

			newMeshes.push_back(
				new Mesh(
					newMaterial,
					newVertices,
					newIndices,
					this->LB.ObjectMovementState == 0 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW
				)
			);
		}
		this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetMeshes(newMeshes);

		// Set the physics enabled state of the object
		this->LB.LevelData[this->LB.CurrentLevelEdited]->GetParsedObject(this->LB.CurrentEditedObject)->SetPhysicsEnabled(this->LB.ObjectPhysicsState);
	}
}

void GUIEngine::GUI_LevelBuilder_RevertObject() {
	this->LB.PreviousEditedObject = -1;
	this->LB.PreviousLevelEdited = -1;
}

void GUIEngine::GUI_LevelBuilder_DeleteObject() {
	if (this->LB.LevelData[this->LB.CurrentLevelEdited]->GetSceneParsedObjects().size() > 0) {
		// Clears the memory of the specific object instance 
		// and erases the object from the array
		this->LB.LevelData[this->LB.CurrentLevelEdited]->RemoveObjectById(this->LB.CurrentEditedObject);

		// Reverts the object instance back to normal
		this->LB.CurrentEditedObject = 0;
		this->GUI_LevelBuilder_RevertObject();
	}
}

void GUIEngine::GUI_LevelBuilder_ObjectModifier(GameObject* currentObject) {
	if (currentObject != nullptr && !this->WasEdited()) {
		// Fetch the active state of the object
		this->LB.ObjectState = currentObject->GetIsActive();

		// Fetch the object mass of the object
		this->LB.ObjectMass = currentObject->GetRigidBody()->getMass();

		// Fetch the object type state of the object
		this->LB.ObjectType = currentObject->GetObjectType() == ObjectType::PLAYER ? 0 : 1;

		// Fetch the physics enabled state of the object
		this->LB.ObjectPhysicsState = 
			currentObject->GetCollisionBox() == nullptr && currentObject->GetCollisionSphere() == nullptr ? 0 : 1;

		// Fetch the object transform position of the selected object
		this->LB.ObjectPosition[0] = currentObject->GetTransform()->GetPos().x;
		this->LB.ObjectPosition[1] = currentObject->GetTransform()->GetPos().y;
		this->LB.ObjectPosition[2] = currentObject->GetTransform()->GetPos().z;

		// Fetch the object transform rotation of the selected object
		this->LB.ObjectRotation[0] = glm::degrees(currentObject->GetTransform()->GetRot().x);
		this->LB.ObjectRotation[1] = glm::degrees(currentObject->GetTransform()->GetRot().y);
		this->LB.ObjectRotation[2] = glm::degrees(currentObject->GetTransform()->GetRot().z);

		// Fetch the object transform scale of the selected object
		this->LB.ObjectScale[0] = currentObject->GetTransform()->GetScale().x;
		this->LB.ObjectScale[1] = currentObject->GetTransform()->GetScale().y;
		this->LB.ObjectScale[2] = currentObject->GetTransform()->GetScale().z;

		// Fetch the object movement state of the selected object
		this->LB.ObjectMovementState = currentObject->GetMeshes()[0]->GetMovementState() == GL_STATIC_DRAW ? 0 : 1;

		// Fetch the object file name of the selected object
		this->LB.Imported3DFilePath = currentObject->GetFileName();

		// If the object is an imported one
		if (currentObject->GetIsImported()) {

			// Fetch the object colour of the selected object
			this->LB.ObjectColour.x = currentObject->GetMeshById(0)->GetVertices()[0].colour.r;
			this->LB.ObjectColour.y = currentObject->GetMeshById(0)->GetVertices()[0].colour.g;
			this->LB.ObjectColour.z = currentObject->GetMeshById(0)->GetVertices()[0].colour.b;
		}
		else {
			// Clear the previous that about the textures, material, vertices, triangles and shader
			this->LB.ShaderType.clear();
			this->LB.NumberOfTextures.clear();

			for (int i = 0; i < this->LB.NumberOfMeshes; i++) {
				this->LB.TextureSize[i].clear();
				this->LB.TextureFilePath[i].clear();
				this->LB.TextureType[i].clear();

				for (int j = 0; j < this->LB.NumberOfVertices[i]; j++) {
					delete this->LB.VertexPos[i][j];
					delete this->LB.VertexColour[i][j];
					delete this->LB.VertexNormal[i][j];
					delete this->LB.VertexTexture[i][j];
				}

				this->LB.VertexPos[i].clear();
				this->LB.VertexColour[i].clear();
				this->LB.VertexNormal[i].clear();
				this->LB.VertexTexture[i].clear();

				delete this->LB.MaterialDiffuse[i];
				delete this->LB.MaterialSpecular[i];
				delete this->LB.MaterialEmission[i];
			}
			this->LB.TextureSize.clear();
			this->LB.TextureFilePath.clear();
			this->LB.TextureType.clear();

			this->LB.MaterialDiffuse.clear();
			this->LB.MaterialSpecular.clear();
			this->LB.MaterialEmission.clear();
			this->LB.MaterialShininess.clear();

			this->LB.VertexPos.clear();
			this->LB.VertexColour.clear();
			this->LB.VertexNormal.clear();
			this->LB.VertexTexture.clear();

			this->LB.NumberOfVertices.clear();

			// Fetch the number of meshes in the object
			this->LB.NumberOfMeshes = currentObject->GetMeshes().size();

			// Loop thorugh all the meshes present in the current selected object
			for (int i = 0; i < this->LB.NumberOfMeshes; i++) {
				// Fetch the shader type of the material of a specific Mesh
				this->LB.ShaderType.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetShader()->GetShaderType());

				// Fetch the number of textures in this specific mesh
				this->LB.NumberOfTextures.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetTextures().size());

				// Loop thorugh the textures
				std::vector<int> tempTexturesType;
				std::vector<int> tempTexturesSize;
				std::vector<std::string> tempTexturesPath;

				for (int j = 0; j < this->LB.NumberOfTextures[i]; j++) {
					// Fetch the type of the current texture
					tempTexturesType.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetTextures()[j]->GetTextureType());
					
					// Fetch the size of the current texture
					tempTexturesSize.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetTextures()[j]->GetTextureSize());
					
					// Fetch the filepath of the current texture
					tempTexturesPath.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetTextures()[j]->GetTextureName());
				}

				this->LB.TextureType.push_back(tempTexturesType);
				this->LB.TextureSize.push_back(tempTexturesSize);
				this->LB.TextureFilePath.push_back(tempTexturesPath);

				this->LB.MaterialDiffuse.push_back(
					new float[3]{ 
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetDiffuseColour().x, 
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetDiffuseColour().y, 
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetDiffuseColour().z 
					});
				this->LB.MaterialSpecular.push_back(
					new float[3]{
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetSpecularColour().x,
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetSpecularColour().y,
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetSpecularColour().z
					});
				this->LB.MaterialEmission.push_back(
					new float[3]{
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetEmissionColour().x,
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetEmissionColour().y,
						currentObject->GetMeshById(i)->GetMeshMaterial()->GetEmissionColour().z
					});
				this->LB.MaterialShininess.push_back(currentObject->GetMeshById(i)->GetMeshMaterial()->GetShininess());

				// Fetch the new object's vertices number
				this->LB.NumberOfVertices.push_back(currentObject->GetMeshById(i)->GetVertices().size());
			
				// Fetch the new data about the new vertices
				std::vector<float*> tempPosVert;
				std::vector<float*> tempPosNorm;
				std::vector<float*> tempPosColour;
				std::vector<float*> tempPosText;
				for (int j = 0; j < this->LB.NumberOfVertices[i]; j++) {
					Vertex currentVertex = currentObject->GetMeshById(i)->GetVertices()[j];
					tempPosVert.push_back(new float[3]{ currentVertex.pos.x, currentVertex.pos.y, currentVertex.pos.z });
					tempPosColour.push_back(new float[3]{ currentVertex.colour.r, currentVertex.colour.g, currentVertex.colour.b });
					tempPosNorm.push_back(new float[3]{ currentVertex.normals.x, currentVertex.normals.y, currentVertex.normals.z });
					tempPosText.push_back(new float[2]{ currentVertex.textureCoord.x, currentVertex.textureCoord.y });
				}

				this->LB.VertexPos.push_back(tempPosVert);
				this->LB.VertexNormal.push_back(tempPosNorm);
				this->LB.VertexColour.push_back(tempPosColour);
				this->LB.VertexTexture.push_back(tempPosText);

				std::vector<unsigned int> tempIndices = currentObject->GetMeshById(i)->GetIndices();
				std::string tempIndicesStream = "";
				// Fetch and build the data for the vertices indices
				for (int i = 0; i < tempIndices.size(); i++) {
					tempIndicesStream += std::to_string(tempIndices[i]);
					if (i < tempIndices.size() - 1)
						tempIndicesStream += " ";
				}
				this->LB.VertexIndices.push_back(tempIndicesStream);
			}
		}

		// Set the refference of the previous frame's edited objects
		this->LB.PreviousEditedObject = this->LB.CurrentEditedObject;
		this->LB.PreviousLevelEdited = this->LB.CurrentLevelEdited;
	}

	/* Object Properties */
	if (ImGui::TreeNode("Object properties")) {
		// Set the active state of the object
		ImGui::Combo("Active", &this->LB.ObjectState, "HIDDEN\0VISIBLE\0");

		// Set the object type state of the object
		ImGui::Combo("Object Type", &this->LB.ObjectType, "PLAYER\0COMMON\0");

		// Set the physics enabled state state of the object
		ImGui::Combo("Physics enabled", &this->LB.ObjectPhysicsState, "FALSE\0TRUE\0");

		// Set the object mass of the object
		ImGui::DragFloat("Object mass", &this->LB.ObjectMass);

		// Create the position selector for the edited object
		ImGui::DragFloat3("Position", this->LB.ObjectPosition);
		ImGui::SameLine(); this->HelpMarker("(Properties: float X, float Y, float Z)\nPosition of the object which is used by the Transform\nproperty after drawing the object into the scene.");

		// Create the position selector for the edited object
		ImGui::DragFloat3("Rotation", this->LB.ObjectRotation);
		ImGui::SameLine(); this->HelpMarker("(Properties: float X, float Y, float Z)\Rotation of the object which is used by the Transform\nproperty after drawing the object into the scene.");

		// Create the position selector for the edited object
		ImGui::DragFloat3("Scale", this->LB.ObjectScale);
		ImGui::SameLine(); this->HelpMarker("(Properties: float X, float Y, float Z)\Scale of the object which is used by the Transform\nproperty after drawing the object into the scene.");

		// Create the dropdown for object's state
		ImGui::Combo("Movement State", &this->LB.ObjectMovementState, "STATIC\0DYNAMIC\0");

		// Create the file path input for the 3d model to be imported from
		ImGui::InputText("Model filename", &this->LB.Imported3DFilePath);
		ImGui::SameLine(); this->HelpMarker("Name of the file that will be imported to generate\na 3DMesh from /Resources/3DModels. The default one is teapot_simple_smooth.obj");

		ImGui::TreePop();
	}
	ImGui::NewLine();

	if (this->LB.Imported3DFilePath != "") {
		if (ImGui::TreeNode("Imported 3D Models")) {

			// Create the colour picker for the colour of the imported 3DModel
			ImGui::ColorEdit3("Colour", &this->LB.ObjectColour.x);
			ImGui::TreePop();
		}
	}
	else if (this->LB.Imported3DFilePath == "") {
		if (ImGui::TreeNode("Hardcoded Mesh Models")) {
			// Number of meshes in the object
			ImGui::InputInt("Number of meshes", &this->LB.NumberOfMeshes);

			// Clear the meshes if the number was decreased
			if (this->LB.ShaderType.size() > this->LB.NumberOfMeshes) {
				for (int i = this->LB.NumberOfMeshes; i < this->LB.ShaderType.size(); i++) {
					this->LB.TextureSize[i].clear();
					this->LB.TextureFilePath[i].clear();
					this->LB.TextureType[i].clear();

					delete this->LB.MaterialDiffuse[i];
					delete this->LB.MaterialSpecular[i];
					delete this->LB.MaterialEmission[i];

					for (int j = 0; j < this->LB.VertexPos[i].size(); j++) {
						delete this->LB.VertexPos[i][j];
						delete this->LB.VertexColour[i][j];
						delete this->LB.VertexNormal[i][j];
						delete this->LB.VertexTexture[i][j];
					}

					this->LB.VertexPos[i].clear();
					this->LB.VertexColour[i].clear();
					this->LB.VertexNormal[i].clear();
					this->LB.VertexTexture[i].clear();
				}
				this->LB.ShaderType.resize(this->LB.NumberOfMeshes);
				this->LB.NumberOfTextures.resize(this->LB.NumberOfMeshes);

				this->LB.TextureSize.resize(this->LB.NumberOfMeshes);
				this->LB.TextureFilePath.resize(this->LB.NumberOfMeshes);
				this->LB.TextureType.resize(this->LB.NumberOfMeshes);
				
				this->LB.MaterialDiffuse.resize(this->LB.NumberOfMeshes);
				this->LB.MaterialSpecular.resize(this->LB.NumberOfMeshes);
				this->LB.MaterialEmission.resize(this->LB.NumberOfMeshes);
				this->LB.MaterialShininess.resize(this->LB.NumberOfMeshes);

				this->LB.VertexPos.resize(this->LB.NumberOfMeshes);
				this->LB.VertexColour.resize(this->LB.NumberOfMeshes);
				this->LB.VertexNormal.resize(this->LB.NumberOfMeshes);
				this->LB.VertexTexture.resize(this->LB.NumberOfMeshes);
				this->LB.VertexIndices.resize(this->LB.NumberOfMeshes);
			}

			for (int i = 0; i < this->LB.NumberOfMeshes; i++) {

				// Set up the mesh main properties in case if the size of the list
				// is not as big as the number of created meshes
				if (this->LB.ShaderType.size() < this->LB.NumberOfMeshes) {
					this->LB.ShaderType.push_back(0);
					
					this->LB.NumberOfTextures.push_back(0);
					this->LB.TextureType.push_back(std::vector<int>{});
					this->LB.TextureSize.push_back(std::vector<int>{});
					this->LB.TextureFilePath.push_back(std::vector<std::string>{});

					this->LB.MaterialDiffuse.push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
					this->LB.MaterialSpecular.push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
					this->LB.MaterialEmission.push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
					this->LB.MaterialShininess.push_back(0);

					this->LB.NumberOfVertices.push_back(0);
					this->LB.VertexPos.push_back(std::vector<float*>{});
					this->LB.VertexColour.push_back(std::vector<float*>{});
					this->LB.VertexNormal.push_back(std::vector<float*>{});
					this->LB.VertexTexture.push_back(std::vector<float*>{});
					this->LB.VertexIndices.push_back("");
				}

				char MeshLabel[128];
				sprintf(MeshLabel, "Mesh number %d", i + 1);
				if (ImGui::TreeNode(MeshLabel)) {
					// Shader type of the mesh
					ImGui::Combo("Shader type", &this->LB.ShaderType[i], "EMPTY\0FLAT\0PHONG\0");

					// Number of textures
					ImGui::InputInt("Number of textures", &this->LB.NumberOfTextures[i]);

					// Clear the textures if the number was decreased
					if (this->LB.TextureType[i].size() > this->LB.NumberOfTextures[i]) {
						this->LB.TextureSize[i].resize(this->LB.NumberOfTextures[i]);
						this->LB.TextureFilePath[i].resize(this->LB.NumberOfTextures[i]);
						this->LB.TextureType[i].resize(this->LB.NumberOfTextures[i]);
					}
					
					for (int j = 0; j < this->LB.NumberOfTextures[i]; j++) {

						// Set up the mesh main properties in case if the size of the list
						// is not as big as the number of created meshes
						if (this->LB.TextureType[i].size() < this->LB.NumberOfTextures[i]) {
							this->LB.TextureType[i].push_back(0);
							this->LB.TextureSize[i].push_back(0);
							this->LB.TextureFilePath[i].push_back("");
						}

						char TextureLabel[128];
						sprintf(TextureLabel, "Texture number %d", j + 1);
						if (ImGui::TreeNode(TextureLabel)) {
							// Texture type of the material
							ImGui::Combo("Texture type", &this->LB.TextureType[i][j], "UNKNOWN\0DIFFUSE\0SPECULAR\0SPECULAR\0NORMAL\0BUMP\0EMISSIVE\0ROUGHNESS\0");

							// Texture size of the material
							ImGui::InputInt("Texture size", &this->LB.TextureSize[i][j]);

							// Texture filepath of the material
							ImGui::InputText("Texture file path", &this->LB.TextureFilePath[i][j]);
							ImGui::SameLine(); this->HelpMarker("Name of the texture file that will be picked automatically\nfrom inside /Resources/Textures. The default one is Default.jpg");

							ImGui::TreePop();
						}
					}

					ImGui::ColorEdit3("Material diffuse", &this->LB.MaterialDiffuse[i][0]);
					ImGui::ColorEdit3("Material specular", &this->LB.MaterialSpecular[i][0]);
					ImGui::ColorEdit3("Material emission", &this->LB.MaterialEmission[i][0]);
					ImGui::DragFloat("Material shininess", &this->LB.MaterialShininess[i]);

					// Number of vertices in the mesh
					ImGui::InputInt("Number of vertices", &this->LB.NumberOfVertices[i]);

					// Clear the vertices if the number was decreased
					if (this->LB.VertexPos[i].size() > this->LB.NumberOfVertices[i]) {
						for (int j = this->LB.NumberOfVertices[i]; j < this->LB.VertexPos[i].size(); j++) {
							delete this->LB.VertexPos[i][j];
							delete this->LB.VertexColour[i][j];
							delete this->LB.VertexNormal[i][j];
							delete this->LB.VertexTexture[i][j];
						}
						this->LB.VertexPos[i].resize(this->LB.NumberOfVertices[i]);
						this->LB.VertexColour[i].resize(this->LB.NumberOfVertices[i]);
						this->LB.VertexNormal[i].resize(this->LB.NumberOfVertices[i]);
						this->LB.VertexTexture[i].resize(this->LB.NumberOfVertices[i]);
					}

					// Set up the vertices in case if the size of the list
					// is not as big as the number of created vertices
					for (int j = 0; j < this->LB.NumberOfVertices[i]; j++) {
						if (this->LB.VertexPos[i].size() < this->LB.NumberOfVertices[i]) {
							this->LB.VertexPos[i].push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
							this->LB.VertexColour[i].push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
							this->LB.VertexNormal[i].push_back(new float[3]{ 0.0f, 0.0f, 0.0f });
							this->LB.VertexTexture[i].push_back(new float[2]{ 0.0f, 0.0f });
						}

						char VertexLabel[128];
						sprintf(VertexLabel, "Vertex number %d", j + 1);
						if (ImGui::TreeNode(VertexLabel)) {
							ImGui::DragFloat3("Position", this->LB.VertexPos[i][j]);
							ImGui::SameLine(); this->HelpMarker("(Properties: float X, float Y, float Z)");

							ImGui::ColorEdit3("Colour", this->LB.VertexColour[i][j]);
							ImGui::DragFloat3("Normals", this->LB.VertexNormal[i][j]);
							ImGui::DragFloat2("Texture coord", this->LB.VertexTexture[i][j]);

							ImGui::TreePop();
						}
					}

					if (this->LB.NumberOfVertices[i] > 0) {
						// If the number of vertices is greater than 0, have an input for the indices
						ImGui::InputText("Vertex indices", &this->LB.VertexIndices[i]);
						ImGui::SameLine(); this->HelpMarker("Indices in the order that you want to draw, separated by \"{space/blankspace}\" without any additional spaces.");
					}

					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}
	}

	ImGui::NewLine();
	ImGui::Button("Save");
	if (ImGui::IsItemClicked()) {
		this->GUI_LevelBuilder_SaveObject();
	}

	ImGui::SameLine();  ImGui::Button("Revert");
	if (ImGui::IsItemClicked()) {
		this->GUI_LevelBuilder_RevertObject();
	}

	ImGui::SameLine();  ImGui::Button("Delete");
	if (ImGui::IsItemClicked()) {
		this->GUI_LevelBuilder_DeleteObject();
	}
}

void GUIEngine::GUI_DemoWindow() {
	if (this->WM_DemoWindowActive) {
		ImGui::ShowDemoWindow();
	}
}

void GUIEngine::HelpMarker(const char* desc) {
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void GUIEngine::CheckSelectedWindowLevel() {
	if (this->m_graphicEngineEntity->GetSceneManager() != nullptr &&
		this->m_graphicEngineEntity->GetSceneManager()->GetCurrentLevel() - 1 == this->m_currentlySelectedGame) {
		this->m_graphicEngineEntity->SetSceneManager(nullptr);
	}
}
