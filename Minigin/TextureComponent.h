#pragma once
#include "BaseComponent.h"
#include "TransformComponent.h"

class Font;
class Texture2D;
class TextureComponent final : public BaseComponent
{
public:
	virtual void Update(float );
	//virtual void Render()  override;

	void SetTexture(const std::string& texture);
	void SetPosition(const glm::vec3& trans);
	void Translate(const glm::vec3& trans);
	std::shared_ptr<Texture2D> GetTexture()const;
	virtual void Render()const override;

	TextureComponent(std::weak_ptr<GameObject> parent, const std::string& texture,const glm::vec3 position);
	TextureComponent(std::weak_ptr<GameObject> parent, const std::string& texture);
	virtual ~TextureComponent() = default;
	TextureComponent(const TextureComponent& other) = delete;
	TextureComponent(TextureComponent&& other) = delete;
	TextureComponent& operator=(const TextureComponent& other) = delete;
	TextureComponent& operator=(TextureComponent&& other) = delete;
private:
	std::shared_ptr<Texture2D> m_Texture;
	std::shared_ptr<TransformComponent> m_TransformComponent;
};


