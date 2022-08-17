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
		case sf::Event::KeyPressed:
			handleKeyPress(e.key.code);
			break;
		case sf::Event::KeyReleased:
			handleKeyRelease(e.key.code);
			break;
		case sf::Event::MouseButtonPressed:
			handleButtonPress(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
			break;
		case sf::Event::MouseButtonReleased:
			handleButtonRelease(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
			break;
		case sf::Event::MouseWheelScrolled:
			handleWheelScroll(e.mouseWheelScroll.delta, e.mouseWheelScroll.wheel, e.mouseWheelScroll.x, e.mouseWheelScroll.y);
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
		
		bool foo = false;
		foo += handleMouseHovering(&m_easyShape, &m_easyText);
		foo += handleMouseHovering(&m_normalShape, &m_normalText);
		foo += handleMouseHovering(&m_hardShape, &m_hardText);
		
		if (!foo) m_isHovering = false;
		else m_isHovering += foo;
	}
}

void MainWindow::handleKeyPress(sf::Keyboard::Key key)
{
}

void MainWindow::handleKeyRelease(sf::Keyboard::Key key)
{
}

void MainWindow::handleWheelScroll(int delta, sf::Mouse::Wheel wheel, int x, int y)
{
#ifdef _DEBUG
	int _x = x / m_cellSize;
	int _y = y / m_cellSize;
	Cell* cellPtr = &m_table[_y][_x];
	CellState state = cellPtr->getState();
	state = (CellState)(state + delta);
	if (state < 0) state = (CellState)(CellState::__SIZE - 1);
	else if (state >= CellState::__SIZE) state = (CellState) 0;
	cellPtr->setState(state);
#endif // _DEBUG
}

