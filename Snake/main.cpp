// Snakentor v1.02 by nordicFrench

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <deque>
#include <list>
#include <time.h>
#include <cassert>
#include <sstream>
#include "version.h"


template <typename T> std::string toString(const T& value){
    std::stringstream stream;
    stream << value;
    return stream.str();
}

// I) Global constants
// 1) Graphics
const sf::Vector2i WINDOW_RESOLUTION(800, 640);
const int SPRITE_SIZE = 16;
const int MAX_FPS(60);
// 2) Game Constants
const float GAME_DELAY = 0.1f;
const sf::Vector2i GRID_SIZE(50, 30);
const int FRUIT_NUMBER(35);
int fruitsNumber=FRUIT_NUMBER;
const int SNAKE_INITIAL_SIZE(12);
enum class Direction{Up, Down, Left, Right};
enum class GameState{Gaming, GameOver, GameWon};

// II) Game Classes
class Snake
{
public:

    Snake(){
        direction = Direction::Up;
        isDigestingFruit = false;
    }

    sf::Vector2i head(){ return body.front(); }
    void init(){
        sf::Vector2i tilePosition(GRID_SIZE.x / 2, GRID_SIZE.y);
        for(int i = 0; i < SNAKE_INITIAL_SIZE; i++)
        {
            body.push_front(tilePosition);
            tilePosition.y--;
        }
    }
    void advance(){
        sf::Vector2i newHead(head());
        if (direction == Direction::Up) newHead.y -= 1;
        if (direction == Direction::Down) newHead.y += 1;
        if (direction == Direction::Left) newHead.x -= 1;
        if (direction == Direction::Right) newHead.x += 1;
        body.push_front(newHead);
        if (!isDigestingFruit)
            body.pop_back();
        else
            isDigestingFruit = false;
    }
    std::deque<sf::Vector2i> body;
    Direction direction;
    bool isDigestingFruit;
};

class Game
{
public:
    Game(int width, int height, const sf::String & name)
    {
        initWindow(width, height, name);
    }

    void initWindow(int width, int height, const sf::String & name){
        sf::VideoMode videoMode(width, height);
        window.create(videoMode, name);
    }

    void gameLoop(){
        sf::Clock clock;
        float timer=0;
        while(window.isOpen()){
            window.clear(sf::Color(0, 0, 0));
            window.draw(bgSprite);
            window.draw(scoreText);
            window.draw(highscoreText);
            window.draw(gameTimerText);
            if(state == GameState::Gaming){
                window.draw(gameInterfaceText);
                float time = clock.getElapsedTime().asSeconds();
                clock.restart();
                timer+=time;
                if (timer > GAME_DELAY) { timer=0; state = gameUpdate(); }
                for(const sf::Vector2i & p : fruits){ fruitTile.setPosition((float) p.x*SPRITE_SIZE, (float) p.y*SPRITE_SIZE); window.draw(fruitTile);}
                for(sf::Vector2i p: snake.body){ snakeTile.setPosition((float) p.x*SPRITE_SIZE, (float) p.y*SPRITE_SIZE);  window.draw(snakeTile); }
            }
            else if(state == GameState::GameOver)
                window.draw(gameOverText);
            else if(state == GameState::GameWon)
                window.draw(gameWonText);
            gameEvent();
            window.display();
        }
    }

    void setup(){
        window.setFramerateLimit(MAX_FPS);
        assert(snakeTexture.loadFromFile("./resources/images/green.png"));
        snakeTile.setTexture(snakeTexture);
        assert(fruitTexture.loadFromFile("./resources/images/red.png"));
        fruitTile.setTexture(fruitTexture);
        assert(bgTexture.loadFromFile("./resources/images/background.png"));
        bgSprite.setTexture(bgTexture);
        assert(font.loadFromFile("./resources/fonts/regular.ttf"));
        initFruits();
        snake.init();
        state = GameState::Gaming;
        gameOverText = sf::Text("The Game is Over.\n\nYour snake died.\n\nBetter luck next time!", font, 15);
        gameOverText.setPosition(100, 550);
        gameOverText.setFillColor(sf::Color::Green);
        gameWonText = sf::Text("You have won the game!\n\nYour snake has managed to eat all targets!\n\nWell played!", font, 15);
        gameWonText.setPosition(100, 550);
        gameWonText.setFillColor(sf::Color::Green);
        gameInterfaceText = sf::Text("Snakentor\nUse the arrow keys to move. \nEat all targets to win.\nMade by nordicFrench in C++ / SFML 2, \nfor KC Game Jam #01.", font, 15);
        gameInterfaceText.setPosition(10, 550);
        gameWonText.setFillColor(sf::Color::Green);
        scoreText = sf::Text("0", font, 20);
        scoreText .setPosition(120, 514);
        scoreText.setFillColor(sf::Color::White);
        highscoreText = sf::Text("3000", font, 20);
        highscoreText.setPosition(660, 514);
        highscoreText.setFillColor(sf::Color::Red);
        gameTimerText = sf::Text("0", font, 20);
        gameTimerText.setPosition(390, 514);
        gameTimerText.setFillColor(sf::Color::Yellow);
        score = 0;
        highscore = 3000;
        gameTimer = 0;
    }

