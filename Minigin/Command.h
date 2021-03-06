#pragma once
#include <iostream>
#include "GameObject.h"
#include "ServiceLocator.h"


class TextComponent;
class ScoreComponent;



class Command
{
public:
	Command()=default;
	virtual ~Command()=default;
	virtual void Execute() = 0;
	bool isQuit{};
private:
protected:
};
class ScoreCommand : public Command
{
public:
	ScoreCommand( std::shared_ptr<ScoreComponent> scoreComponent,int increaseValue);
	virtual ~ScoreCommand();
	virtual void Execute()override;

private:
	std::shared_ptr<ScoreComponent> m_ScoreComponent;
	int m_IncreaseValue;
};
class QuitCommand : public Command
{
public:
	QuitCommand(bool* quit);
	virtual ~QuitCommand();
	virtual void Execute()override { *m_Quit=true; }

private:
	bool* m_Quit;
};
class DamageCommand : public Command
{
public:
	DamageCommand(std::shared_ptr<GameObject> object, int damage);
	virtual ~DamageCommand();
	virtual void Execute()override;


private:
	std::shared_ptr<GameObject> m_ObjectThatsDamaged;
	int m_Damage;

};

class PlayCommand : public Command
{
public:
	PlayCommand(SoundID  id,float volume);
	virtual ~PlayCommand();
	virtual void Execute()override;


private:
	SoundID  m_Id;
	float m_Volume;

};