void MainWindow::handleButtonPress(sf::Mouse::Button button, int x, int y)
{
	if (!m_window.hasFocus()) return;
	if (button == sf::Mouse::Left)
	{
		if (!m_isLeftClicking && m_isHovering && m_scene == Scene::MAIN_MENU)
		{
			m_clickSound.play();
			if (m_easyShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
				switchToInGame(10, { 8, 8 }, 64, 1);
			else if (m_normalShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
				switchToInGame(40, { 16, 16 }, 32, 2);
			else if (m_hardShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
				switchToInGame(99, { 32, 16 }, 32, 3);
		}
		else if (!m_isLeftClicking && m_scene == Scene::IN_GAME)
		{
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;

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
				if (!m_youWinMsgBox->isEnabled()) m_winSound.play();
				m_youLostMsgBox->setEnabled(false);
				m_youLostMsgBox->setVisible(false);
				m_youWinMsgBox->setEnabled(true);
				m_youWinMsgBox->setVisible(true);
				return;
			}
			if (m_youLost)
			{
				if(!m_youLostMsgBox->isEnabled()) m_loseSound.play();
				m_youLostMsgBox->setEnabled(true);
				m_youLostMsgBox->setVisible(true);
				m_youWinMsgBox->setEnabled(false);
				m_youWinMsgBox->setVisible(false);
				return;
			}
		}
		m_isLeftClicking = true;
	}
	else if (button == sf::Mouse::Right)
	{
		if (!m_isRightClicking && m_scene == Scene::IN_GAME)
		{
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;
			m_table[_y][_x].setFlagged(!m_table[_y][_x].getFlagged());
		}
		m_isRightClicking = true;
	}
	else if (button == sf::Mouse::Middle)
	{
		m_isMiddleClicking = true;
	}
}

void MainWindow::handleButtonRelease(sf::Mouse::Button button, int x, int y)
{
	if (button == sf::Mouse::Left)
		m_isLeftClicking = false;
	else if (button == sf::Mouse::Right)
		m_isRightClicking = false;
	else if (button == sf::Mouse::Middle)
		m_isMiddleClicking = false;
}

bool MainWindow::handleMouseHovering(sf::RectangleShape* buttonShape, sf::Text* buttonText)
{
	if (buttonShape->getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
	{
		buttonShape->setFillColor(sf::Color::Red);
		buttonText->setFillColor(sf::Color::Black);
		if (!m_isHovering) m_hoverSound.play();
		return true;
	}
	else
	{
		buttonShape->setFillColor(sf::Color::Black);
		buttonText->setFillColor(sf::Color::Red);
		return false;
	}
}

void MainWindow::switchToMainMenu()
{
	m_scene = Scene::MAIN_MENU;
	m_youLost = false;
	m_youWin = false;
	m_table.clear();
	m_isFirstClick = true;
	m_window.setSize({ WINDOWSIZEX, WINDOWSIZEY });
	m_youLostMsgBox->setVisible(false);
	m_youLostMsgBox->setEnabled(false);
	m_youWinMsgBox->setVisible(false);
	m_youWinMsgBox->setEnabled(false);
	m_mainMenuGroup->setVisible(true);
	m_mainMenuGroup->setEnabled(true);

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

	m_mainMenuGroup->setVisible(false);
	m_mainMenuGroup->setEnabled(false);

	m_scene = Scene::IN_GAME;
	m_isHovering = false;
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
	if (buttonText == "Menu")
		switchToMainMenu();
	else
	{
		debugPrint(buttonText.toStdString());
	}
}

void MainWindow::makeMenuShape(sf::RectangleShape& shape, sf::Vector2f position)
{
	shape.setSize(sf::Vector2f(MENUSHAPEW, MENUSHAPEH));
	shape.setFillColor(sf::Color::Black);
	shape.setOrigin(shape.getSize().x / 2.f, shape.getSize().y / 2.f);
	shape.setPosition(position);
}

void MainWindow::makeMenuText(sf::Text& text, sf::String str, sf::Vector2f position)
{
	text.setFont(m_font);
	text.setString(str);
	text.setCharacterSize(45);
	text.setFillColor(sf::Color::Red);
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(
		textRect.left + textRect.width / 2.0f, 
		textRect.top + textRect.height / 2.0f
	);
	text.setPosition(position);
}

MainWindow::MainWindow()
{
	m_window.create(sf::VideoMode(512, 512), "Minesweeper", sf::Style::Close);
	m_window.setFramerateLimit(FPS);
	m_gui.setTarget(m_window);

	m_mainMenuGroup = tgui::Group::create();
	m_gui.add(m_mainMenuGroup);

	m_youLostMsgBox = tgui::MessageBox::create("Game Over", "You Lost", { "Menu" });
	m_youLostMsgBox->onButtonPress(&MainWindow::handleTguiButtonPress, this);
	m_youLostMsgBox->setVisible(false);
	m_youLostMsgBox->setEnabled(false);

	m_youWinMsgBox = tgui::MessageBox::create("Game Over", "You Win!", { "Menu" });
	m_youWinMsgBox->onButtonPress(&MainWindow::handleTguiButtonPress, this);
	m_youWinMsgBox->setVisible(false);
	m_youWinMsgBox->setEnabled(false);

	m_gui.add(m_youLostMsgBox, "YouLostMsgBox");
	m_gui.add(m_youWinMsgBox, "YouWinMsgBox");

	m_font.loadFromMemory(&fontData, sizeof(fontData));

	makeMenuShape(m_easyShape, sf::Vector2f(256, 64 + MENUSHAPEH * (.5f)));
	makeMenuShape(m_normalShape, sf::Vector2f(256, 64 + MENUSHAPEH * (1.5f) + 10));
	makeMenuShape(m_hardShape, sf::Vector2f(256, 64 + MENUSHAPEH * (2.5f) + 20));

	makeMenuText(m_easyText, "EASY", m_easyShape.getPosition());
	makeMenuText(m_normalText, "NORMAL", m_normalShape.getPosition());
	makeMenuText(m_hardText, "HARD", m_hardShape.getPosition());

	m_clickSoundBuffer.loadFromMemory(clickSoundData, sizeof(clickSoundData));
	m_hoverSoundBuffer.loadFromMemory(hoverSoundData, sizeof(hoverSoundData));
	m_winSoundBuffer.loadFromMemory(winSoundData, sizeof(winSoundData));
	m_loseSoundBuffer.loadFromMemory(loseSoundData, sizeof(loseSoundData));
	m_clickSound.setBuffer(m_clickSoundBuffer);
	m_hoverSound.setBuffer(m_hoverSoundBuffer);
	m_winSound.setBuffer(m_winSoundBuffer);
	m_loseSound.setBuffer(m_loseSoundBuffer);
	m_textures32.loadFromMemory(sprites32Data, sizeof(sprites32Data));
	m_textures64.loadFromMemory(sprites64Data, sizeof(sprites64Data));
}

MainWindow::~MainWindow()
{
	
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
		m_window.draw(m_easyShape);
		m_window.draw(m_normalShape);
		m_window.draw(m_hardShape);

		m_window.draw(m_easyText);
		m_window.draw(m_normalText);
		m_window.draw(m_hardText);
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