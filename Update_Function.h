#include <vector>
#include <algorithm>
#include <cmath>

#include "Ball.h"

using namespace std;

bool Compare_by_x(const Ball &a, const Ball &b){
    return a.x - a.radius < b.x - b.radius;
}
bool operator==(const Ball& left, const Ball& right)
{
    return left.id == right.id;
}

class Update_Function final {
public:
    vector<Ball> vector_of_balls;
    const float dt = 1;
    const vector<float> box{0, 0, 200, 200};

    void Add_Ball(float x, float y, float r = 5.0, float vx = 0, float vy = 0) {
        Ball ball{x, y, vx, vy, 0, 0, r};
        ball.mass = ball.radius * 10;
        ball.id = vector_of_balls.size();
        vector_of_balls.emplace_back(ball);
    }


    void Collision_With_Box(Ball &ball) {
        // simple bouncing from ground and roof
        Ball *pBall = &ball;
        if (pBall->x - pBall->radius <= box[0] or pBall->x + pBall->radius >= box[2]) {
            pBall->vx = -pBall->vx;
            pBall->x += pBall->vx * dt;
        }
        if (pBall->y - pBall->radius <= box[1] or pBall->y + pBall->radius >= box[3]) {
            pBall->vy = -pBall->vy;
            pBall->y += pBall->vy * dt;
        }
    }

    // Sort and sweep algorithm
    // Const because it does not change values
    vector<pair<Ball, Ball>> Find_Possible_Collisions() const {
        vector<Ball> axis_list = vector_of_balls;
        std::sort(axis_list.begin(), axis_list.end(), Compare_by_x);
        vector<Ball> active_list;
        vector<Ball> to_remove;
        vector<pair<Ball, Ball>> possible_collisions;
        for (auto &axis_ball : axis_list){
            for (auto &active_ball : active_list) {
                if (axis_ball.x - axis_ball.radius > active_ball.x + active_ball.radius) {
                    to_remove.emplace_back(active_ball);
                }
            }
            for (auto &remove_ball : to_remove){
                active_list.erase(remove(active_list.begin(), active_list.end(), remove_ball), active_list.end());
            }

            for (auto &active_ball : active_list){
                possible_collisions.emplace_back(make_pair(axis_ball, active_ball));
            }

            active_list.emplace_back(axis_ball);
        }

        return possible_collisions;
    }


    void Collision_With_Balls() {
        vector<pair<Ball, Ball>> possible_collisions = Find_Possible_Collisions();

        for (auto & possible_collision : possible_collisions){
            Ball b1 = possible_collision.first;
            Ball b2 = possible_collision.second;
            if (b1.id != b2.id){
                float dist =  sqrtf((b1.x - b2.x)*(b1.x - b2.x) + (b1.y - b2.y)*(b1.y - b2.y));
                if (dist <= b1.radius + b2.radius){
                    float nx = (b2.x - b1.x)/dist;
                    float ny = (b2.y - b1.y)/dist;

                    float kx = b1.vx - b2.vx;
                    float ky = b1.vy - b2.vy;

                    float p = 2 * (nx * kx + ny * ky) / (b1.mass + b2.mass);

                    vector_of_balls[b1.id].vx -= p * b2.mass * nx;
                    vector_of_balls[b1.id].vy -= p * b2.mass * ny;
                    vector_of_balls[b2.id].vx += p * b1.mass * nx;
                    vector_of_balls[b2.id].vy += p * b1.mass * ny;

                }
            }
        }
    }

    void Update_Balls() {
        for (auto &ball: vector_of_balls) {
            Ball *pBall = &ball;
            pBall->ax = 0; pBall->ay = 0; // definitely can change that if needed
            pBall->vx += pBall->ax * dt;
            pBall->vy += pBall->ay * dt;
            pBall->x += pBall->vx * dt;
            pBall->y += pBall->vy * dt;
            Collision_With_Box(ball);
            pBall->x += pBall->vx * dt;
            pBall->y += pBall->vy * dt;
        }
        Collision_With_Balls();
        for (auto &ball: vector_of_balls) {
            Ball *pBall = &ball;
            pBall->x += pBall->vx * dt;
            pBall->y += pBall->vy * dt;
        }
    }
};

