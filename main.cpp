#include <iostream>
#include "Update_Function.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

int main() {
    int timer = 5000;

    Update_Function game(vector<float>({0,0, float(SCREEN_WIDTH), float(SCREEN_HEIGHT)}), 0.001);

    game.Create_Pool(100, 10, true, 10);

    vector<sf::CircleShape> ball_graph;

    for (int i = 0; i < game.vector_of_balls.size(); i++) {
        sf::CircleShape circle;
        ball_graph.push_back(circle);
        ball_graph[i].setRadius(game.vector_of_balls[i].radius);
        ball_graph[i].setOutlineColor(sf::Color::Green);
        ball_graph[i].setPosition(game.vector_of_balls[i].x - game.vector_of_balls[i].radius,
                                  game.vector_of_balls[i].y - game.vector_of_balls[i].radius);
    }

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "gas", sf::Style::Default);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();

        ofstream output;

        if (timer == 5000) {
            timer = 0;
            output.open("output.csv");
            for (auto &ball : game.vector_of_balls) {
                output << sqrt(ball.vx * ball.vx +
                               ball.vy * ball.vy) << "\n";
                cout << sqrt(ball.vx * ball.vx +
                             ball.vy * ball.vy) << " ";
            }
            output.close();
        }

        timer += 1;

        for (int i = 0; i < game.vector_of_balls.size(); i++) {
            ball_graph[i].setPosition(game.vector_of_balls[i].x - game.vector_of_balls[i].radius,
                                      game.vector_of_balls[i].y - game.vector_of_balls[i].radius);
            window.draw(ball_graph[i]);
        }

        game.Update_Balls();
        window.display();
    }

    return 0;
}
