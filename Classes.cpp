//Classes.cpp by Sam Brind (class function implementation)
#include "Classes.h"
//message window member functions
//constructor
Msg_window::Msg_window(const sf::Font &font, const std::string msg) {
	//set the font and string of the msg text
	message.setFont(font);
	message.setString(msg);
}
//function to display a message window and then wait for user input to close
void Msg_window::Display(const sf::Texture &msg_img, const bool colour, const bool neutral, sf::Sound &click_snd) {
	sf::RenderWindow window;
	window.create(sf::VideoMode(screen_width, screen_height, 32), "Connect 4", sf::Style::Close | sf::Style::Titlebar);
	//draw the background image
	window.draw(sf::Sprite(msg_img));
	//set text colour (R/Y) based on colour or dark blue if neutral message
	(colour) ? message.setFillColor(sf::Color(234,187,4)) : message.setFillColor(sf::Color(191,24,43));
	if (neutral) { message.setFillColor(sf::Color(6, 43, 117)); }
	//position text centrally
	message.setPosition((screen_width - message.getLocalBounds().width)/2, 10);
	//draw text and display the window
	window.draw(message);
	window.display();
	//poll events for the window
	sf::Event UserEvent; bool isOpen{ true };
	while (isOpen) {
		while (window.pollEvent(UserEvent)) {
			//mouse click event
			if (UserEvent.type == sf::Event::MouseButtonPressed) {
				//check if click is within the ok button
				int x{ UserEvent.mouseButton.x }, y{ UserEvent.mouseButton.y };
				if (x >= 83 && x <= 217 && y >= 56 && y <= 89) { isOpen = 0; }
			}
			//pressed the close button
			if (UserEvent.type == sf::Event::Closed) { isOpen = 0; }
		}
	}
	click_snd.play();
	window.close();
}
//function to get window size
const sf::Vector2u Msg_window::GetSize() const { return sf::Vector2u(screen_width, screen_height); }

//counter member functions
//function to draw the counter to a window
void Counter::Draw(sf::RenderWindow &window, const sf::Texture &red_img, const sf::Texture &yellow_img) {
	sf::Sprite counter;
	//set texture depending on counter colour
	(colour) ? counter.setTexture(yellow_img) : counter.setTexture(red_img);
	//set counter position (counter images are 80x80px and spaced 6px apart)
	sf::Vector2i pos(51 + 86 * (column - 1), 118 + 86 * (6 - row));
	counter.setPosition(sf::Vector2f(pos));
	//draw counter to the window
	window.draw(counter);
}
//function to get the counter's colour
const bool Counter::GetColour() const { return colour; }
//function to get the counter's position
const sf::Vector2i Counter::GetPosition() const { return sf::Vector2i(column, row); }

