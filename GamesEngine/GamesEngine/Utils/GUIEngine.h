#pragma once
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <ImGUI/imgui.h>
#include <ImGUI/examples/imgui_impl_opengl3.h>
#include <ImGUI/examples/imgui_impl_glfw.h>
#include "../ExtensionDep/ImGUI/imgui_stdlib.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>

// Forward declarations
class WindowDisplay;
class Scene;
class GameObject;

/**
 * Level Builder properties
 */
struct WM_LevelBuilder {
	std::vector<Scene*> LevelData;

	int ObjectState = 1;
	int ObjectPhysicsState = 1;
	int ObjectType = 1;
	float ObjectMass = 0.0;

	std::string LevelListSet;
	int CurrentLevelEdited = 0;
	int PreviousLevelEdited = -1;

	std::string ObjectListSet;
	int CurrentEditedObject = 0;
	int PreviousEditedObject = -1;

	std::string Imported3DFilePath = "teapot_simple_smooth.obj";
	ImVec4 ObjectColour = { 1.0f, 1.0f, 1.0f, 1.0f };

	int ObjectMovementState = 0;
	float ObjectPosition[3] = { 0.0f, 0.0f, 0.0f };
	float ObjectRotation[3] = { 0.0f, 0.0f, 0.0f };
	float ObjectScale[3] = { 0.0f, 0.0f, 0.0f };

	int NumberOfMeshes = 0;
	std::vector<int> ShaderType;
	std::vector<int> NumberOfTextures;
	std::vector<std::vector<int>> TextureType;
	std::vector<std::vector<int>> TextureSize;
	std::vector<std::vector<std::string>> TextureFilePath;
	std::vector<float*> MaterialDiffuse;
	std::vector<float*> MaterialSpecular;
	std::vector<float*> MaterialEmission;
	std::vector<float> MaterialShininess;

	std::vector<int> NumberOfVertices;
	std::vector<std::vector<float*>> VertexPos;
	std::vector<std::vector<float*>> VertexColour;
	std::vector<std::vector<float*>> VertexNormal;
	std::vector<std::vector<float*>> VertexTexture;
	std::vector<std::string> VertexIndices;
};

struct WM_SubsystemProfiling {
	std::string SubsystemName;
	float maxSize;
	std::vector<float> DataSet;
};

struct WM_Profiler {
	bool PauseProfiling = false;
	std::vector<WM_SubsystemProfiling> SubSystems;
};

class GUIEngine {
private:
	WindowDisplay* m_graphicEngineEntity;

	/**
	 * Window Manager variables
	 */
	bool WM_DemoWindowActive = false;
	bool WM_ProfilerActive = false;
	bool WM_LevelBuilderActive = false;

	WM_LevelBuilder LB;
	WM_Profiler PF;

	int m_currentlySelectedGame = 0;

public:
	GUIEngine(WindowDisplay* graphicEngine);
	~GUIEngine();

	/**
	 * Renders every frame the GUI that was previously initialised
	 * in the constructor
	 * @param displayGUI				Describes whether any GUIEditor element should be visible
	 */
	void DrawGUI(bool& displayGUI);

	/**
	 * Defines the windows that can be oppened in your levelEditor
	 * @param displayGUI				Describes whether any GUIEditor element should be visible
	 */
	void GUI_WindowManager(bool& displayGUI);

	/**
	 * Defines and draw the profiling for each individual subsystem in a nice line chart
	 */
	void GUI_ProfilerWindow();

	/**
	 * Defines the levelBuilder window and with its properties
	 */
	void GUI_LevelBuilder();

	/**
	 * Draw level builder's selected level scene preview
	 */
	void GUI_LevelBuilder_ScenePreview();

	/**
	 * Refresh the data that the level builder uses from the
	 * level files ( Level1.xml )
	 */
	void GUI_LevelBuilder_DataRefresh();

	/**
	 * Export the data that was collected and changed for a specific level
	 * it will generate the XML file based on the parser found in Scene.cpp
	 * @param exportAll						Export all the level data to refresh all the files
	 */
	void GUI_LevelBuilder_ExportData(bool exportAll = false);

	/**
	 * Adds a new level to be created to the level data
	 */
	void GUI_LevelBuilder_AddLevel();

	/**
	 * Deletes all refrrences to the currently selected level data
	 */
	void GUI_LevelBuilder_DeleteLevel();

	/**
	 * Clear the data of the objects
	 */
	void GUI_LevelBuilder_ClearData();

	/**
	 * Add an object to the currently selected level
	 */
	void GUI_LevelBuilder_AddObject();

	/**
	 * Delete an object that's currently present in the level
	 */
	void GUI_LevelBuilder_DeleteObject();

	/**
	 * Revert the properties that were changed back to the normal ones
	 */
	void GUI_LevelBuilder_RevertObject();

	/**
	 * Save the changes that were done to the object
	 */
	void GUI_LevelBuilder_SaveObject();

	/**
	 * Object properties modifier
	 * 
	 * @empty							For new levels
	 *		AND
	 * @param gameObject				For existing gameObjects in the current selected level
	 */
	void GUI_LevelBuilder_ObjectModifier(GameObject* currentObject = nullptr);

	/**
	 * Defines the demo window to have an idea of how to build it
	 */
	void GUI_DemoWindow();

	/**
	 * Helper to display a little (?) mark which shows a tooltip when hovered.
	 */
	void HelpMarker(const char* desc);

	/**
	 * Check if the selected level is currently the deleted level
	 */
	void CheckSelectedWindowLevel();

	/**
	 * Check if the current edited object and level were the previous ones
	 * @return bool					Returns true if the element that's currently 
	 *								edited has been updated already
	 */
	bool WasEdited() { 
		return this->LB.CurrentEditedObject
			== this->LB.PreviousEditedObject
			&& this->LB.CurrentLevelEdited
			== this->LB.PreviousLevelEdited;
	}

	/**
	 * Getters and setters
	 */
public:
	inline const bool& GetIsBuilderActive() const { return this->WM_LevelBuilderActive; }
	inline const bool& GetIsProfilerActive() const { return this->WM_ProfilerActive; }
	inline Scene* GetLevelDataById(const int& levelSelected) const { return this->LB.LevelData[levelSelected]; }
	inline const WM_Profiler& GetProfilerManager() const { return this->PF; }
	inline const std::vector<float>& GetProfilerDataSetByName(const std::string& systemName) {
		for (WM_SubsystemProfiling system : this->PF.SubSystems) {
			if (system.SubsystemName == systemName) {
				return system.DataSet;
			}
		}
	}

	inline void PushProfilerDataSetByName(const std::string& systemName, const float& dataSet) {
		for (unsigned int i = 0; i < this->PF.SubSystems.size(); i++) {
			if (this->PF.SubSystems[i].SubsystemName == systemName) {
				if (this->PF.SubSystems[i].DataSet.size() > 50) {
					this->PF.SubSystems[i].DataSet.erase(this->PF.SubSystems[i].DataSet.begin());
				}
				if (!this->PF.PauseProfiling) {
					this->PF.SubSystems[i].DataSet.push_back(dataSet);
				}
				break;
			}
		}
	}
	inline void PushSystemProfiler(const WM_SubsystemProfiling& newSystemPF) { this->PF.SubSystems.push_back(newSystemPF); }
};