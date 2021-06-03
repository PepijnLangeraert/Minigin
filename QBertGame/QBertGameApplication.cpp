#include "QBertPCH.h"
#include "QBertGameApplication.h"
#include "QbertCommands.h"
#include "GameObject.h"
#include "RenderComponent.h"
#include "SceneManager.h"
#include "TextureComponent.h"
#include "Scene.h"
#include "GridComponent.h"
#include "InputManager.h"
#include "BlockManager.h"
#include "QbertMovement.h"
#include "JsonLevelReader.h"
#include "LevelInfo.h"

void QBertGameApplication::UserLoadGame() const
{
	LevelInfo level;
	JsonLevelReader tet{"Resources/Level.json"};
	tet.ReadFile(level);
	auto& scene = SceneManager::GetInstance().CreateScene("QbertScene");
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	go->AddComponent(std::make_shared <TextureComponent>(go, "background.jpg"));
	go->AddComponent(std::make_shared <RenderComponent>(go));
	scene.Add(go);
	CreateBlocks(scene,level.activeTex, level.inActiveTex, level.blockSize, level.gridSize, level.offset);
	CreateQBert(scene, level.gridSize, level.blockSize, level.posFix, level.offset,QBertMovement{},glm::ivec2{0,6});
	QBertMovement movement{ SDL_SCANCODE_X,SDL_SCANCODE_V,SDL_SCANCODE_W,SDL_SCANCODE_R };
	CreateQBert(scene, level.gridSize, level.blockSize, level.posFix, level.offset, QBertMovement{}, glm::ivec2{ 6,6 });


}

void QBertGameApplication::UserCleanUp()
{
}

void QBertGameApplication::UserUpdate(float)
{

}

void QBertGameApplication::CreateBlocks(Scene& scene, const std::string& activeTex, const std::string& inActiveTex, const glm::vec2& blockSize, const glm::vec2& gridSize, const glm::vec2& offset)const
{


	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	int locationCounter = 0;
	BlockManager::GetInstance().SetActiveTex(activeTex);
	BlockManager::GetInstance().SetInActiveTex(inActiveTex);
	BlockManager::GetInstance().SetCanRevert(true);
	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j <= i; j++)
		{
			if (i % 2 == 1)
			{

				go = std::make_shared<GameObject>();
				auto pos = glm::vec3(offset.x + (j - i / 2) * blockSize.x - blockSize.x / 2, offset.y + i * blockSize.y, 0);
				auto texComp = std::make_shared <TextureComponent>(go, "Cube1.png", pos);
				go->AddComponent(texComp);
				go->AddComponent(std::make_shared <RenderComponent>(go));
				go->AddComponent(std::make_shared <GridComponent>(go, gridSize, blockSize,glm::ivec2{j,i},offset));
				BlockManager::GetInstance().AddBlock(int(j* gridSize.x+i),texComp);
				scene.Add(go);
			}
			else
			{
				go = std::make_shared<GameObject>();
				auto pos = glm::vec3(offset.x + (j - i / 2) * blockSize.x , offset.y + i * blockSize.y, 0);
				auto texComp = std::make_shared <TextureComponent>(go, "Cube1.png", pos);
				go->AddComponent(texComp);
				go->AddComponent(std::make_shared <RenderComponent>(go));
				go->AddComponent(std::make_shared <GridComponent>(go, glm::ivec2{ 7,7 }, blockSize, glm::ivec2{ j,i }, offset));
				BlockManager::GetInstance().AddBlock(int(j* gridSize.x+i),texComp);
				scene.Add(go);
			}
			locationCounter++;
		}
	}
}

std::shared_ptr<GameObject> QBertGameApplication::CreateQBert(Scene& scene,const glm::vec2& gridSize, const glm::vec2& blockSize, const glm::vec3& posFix, const glm::vec2& offset, const QBertMovement& movement, const glm::ivec2& gridLoc) const
{
	std::shared_ptr<GameObject> qbert = std::make_shared<GameObject>();
	qbert = std::make_shared<GameObject>();

	auto gridComp = std::make_shared <GridComponent>(qbert, gridSize, blockSize, gridLoc, offset);
	qbert->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += posFix;
	qbert->AddComponent(std::make_shared <TextureComponent>(qbert, "tempQbert.png", pos));
	qbert->AddComponent(std::make_shared <RenderComponent>(qbert));
	scene.Add(qbert);



	InputManager::GetInstance().AddCommand(movement.LeftDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.RightDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightDown>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.LeftUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.RightUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightUp>(qbert, posFix));

	InputManager::GetInstance().AddCommand(movement.RightUpGamepad, ExecuteType::Pressed, std::make_shared<MoveRightUp>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.LeftDownGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.RightDownGamepad, ExecuteType::Pressed, std::make_shared<MoveRightDown>(qbert, posFix));
	InputManager::GetInstance().AddCommand(movement.LeftUpKeyGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(qbert, posFix));

	return qbert;
}