//gameboard member functions
//constructor
Gameboard::Gameboard(const sf::Font &font) : turn{ 0 }, player1{ 0 }, player2{ 0 }, counters {} {
	//loop over each column of the board
	for (int i{ 0 }; i < board_width; i++) {
		//push back an empty column
		counters.push_back(std::move(std::vector<Counter>()));
	}
	//set the font and style of scoreboard text
	p1_text.setFont(font);
	p2_text.setFont(font);
	space_text.setFont(font);
	p1_text.setFillColor(sf::Color(191, 24, 43));
	p2_text.setFillColor(sf::Color(234, 187, 4));
	space_text.setFillColor(sf::Color(6, 43, 117));
	space_text.setString("  -  ");
	p1_text.setPosition(8,8);
}
//function to make an AI turn
void Gameboard::AIturn() {
	//generate a random move (with greater probability closer to placed counters)
	unsigned int rand_col;
	unsigned int prob[7] = { 100, 100, 100, 100, 100, 100, 100 };
	unsigned int total{ 700 };
	//set up probabilities from sums [p(x) = 1*1.08^x.size]
	for (int i{ 0 }; i < board_width; i++) {
		prob[i] *= pow(1.08, counters[i].size());
		for (int j{ 0 }; j < board_width; j++) {
			if (i != j) { prob[j] /= pow(1.08, counters[i].size()); }
		}
	}
	total = prob[0] + prob[1] + prob[2] + prob[3] + prob[4] + prob[5] + prob[6];
	//loop until random column is valid (not full)
	do {
		//generate random number
		do {
			rand_col = rand();
		} while (rand_col >= (RAND_MAX - RAND_MAX % total));
		rand_col %= total;
		//convert to a column number 1-7
		if (rand_col > prob[0] + prob[1] + prob[2] + prob[3] + prob[4] + prob[5]) { rand_col = 7; } else {
		if (rand_col > prob[0] + prob[1] + prob[2] + prob[3] + prob[4]) { rand_col = 6; } else {
		if (rand_col > prob[0] + prob[1] + prob[2] + prob[3]) { rand_col = 5; } else {
		if (rand_col > prob[0] + prob[1] + prob[2]) { rand_col = 4; } else {
		if (rand_col > prob[0] + prob[1]) { rand_col = 3; } else {
		if (rand_col > prob[0]) { rand_col = 2; } else { rand_col = 1; }}}}}}
	} while (counters[rand_col - 1].size() == board_height);
	AddCounter(rand_col);
}
//function to add a counter to the board
void Gameboard::AddCounter(const int column) {
	//check for invalid column number
	if (column < 1 || column > board_width) {
		std::cerr << "Error: Trying to access invalid board spaces!" << std::endl;
		exit(2);
	}
	//check column isn't full
	int row{ FindRow(column) };
	if (row <= board_height) {
		//push the counter onto the column
		counters[column - 1].push_back(std::move(Counter(turn, column, row)));
		//swap the player turn
		SwapTurn();
	}
}
//function to check if a counter space, displaced by (x,y), is the same colour as the parameter counter
const bool Gameboard::CheckSpace(const int column, const int row, const int x, const int y) const {
	//determine how many columns are to the left and to the right of starting counter
	int left{ column - 1 }, right{ board_width - column };
	//determine how many rows are above and below the starting counter
	int top{ board_height - row }, bottom{ row - 1 };
	//check the displaced column and displaced row are within the board
	if (((x <= 0 && std::abs(x) <= left) || (x > 0 && std::abs(x) <= right))&& ((y <= 0 && std::abs(y) <= bottom) || (y > 0 && std::abs(y) <= top))) {
		//check there are sufficient counters in that column
		if (FindRow(column + x) > row + y) {
			//check whether displaced counter is matching colour
			if (counters[column - 1 + x][row - 1 + y].GetColour() == counters[column - 1][row - 1].GetColour()) { return true; }
		}
	}
	return false;
}
//funtion to check for a horizontal win from a starting counter
const bool Gameboard::CheckHorizontal(const Counter &counter) const {
	int count{ 1 };
	//loop over possible displacements (3 right)
	for (int i{ 1 }; i <= 3; i++) {
		//check space i to the right:
		//	0 if it blocks a horizontal or 1 if it contributes to a horizontal
		bool space{ CheckSpace(counter.GetPosition().x, counter.GetPosition().y, i, 0) };
		count = (count*space) + space;
		if (count == 0) { return false; }
		if (count == 4) { return true; }
	}
	return false;
}
//function to check for a diagonal win from a starting counter
const bool Gameboard::CheckDiagonal(const Counter &counter) const{
	int up_count{ 1 }, down_count{ 1 };
	//loop over possible displacements (3 right, 3 across/down)
	for (int i{ 1 }; i <= 3; i++) {
		//check space i to the right and i up (/ diagonal):
		bool space{ CheckSpace(counter.GetPosition().x, counter.GetPosition().y, i, i) };
		up_count = (up_count*space) + space;
		//check space i to the right and i down (\ diagonal):
		space = CheckSpace(counter.GetPosition().x, counter.GetPosition().y, i, -i);
		down_count = (down_count*space) + space;
		if (up_count == 0 && down_count == 0) { return false; }
		if (up_count == 4 || down_count == 4) { return true; }
	}
	return false;
}
//function to check for a vertical win within a column
const bool Gameboard::CheckVertical(const std::vector<Counter> &column) const {
	//pointless checking, if there are less than 4 counters
	if (column.size() < 4) { return false; }
	int count{ 1 }; bool colour{ column[0].GetColour() };
	//loop over the column
	for (auto it{ column.begin() + 1 }; it != column.end(); it++) {
		//if colour is the same, increment count, otherwise reset the count/colour
		if(colour == it->GetColour()) {
			count++;
		} else {
			colour = !colour; count = 1;
		}
		if (count == 4) { return true; }
	}
	return false;
}
//function to check for a win
const bool Gameboard::CheckWin() const {
	//loop through the board's columns
	for (auto col{ counters.begin() }; col != counters.end(); col++) {
		//check for a vertical win in the column
		if (CheckVertical(*col)) { return true; }
		//loop through the counters in the column
		for (auto it{ col->begin() }; it != col->end(); it++) {
			//check for a horizontal win
			if (CheckHorizontal(*it)) { return true; }
			//check for a diagonal win
			if (CheckDiagonal(*it)) { return true; }
		}
	}
	return false;
}
//function to draw the gameboard to a window
void Gameboard::Draw(sf::RenderWindow &window, const sf::Texture &board_img, const sf::Texture &red_img, const sf::Texture &yellow_img) {
	window.clear();
	//draw background image
	window.draw(sf::Sprite(board_img));
	//loop through the counters and draw each one
	for (auto col{ counters.begin() }; col != counters.end(); col++) {
		for (auto it{ col->begin() }; it != col->end(); it++) {
			it->Draw(window, red_img, yellow_img);
		}
	}
	//draw the scoreboard text
	p1_text.setString("Player1 (" + std::to_string(player1) + ")");
	p2_text.setString("(" + std::to_string(player2) + ") Player2");
	space_text.setPosition(8 + p1_text.getLocalBounds().width, 8);
	p2_text.setPosition(8 + p1_text.getLocalBounds().width + space_text.getLocalBounds().width, 8);
	window.draw(p1_text);
	window.draw(space_text);
	window.draw(p2_text);
	window.display();
}
//function to end the player's turn (returns true if turn has ended game)
const bool Gameboard::EndTurn(const sf::Font &font, const sf::Texture &msg_img, sf::Sound &click_snd, sf::Sound &win_snd) {
	std::string msg{ "" }; bool draw{ false };
	//calculate the total no of counters on the board
	unsigned int total{ counters[0].size() + counters[1].size() + counters[2].size() + counters[3].size() + counters[4].size() + counters[5].size() + counters[6].size() };
	//check for a draw
	if (total == 42) {
		msg = "Game was a draw!";
		draw = true;
		std::cout << "Draw has occurred!" << std::endl;
	}
	//check for a win
	if (CheckWin()) {
		(turn) ? msg = "Red has won!" : msg = "Yellow has won!";
		//increment player scores
		player1 += turn; player2 += !turn;
		std::cout << "Player" << !turn + 1 << " has won" << std::endl;
		win_snd.play();
	}
	//check if the game has been ended and hence display the endgame message window
	if (msg != "") {
		Msg_window(font, msg).Display(msg_img, !turn, draw, click_snd);
		return true;
	}
	return false;
}
//function to find the row number that is free for a column
const int Gameboard::FindRow(const int column) const {
	//check for invalid column number
	if (column < 1 || column > board_width) {
		std::cerr << "Error: Trying to access invalid board spaces!" << std::endl;
		exit(2);
	}
	return (counters[column - 1].size() + 1);
}
//function to get window size
const sf::Vector2u Gameboard::GetSize() const { return sf::Vector2u(screen_width, screen_height); }
//function to get the player turn
const bool Gameboard::GetTurn() const { return turn; }
//function to reset the gameboard
void Gameboard::Reset() {
	//clear the counters vector
	counters.clear();
	//loop over each column of the board
	for (int i{ 0 }; i < board_width; i++) {
		//push back an empty column
		counters.push_back(std::move(std::vector<Counter>()));
	}
}
