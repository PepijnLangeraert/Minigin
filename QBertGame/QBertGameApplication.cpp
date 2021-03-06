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
#include "HealthComponent.h"
#include "HudManager.h"
#include "HudRenderComponent.h"
#include "CharacterMovement.h"
#include "JsonLevelReader.h"
#include "LevelInfo.h"
#include "HudTextComponent.h"
#include "CoilyMoveComponent.h"
#include "QbertHealthObserver.h"
#include "QbertObserver.h"
#include "ResourceManager.h"
#include "SubjectComponent.h"
#include "TextComponent.h"
#include "ScoreComponent.h"
#include "SlicknSamMoveComponent.h"
#include "UggnWrongWayMoveComponent.h"
#include "QBertMoveComponent.h"
#include "QBertImguiRenderer.h"
#include "SDLMixerSoundSystem.h"

void QBertGameApplication::LoadNextLevel()
{
	InputManager::GetInstance().RefreshControllerCount();
	m_CurrentLevelIdx++;
	if (size_t(m_CurrentLevelIdx) >= m_Levels.size())
	{
		Quit();
		return;
	}
	m_pQBert1->GetComponent<ScoreComponent>()->IncreaseScore(50);
	auto& blockManager = BlockManager::GetInstance();
	blockManager.Clear();

	auto& scene = SceneManager::GetInstance().CreateScene("QbertScene" + std::to_string(m_Levels[m_CurrentLevelIdx].levelNo));
	
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	go->AddComponent(std::make_shared <TextureComponent>(go, "background.jpg"));
	go->AddComponent(std::make_shared <RenderComponent>(go));
	scene.Add(go);

	blockManager.SetCanRevert(m_Levels[m_CurrentLevelIdx].canRevert);
	blockManager.SetNeedsDoubleTouch(m_Levels[m_CurrentLevelIdx].needsDoubleTouch);
	blockManager.SetActiveTex(m_Levels[m_CurrentLevelIdx].activeTex);
	blockManager.SetInActiveTex(m_Levels[m_CurrentLevelIdx].inActiveTex);
	blockManager.SetInBetweenTex(m_Levels[m_CurrentLevelIdx].InBetweenTex);
	CreateBlocks(scene);

	auto gridComp = m_pQBert1->GetComponent<GridComponent>();
	gridComp->ResetGridLocation();
	auto pos = gridComp->CalcGridPos();
	m_pQBert1->GetComponent<TextureComponent>()->SetPosition(pos + m_Levels[m_CurrentLevelIdx].posFix);
	scene.Add(m_pQBert1);

	switch (m_GameMode)
	{
	case GameMode::SinglePlayer:
	{

		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	case GameMode::MultiPlayerCoop:
	{
		auto gridComp2 = m_pQBert2->GetComponent<GridComponent>();
		gridComp2->ResetGridLocation();
		auto pos2 = gridComp2->CalcGridPos();
		m_pQBert2->GetComponent<TextureComponent>()->SetPosition(pos2 + m_Levels[m_CurrentLevelIdx].posFix);
		scene.Add(m_pQBert2);
		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	case GameMode::MultiPlayerVS:
	{
		const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
		auto coily =CreateCoily(scene, movement, glm::ivec2{ rand() % 2,1 },1);
		m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	}


	

}

void QBertGameApplication::ReloadLevel()
{
	auto& blockManager = BlockManager::GetInstance();
	blockManager.Clear();
	InputManager::GetInstance().RefreshControllerCount();
	m_pQBert1->GetComponent<HealthComponent>()->ResetHealth();
	m_pQBert1->GetComponent<ScoreComponent>()->SetScore(0);
	auto& scene = SceneManager::GetInstance().CreateScene("QbertScene" + std::to_string(m_Levels[m_CurrentLevelIdx].levelNo));

	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	go->AddComponent(std::make_shared <TextureComponent>(go, "background.jpg"));
	go->AddComponent(std::make_shared <RenderComponent>(go));
	scene.Add(go);


	
	blockManager.SetCanRevert(m_Levels[m_CurrentLevelIdx].canRevert);
	blockManager.SetNeedsDoubleTouch(m_Levels[m_CurrentLevelIdx].needsDoubleTouch);
	blockManager.SetActiveTex(m_Levels[m_CurrentLevelIdx].activeTex);
	blockManager.SetInActiveTex(m_Levels[m_CurrentLevelIdx].inActiveTex);
	blockManager.SetInBetweenTex(m_Levels[m_CurrentLevelIdx].InBetweenTex);
	CreateBlocks(scene);
	auto gridComp = m_pQBert1->GetComponent<GridComponent>();
	gridComp->ResetGridLocation();
	auto pos = gridComp->CalcGridPos();
	m_pQBert1->GetComponent<TextureComponent>()->SetPosition(pos + m_Levels[m_CurrentLevelIdx].posFix);
	scene.Add(m_pQBert1);
	auto txts =m_pHudObj->GetComponentsType<TextComponent>();
	for (size_t i = 0; i < txts.size(); ++i)
	{
		txts[i]->SetText(std::to_string(i * 3));
	}
	scene.Add(m_pHudObj);
	switch (m_GameMode)
	{
	case GameMode::SinglePlayer:
	{

		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	case GameMode::MultiPlayerCoop:
	{
		auto gridComp2 = m_pQBert2->GetComponent<GridComponent>();
		gridComp2->ResetGridLocation();
		auto pos2 = gridComp2->CalcGridPos();
		m_pQBert2->GetComponent<TextureComponent>()->SetPosition(pos2 + m_Levels[m_CurrentLevelIdx].posFix);
		scene.Add(m_pQBert2);
		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
	
		break;
	}
	case GameMode::MultiPlayerVS:
	{
		const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
		auto coily = CreateCoily(scene, movement, glm::ivec2{ rand() % 2,1 },1);
		m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	}
	
}

void QBertGameApplication::ResetLevel()
{
	InputManager::GetInstance().RefreshControllerCount();
	auto& blockManager = BlockManager::GetInstance();
	blockManager.Clear();

	m_pQBert1->GetComponent<HealthComponent>()->ResetHealth();
	m_pQBert1->GetComponent<ScoreComponent>()->SetScore(0);

	auto& scene = SceneManager::GetInstance().CreateScene("QbertScene" + std::to_string(m_Levels[m_CurrentLevelIdx].levelNo));

	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	go->AddComponent(std::make_shared <TextureComponent>(go, "background.jpg"));
	go->AddComponent(std::make_shared <RenderComponent>(go));
	scene.Add(go);



	blockManager.SetCanRevert(m_Levels[m_CurrentLevelIdx].canRevert);
	blockManager.SetNeedsDoubleTouch(m_Levels[m_CurrentLevelIdx].needsDoubleTouch);
	blockManager.SetActiveTex(m_Levels[m_CurrentLevelIdx].activeTex);
	blockManager.SetInActiveTex(m_Levels[m_CurrentLevelIdx].inActiveTex);
	blockManager.SetInBetweenTex(m_Levels[m_CurrentLevelIdx].InBetweenTex);
	CreateBlocks(scene);

	auto txts = m_pHudObj->GetComponentsType<TextComponent>();
	for (size_t i = 0; i < txts.size(); ++i)
	{
		txts[i]->SetText(std::to_string(i * 3));
	}
	scene.Add(m_pHudObj);
	switch (m_GameMode)
	{
	case GameMode::SinglePlayer:
	{
		auto gridComp = m_pQBert1->GetComponent<GridComponent>();
		gridComp->SetGridLocation({0,0});
		gridComp->SetInitialGridLocation({ 0,0 });
		auto pos = gridComp->CalcGridPos();
		m_pQBert1->GetComponent<TextureComponent>()->SetPosition(pos + m_Levels[m_CurrentLevelIdx].posFix);
		scene.Add(m_pQBert1);
		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	case GameMode::MultiPlayerCoop:
	{
		auto gridComp = m_pQBert1->GetComponent<GridComponent>();
		gridComp->SetGridLocation({ 6,6 });
		gridComp->SetInitialGridLocation({ 6,6 });
		auto pos = gridComp->CalcGridPos();
		m_pQBert1->GetComponent<TextureComponent>()->SetPosition(pos + m_Levels[m_CurrentLevelIdx].posFix);
		scene.Add(m_pQBert1);
		if (m_pQBert2.get())
		{

			auto gridComp2 = m_pQBert2->GetComponent<GridComponent>();
			gridComp2->ResetGridLocation();
			auto pos2 = gridComp2->CalcGridPos();
			m_pQBert2->GetComponent<TextureComponent>()->SetPosition(pos2 + m_Levels[m_CurrentLevelIdx].posFix);
			scene.Add(m_pQBert2);
			const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
			AddCharacterControls(movement, m_pQBert2, 1);
		}
		else
		{
			const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
			m_pQBert2 = CreateQBert(scene, movement, glm::ivec2{ 0,6 }, 1);

			auto subjComp2 = std::make_shared <SubjectComponent>(m_pQBert2);
			auto healthComp = m_pQBert1->GetComponent<HealthComponent>();
			auto scoreComp = m_pQBert1->GetComponent<ScoreComponent>();
			auto gridComp2 = m_pQBert2->GetComponent<GridComponent>();
			gridComp2->AddHealthComp(healthComp);
			gridComp2->AddScoreComp(scoreComp);
			m_pQBert2->AddComponent(healthComp);
			m_pQBert2->AddComponent(scoreComp);
			m_pQBert2->AddComponent(subjComp2);

		}
		std::shared_ptr<GameObject>coily;
		if (m_Levels[m_CurrentLevelIdx].hasCoily)
		{
			coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		}
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);

		break;
	}
	case GameMode::MultiPlayerVS:
	{
		auto gridComp = m_pQBert1->GetComponent<GridComponent>();
		gridComp->SetGridLocation({ 0,0 });
		gridComp->SetInitialGridLocation({ 0,0 });
		auto pos = gridComp->CalcGridPos();
		m_pQBert1->GetComponent<TextureComponent>()->SetPosition(pos + m_Levels[m_CurrentLevelIdx].posFix);
		scene.Add(m_pQBert1);
		const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
		auto coily = CreateCoily(scene, movement, glm::ivec2{ rand() % 2,1 }, 1);
		m_pQBert1->GetComponent<SubjectComponent>()->AddObserver(coily->GetComponent<ObserverComponent>());
		if (m_Levels[m_CurrentLevelIdx].hasSam)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasSlick)
			CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
		if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
			CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
		if (m_Levels[m_CurrentLevelIdx].hasUgg)
			CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
		break;
	}
	}
}

void QBertGameApplication::AddCharacterControls(const CharacterMovement& movement, std::shared_ptr<GameObject> object, unsigned long playerID)
{
	auto& inputManager = InputManager::GetInstance();
	inputManager.AddCommand(movement.LeftDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(object));
	inputManager.AddCommand(movement.RightDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightDown>(object));
	inputManager.AddCommand(movement.LeftUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(object));
	inputManager.AddCommand(movement.RightUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightUp>(object));

	inputManager.AddCommand(movement.RightUpGamepad, ExecuteType::Pressed, std::make_shared<MoveRightUp>(object), playerID);
	inputManager.AddCommand(movement.LeftDownGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(object), playerID);
	inputManager.AddCommand(movement.RightDownGamepad, ExecuteType::Pressed, std::make_shared<MoveRightDown>(object), playerID);
	inputManager.AddCommand(movement.LeftUpKeyGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(object), playerID);

}

void QBertGameApplication::UserInitialize()
{
	std::srand(unsigned int(time(NULL)));
	m_pImguiRenderer = &QBertImguiRenderer::GetInstance();
	m_pImguiRenderer->Init(m_Window);
}

void QBertGameApplication::UserLoadGame()
{
	ServiceLocator::RegisterSoundSystem(new SDLMixerSoundSystem{ false });
	SDLMixerSoundSystem* temp = static_cast<SDLMixerSoundSystem*>(ServiceLocator::GetSoundSystem());
	temp->AddSound("Resources/jumpqBert.wav");
	temp->AddSound("Resources/jumpEnemy.wav");
	temp->AddSound("Resources/BackGroundMusic.wav");


	const auto soundSystem = ServiceLocator::GetSoundSystem();
	if (soundSystem)
	{
		soundSystem->Play(2, 25.f);
	}
	
	m_GameMode = GameMode::SinglePlayer;
	auto& blockManager = BlockManager::GetInstance();
	JsonLevelReader jsonReader{ "Resources/Level.json" };
	jsonReader.ReadFile(m_Levels);
	blockManager.LinkLevelDone(m_LevelIsDone);

	auto& scene = SceneManager::GetInstance().CreateScene("QbertScene" + std::to_string(m_Levels[m_CurrentLevelIdx].levelNo));
	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();
	go->AddComponent(std::make_shared <TextureComponent>(go, "background.jpg"));
	go->AddComponent(std::make_shared <RenderComponent>(go));
	scene.Add(go);
	blockManager.SetCanRevert(m_Levels[m_CurrentLevelIdx].canRevert);
	blockManager.SetNeedsDoubleTouch(m_Levels[m_CurrentLevelIdx].needsDoubleTouch);
	blockManager.SetActiveTex(m_Levels[m_CurrentLevelIdx].activeTex);
	blockManager.SetInActiveTex(m_Levels[m_CurrentLevelIdx].inActiveTex);
	blockManager.SetInBetweenTex(m_Levels[m_CurrentLevelIdx].InBetweenTex);
	CreateBlocks(scene);
	std::shared_ptr<GameObject> coily = nullptr;
	switch (m_GameMode)
	{
		case GameMode::SinglePlayer:
		{

			m_pQBert1 = CreateQBert(scene,  CharacterMovement{}, glm::ivec2{ 0,0 },0);
			if(m_Levels[m_CurrentLevelIdx].hasCoily)
				coily=CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			if (m_Levels[m_CurrentLevelIdx].hasSam)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasSlick)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
				CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 },true);
			if (m_Levels[m_CurrentLevelIdx].hasUgg)
				CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 },false);
			break;
		}
		case GameMode::MultiPlayerCoop:
		{

			m_pQBert1 = CreateQBert(scene, CharacterMovement{}, glm::ivec2{ 6,6 },0);
			const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
			m_pQBert2 = CreateQBert(scene,  movement, glm::ivec2{ 0,6 },1);
			if (m_Levels[m_CurrentLevelIdx].hasCoily)
				coily = CreateCoily(scene, glm::ivec2{ rand() % 2,1 });
			if (m_Levels[m_CurrentLevelIdx].hasSam)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasSlick)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
				CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
			if (m_Levels[m_CurrentLevelIdx].hasUgg)
				CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
			break;
		}
		case GameMode::MultiPlayerVS:
		{

			m_pQBert1 = CreateQBert(scene, CharacterMovement{}, glm::ivec2{ 0,0 },0);
			const CharacterMovement movement{ SDL_SCANCODE_S,SDL_SCANCODE_D,SDL_SCANCODE_A,SDL_SCANCODE_W };
			coily = CreateCoily(scene,movement, glm::ivec2{ rand() % 2,1 },1);
			if (m_Levels[m_CurrentLevelIdx].hasSam)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasSlick)
				CreateSlickOrSam(scene, glm::ivec2{ rand() % 2,1 }, false);
			if (m_Levels[m_CurrentLevelIdx].hasWrongWay)
				CreateUggOrWrongWay(scene, glm::ivec2{ 6,6 }, true);
			if (m_Levels[m_CurrentLevelIdx].hasUgg)
				CreateUggOrWrongWay(scene, glm::ivec2{ 0,6 }, false);
			break;
		}
	}


	

	

	auto subjComp = std::make_shared <SubjectComponent>(m_pQBert1);
	std::shared_ptr<SubjectComponent> subjComp2 = nullptr;
	m_pQBert1->AddComponent(subjComp);
	if (m_GameMode == GameMode::MultiPlayerCoop)
	{
		subjComp2 = std::make_shared <SubjectComponent>(m_pQBert2);
		auto healthComp = m_pQBert1->GetComponent<HealthComponent>();
		auto scoreComp = m_pQBert1->GetComponent<ScoreComponent>();
		auto gridComp = m_pQBert2->GetComponent<GridComponent>();
		gridComp->AddHealthComp(healthComp);
		gridComp->AddScoreComp(scoreComp);
		m_pQBert2->AddComponent(healthComp);
		m_pQBert2->AddComponent(scoreComp);
		m_pQBert2->AddComponent(subjComp2);
	}

	auto hud = HudManager::GetInstance().CreateHud();

	m_pHudObj = std::make_shared<GameObject>(glm::vec3(100.f, 25.f, 0.f));
	auto font = ResourceManager::GetInstance().LoadFont("Lingua.otf", 18);
	auto scorep1 = std::make_shared <TextComponent>(m_pHudObj, " score p1", font, glm::tvec3<uint8_t> { 255, 255, 0 }, glm::vec3(100.f, 25.f, 0.f));
	m_pHudObj->AddComponent(scorep1);
	auto healthp1 = std::make_shared <TextComponent>(m_pHudObj, " health p1", font, glm::tvec3<uint8_t> { 255, 255, 0 }, glm::vec3(100.f, 25.f, 0.f));
	m_pHudObj->AddComponent(healthp1);
	auto observer1 = std::make_shared <ObserverComponent>(m_pHudObj, std::make_shared<QbertObserver>());
	subjComp->AddObserver(observer1);
	if (m_Levels[m_CurrentLevelIdx].hasCoily)
		subjComp->AddObserver(coily->GetComponent<ObserverComponent>());
	if(subjComp2!=nullptr)
		subjComp2->AddObserver(observer1);
	m_pHudObj->AddComponent(observer1);

	hud->AddComponent(std::make_shared<HudRenderComponent>(hud));
	auto scoreHud1 = hud->AddComponent(std::make_shared<HudTextComponent>(hud, "QBertScore: 0", font, glm::tvec3<uint8_t> { 255, 255, 0 }, glm::vec3(400.f, 25.f, 0.f), "QBertScore: "));
	auto healthHud1 = hud->AddComponent(std::make_shared<HudTextComponent>(hud, "Health: 3", font, glm::tvec3<uint8_t> { 255, 255, 0 }, glm::vec3(400.f, 5.f, 0.f), "Health: "));

	
	scorep1->SetHudElement(std::static_pointer_cast<HudTextComponent>(scoreHud1));
	healthp1->SetHudElement(std::static_pointer_cast<HudTextComponent>(healthHud1));
	InputManager::GetInstance().RefreshControllerCount();
	scene.Add(m_pHudObj);

}

