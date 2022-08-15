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
		m_isHovering = false;
		
		handleMouseHovering(&m_easyShape, &m_easyText);
		handleMouseHovering(&m_normalShape, &m_normalText);
		handleMouseHovering(&m_hardShape, &m_hardText);
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
			bool didStartedGame = false;
			if (m_easyShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
			{
				m_mineCount = 10;
				m_tableSize = sf::Vector2f(8, 8);
				m_cellSize = 64;
				m_texturePtr = &m_textures64;
				didStartedGame = true;
				debugPrint("Difficulty: Easy\n");
			}
			else if (m_normalShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
			{
				m_mineCount = 60;
				m_tableSize = sf::Vector2f(16, 16);
				m_cellSize = 32;
				m_texturePtr = &m_textures32;
				didStartedGame = true;
				debugPrint("Difficulty: Normal\n");
			}
			else if (m_hardShape.getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
			{
				m_mineCount = 125;
				m_tableSize = sf::Vector2f(32, 16);
				m_cellSize = 32;
				m_texturePtr = &m_textures32;
				didStartedGame = true;
				debugPrint("Difficulty: Hard\n");
			}

			if (didStartedGame)
			{
				m_scene = Scene::IN_GAME;
				m_isHovering = false;
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
		}
		else if (!m_isLeftClicking && m_scene == Scene::IN_GAME)
		{
#ifdef _DEBUG
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;
			m_table[_y][_x].setHidden(!m_table[_y][_x].getHidden());
#endif // _DEBUG
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
		if (!m_isMiddleClicking)
		{
#ifdef _DEBUG
			int _x = x / m_cellSize;
			int _y = y / m_cellSize;
			m_table[_y][_x].setActivatedMine(!m_table[_y][_x].getActivatedMine());
#endif // _DEBUG
		}
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

void MainWindow::handleMouseHovering(sf::RectangleShape* buttonShape, sf::Text* buttonText)
{
	if (buttonShape->getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
	{
		buttonShape->setFillColor(sf::Color::Red);
		buttonText->setFillColor(sf::Color::Black);
		m_isHovering = true;
	}
	else
	{
		buttonShape->setFillColor(sf::Color::Black);
		buttonText->setFillColor(sf::Color::Red);
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

	m_font.loadFromMemory(&fontData, sizeof(fontData));

	makeMenuShape(m_easyShape, sf::Vector2f(256, 64 + MENUSHAPEH * (.5f)));
	makeMenuShape(m_normalShape, sf::Vector2f(256, 64 + MENUSHAPEH * (1.5f) + 10));
	makeMenuShape(m_hardShape, sf::Vector2f(256, 64 + MENUSHAPEH * (2.5f) + 20));

	makeMenuText(m_easyText, "EASY", m_easyShape.getPosition());
	makeMenuText(m_normalText, "NORMAL", m_normalShape.getPosition());
	makeMenuText(m_hardText, "HARD", m_hardShape.getPosition());

	m_clickSoundBuffer.loadFromMemory(clickSoundData, sizeof(clickSoundData));
	//m_hoverSoundBuffer.loadFromMemory(hoverSoundData, sizeof(hoverSoundData));
	m_clickSound.setBuffer(m_clickSoundBuffer);
	m_hoverSound.setBuffer(m_hoverSoundBuffer);
	m_textures32.loadFromMemory(sprites32Data, sizeof(sprites32Data));
	m_textures64.loadFromMemory(sprites64Data, sizeof(sprites64Data));
}

void MainWindow::mainLoop()
{
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
	
	m_window.display();
}