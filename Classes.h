//Classes.h by Sam Brind (class definitions)
#ifndef CONNECT4_H
#define CONNECT4_H
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <string>
#include <utility>
//template to load a file e.g. textures,fonts
template <typename T>
T loadFile(const std::string filepath) {
	T temp;
	//try and load the file, otherwise display an error
	if (!temp.loadFromFile(filepath)) {
		std::cerr << "Error: can't load '"<< filepath << "'!" << std::endl;
		exit(1);
	}
	return temp;
}
//message window class
class Msg_window {
private:
	const unsigned int screen_height{ 100 };
	const unsigned int screen_width{ 320 };
	sf::Text message;
public:
	Msg_window(const sf::Font& font, const std::string msg = "");
	~Msg_window() { std::cout << "Destroying message window..." << std::endl; }
	void Msg_window::Display(const sf::Texture &msg_img, bool colour, bool neutral, sf::Sound &click_snd);
	const sf::Vector2u GetSize() const;
};
//counter class
class Counter {
private:
	const bool colour; //counter colour (0 -> Red , 1 -> Yellow)
	const int column, row;
public:
	Counter(const bool colour_in = 0, const int column_in = 1, const int row_in = 1):
		colour{ colour_in }, column{ column_in }, row{ row_in } {}
	~Counter() { std::cout << "Destroying counter..." << std::endl; }
	void Draw(sf::RenderWindow &window, const sf::Texture &red_img, const sf::Texture &yellow_img);
	const bool GetColour() const;
	const sf::Vector2i GetPosition() const;
};
//gameboard class
class Gameboard {
private:
	const unsigned int screen_height{ 640 };
	const unsigned int screen_width{ 700 };
	bool turn; //player turn [0 -> player1 (R) , 1 -> player2(Y)]
	std::vector<std::vector<Counter>> counters; //vector of columns (which are vectors of counters)
	//player scores
	unsigned int player1;
	unsigned int player2;
	//text for player scores
	sf::Text p1_text;
	sf::Text p2_text;
	sf::Text space_text;
	//no of rows is board height and no of columns is board width
	const int board_height{ 6 }; const int board_width{ 7 };
public:
	Gameboard(const sf::Font &font);
	~Gameboard() { std::cout << "Destroying gameboard..." << std::endl; }
	void AddCounter(int column);
	void AIturn();
	const bool Gameboard::CheckSpace(int column, int row, int x, int y) const;
	const bool CheckHorizontal(const Counter &counter) const;
	const bool CheckDiagonal(const Counter &counter) const;
	const bool CheckVertical(const std::vector<Counter> &column) const;
	const bool CheckWin() const;
	void Draw(sf::RenderWindow &window, const sf::Texture &board_img, const sf::Texture &red_img, const sf::Texture &yellow_img);
	const bool EndTurn(const sf::Font &font, const sf::Texture &msg_img, sf::Sound &click_snd, sf::Sound &win_snd);
	const int FindRow(int column) const;
	const sf::Vector2u GetSize() const;
	const bool GetTurn() const;
	void Reset();
	void SwapTurn() { turn = !turn; }
};
#endif