void QBertGameApplication::UserCleanUp()
{
	BlockManager::GetInstance().Clear();
	QBertImguiRenderer::GetInstance().Destroy();
}

void QBertGameApplication::UserRender()
{
	QBertImguiRenderer::GetInstance().RenderUI();
}

void QBertGameApplication::UserUpdate(float)
{
	if (m_LevelIsDone)
	{
		LoadNextLevel();
		m_LevelIsDone = false;
	}
	if (m_pQBert1->GetComponent<HealthComponent>()->GetHealth()==0)
	{
		m_CurrentLevelIdx = 0;
		ReloadLevel();
	}
	if(m_pImguiRenderer->GetGameMode()!=m_GameMode)
	{
		m_GameMode = m_pImguiRenderer->GetGameMode();
		ResetLevel();

		
	}
}

void QBertGameApplication::CreateBlocks(Scene& scene)const
{


	std::shared_ptr<GameObject> go = std::make_shared<GameObject>();

	int locationCounter = 0;

	auto& blockManager = BlockManager::GetInstance();
	for (int i = 0; i < 7; ++i)
	{
		for (int j = 0; j <= i; j++)
		{
			if (i % 2 == 1)
			{

				go = std::make_shared<GameObject>();
				go->SetTag("Block");
				auto pos = glm::vec3(m_Levels[m_CurrentLevelIdx].offset.x + (j - i / 2) * m_Levels[m_CurrentLevelIdx].blockSize.x - m_Levels[m_CurrentLevelIdx].blockSize.x / 2, m_Levels[m_CurrentLevelIdx].offset.y + i * m_Levels[m_CurrentLevelIdx].blockSize.y, 0);
				auto texComp = std::make_shared <TextureComponent>(go, m_Levels[m_CurrentLevelIdx].inActiveTex, pos);
				go->AddComponent(texComp);
				go->AddComponent(std::make_shared <RenderComponent>(go));
				go->AddComponent(std::make_shared <GridComponent>(go, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize,glm::ivec2{j,i}, m_Levels[m_CurrentLevelIdx].offset));
				blockManager.AddBlock(int(j* m_Levels[m_CurrentLevelIdx].gridSize.x +i),texComp);
				scene.Add(go);
			}
			else
			{
				go = std::make_shared<GameObject>();
				auto pos = glm::vec3(m_Levels[m_CurrentLevelIdx].offset.x + (j - i / 2) * m_Levels[m_CurrentLevelIdx].blockSize.x , m_Levels[m_CurrentLevelIdx].offset.y + i * m_Levels[m_CurrentLevelIdx].blockSize.y, 0);
				auto texComp = std::make_shared <TextureComponent>(go, m_Levels[m_CurrentLevelIdx].inActiveTex, pos);
				go->AddComponent(texComp);
				go->AddComponent(std::make_shared <RenderComponent>(go));
				go->AddComponent(std::make_shared <GridComponent>(go, glm::ivec2{ 7,7 }, m_Levels[m_CurrentLevelIdx].blockSize, glm::ivec2{ j,i }, m_Levels[m_CurrentLevelIdx].offset));
				blockManager.AddBlock(int(j* m_Levels[m_CurrentLevelIdx].gridSize.x+i),texComp);
				scene.Add(go);
			}
			locationCounter++;
		}
	}
}
/// creates coily but needs to have a qbert to do so
std::shared_ptr<GameObject> QBertGameApplication::CreateQBert(Scene& scene, const CharacterMovement& movement, const glm::ivec2& gridLoc, unsigned long playerID) const
{
	auto& inputManager = InputManager::GetInstance();
	std::shared_ptr<GameObject> qbert = std::make_shared<GameObject>();
	qbert->SetTag("QBert");
	qbert = std::make_shared<GameObject>();

	auto gridComp = std::make_shared <GridComponent>(qbert, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize, gridLoc, m_Levels[m_CurrentLevelIdx].offset);
	qbert->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += m_Levels[m_CurrentLevelIdx].posFix;
	qbert->AddComponent(std::make_shared <TextureComponent>(qbert, "tempQbert.png", pos));
	qbert->AddComponent(std::make_shared <RenderComponent>(qbert));
	qbert->AddComponent(std::make_shared <QBertMoveComponent>(qbert, 2.f, m_Levels[m_CurrentLevelIdx].posFix,unsigned short(0)));


	
	if (playerID == 0)
	{

		
		const auto healthComp = std::make_shared<HealthComponent>(qbert, 3);
		qbert->AddComponent(healthComp);
		gridComp->AddHealthComp(healthComp);
		const auto scoreComp = std::make_shared<ScoreComponent>(qbert);
		qbert->AddComponent(scoreComp);
		gridComp->AddScoreComp(scoreComp);
	}
	scene.Add(qbert);




	inputManager.AddCommand(movement.LeftDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(qbert));
	inputManager.AddCommand(movement.RightDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightDown>(qbert));
	inputManager.AddCommand(movement.LeftUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(qbert));
	inputManager.AddCommand(movement.RightUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveRightUp>(qbert));

	inputManager.AddCommand(movement.RightUpGamepad, ExecuteType::Pressed, std::make_shared<MoveRightUp>(qbert), playerID);
	inputManager.AddCommand(movement.LeftDownGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftDown>(qbert), playerID);
	inputManager.AddCommand(movement.RightDownGamepad, ExecuteType::Pressed, std::make_shared<MoveRightDown>(qbert), playerID);
	inputManager.AddCommand(movement.LeftUpKeyGamepad, ExecuteType::Pressed, std::make_shared<MoveLeftUp>(qbert), playerID);

	return qbert;
}

