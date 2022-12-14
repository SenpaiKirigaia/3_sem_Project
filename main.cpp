#include <iostream>
#include "Update_Function.h"

using namespace std;


int main() {
    // My simple test for collision before we have any graphic interface
    Update_Function game;
    game.Create_Pool(30, 10, false, 10);

    for (int i = 0; i < 10; i++) {
        std::cout << game.vector_of_balls[0].vx << " " << game.vector_of_balls[0].vy << std::endl;
        std::cout << game.vector_of_balls[1].vx << " " << game.vector_of_balls[1].vy << "!!!" << std::endl;
        game.Update_Balls();
    }
    return 0;
}
