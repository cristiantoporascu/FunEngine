#include "SkyBox.h"

SkyBox::SkyBox(float size) {
	Transform* newSkyBoxTransform = new Transform(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	std::vector<unsigned int> indices = {
		0, 3, 1,
		1, 3, 2
	};

	std::vector<Vertex> verticesFRONT =
	{
		Vertex(glm::vec3(size, -size, size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, size, size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, -size, size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0))
	};

	std::vector<Vertex> verticesBACK =
	{
		Vertex(glm::vec3(-size, -size, -size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, -size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, size, -size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, -size, -size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0))
	};

	std::vector<Vertex> verticesTOP =
	{
		Vertex(glm::vec3(size, size, size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, size, -size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, -size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0))
	};

	std::vector<Vertex> verticesBOTTOM =
	{
		Vertex(glm::vec3(size, -size, -size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, -size, size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, -size, size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, -size, -size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
	};

	std::vector<Vertex> verticesRIGHT =
	{
		Vertex(glm::vec3(size, -size, -size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, size, -size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, size, size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(size, -size, size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0))
	};

	std::vector<Vertex> verticesLEFT =
	{
		Vertex(glm::vec3(-size, -size, size), glm::vec2(0.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, size), glm::vec2(0.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, size, -size), glm::vec2(1.0,0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0)),
		Vertex(glm::vec3(-size, -size, -size), glm::vec2(1.0,1.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1.0,1.0,1.0), glm::vec3(0.0,0.0,0.0),glm::vec3(0.0,0.0,0.0))
	};

	this->m_skyBoxObject = 
		new GameObject(
			newSkyBoxTransform,
			std::vector<Mesh*> {
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Front.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesFRONT,
					indices,
					GL_DYNAMIC_DRAW
				),
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Back.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesBACK,
					indices,
					GL_DYNAMIC_DRAW
				),
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Top.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesTOP,
					indices,
					GL_DYNAMIC_DRAW
				),
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Bottom.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesBOTTOM,
					indices,
					GL_DYNAMIC_DRAW
				),
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Right.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesRIGHT,
					indices,
					GL_DYNAMIC_DRAW
				),
				new Mesh(
					new Material(
						std::vector<Texture*>{new Texture("SkyBox/Left.bmp", TextureType::DIFFUSE, 2048, 2048)},
						new Shader(ShaderType::FLAT)),
					verticesLEFT,
					indices,
					GL_DYNAMIC_DRAW
				)
			},
			false
		);
}

SkyBox::~SkyBox() {
	delete this->m_skyBoxObject;
}

void SkyBox::DrawSkyBox() {
	glm::vec3 currentCamPos = Camera::s_camera->GetCurrentActiveTransform()->GetPos();
	if (this->m_skyBoxObject->GetTransform()->GetPos() 
		!= currentCamPos) {
		this->m_skyBoxObject->GetTransform()->SetPos(currentCamPos);
	}
	this->m_skyBoxObject->DrawMesh();
}