std::shared_ptr<GameObject> QBertGameApplication::CreateCoily(Scene& scene, const glm::ivec2& gridLoc) const
{
	if (!m_pQBert1)
		return nullptr;
	std::shared_ptr<GameObject> coily = std::make_shared<GameObject>();
	coily->SetTag("Coily");


	auto gridComp = std::make_shared <GridComponent>(coily, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize, gridLoc, m_Levels[m_CurrentLevelIdx].offset);
	coily->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += m_Levels[m_CurrentLevelIdx].posFix;
	coily->AddComponent(std::make_shared <TextureComponent>(coily, "CoilyBall.png", pos));
	coily->AddComponent(std::make_shared <RenderComponent>(coily));
	
	auto qbertGridComp = m_pQBert1->GetComponent<GridComponent>();
	auto posFixActive = m_Levels[m_CurrentLevelIdx].posFix;
	posFixActive.y -= 23;
	coily->AddComponent(std::make_shared <CoilyMoveComponent>(coily,1.f, m_Levels[m_CurrentLevelIdx].posFix,posFixActive,m_pQBert1,m_pQBert2, "CoilyBall.png","CoilySnake.png",false, unsigned short(1)));
	coily->AddComponent(std::make_shared <ObserverComponent>(coily, std::make_shared<QbertHealthObserver>()));
	coily->AddComponent(std::make_shared<SubjectComponent>(coily));
	const auto healthComp = std::make_shared<HealthComponent>(coily, 100);
	
	coily->AddComponent(healthComp);
	gridComp->AddHealthComp(healthComp);
	scene.Add(coily);
	return coily;
}

