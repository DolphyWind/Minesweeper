#include "Cell.h"

Cell::Cell(sf::Texture *texturePtr, int cellSize, CellState state)
{
	m_isFlagged = false;
	m_isHidden = true;
	m_state = state;
	m_tileTexture = texturePtr;
	m_cellSize = cellSize;
	m_tileSprite.setTexture(*m_tileTexture);
	updateCellSprite();
}

sf::Sprite* Cell::getSprite()
{
	return &m_tileSprite;
}

void Cell::setState(CellState newState)
{
	m_state = newState;
	updateCellSprite();
}

CellState Cell::getState()
{
	return m_state;
}

void Cell::setFlagged(bool isFlagged)
{
	if (!m_isHidden) return;
	m_isFlagged = isFlagged;
	updateCellSprite();
}

bool Cell::getFlagged()
{
	return m_isFlagged;
}

void Cell::setActivatedMine(bool isActivatedMine)
{
	if (m_state != CellState::MINE) return;
	m_isActivatedMine = isActivatedMine;
	updateCellSprite();
}

bool Cell::getActivatedMine()
{
	return m_isActivatedMine;;
}

void Cell::setHidden(bool isHidden)
{
	if (m_isFlagged) return;
	m_isHidden = isHidden;
	updateCellSprite();
}

bool Cell::getHidden()
{
	return m_isHidden;
}

void Cell::setPosition(sf::Vector2f pos)
{
	m_tileSprite.setPosition(pos);
}

void Cell::setPosition(float x, float y)
{
	m_tileSprite.setPosition(x, y);
}

sf::Vector2f Cell::getPosition()
{
	return m_tileSprite.getPosition();
}

void Cell::updateCellSprite()
{
	sf::IntRect mask;
	mask.width = m_cellSize;
	mask.height = m_cellSize;
	if (m_isHidden)
	{
		if (m_isFlagged) maskPos(1, 0)
		else maskPos(0, 0)
	}
	else
	{
		if (m_state == CellState::EMPTY) maskPos(2, 0)
		else if (m_state == CellState::MINE && !m_isActivatedMine) maskPos(3, 0)
		else if (m_state == CellState::MINE && m_isActivatedMine) maskPos(4, 0)
		else if (m_state == CellState::NUMBER_1) maskPos(5, 0)
		else if (m_state == CellState::NUMBER_2) maskPos(6, 0)
		else if (m_state == CellState::NUMBER_3) maskPos(0, 1)
		else if (m_state == CellState::NUMBER_4) maskPos(1, 1)
		else if (m_state == CellState::NUMBER_5) maskPos(2, 1)
		else if (m_state == CellState::NUMBER_6) maskPos(3, 1)
		else if (m_state == CellState::NUMBER_7) maskPos(4, 1)
		else if (m_state == CellState::NUMBER_8) maskPos(5, 1)
	}
	m_tileSprite.setTextureRect(mask);
}

void Cell::open(int _y, int _x, std::vector<std::vector<Cell>>* table)
{
	if (!m_isHidden) return;
	setHidden(false);
	if (this->getState() != CellState::EMPTY) return;
	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			try { table->at(_y + y).at(_x + x).open(_y + y, _x + x, table); }
			catch (std::exception&) {}
		}
	}
}
