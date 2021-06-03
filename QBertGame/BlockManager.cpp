#include "QBertPCH.h"
#include "BlockManager.h"
#include "GameObject.h"
#include "TextureComponent.h"

void BlockManager::AddBlock(int index,std::shared_ptr<TextureComponent>block)
{
	m_Blocks.insert(std::pair<int,std::shared_ptr<TextureComponent>>(index, block));
	m_BlockChanged.insert(std::pair<int,bool>(index, false));
}

void BlockManager::ChangeBlock(int index)
{
	auto temp = m_Blocks.find(index);
	if (temp != m_Blocks.end())
	{
		if (!m_BlockChanged[index])
		{
			m_Blocks[index]->SetTexture(m_TexActive);
			m_BlockChanged[index] = true;
		}
		else if (m_CanRevert)
		{
			m_Blocks[index]->SetTexture(m_TexInActive);
			m_BlockChanged[index] = !m_BlockChanged[index];
		}

	}
}

void BlockManager::SetCanRevert(bool canRevert)
{
	m_CanRevert = canRevert;
}

void BlockManager::SetActiveTex(const std::string& newTex)
{
	m_TexActive = newTex;
}

void BlockManager::SetInActiveTex(const std::string& newTex)
{
	m_TexInActive = newTex;
}