std::shared_ptr<GameObject> QBertGameApplication::CreateCoily(Scene& scene, const CharacterMovement& movement,
	const glm::ivec2& gridLoc,unsigned long controlerID) const
{

	if (!m_pQBert1)
		return nullptr;
	auto& inputManager = InputManager::GetInstance();
	std::shared_ptr<GameObject> coily = std::make_shared<GameObject>();
	coily->SetTag("Coily");


	auto gridComp = std::make_shared <GridComponent>(coily, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize, gridLoc, m_Levels[m_CurrentLevelIdx].offset);
	coily->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += m_Levels[m_CurrentLevelIdx].posFix;
	coily->AddComponent(std::make_shared <TextureComponent>(coily, "CoilyBall.png", pos));
	coily->AddComponent(std::make_shared <RenderComponent>(coily));

	auto qbertGridComp = m_pQBert1->GetComponent<GridComponent>();
	auto posFixActive = m_Levels[m_CurrentLevelIdx].posFix;
	posFixActive.y -= 23;
	coily->AddComponent(std::make_shared <CoilyMoveComponent>(coily, 2.f, m_Levels[m_CurrentLevelIdx].posFix, posFixActive, m_pQBert1,m_pQBert2, "CoilyBall.png", "CoilySnake.png",true, unsigned short(1)));
	coily->AddComponent(std::make_shared <ObserverComponent>(coily, std::make_shared<QbertHealthObserver>()));
	const auto healthComp = std::make_shared<HealthComponent>(coily, 3);
	coily->AddComponent( std::make_shared<SubjectComponent>(coily));
	coily->AddComponent(healthComp);
	gridComp->AddHealthComp(healthComp);
	scene.Add(coily);
	inputManager.AddCommand(movement.LeftDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily,glm::ivec3{ 0,1,0 }));
	inputManager.AddCommand(movement.RightDownKeyboard, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ 1,1,0 }));
	inputManager.AddCommand(movement.LeftUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ -1,-1,0 }));
	inputManager.AddCommand(movement.RightUpKeyboard, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ 0,-1,0 }));

	inputManager.AddCommand(movement.RightUpGamepad, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ 0,-1,0 }),controlerID);
	inputManager.AddCommand(movement.LeftDownGamepad, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ 0,1,0 }),controlerID);
	inputManager.AddCommand(movement.RightDownGamepad, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ 1,1,0 }),controlerID);
	inputManager.AddCommand(movement.LeftUpKeyGamepad, ExecuteType::Pressed, std::make_shared<MoveCoily>(coily, glm::ivec3{ -1,-1,0 }),controlerID);
	return coily;
}

