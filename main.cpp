#include <SFML/Graphics.hpp>

const int rows = 20;									// Rows
const int cols = 10;									// Colums
int score = 0;
int board[rows][cols] = { 0 };							// Board for the blocks

struct Point { int x, y; } a[4], b[4];					// Point array for single block piece

int blocks[7][4] =
{
	1,3,5,7, // I		
	2,4,5,7, // Z	  Blocks:	
	3,5,4,6, // S		0, 1
	3,5,4,7, // T		2, 3
	2,3,5,7, // L		4, 5
	3,5,7,6, // J		6, 7
	2,3,4,5, // O
};

bool check()
{
	for (int i = 0; i < 4; i++)
		if (a[i].x < 0 || a[i].x >= cols || a[i].y >= rows)			 // Collision (New block -> sides of play area)
		{
			return 0;
		}
		else if (board[a[i].y][a[i].x] != 0)						// Collision (New block -> already placed block)
		{
			return 0;
		}
	return 1;
};

void randomiseBlock()
{
	int n = rand() % 7;					// Create a random number
	for (int i = 0; i < 4; i++)			
	{
		a[i].x = blocks[n][i] % 2;
		a[i].y = blocks[n][i] / 2;
	}
}


int main() {
	srand((unsigned int)time(NULL));								// Random seed for the random function

	// Create an Window 320x480 with Frame Rate Cap of 60
	sf::RenderWindow window(sf::VideoMode(320, 480), "Tetris Clone", sf::Style::Close);
	window.setFramerateLimit(60);	

	sf::Texture t1, t2, t3;																							// Creates textures for blocks, background and frame
	if (!t1.loadFromFile("images/tiles.png")) { throw("ERROR::Could Not Load image [tiles.png]!"); }				// Loads from file else throw error
	if (!t2.loadFromFile("images/background.png")) { throw("ERROR::Could Not Load image [background.png]!"); }		// Loads from file else throw error
	if (!t3.loadFromFile("images/frame.png")) { throw("ERROR::Could Not Load image [frame.png]!"); }				// Loads from file else throw error
	sf::Sprite block(t1), background(t2), frame(t3);																// Applies the textures to sprites

	int dx = 0;							// Direction X (Movement)
	int colorNum = 1;					// Give the blocks colour
	bool rotate = false;				// Boolean to allow block rotation
	float timer = 0;					// Set a timer to 0
	float delay = .5f;					// Set a delay amount
	sf::Clock clock;					// Creates a clock to store time passed

	randomiseBlock();					// Randmise the first block in the game

	// run the program as long as the window is open
	while (window.isOpen())
	{
		float time = clock.getElapsedTime().asSeconds();						// Stores the time that has passed in seconds
		clock.restart();														// Restarts the clock
		timer += time;															// Adds the time that has passed to the timer

		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) { window.close(); }				// Closes the window

			if (event.type == sf::Event::KeyPressed)								// Checks for key presses
			{
				if (event.key.code == sf::Keyboard::Up) { rotate = true; }			// Turns the rotate boolean to true / allows rotation
				else if (event.key.code == sf::Keyboard::Left) { dx = -1; }			// Sets direction to the left
				else if (event.key.code == sf::Keyboard::Right) { dx = 1; }			// Sets direction to the right
			}
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) delay = 0.1f;			// Soft drop (push block down)

		// - Movement - //
		for (int i = 0; i < 4; i++)
		{
			b[i] = a[i];
			a[i].x += dx;											// Move block based on direction
		}
		if (!check())
		{
			for (int i = 0; i < 4; i++) { a[i] = b[i]; }			// Check for any sort of collisions
		}

		// - Rotation - //
		if (rotate)
		{
			Point p = a[1];											// Center of rotation
			for (int i = 0; i < 4; i++)
			{
				int x = a[i].y - p.y;
				int y = a[i].x - p.x;								// Get X, Y to rotate around
				a[i].x = p.x - x;
				a[i].y = p.y + y;									// Apply Rotation
			}
			if (!check())
			{
				for (int i = 0; i < 4; i++) { a[i] = b[i]; }		// Check for collisions
			}
		}

		// - Clock Tick - //
		if (timer > delay)
		{
			for (int i = 0; i < 4; i++) 
			{ 
				b[i] = a[i];				// Store the blocks data in a second array
				a[i].y += 1;				// Move block down on the Y-axis
			}

			if (!check())
			{
				for (int i = 0; i < 4; i++) { board[b[i].y][b[i].x] = colorNum; }	// Give colour

				colorNum = 1 + rand() % 7;											// Randomise a new colour
				randomiseBlock();
			}
			timer = 0;			// Set timer back to 0
		}

		// - Check for line clear - //
		int k = rows - 1;
		for (int i = rows - 1; i > 0; i--)
		{
			int count = 0;
			for (int j = 0; j < cols; j++)
			{
				if (board[i][j]) { count++;	}		// Count each block on a line
				board[k][j] = board[i][j];
			}
			if (count < cols){ k--;	}				// Clear line
		}
											
		dx = 0; 
		rotate = false;								// Reset Values back to default
		delay = 0.3f;

		// - Drawing - //
		window.clear(sf::Color::Black);				// Clear the window
		window.draw(background);					// Draw background

		// - Placed Blocks - //
		for (int i = 0; i < rows; i++) {

			for (int j = 0; j < cols; j++)
			{
				if (board[i][j] == 0) { continue; }									// If empty, continue
				block.setTextureRect(sf::IntRect(board[i][j] * 18, 0, 18, 18));		// Set the sub-rect texture size
				block.setPosition(j * 18.f, i * 18.f);								// Set position of the placed block
				block.move(28, 32);													// Position Offset
				window.draw(block);													// Draw the placed block
			}
		}

		// - Current/New block -//
		for (int i = 0; i < 4; i++)
		{
			block.setTextureRect(sf::IntRect(colorNum * 18, 0, 18, 18));			// Set the sub-rect texture size
			block.setPosition(a[i].x * 18.f, a[i].y * 18.f);						// Set the blocks position
			block.move(28, 32);														// Position Offset
			window.draw(block);														// Draw the block
		}

		window.draw(frame);															// Draw the frame
		window.display();					
	}
	return 0;
}