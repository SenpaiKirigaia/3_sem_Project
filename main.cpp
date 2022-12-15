#include <iostream>
#include "Update_Function.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

int main() {
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

    sf::RenderWindow graph(sf::VideoMode(200, 100), "graph", sf::Style::Titlebar);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
                graph.close();
            }



        }
        window.clear();

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
