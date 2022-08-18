#include "MainWindow.hpp"

void MainWindow::centerWindow()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	sf::Vector2i centerPos;
	centerPos.x = desktop.width / 2 - m_window.getSize().x / 2;
	centerPos.y = desktop.height / 2 - m_window.getSize().y / 2;
	m_window.setPosition(centerPos);
}

void MainWindow::processEvents()
{
	sf::Event e;
	while (m_window.pollEvent(e))
	{
		m_gui.handleEvent(e);
		switch (e.type)
		{
		case sf::Event::Closed:
			m_window.close();
			break;
		case sf::Event::MouseButtonPressed:
			handleButtonPress(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
			break;
		case sf::Event::MouseButtonReleased:
			handleButtonRelease(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
			break;
		case sf::Event::Resized:
		{
			sf::FloatRect visibleArea(0, 0, e.size.width, e.size.height);
			m_window.setView(sf::View(visibleArea));
		}
			break;
		default:
			break;
		}
		if (e.type == sf::Event::Closed)
			m_window.close();
	}
}

void MainWindow::update(sf::Time deltaTime)
{
	if (m_scene == Scene::MAIN_MENU && m_window.hasFocus())
	{
		sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
		m_mouseShape.setSize(sf::Vector2f(1, 1));
		m_mouseShape.setOrigin(.5f, .5f);
		m_mouseShape.setPosition(mousePos.x, mousePos.y);
	}
}

void MainWindow::handleButtonPress(sf::Mouse::Button button, int x, int y)
{
	if (!m_window.hasFocus()) return;
	if (button == sf::Mouse::Left)
	{
		if (!m_isLeftClicking && m_scene == Scene::IN_GAME && !m_youLost && !m_youWin)
		{
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;

			// Create board
			if (m_isFirstClick)
			{
				m_isFirstClick = false;
				std::set<std::pair<int, int>> mineCoords;
				std::set<std::pair<int, int>> bannedCoords;

				for (int j = -m_safeRange; j <= m_safeRange; j++)
				{
					for (int i = -m_safeRange; i <= m_safeRange; i++)
					{
						bannedCoords.insert({ _x + i, _y + j });
					}
				}

				while (mineCoords.size() != m_mineCount)
				{
					std::pair<int, int> randCoord = {
						std::rand() % m_tableSize.x,
						std::rand() % m_tableSize.y
					};
						
					if (bannedCoords.contains(randCoord)) continue;
					mineCoords.insert(randCoord);
				}

				for (auto& p : mineCoords)
				{
					m_table[p.second][p.first].setState(CellState::MINE);
				}

				for (int j = 0; j < m_tableSize.y; j++)
				{
					for (int i = 0; i < m_tableSize.x; i++)
					{
						if (m_table[j][i].getState() == CellState::MINE) continue;
						int mines = 0;
						for (int l = -1; l <= 1; l++)
						{
							for (int k = -1; k <= 1; k++)
							{
								try
								{
									mines += m_table.at(j + l).at(i + k).getState() == CellState::MINE;
								}
								catch (const std::exception&) {}
							}
						}
						m_table[j][i].setState((CellState)mines);
					}
				}
			}

			Cell* currentCell = &m_table[_y][_x];
			if (currentCell->getState() == CellState::MINE && !m_youLost && !currentCell->getFlagged())
			{
				currentCell->setActivatedMine(true);
				for (int j = 0; j < m_tableSize.y; j++)
				{
					for (int i = 0; i < m_tableSize.x; i++)
					{
						Cell* iteratedCell = &m_table[j][i];
						if (iteratedCell->getState() == CellState::MINE)
						{
							iteratedCell->setFlagged(false);
							iteratedCell->setHidden(false);
							iteratedCell->updateCellSprite();
						}
					}
				}
				m_youLost = true;
			}
			if(!m_youLost) m_table[_y][_x].open(_y, _x, &m_table);

			m_youWin = true;

			for (int j = 0; j < m_table.size(); j++)
			{
				for (int i = 0; i < m_table.size(); i++)
				{
					if (m_table[j][i].getState() == CellState::MINE && !m_table[j][i].getHidden()) m_youWin = false;
					if (m_table[j][i].getState() != CellState::MINE && m_table[j][i].getHidden()) m_youWin = false;
				}
			}

			if (m_youWin)
			{
				if (!m_youWinMsgBox->isVisible()) m_winSound.play();
				m_youLostMsgBox->setVisible(false);
				m_youWinMsgBox->setVisible(true);
				return;
			}
			if (m_youLost)
			{
				if(!m_youLostMsgBox->isVisible()) m_loseSound.play();
				m_youLostMsgBox->setVisible(true);
				m_youWinMsgBox->setVisible(false);
				return;
			}
		}
		m_isLeftClicking = true;
	}
	else if (button == sf::Mouse::Right)
	{
		if (!m_isRightClicking && m_scene == Scene::IN_GAME && !m_youLost && !m_youWin)
		{
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;
			m_table[_y][_x].setFlagged(!m_table[_y][_x].getFlagged());
		}
		m_isRightClicking = true;
	}
}

void MainWindow::handleButtonRelease(sf::Mouse::Button button, int x, int y)
{
	if (button == sf::Mouse::Left)
		m_isLeftClicking = false;
	else if (button == sf::Mouse::Right)
		m_isRightClicking = false;
}

void MainWindow::switchToGroup(Group group)
{
	for (int i = 0; i < Group::SIZE; i++)
		m_groups[(Group)i]->setVisible(group == (Group)i);
}

Group MainWindow::getActivatedGroup()
{
	for (int i = 0; i < Group::SIZE; i++)
		if (m_groups[(Group)i]->isVisible()) return (Group)i;
}

void MainWindow::switchToMainMenu()
{
	m_scene = Scene::MAIN_MENU;
	m_window.setSize({ WINDOWSIZEX, WINDOWSIZEY });
	switchToGroup(Group::MainMenu);
	m_youLostMsgBox->setVisible(false);
	m_youWinMsgBox->setVisible(false);

	centerWindow();
}

void MainWindow::switchToInGame(int mineCount, sf::Vector2i tableSize, int cellSize, int safeRange)
{
	m_mineCount = mineCount;
	m_tableSize = tableSize;
	m_cellSize = cellSize;
	m_safeRange = safeRange;
	if (cellSize == 32) m_texturePtr = &m_textures32;
	if (cellSize == 64) m_texturePtr = &m_textures64;
	m_youLostMsgBox->setPosition(
		(tableSize.x * cellSize - m_youLostMsgBox->getSize().x) / 2,
		(tableSize.y * cellSize - m_youLostMsgBox->getSize().y) / 2
	);
	m_youWinMsgBox->setPosition(
		(tableSize.x * cellSize - m_youWinMsgBox->getSize().x) / 2,
		(tableSize.y * cellSize - m_youWinMsgBox->getSize().y) / 2
	);
	
	switchToGroup(Group::SIZE); // Disables all groups
	m_youLost = false;
	m_youWin = false;
	m_scene = Scene::IN_GAME;
	m_isFirstClick = true;
	m_youLostMsgBox->setVisible(false);
	m_youWinMsgBox->setVisible(false);

	m_table.clear();
	m_window.setSize(sf::Vector2u(m_tableSize.x * m_cellSize, m_tableSize.y * m_cellSize));
	centerWindow();
	for (int y = 0; y < m_tableSize.y; y++)
	{
		std::vector<Cell> newRow;
		for (int x = 0; x < m_tableSize.x; x++)
		{
			Cell cell(m_texturePtr, m_cellSize, CellState::EMPTY);
			cell.setPosition(x * m_cellSize, y * m_cellSize);
			newRow.push_back(cell);
		}
		m_table.push_back(newRow);
	}
}

void MainWindow::handleTguiButtonPress(tgui::String buttonText)
{
	if (buttonText == MAINMENUTEXT)
		switchToMainMenu();
	else if (buttonText == PLAYAGAINTEXT)
		switchToInGame(m_mineCount, m_tableSize, m_cellSize, m_safeRange);
	
	else if (buttonText == m_playButton->getText())
		switchToGroup(Group::DifficultySelector);
	else if (buttonText == m_creditsButton->getText())
		switchToGroup(Group::Credits);
	else if (buttonText == m_howToPlayButton->getText())
		switchToGroup(Group::HowToPlay);
	else if (buttonText == m_backButtonCredits->getText())
		switchToGroup(Group::MainMenu);
	else if (buttonText == m_quitButton->getText())
		m_window.close();

	else if (buttonText == m_easyButton->getText())
		switchToInGame(10, { 8, 8 }, 64, 1);
	else if (buttonText == m_normalButton->getText())
		switchToInGame(40, { 16, 16 }, 32, 2);
	else if (buttonText == m_hardButton->getText())
		switchToInGame(99, { 32, 16 }, 32, 3);

	else
		debugPrint(buttonText.toStdString());
}

void MainWindow::createMainMenuButton(tgui::Button::Ptr* button, tgui::String buttonText, tgui::Layout y)
{
	(*button) = tgui::Button::create(buttonText);
	(*button)->setSize(m_menuButtonWidth, m_menuButtonHeight);
	(*button)->setOrigin(.5f, .5f);
	(*button)->setPosition(m_menuButtonX, y);
	(*button)->setTextSize(m_menuButtonTextSize);
	(*button)->onClick(&MainWindow::handleTguiButtonPress, this, (*button)->getText());
}

MainWindow::MainWindow()
{
	m_window.create(sf::VideoMode(512, 512), "Minesweeper", sf::Style::Close);
	m_window.setFramerateLimit(FPS);
	m_gui.setTarget(m_window);

	createMainMenuButton(&m_playButton, "Play", "60%");
	createMainMenuButton(&m_creditsButton, "Credits", "70.5%");
	createMainMenuButton(&m_howToPlayButton, "How To Play", "81%");
	createMainMenuButton(&m_quitButton, "Quit", "91.5%");

	createMainMenuButton(&m_easyButton, "Easy", "60%");
	createMainMenuButton(&m_normalButton, "Normal", "70.5%");
	createMainMenuButton(&m_hardButton, "Hard", "81%");

	createMainMenuButton(&m_backButtonDifficulty, "Back", "91.5%");
	createMainMenuButton(&m_backButtonCredits, "Back", "91.5%");
	createMainMenuButton(&m_backButtonHowToPlay, "Back", "91.5%");

	m_groups[Group::MainMenu] = tgui::Group::create();
	m_groups[Group::MainMenu]->add(m_playButton);
	m_groups[Group::MainMenu]->add(m_creditsButton);
	m_groups[Group::MainMenu]->add(m_howToPlayButton);
	m_groups[Group::MainMenu]->add(m_quitButton);
	m_gui.add(m_groups[Group::MainMenu]);

	m_groups[Group::DifficultySelector] = tgui::Group::create();
	m_groups[Group::DifficultySelector]->add(m_easyButton);
	m_groups[Group::DifficultySelector]->add(m_normalButton);
	m_groups[Group::DifficultySelector]->add(m_hardButton);
	m_groups[Group::DifficultySelector]->add(m_backButtonDifficulty);
	m_gui.add(m_groups[Group::DifficultySelector]);

	m_groups[Group::Credits] = tgui::Group::create();
	m_groups[Group::Credits]->add(m_backButtonCredits);
	m_gui.add(m_groups[Group::Credits]);

	m_groups[Group::HowToPlay] = tgui::Group::create();
	m_groups[Group::HowToPlay]->add(m_backButtonHowToPlay);
	m_gui.add(m_groups[Group::HowToPlay]);

	switchToGroup(Group::MainMenu);

	m_youLostMsgBox = tgui::MessageBox::create("Game Over", "You Lost", { PLAYAGAINTEXT, MAINMENUTEXT });
	m_youLostMsgBox->onButtonPress(&MainWindow::handleTguiButtonPress, this);
	m_youLostMsgBox->setVisible(false);

	m_youWinMsgBox = tgui::MessageBox::create("Game Over", "You Win!", { PLAYAGAINTEXT, MAINMENUTEXT });
	m_youWinMsgBox->onButtonPress(&MainWindow::handleTguiButtonPress, this);
	m_youWinMsgBox->setVisible(false);

	m_gui.add(m_youLostMsgBox, "YouLostMsgBox");
	m_gui.add(m_youWinMsgBox, "YouWinMsgBox");

	m_titleTexture.loadFromMemory(titleTextData, sizeof(titleTextData));
	m_titleSprite.setTexture(m_titleTexture);
	m_titleSprite.setScale(0.5f, 0.5f);
	m_titleSprite.setPosition((m_window.getSize().x - m_titleTexture.getSize().x * m_titleSprite.getScale().x) / 2,
		35
		);

	m_winSoundBuffer.loadFromMemory(winSoundData, sizeof(winSoundData));
	m_loseSoundBuffer.loadFromMemory(loseSoundData, sizeof(loseSoundData));
	m_winSound.setBuffer(m_winSoundBuffer);
	m_loseSound.setBuffer(m_loseSoundBuffer);
	m_textures32.loadFromMemory(sprites32Data, sizeof(sprites32Data));
	m_textures64.loadFromMemory(sprites64Data, sizeof(sprites64Data));
	m_backgroundTexture.loadFromMemory(backgroundData, sizeof(backgroundData));
	m_backgroundSprite.setTexture(m_backgroundTexture);

	m_font.loadFromMemory(fontData, sizeof(fontData));
	m_creditsTopText.setFont(m_font);
	m_creditsTopText.setFillColor(sf::Color::White);
	m_creditsTopText.setOutlineColor(sf::Color::Black);
	m_creditsTopText.setOutlineThickness(1.5);
	m_creditsTopText.setCharacterSize(20);
	m_creditsTopText.setString(
		"Programming: Yunus Emre Aydin\n"
		"Artwork: Yunus Emre Aydin\n"
		"Sounds: Yunus Emre Aydin"
	);
	m_creditsTopText.setPosition(
		(m_window.getSize().x - m_creditsTopText.getGlobalBounds().width) / 4 + m_creditsTopText.getGlobalBounds().left,
		(m_window.getSize().y - m_creditsTopText.getGlobalBounds().height) / 4 + m_creditsTopText.getGlobalBounds().top + 15
	);

	m_creditsBottomText.setFont(m_font);
	m_creditsBottomText.setFillColor(sf::Color::Yellow);
	m_creditsBottomText.setOutlineColor(sf::Color::Black);
	m_creditsBottomText.setOutlineThickness(1.5);
	m_creditsBottomText.setCharacterSize(20);
	m_creditsBottomText.setString(
		"Special thanks to SFML and TGUI teams!"
	);
	m_creditsBottomText.setPosition(
		m_creditsTopText.getPosition().x,
		3 * (m_window.getSize().y - m_creditsBottomText.getGlobalBounds().height) / 4 + m_creditsBottomText.getGlobalBounds().top + 20
	);

	m_howToPlayText.setFont(m_font);
	m_howToPlayText.setFillColor(sf::Color::Green);
	m_howToPlayText.setOutlineColor(sf::Color::Black);
	m_howToPlayText.setOutlineThickness(1.5);
	m_howToPlayText.setCharacterSize(19);
	m_howToPlayText.setString(
		"The board has divided into cells based\n"
		"on difficulty. The mines are randomly\n"
		"distrubuted after your first click.\n"
		"The numbers show how many mines around\n"
		"them. With that in mind, you can right\n"
		"click and flag a cell if you thing that\n"
		"cell contains a mine. To win you need\n"
		"to open all cells that doesn't contain\n"
		"a mine.\n\n\n\n"
		"Easy: 8x8 board, contains 10 mines\n"
		"Normal: 16x16 board, contains 40 mines\n"
		"Hard: 32x16 board, contains 99 mines\n"
	);
	m_howToPlayText.setPosition(
		m_creditsBottomText.getPosition().x,
		(m_window.getSize().y - m_howToPlayText.getGlobalBounds().height) / 4 + m_howToPlayText.getGlobalBounds().top + 70
	);
}

void MainWindow::mainLoop()
{
	std::srand(std::time(0));
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	while (m_window.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		draw();
	}
}

void MainWindow::draw()
{
	m_window.clear(sf::Color::Black);

	if (m_scene == Scene::MAIN_MENU)
	{
		m_window.draw(m_backgroundSprite);
		m_window.draw(m_titleSprite);
		if (getActivatedGroup() == Group::Credits)
		{
			m_window.draw(m_creditsTopText);
			m_window.draw(m_creditsBottomText);
		}
		else if (getActivatedGroup() == Group::HowToPlay)
			m_window.draw(m_howToPlayText);
	}
	else if (m_scene == Scene::IN_GAME)
	{
		for (int y = 0; y < m_table.size(); y++)
		{
			for (int x = 0; x < m_table[y].size(); x++)
			{
				m_window.draw(*m_table[y][x].getSprite());
			}
		}
	}

	m_gui.draw();
	m_window.display();
}