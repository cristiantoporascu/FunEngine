#pragma once
#include "GameObject.h"

class SkyBox {
private:
	GameObject* m_skyBoxObject;

public:
	SkyBox(float size);
	~SkyBox();

	/**
	 * Method that deals with all the bindings in order to draw skybox
	 */
	void DrawSkyBox();

	/**
	 * Getters and setters
	 */

public:
	inline GameObject* GetSkyBoxFaces() const { return this->m_skyBoxObject; }
};