    sf::Vector2i randomTarget(){
        return sf::Vector2i(rand() % GRID_SIZE.x, rand() % GRID_SIZE.y);
    }

    void initFruits(){
        srand((unsigned) time(0));
        int i=0;
        while(i < FRUIT_NUMBER)
        {
            sf::Vector2i target = randomTarget();
            bool isNew = true;
            for(auto e: fruits)
            {
                if(e == target)
                {
                    isNew = false;
                    break;
                }
            }
            if(isNew)
            {
                fruits.push_front(target);
                i++;
            }
        }
    }

    static bool isQuitRequested(const sf::Event & e) {
        if(e.type == sf::Event::Closed)
            return true;
        if((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Escape))
            return true;
        return false;
    }

    void gameEvent(){
        sf::Event event; while(window.pollEvent(event)) if (isQuitRequested(event)) window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && snake.direction != Direction::Down) snake.direction = Direction::Up;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && snake.direction != Direction::Up) snake.direction = Direction::Down;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && snake.direction != Direction::Right) snake.direction = Direction::Left;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && snake.direction != Direction::Left) snake.direction = Direction::Right;
    }

    void updateScore(int points){
        score += points;
        highscore = std::max(score, highscore);
        scoreText.setString(toString(score));
        highscoreText.setString(toString(highscore));
    }

    int fruitHeadCollision(const sf::Vector2i & head, std::list<sf::Vector2i> & fruits)
    {
        unsigned int i = 0;
        for (const sf::Vector2i & p : fruits){
            if (p == head)
                return i;
            i++;
        }
        return -1;
    }

    GameState gameUpdate(){
        snake.advance();gameTimer++; gameTimerText.setString(toString(int(gameTimer / 10)));
        sf::Vector2i head = snake.head();

        // Head collision with a fruit
        int fruit = fruitHeadCollision(head, fruits);
        if(fruit != -1)
        {
            std::list<sf::Vector2i>::iterator it = fruits.begin();
            if(fruit >=0 && fruit < fruitsNumber)
                std::advance(it, fruit);
                fruits.erase(it);
                fruitsNumber--;
                snake.isDigestingFruit = true;
                updateScore(100);
        }

        // Spherical topology of the level
        if (head.x > GRID_SIZE.x)
            snake.body[0].x=0;
        if (head.x < 0)
            snake.body[0].x= GRID_SIZE.x;
        if (head.y > GRID_SIZE.y)
            snake.body[0].y=0;
        if (head.y < 0)
            snake.body[0].y= GRID_SIZE.y;

        // Head collision with the body
        auto result = std::find(std::begin(snake.body) + 1, std::end(snake.body), head);
        if (result != std::end(snake.body))
            return GameState::GameOver;
        if (score >= FRUIT_NUMBER * 100)
        {
            updateScore(500);
            return GameState::GameWon;
        }
        return GameState::Gaming;
    }
private:
    sf::Texture snakeTexture;
    sf::Texture fruitTexture;
    sf::Texture bgTexture;
    sf::Sprite snakeTile;
    sf::Sprite fruitTile;
    sf::Sprite bgSprite;
    sf::RenderWindow window;
    sf::Font font;
    sf::Text gameOverText;
    sf::Text gameWonText;
    sf::Text gameInterfaceText;
    sf::Text scoreText;
    sf::Text highscoreText;
    sf::Text gameTimerText;
    Snake snake;
    std::list<sf::Vector2i> fruits;
    GameState state;
    int score;
    int highscore;
    int gameTimer;
};

int main(){
    Game game(WINDOW_RESOLUTION.x, WINDOW_RESOLUTION.y, "Snakentor by nordicFrench v" + std::string(AutoVersion::FULLVERSION_STRING));
    game.setup();
    game.gameLoop();
    return EXIT_SUCCESS;
}
