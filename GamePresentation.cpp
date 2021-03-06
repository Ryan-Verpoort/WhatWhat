#include "GamePresentation.h"

using std::string;

sf::Sprite tempSprite;
sf::Texture tempTexture;

GamePresentation::GamePresentation()
    : _window(sf::VideoMode(1000, 750), "Gyruss")
{

    _backgroundTexture.loadFromFile("resources/Nebula.jpg");
    _backgroundTexture.setSmooth(true);
    _backgroundSprite.setTexture(_backgroundTexture);
    displayBackground();

    tempTexture.loadFromFile("resources/Bullet.png");
    tempSprite.setTexture(tempTexture);
    Textures();
}

void GamePresentation::displayBackground()
{
    _window.draw(_backgroundSprite);
}

void GamePresentation::renderWindow(vector<shared_ptr<MovingObjects> >& gameObjects)
{
    _window.clear();
    _window.draw(_backgroundSprite);
    for(auto const& gameObject : gameObjects) {
	for(auto const& i : _textureslList) {
	    if(i.GameObject == gameObject->GetObject() && gameObject->Status()) {
		sf::Sprite gameObjectSprite;
		gameObjectSprite.setTexture(i.texture);
		gameObjectSprite.setOrigin(i.texture.getSize().x / 2, i.texture.getSize().y / 2);
		gameObjectSprite.setPosition(
		    gameObject->GetPosition().GetCurrentX(), gameObject->GetPosition().GetCurrentY());

		if(gameObject->GetObject() != Objects::Satellites) {
		    gameObjectSprite.setRotation(gameObject->GetPosition().GetAngle() * -0.5);
		}

		gameObjectSprite.setRotation(gameObject->GetPosition().GetAngle() * -0.5);
		_window.draw(gameObjectSprite);
	    }
	}
    }
    _window.display();
}

void GamePresentation::UserInputs()
{
    sf::Event event;
    while(_window.pollEvent(event)) {
	switch(event.type) {
	case sf::Event::KeyPressed:
	    UserInputEvents(event.key.code, true);
	    break;

	case sf::Event::KeyReleased:
	    UserInputEvents(event.key.code, false);
	    break;

	default:
	    break;
	}
    }
}

void GamePresentation::UserInputEvents(sf::Keyboard::Key key, bool isPressed)
{
    if((key == sf::Keyboard::Right))
	RightKeyPressed = isPressed;
    else if((key == sf::Keyboard::Left))
	LeftKeyPressed = isPressed;
    if((key == sf::Keyboard::Escape))
	_window.close();
    if(key == sf::Keyboard::Space && isPressed == true)
	if(Delay == 0) {
	    SpacePressed = true;
	    Delay++;
	}
    if(key == sf::Keyboard::Space && isPressed == false)
	SpacePressed = false;
    Delay = 0;
}

void GamePresentation::Textures()
{
    for(unsigned int i = 0; i != _paths.size(); i++) {
	Resources resource{ static_cast<Objects>(i), _paths.at(i) };
	_ObjectsTextures.push_back(resource);
    }

    for(auto ObjectsTexture : _ObjectsTextures) {
	sf::Texture texture;
	texture.loadFromFile(ObjectsTexture.GetObjectResourcePath());
	texture.setSmooth(true);

	textureIdentifier ObjectTexture;
	ObjectTexture.GameObject = ObjectsTexture.GetGameObjectType();
	ObjectTexture.texture = texture;
	_textureslList.push_back(ObjectTexture);
    }
}

void GamePresentation::displaySplashScreen()
{
    SplashScreen splashScreen;
    splashScreen.Display(_window);
}

void GamePresentation::displayControlScreen()
{
    ControlScreen controlscreen;
    controlscreen.Display(_window);
}

void GamePresentation::displayGameOverScreen()
{
    GameOverScreen gameOverScreen;
    gameOverScreen.Display(_window);
}