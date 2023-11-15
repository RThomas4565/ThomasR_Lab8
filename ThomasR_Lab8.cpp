#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <SFML/Graphics.hpp>

// Function to clear the console screen
void clearScreen() {
#ifdef _WIN32
    std::system("cls");
#else
    std::system("clear");
#endif
}

class Arrow {
public:
    float x, y;               // Arrow position
    bool isActive;            // Whether the arrow is currently on the screen
    sf::Texture texture;      // Arrow texture
    sf::Sprite sprite;        // Arrow sprite

    Arrow(float startX, float startY, const sf::Texture& arrowTexture) : x(startX), y(startY), isActive(true) {
        sprite.setTexture(arrowTexture);
        sprite.setPosition(x, y);
    }

    // Update method to handle arrow movement
    void update() {
        // Double the arrow's speed (change the value as needed)
        y -= 80; // Adjust the speed as needed
        sprite.setPosition(x, y);

        // Deactivate the arrow if it goes off the top of the screen
        if (y < 0) {
            isActive = false;
        }
    }
};

// Define the Duck class
class Duck {
public:
    float x, y;               // Duck position
    bool isActive;            // Whether the duck is currently on the screen
    sf::Texture texture;      // Duck texture
    sf::Sprite sprite;        // Duck sprite

    Duck(float startX, float startY, const sf::Texture& duckTexture) : x(startX), y(startY), isActive(true) {
        sprite.setTexture(duckTexture);
        sprite.setPosition(x, y);
    }

    void update() {
        // Triple the duck's speed (change the value as needed)
        x += 30; // Adjust the speed as needed
        sprite.setPosition(x, y);

        // Deactivate the duck if it goes off the right side of the screen
        if (x > 800) { // Adjust the screen width as needed
            isActive = false;
        }
    }
};

class Crossbow {
public:
    float x, y;               // Crossbow position
    sf::Texture texture;      // Crossbow texture
    sf::Sprite sprite;        // Crossbow sprite

    Crossbow(float startX, float startY, const std::string& imagePath) : x(startX), y(startY) {
        if (!texture.loadFromFile(imagePath)) {
            std::cerr << "Failed to load crossbow image!" << std::endl;
        }

        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    Arrow shootArrow(const sf::Texture& arrowTexture) {
        std::cout << "Arrow shot!" << std::endl;
        return Arrow(x + sprite.getGlobalBounds().width / 2, y, arrowTexture);
    }
};

int main() {
    // Initialize game objects and variables
    const int screenWidth = 800;
    const int screenHeight = 600;

    std::vector<Duck> ducks;
    std::vector<Arrow> arrows;
    int score = 0;
    int arrowsLeft = 5;

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "Duck Hunter Game");
    Crossbow crossbow(screenWidth / 2, screenHeight - 100, "Lab08Images/crossbow.png");

    // Load red_ballon image
    sf::Texture duckTexture;
    if (!duckTexture.loadFromFile("Lab08Images/red_ballon.png")) {
        std::cerr << "Failed to load red_ballon image!" << std::endl;
        return -1;
    }
    sf::Texture arrowTexture;
    if (!arrowTexture.loadFromFile("Lab08Images/arrow.png")) {
        std::cerr << "Failed to load arrow image!" << std::endl;
        return -1;
    }

    // Game loop
    while (window.isOpen() && arrowsLeft > 0) {
        // Handle window events
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    Arrow arrow(
                        crossbow.sprite.getPosition().x + crossbow.sprite.getGlobalBounds().width / 2,
                        crossbow.sprite.getPosition().y,
                        arrowTexture); // Passing the arrow texture here
                    arrows.push_back(arrow);
                    arrowsLeft--;
                    // Decrease the number of arrows
                }
            }
        }

        // Spawn a new duck at regular intervals
        if (rand() % 100 < 5) {
            Duck duck(0, rand() % (screenHeight - 100) + 1, duckTexture);
            ducks.push_back(duck);
        }

        // Update ducks
        for (auto& duck : ducks) {
            if (duck.isActive) {
                duck.update();
            }
        }

        // Check for collisions between ducks and arrows
        for (auto& arrow : arrows) {
            if (arrow.isActive) {
                for (auto& duck : ducks) {
                    if (duck.isActive && arrow.sprite.getGlobalBounds().intersects(duck.sprite.getGlobalBounds())) {
                        // Collision detected
                        arrow.isActive = false;
                        duck.isActive = false;
                        score++; // Increment the score
                    }
                }
            }
        }

        // Update arrows
        for (auto& arrow : arrows) {
            if (arrow.isActive) {
                arrow.update();
            }
        }

        // Display game state
        window.clear();

        // Draw ducks
        for (auto& duck : ducks) {
            if (duck.isActive) {
                window.draw(duck.sprite);
            }
        }

        // Draw arrows
        for (auto& arrow : arrows) {
            if (arrow.isActive) {
                window.draw(arrow.sprite);
            }
        }

        // Draw crossbow
        window.draw(crossbow.sprite);

        // Draw player information (arrows left)
        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            return -1;
        }

        sf::Text arrowsLeftText;
        arrowsLeftText.setFont(font);
        arrowsLeftText.setString("Arrows Left: " + std::to_string(arrowsLeft));
        arrowsLeftText.setCharacterSize(20);
        arrowsLeftText.setFillColor(sf::Color::White);
        arrowsLeftText.setPosition(10, screenHeight - 30);
        window.draw(arrowsLeftText);

        // Draw player information (score)
        sf::Text scoreText;
        scoreText.setFont(font);
        scoreText.setString("Score: " + std::to_string(score));
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(screenWidth - 150, screenHeight - 30);
        window.draw(scoreText);

        // Wait for a short time to control the game speed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Deactivate ducks that have gone off the right side of the screen
        ducks.erase(std::remove_if(ducks.begin(), ducks.end(),
            [screenWidth](const Duck& duck) { return duck.x > screenWidth; }),
            ducks.end());

        window.display();
    }

    // Display game over screen
    clearScreen();
    std::cout << "GAME OVER" << std::endl;
    std::cout << "Final Score: " << score << std::endl;

    return 0;
}
