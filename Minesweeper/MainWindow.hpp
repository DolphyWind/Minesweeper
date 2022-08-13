#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "constants.h"
#include "Resources.h"

enum Scene
{
	MAIN_MENU = 0,
	IN_GAME = 1,
};

class MainWindow
{
private:
	sf::RenderWindow m_window;
	const float FPS = 60.f;
	const sf::Time TimePerFrame = sf::seconds(1.f / FPS);

	sf::Font m_font;
	//bool m_inMainMenu = true;
	Scene m_scene = Scene::MAIN_MENU;
	bool m_isLeftClicking = false;
	bool m_isHovering = false;
	sf::RectangleShape m_mouseShape;

	sf::Text m_easyText;
	sf::Text m_normalText;
	sf::Text m_hardText;
	sf::RectangleShape m_easyShape;
	sf::RectangleShape m_normalShape;
	sf::RectangleShape m_hardShape;

	sf::SoundBuffer m_clickSoundBuffer;
	sf::SoundBuffer m_hoverSoundBuffer;
	sf::Sound m_clickSound;
	sf::Sound m_hoverSound;

	sf::Vector2f tableSize;
	sf::Vector2f cellSize;
	int mineCount;

	void centerWindow();
	void processEvents();
	void update(sf::Time deltaTime);
	void draw();
	void handleKeyPress(sf::Keyboard::Key key);
	void handleKeyRelease(sf::Keyboard::Key key);
	void handleButtonPress(sf::Mouse::Button button, float x, float y);
	void handleButtonRelease(sf::Mouse::Button button, float x, float y);
	void handleMouseHovering(sf::RectangleShape* buttonShape, sf::Text* buttonText);

	void makeMenuShape(sf::RectangleShape &shape, sf::Vector2f position);
	void makeMenuText(sf::Text &text, sf::String str, sf::Vector2f position);

	template<typename T>
	void debugPrint(T var);

public:
	MainWindow();
	void mainLoop();

};

template<typename T>
inline void MainWindow::debugPrint(T var)
{
#ifdef _DEBUG
	std::cout << var;
#endif
}
