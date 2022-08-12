#include "MainWindow.hpp"

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
		bool didHovered = false;
		
		handleMouseHovering(&m_easyShape, &m_easyText, &didHovered);
		handleMouseHovering(&m_normalShape, &m_normalText, &didHovered);
		handleMouseHovering(&m_hardShape, &m_hardText, &didHovered);
	
		if (!m_isHovering && didHovered)
		{
			m_hoverSound.play();
		}
		m_isHovering = didHovered;
	}
}

void MainWindow::handleKeyPress(sf::Keyboard::Key key)
{
}

void MainWindow::handleKeyRelease(sf::Keyboard::Key key)
{
}

void MainWindow::handleButtonPress(sf::Mouse::Button button, float x, float y)
{
	if (button == sf::Mouse::Left && m_window.hasFocus())
	{
		if (!m_isLeftClicking && m_isHovering)
		{
			m_clickSound.play();
			m_isLeftClicking = true;
		}
	}
}

void MainWindow::handleButtonRelease(sf::Mouse::Button button, float x, float y)
{
	if (button == sf::Mouse::Left)
		m_isLeftClicking = false;
}

void MainWindow::handleMouseHovering(sf::RectangleShape* buttonShape, sf::Text* buttonText, bool* didHovered)
{
	if (buttonShape->getGlobalBounds().intersects(m_mouseShape.getGlobalBounds()))
	{
		buttonShape->setFillColor(sf::Color::Red);
		buttonText->setFillColor(sf::Color::Black);
		*didHovered = true;
		m_hoveredText = buttonText->getString();
		std::cout << m_hoveredText.toAnsiString() << std::endl;
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
	makeMenuShape(m_normalShape, sf::Vector2f(256, 64 + MENUSHAPEH * (1.5f)));
	makeMenuShape(m_hardShape, sf::Vector2f(256, 64 + MENUSHAPEH * (2.5f)));

	makeMenuText(m_easyText, "EASY", m_easyShape.getPosition());
	makeMenuText(m_normalText, "NORMAL", m_normalShape.getPosition());
	makeMenuText(m_hardText, "HARD", m_hardShape.getPosition());

	m_clickSoundBuffer.loadFromMemory(clickSoundData, sizeof(clickSoundData));
	//m_hoverSoundBuffer.loadFromMemory(hoverSoundData, sizeof(hoverSoundData));
	m_clickSound.setBuffer(m_clickSoundBuffer);
	m_hoverSound.setBuffer(m_hoverSoundBuffer);
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

	}
	
	m_window.display();
}