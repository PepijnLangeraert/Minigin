#include "MiniginPCH.h"
#include "ScoreComponent.h"

#include "GameObject.h"
#include "Observer.h"
#include "Subject.h"
#include "TextComponent.h"

void dae::ScoreComponent::Update(float )
{

}

int dae::ScoreComponent::GetScore() const
{
	return m_Score;
}

void dae::ScoreComponent::SetScore(int score)
{
	m_Score = score;

}

void dae::ScoreComponent::IncreaseScore(int diff)
{
	if (diff > 0)
	{
		m_Score += diff;
		m_pParent.lock()->GetSubject()->Notify(m_pParent.lock(), Event::Scored);
	}
}

void dae::ScoreComponent::DecreaseScore(int diff)
{
	if (diff > 0)
	{
		m_Score -= diff;
	}
}

dae::ScoreComponent::ScoreComponent(std::weak_ptr<GameObject> parent)
	:BaseComponent(parent)
{

}