std::shared_ptr<GameObject> QBertGameApplication::CreateSlickOrSam(Scene& scene, const glm::ivec2& gridLoc,bool isSlick) const
{
	if (!m_pQBert1)
		return nullptr;
	std::shared_ptr<GameObject> slick = std::make_shared<GameObject>();
	slick->SetTag("SlickorSam");


	auto gridComp = std::make_shared <GridComponent>(slick, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize, gridLoc, m_Levels[m_CurrentLevelIdx].offset);
	slick->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += m_Levels[m_CurrentLevelIdx].posFix;
	std::shared_ptr<TextureComponent> texComp = nullptr;
	if (isSlick)
		texComp = std::make_shared <TextureComponent>(slick, "Slick.png", pos);
	else
		texComp = std::make_shared <TextureComponent>(slick, "Sam.png", pos);
	slick->AddComponent(texComp);
	slick->AddComponent(std::make_shared <RenderComponent>(slick));

	auto qbertGridComp = m_pQBert1->GetComponent<GridComponent>();
	auto posFixActive = m_Levels[m_CurrentLevelIdx].posFix;
	posFixActive.y -= 23;
	slick->AddComponent(std::make_shared <SlicknSamMoveComponent>(slick, 1.f, m_Levels[m_CurrentLevelIdx].posFix, rand() % 10 + 4.f,texComp, unsigned short(1),gridComp,m_pQBert1,m_pQBert2));
	slick->AddComponent(std::make_shared <ObserverComponent>(slick, std::make_shared<QbertHealthObserver>()));
	slick->AddComponent(std::make_shared<SubjectComponent>(slick));
	const auto healthComp = std::make_shared<HealthComponent>(slick, 1);

	slick->AddComponent(healthComp);
	gridComp->AddHealthComp(healthComp);
	scene.Add(slick);
	return slick;
}

