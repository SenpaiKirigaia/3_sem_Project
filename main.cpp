#include <iostream>
#include "Update_Function.h"

using namespace std;


int main() {
    Update_Function game;
    game.Add_Ball(50, 100, 5, 10, 0);
    game.Add_Ball(150, 100, 5, -20, 0);
    for (int i = 0; i < 10; i++) {
        std::cout << game.vector_of_balls[1].x << " " << game.vector_of_balls[1].vx << std::endl;
        std::cout << game.vector_of_balls[0].x << " " << game.vector_of_balls[0].vx << "!!!" << std::endl;
        game.Update_Balls();
    }

    return 0;
}
