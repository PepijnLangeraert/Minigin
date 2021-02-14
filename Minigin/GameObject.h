#pragma once
#include "Transform.h"
#include "SceneObject.h"
#include "BaseComponent.h"

namespace dae
{
	class Texture2D;
	class GameObject final: public SceneObject
	{
	public:
		


		virtual void Update(float deltaTime)override;
		virtual void Render()  override ;
	
		template <typename T>
		std::shared_ptr <T> GetComponent()const;

		const std::vector<std::shared_ptr<BaseComponent>>& GetComponents();

		std::shared_ptr<BaseComponent> AddComponent(std::shared_ptr<BaseComponent> newComponent);


		GameObject() = default;
		virtual ~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		

	private:
		std::vector<std::shared_ptr<BaseComponent>> m_Components{};
	};


	template<typename T>
	inline std::shared_ptr <T> GameObject::GetComponent() const
	{
		for (std::shared_ptr<BaseComponent> component : m_Components)
		{
			//auto temp = dynamic_cast<T*>(component.get()) ;
			auto temp = std::dynamic_pointer_cast<T>(component) ;
			if (temp!=nullptr)
			{
				return temp;
			}
		}
		return nullptr;
	}

}