std::shared_ptr<GameObject> QBertGameApplication::CreateUggOrWrongWay(Scene& scene, const glm::ivec2& gridLoc,bool isWrongWay) const
{
	if (!m_pQBert1)
		return nullptr;
	std::shared_ptr<GameObject> ugg = std::make_shared<GameObject>();
	ugg->SetTag("UggorWrongWay");


	auto gridComp = std::make_shared <GridComponent>(ugg, m_Levels[m_CurrentLevelIdx].gridSize, m_Levels[m_CurrentLevelIdx].blockSize, gridLoc, m_Levels[m_CurrentLevelIdx].offset);
	ugg->AddComponent(gridComp);
	auto pos = gridComp->CalcGridPos();
	pos += m_Levels[m_CurrentLevelIdx].posFix;
	std::shared_ptr<TextureComponent> texComp = nullptr;
	glm::ivec3 offset;
	if(isWrongWay)
	{
		
		offset = glm::ivec3{ 20,-18,0 };
		texComp = std::make_shared <TextureComponent>(ugg, "Wrongway.png", pos);
	}
	else
	{
		
		offset = glm::ivec3{ 28,-20,0 };
		texComp = std::make_shared <TextureComponent>(ugg, "Ugg.png", pos);
	}

	ugg->AddComponent(texComp);
	ugg->AddComponent(std::make_shared <RenderComponent>(ugg));

	auto qbertGridComp = m_pQBert1->GetComponent<GridComponent>();
	auto posFixActive = m_Levels[m_CurrentLevelIdx].posFix;
	posFixActive.y -= 23;
	ugg->AddComponent(std::make_shared <UggnWrongWayMoveComponent>(ugg, 1.f, offset, rand()%10+4.f,m_Levels[m_CurrentLevelIdx].posFix, unsigned short(1), m_pQBert1, m_pQBert2,!isWrongWay));
	ugg->AddComponent(std::make_shared <ObserverComponent>(ugg, std::make_shared<QbertHealthObserver>()));
	ugg->AddComponent(std::make_shared<SubjectComponent>(ugg));
	const auto healthComp = std::make_shared<HealthComponent>(ugg, 1);

	ugg->AddComponent(healthComp);
	gridComp->AddHealthComp(healthComp);
	scene.Add(ugg);
	return ugg;
}


