#include "Light.h"

void Light::SetUniforms(Shader* shader)
{
	shader->BindShader();

	if (shader->GetShaderType() == ShaderType::PHONG)
	{
		switch (this->m_lightType)
		{
		case LightType::Directional:
		{
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "directionalLight.direction"), this->m_direction.x, this->m_direction.y, this->m_direction.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "directionalLight.ambient"), this->m_ambient.x, this->m_ambient.y, this->m_ambient.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "directionalLight.diffuse"), this->m_diffuse.x, this->m_diffuse.y, this->m_diffuse.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "directionalLight.specular"), this->m_specular.x, this->m_specular.y, this->m_specular.z);
			break;
		}
		case LightType::Point:
		{
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "pointLight.position"), this->m_position.x, this->m_position.y, this->m_position.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "pointLight.ambient"), this->m_ambient.x, this->m_ambient.y, this->m_ambient.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "pointLight.diffuse"), this->m_diffuse.x, this->m_diffuse.y, this->m_diffuse.z);
			glUniform3f(glGetUniformLocation(shader->GetProgramID(), "pointLight.specular"), this->m_specular.x, this->m_specular.y, this->m_specular.z);
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "pointLight.intensity"), this->m_intensity);
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "pointLight.constant"), this->m_kc);
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "pointLight.linear"), this->m_kl);
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), "pointLight.quadratic"), this->m_kq);
			break;
		}
		default:
			break;
		}
	}
	shader->UnbindShader();
}
