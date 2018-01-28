//Connect4.cpp by Sam Brind (main program functions)
#include "Classes.h"
//function to handle the main menu (returns true if AI game or false for 2 player game)
const bool menu(const sf::Texture& menu_img, sf::Sound& click_snd) {
	//create a window for menu
	sf::RenderWindow window;
	window.create(sf::VideoMode(700, 640, 32), "Connect 4", sf::Style::Close | sf::Style::Titlebar);
	window.draw(sf::Sprite(menu_img));
	window.display();
	//poll events for the window
	sf::Event UserEvent;
	while (true) {
		while (window.pollEvent(UserEvent)) {
			//mouse click
			if (UserEvent.type == sf::Event::MouseButtonPressed) {
				int x{ UserEvent.mouseButton.x }, y{ UserEvent.mouseButton.y };
				//player vs player [457, 275, 212, 69] ([L, T, W, H])
				if (x >= 457 && x <= 669 && y >= 275 && y <= 344) {
					click_snd.play();
					return false;
				}
				//player vs AI [457, 364, 212, 69]
				if (x >= 457 && x <= 669 && y >= 364 && y <= 433) {
					click_snd.play();
					return true;

				}
				//exit button [457, 547, 212, 69]
				if (x >= 457 && x <= 669 && y >= 547 && y <= 616) { exit(0); }
			}
			//close program
			if (UserEvent.type == sf::Event::Closed) { exit(0); }
		}
	}
	return false;
}
//function to play the game
void play(const bool AI, const sf::Texture &board_img, const sf::Texture &red_img, const sf::Texture &yellow_img, const sf::Texture &msg_img, const sf::Font &font, sf::Sound &click_snd, sf::Sound &clunck_snd, sf::Sound &win_snd) {
	//create a gameboard
	Gameboard board(font);
	//create a window for gameplay
	sf::RenderWindow window;
	window.create(sf::VideoMode(board.GetSize().x, board.GetSize().y, 32), "Connect 4", sf::Style::Close | sf::Style::Titlebar);
	//draw the gameboard
	board.Draw(window, board_img, red_img, yellow_img);
	//show the player's turn
	Msg_window(font, "Red goes first").Display(msg_img, 0, 0, click_snd);
	//poll events for the window
	sf::Event UserEvent; bool isOpen{ true };
	while (isOpen) {
		while (window.pollEvent(UserEvent)) {
			//mouse click
			if (UserEvent.type == sf::Event::MouseButtonPressed) {
				//decide on column clicked on by the x pos of click
				int x{ UserEvent.mouseButton.x };
				//click is on board columns area
				if (x >= 48 && x <= 650) {
					//each column image is 80px + 6px spacing
					x = 1 + (x - 48) / 86;
					//add a counter and redraw the gameboard
					board.AddCounter(x);
					clunck_snd.play();
					board.Draw(window, board_img, red_img, yellow_img);
					//end turn and check if game ended
					if (board.EndTurn(font, msg_img, click_snd, win_snd)) {
						//reset the board
						board.Reset();
						board.Draw(window, board_img, red_img, yellow_img);
						//show the player's turn
						std::string msg{ "" };
						(board.GetTurn()) ? msg = "Yellow goes first" : msg = "Red goes first";
						Msg_window(font, msg).Display(msg_img, board.GetTurn(), 0, click_snd);
					}
					//play AI turn if needed
					if (AI) {
						board.AIturn();
						clunck_snd.play();
						board.Draw(window, board_img, red_img, yellow_img);
						//end turn and check if game ended
						if (board.EndTurn(font, msg_img, click_snd, win_snd)) {
							//reset the board
							board.Reset();
							board.Draw(window, board_img, red_img, yellow_img);
							//show the player's turn
							std::string msg{ "" };
							(board.GetTurn()) ? msg = "Yellow goes first" : msg += "Red goes first";
							Msg_window(font, msg).Display(msg_img, board.GetTurn(), 0, click_snd);
						}
					}
				}
			}
			//close game and go back to menu
			if (UserEvent.type == sf::Event::Closed) {
				click_snd.play();
				isOpen = false;
			}
		}
	}
	window.close();
}
//main prgram function
int main() {
	//seed random numbers
	srand(time(NULL));
	//load program images
	sf::Texture menu_img{ loadFile<sf::Texture>("menu.png") };
	sf::Texture board_img{ loadFile<sf::Texture>("board.png") };
	sf::Texture red_img{ loadFile<sf::Texture>("red.png") };
	sf::Texture yellow_img{ loadFile<sf::Texture>("yellow.png") };
	sf::Texture msg_img{ loadFile<sf::Texture>("msg.png") };
	//load message window font
	sf::Font font{ loadFile<sf::Font>("Titillium-Bold.otf") };
	//load game sounds
	sf::SoundBuffer click{ loadFile<sf::SoundBuffer>("click.wav") };
	sf::SoundBuffer clunck{ loadFile<sf::SoundBuffer>("clunck.wav") };
	sf::SoundBuffer win{ loadFile<sf::SoundBuffer>("win.wav") };
	sf::Sound click_snd, clunck_snd, win_snd;
	click_snd.setBuffer(click);
	clunck_snd.setBuffer(clunck);
	clunck_snd.setVolume(40);
	win_snd.setBuffer(win);
	while(true) {
		//show main menu and store whether an AI game
		bool AI{ menu(menu_img, click_snd) };
		//play the game
		play(AI, board_img, red_img, yellow_img, msg_img, font, click_snd, clunck_snd, win_snd);

	}
	return 0;
	/*error return codes: 1 -> failed to load a file
				          2 -> trying to access invalid board space (out of bounds) */
}