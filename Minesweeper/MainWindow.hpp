#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <format>
#include <array>
#include "Cell.h"
#include "constants.h"
#include "Resources.h"

enum Scene
{
	MAIN_MENU = 0,
	IN_GAME = 1,
};

enum Group
{
	MainMenu = 0,
	DifficultySelector,
	Credits,
	HowToPlay,
	SIZE
};

class MainWindow
{
private:
	sf::RenderWindow m_window;
	tgui::GuiSFML m_gui;
	std::array<tgui::Group::Ptr, Group::SIZE> m_groups;

	tgui::Button::Ptr m_playButton;
	tgui::Button::Ptr m_creditsButton;
	tgui::Button::Ptr m_howToPlayButton;
	tgui::Button::Ptr m_quitButton;

	tgui::Button::Ptr m_easyButton;
	tgui::Button::Ptr m_normalButton;
	tgui::Button::Ptr m_hardButton;

	tgui::Button::Ptr m_backButtonDifficulty;
	tgui::Button::Ptr m_backButtonCredits;
	tgui::Button::Ptr m_backButtonHowToPlay;

	tgui::MessageBox::Ptr m_youLostMsgBox;
	tgui::MessageBox::Ptr m_youWinMsgBox;

	const float FPS = 60.f;
	const tgui::Layout m_menuButtonWidth = "27%";
	const tgui::Layout m_menuButtonHeight = "10%";
	const tgui::Layout m_menuButtonX = "50%";
	const unsigned int m_menuButtonTextSize = 20;
	const sf::Time TimePerFrame = sf::seconds(1.f / FPS);

	Scene m_scene = Scene::MAIN_MENU;
	bool m_isLeftClicking = false;
	bool m_isRightClicking = false;
	bool m_isMiddleClicking = false;
	bool m_isFirstClick = true;
	sf::RectangleShape m_mouseShape;

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
	sf::Texture m_backgroundTexture;
	sf::Sprite m_backgroundSprite;
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
	
	void switchToGroup(Group group);
	void switchToMainMenu();
	void switchToInGame(int mineCount, sf::Vector2i tableSize, int cellSize, int safeRange);

	template<typename T>
	void debugPrint(T var, bool nl = true);

public:
	MainWindow();
	~MainWindow();

	void handleTguiButtonPress(tgui::String buttonText);
	void handleTguiHovering();
	void createMainMenuButton(tgui::Button::Ptr* button, tgui::String buttonText, tgui::Layout y);

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