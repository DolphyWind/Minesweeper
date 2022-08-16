#pragma once
#include <SFML/Graphics.hpp>
#define maskPos(x, y) { mask.left = x * m_cellSize; mask.top = y * m_cellSize;}

enum CellState
{
	EMPTY = 0,
	NUMBER_1,
	NUMBER_2,
	NUMBER_3,
	NUMBER_4,
	NUMBER_5,
	NUMBER_6,
	NUMBER_7,
	NUMBER_8,
	MINE,
	__SIZE,
};

class Cell
{
private:
	bool m_isFlagged;
	bool m_isActivatedMine;
	bool m_isHidden;
	int m_cellSize;

	CellState m_state;
	sf::Texture* m_tileTexture;
	sf::Sprite m_tileSprite;
public:
	Cell(sf::Texture *texturePtr, int cellSize, CellState state = CellState::EMPTY);
	sf::Sprite* getSprite();

	void setState(CellState newState);
	CellState getState();
	void setFlagged(bool isFlagged);
	bool getFlagged();
	void setActivatedMine(bool isActivatedMine);
	bool getActivatedMine();
	void setHidden(bool isHidden);
	bool getHidden();
	void setPosition(sf::Vector2f pos);
	void setPosition(float x, float y);
	sf::Vector2f getPosition();

	void updateCellSprite();
	void open(int _y, int _x, std::vector<std::vector<Cell>>* table);
};

