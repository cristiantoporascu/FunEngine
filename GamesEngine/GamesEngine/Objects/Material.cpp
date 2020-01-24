#include "Material.h"

void Material::BindMaterial(const Transform& go_transform)
{
	this->m_shader->BindShader();

	this->m_shader->UpdateShader(go_transform, *Camera::s_camera, 0);

	if (this->m_shader->GetShaderType() == ShaderType::EMPTY) {
		for (unsigned int i = 0; i < this->m_textures.size(); i++)
		{
			this->m_textures[i]->BindTexture(i);
			switch (this->m_textures[i]->GetTextureType())
			{
			case TextureType::DIFFUSE:
				glUniform1f(
					glGetUniformLocation(this->m_shader->GetProgramID(),
						"materialMap.diffuseMap"), i);
				break;
			}
		}
	}
	else if (this->m_shader->GetShaderType() == ShaderType::FLAT)
	{
		glUniform3f(
			glGetUniformLocation(this->m_shader->GetProgramID(),
				"materialProperties.diffuseColour"), this->m_diffuseColour.x, this->m_diffuseColour.y, this->m_diffuseColour.z);

		for (unsigned int i = 0; i < this->m_textures.size(); i++)
		{
			this->m_textures[i]->BindTexture(i);
			switch (this->m_textures[i]->GetTextureType())
			{
			case TextureType::DIFFUSE:
				glUniform1f(
					glGetUniformLocation(this->m_shader->GetProgramID(),
						"materialMap.diffuseMap"), i);
				break;
			}
		}
	}
	else if (this->m_shader->GetShaderType() == ShaderType::PHONG)
	{
		glUniform3f(
			glGetUniformLocation(this->m_shader->GetProgramID(), 
				"material.diffuseColour"), this->m_diffuseColour.x, this->m_diffuseColour.y, this->m_diffuseColour.z);

		glUniform3f(
			glGetUniformLocation(this->m_shader->GetProgramID(),
				"material.specularColour"), this->m_specularColour.x, this->m_specularColour.y, this->m_specularColour.z);

		glUniform3f(
			glGetUniformLocation(this->m_shader->GetProgramID(),
				"material.emissionColour"), this->m_emissionColour.x, this->m_emissionColour.y, this->m_emissionColour.z);

		glUniform1f(
			glGetUniformLocation(this->m_shader->GetProgramID(),
				"material.shininess"), this->m_shininess);

		for (unsigned int i = 0; i < this->m_textures.size(); i++)
		{
			this->m_textures[i]->BindTexture(i);
			switch (this->m_textures[i]->GetTextureType())
			{
			case TextureType::DIFFUSE:
				glUniform1f(
					glGetUniformLocation(this->m_shader->GetProgramID(),
						"material.diffuseMap"), i);
				break;
			case TextureType::SPECULAR:
				glUniform1f(
					glGetUniformLocation(this->m_shader->GetProgramID(),
						"material.specularMap"), i);
				break;
			case TextureType::BUMP:
				glUniform1f(
					glGetUniformLocation(this->m_shader->GetProgramID(),
						"material.bumpMap"), i);
				break;

			default:
				std::cout << "ERROR: The texture could not be loaded" << std::endl;
				break;
			}
		}
	}
	else
	{
		std::cout << "ERROR: Unable to bind the textures! Please check the Shader's Uniforms! Shader Type: {0}", this->m_shader->GetShaderType();
	}
}

void Material::UnbindMaterial()
{
	this->m_shader->UnbindShader();

	for (unsigned int i = 0; i < this->m_textures.size(); i++)
	{
		Texture* texture = this->m_textures[i];
		if (texture)
			texture->UnbindTexture(i);
	}
}