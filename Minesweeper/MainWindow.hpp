#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <format>
#include "Cell.h"
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
	tgui::GuiSFML m_gui;
	tgui::Group::Ptr m_mainMenuGroup;
	tgui::MessageBox::Ptr m_youLostMsgBox;
	tgui::MessageBox::Ptr m_youWinMsgBox;

	const float FPS = 60.f;
	const sf::Time TimePerFrame = sf::seconds(1.f / FPS);

	sf::Font m_font;
	Scene m_scene = Scene::MAIN_MENU;
	bool m_isLeftClicking = false;
	bool m_isRightClicking = false;
	bool m_isMiddleClicking = false;
	bool m_isFirstClick = true;
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
	sf::SoundBuffer m_winSoundBuffer;
	sf::SoundBuffer m_loseSoundBuffer;
	sf::Sound m_clickSound;
	sf::Sound m_hoverSound;
	sf::Sound m_winSound;
	sf::Sound m_loseSound;

	sf::Texture m_textures32;
	sf::Texture m_textures64;
	sf::Texture* m_texturePtr;
	sf::Vector2i m_tableSize;
	int m_cellSize;
	int m_mineCount;
	int m_safeRange;
	bool m_youLost = false;
	bool m_youWin = false;
	std::vector<std::vector<Cell>> m_table;

	void centerWindow();
	void processEvents();
	void update(sf::Time deltaTime);
	void draw();
	void handleKeyPress(sf::Keyboard::Key key);
	void handleKeyRelease(sf::Keyboard::Key key);
	void handleWheelScroll(int delta, sf::Mouse::Wheel wheel, int x, int y);
	void handleButtonPress(sf::Mouse::Button button, int x, int y);
	void handleButtonRelease(sf::Mouse::Button button, int x, int y);
	bool handleMouseHovering(sf::RectangleShape* buttonShape, sf::Text* buttonText);
	
	void switchToMainMenu();
	void switchToInGame(int mineCount, sf::Vector2i tableSize, int cellSize, int safeRange);

	void makeMenuShape(sf::RectangleShape &shape, sf::Vector2f position);
	void makeMenuText(sf::Text &text, sf::String str, sf::Vector2f position);

	template<typename T>
	void debugPrint(T var, bool nl = true);

public:
	MainWindow();
	~MainWindow();

	void handleTguiButtonPress(tgui::String buttonText);
	void mainLoop();

};

template<typename T>
inline void MainWindow::debugPrint(T var, bool nl)
{
#ifdef _DEBUG
	std::cout << var;
	if (nl) std::cout << std::endl;
#endif
}