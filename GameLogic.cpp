#include "GameLogic.h"

GameLogic::GameLogic()
    : _presentation()
    , _IsPlaying(true)
    , NASASpawn(false)
    , DestroyerSpawn(false)
    , LaserSpawn(false)
{
    srand(time(0));
    PlayerShip = make_shared<Player>(_screen, Lives);
    _gameObjects.push_back(PlayerShip);
    for(int i = 1; i <= Lives; i++) {
	_Lives = make_shared<PlayerLives>(_screen, i);
	_gameObjects.push_back(_Lives);
    }
}

void GameLogic::Run()
{
    drawSplashScreen();
    drawControlScreen();

    Timer timer;
    timer.Start();
    auto Update_Time = 0.f;
    // 60 Frames per Second
    auto Frame_Time = 1.0f / 6000.f;

    while(_IsPlaying) {
	Update_Time += timer.getDuration();
	timer.Start();

	while(Update_Time > Frame_Time) {
	    EnemySpawn();
	    SatelliteSpawn();
	    AsteroidSpawn();
	    LasersSpawn();
	    UserInputs();
	    PlayerUpdate();
	    BulletsSpawn();
	    CheckForUpgrade();
	    ObjectUpdate();
	    renderObjects();
	    CheckCollisions();

	    Update_Time -= Frame_Time;

	    if(PlayerShip->GameOver() || EnemiesKilled == 10) {
		_IsPlaying = false;
		_presentation.displayGameOverScreen();
		Restart();
	    }
	}
    }
}

void GameLogic::UserInputs()
{
    _presentation.UserInputs();
}

void GameLogic::PlayerUpdate()
{

    if(_presentation.SpacePressed && Shots == 0) {
	if(PlayerShip->CheckUpgrade()) {
	    _gameObjects.push_back(PlayerShip->Shoot());
	    _gameObjects.push_back(PlayerShip->Shoot());
	} else {
	    _gameObjects.push_back(PlayerShip->Shoot());
	}
	Shots++;
    }
    if(_presentation.LeftKeyPressed)
	PlayerShip->setDirection(PlayerDirection::Clockwise);
    else if(_presentation.RightKeyPressed)
	PlayerShip->setDirection(PlayerDirection::Anticlockwise);
    else
	PlayerShip->setDirection(PlayerDirection::Hold);
    if(!_presentation.SpacePressed && Shots > 0)
	Shots = 0;
}

void GameLogic::renderObjects()
{

    _presentation.renderWindow(_gameObjects);
    // In line Queries on data lambda thing
    _gameObjects.erase(remove_if(_gameObjects.begin(), _gameObjects.end(),
                           [](shared_ptr<MovingObjects>& ObjectsInGame) { return (!ObjectsInGame->Status()); }),
        _gameObjects.end());
}

void GameLogic::ObjectUpdate()
{
    for(auto& gameObject : _gameObjects) {

	if(gameObject->GetObject() == Objects::Satellites) {
	    if(!gameObject->Status()) {
		NumOfSats--;
	    }
	}
	if(gameObject->GetObject() == Objects::Enemy) {
	    if(!gameObject->Status()) {
		EnemiesKilled++;
	    }
	}
	if(gameObject->Status()) {
	    gameObject->Move();
	}
	if(gameObject->Respawns()) {
	    Alien = make_shared<Enemy>(_screen);
	    _gameObjects.push_back(Alien);
	    _ShootingGameObjects.push_back(Alien);
	    _gameObjects.push_back(Alien->Shoot());
	}
    }
}

void GameLogic::EnemySpawn()
{
    if(spawnFactor == 3000 && Enemies < 10) {
	Alien = make_shared<Enemy>(_screen);
	_gameObjects.push_back(Alien);
	_gameObjects.push_back(Alien->Shoot());

	Enemies++;
	spawnFactor = 0;
    }
    spawnFactor++;
}

void GameLogic::AsteroidSpawn()
{
    int randomSpawnFactor = rand() % 10000;
    if(randomSpawnFactor == 10 && DestroyerSpawn == false) {
	Destroyer = make_shared<Asteroid>(_screen);
	Destroyer->PlayersPos(PlayerShip);
	_gameObjects.push_back(Destroyer);
	DestroyerSpawn = true;
    }

    if(DestroyerSpawn == true) {
	if(!Destroyer->Status()) {
	    DestroyerSpawn = false;
	}
    }
}

void GameLogic::drawSplashScreen()
{
    _presentation.displaySplashScreen();
}

void GameLogic::drawControlScreen()
{
    _presentation.displayControlScreen();
}

void GameLogic::SatelliteSpawn()
{
    int randomSpawnFactor = rand() % 10000;
    if(randomSpawnFactor == 1 && NASASpawn == false) {
	for(int i = 1; i <= 3; i++) {
	    NASA = make_shared<Satellite>(_screen, i);
	    NASA->PlayersPos(PlayerShip);
	    _gameObjects.push_back(NASA);
	    _ShootingGameObjects.push_back(NASA);
	    _gameObjects.push_back(NASA->Shoot());
	}
	NASASpawn = true;
    }
}

void GameLogic::BulletsSpawn()
{
    if(BulletSpawnFactor == 1000) {
	for(auto& gameObject : _ShootingGameObjects) {

	    if(gameObject->GetObject() == Objects::Satellites && NASASpawn == true) {
		if(gameObject->Status()) {
		    _gameObjects.push_back(gameObject->Shoot());
		}
	    }
	    if(gameObject->GetObject() == Objects::Enemy) {
		if(gameObject->Status()) {
		    _gameObjects.push_back(gameObject->Shoot());
		}
	    }
	}
	BulletSpawnFactor = 0;
    }
    BulletSpawnFactor++;
}

void GameLogic::CheckForUpgrade()
{
    if(NumOfSats == 0) {
	PlayerShip->WeaponUpgrade();
	NumOfSats--;
    } else if((NASASpawn == true) && (PlayerShip->CheckUpgrade() == false) && (NumOfSats == -1)) {
	NASASpawn = false;
	NumOfSats = 3;
    }
}

void GameLogic::Restart()
{
    _IsPlaying = true;

    NASASpawn = false;
    DestroyerSpawn = false;
    LaserSpawn = false;

    _gameObjects.clear();
    _ShootingGameObjects.clear();

    Lives = 3;

    PlayerShip = make_shared<Player>(_screen, Lives);
    _gameObjects.push_back(PlayerShip);

    for(int i = 1; i <= Lives - 1; i++) {
	_Lives = make_shared<PlayerLives>(_screen, i);
	_gameObjects.push_back(_Lives);
	// Run();
    }

    PlayerShip->setDirection(PlayerDirection::Hold);
    EnemiesKilled = 0;
    Enemies = 0;
    NumOfSats = 3;
}

void GameLogic::CheckCollisions()
{
    _collisionHandler.CheckForCollisions(_gameObjects);
}

void GameLogic::LasersSpawn()
{
    int randomSpawnFactor = rand() % 10000;
    if(randomSpawnFactor == 10 && LaserSpawn == false) {
	Laser = make_shared<LaserGenerators>(_screen);
	Laser->PlayersPos(PlayerShip);
	_gameObjects.push_back(Laser);
	LaserSpawn = true;
    }
}