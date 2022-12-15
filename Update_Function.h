#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>
#include "Ball.h"

using namespace std;
// Used for sorting vector of balls by x coordinate for sort and
// sweep algorithm for finding possible collisions
bool operator< (const Ball &a, const Ball &b){
    return a.x - a.radius < b.x - b.radius;
}
// Used for removing needed struct from vector
bool operator== (const Ball& left, const Ball& right)
{
    return left.id == right.id;
}

// Whole class can be rewritten or deleted entirely,
// but that is my implementation of mechanics for collisions
class Update_Function final {
public:
    // Creating vector that'll hold all data about balls
    vector<Ball> vector_of_balls{};

    // May be set to some number or to amount of FPS,doesn't matter
    const float dt = 1;

    // coordinates of points for creating box in format: x1, y1, x2, y2
    const vector<float> box{0, 0, 200, 200};

    Update_Function(vector<float> box, float dt): box((box)), dt(dt){}

    // Creating circle then adding it to vector of data
    void Add_Ball(float x, float y, float r = 5.0, float vx = 0, float vy = 0) {
        Ball ball{x, y, vx, vy, 0, 0, r};
        ball.mass = ball.radius * 10;

        // ID is used for identifying each ball
        // Also it is conveniently similar to the position of the ball in the vector
        // So we can call vector_of_balls[some_ball.id] and find data about some ball, or change it
        ball.id = vector_of_balls.size(); // NOLINT(cppcoreguidelines-narrowing-conversions)
        vector_of_balls.emplace_back(ball);
    }

    void Create_Pool(int n = 1, float r = 5, bool need_speed = false,  float v_modulus = 0){
        for (int i = 0; i < n; i++){
            mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
            float x = uniform_real_distribution<float>(box[0] + r, box[2] - r)(rng);
            float y = uniform_real_distribution<float>(box[1] + r, box[3] - r )(rng);
            if (Pool_Collision(x, y, r)){
                i -= 1;
                continue;
            }
            if (need_speed) {
                float vx = uniform_real_distribution<float>(0, 2*v_modulus)(rng) - v_modulus;
                float vy = uniform_real_distribution<float>(0, 2*v_modulus)(rng) - v_modulus;
                Add_Ball(x, y, r, vx, vy);
            } else Add_Ball(x, y, r);

        }

    }

    bool Pool_Collision(float x, float y, float r) const{
        for(auto &ball: vector_of_balls){ // NOLINT This shit wanned me to use any_of and I do not like it
            if(sqrtf((ball.x - x)*(ball.x - x) + (ball.y - y)*(ball.y - y)) <= ball.radius + r){
                return true;
            }
        }
        return false;
    }
    // Function used for updating position of each ball.
    // On each call it moves ball 3 time, so we can divide our dt by 3, or smt like that.
    // Why is it so? because I wanted consistency. After colliding with the wall we need to move
    // ball for it not to end up on the other place of the wall, same thing with collisions between balls
    void Update_Balls() {
        for (auto &ball: vector_of_balls) {
            // Usual stuff, updating everything
            ball.ax = 0; ball.ay = 0; // definitely can change that if needed
            ball.vx += ball.ax * dt; ball.vy += ball.ay * dt;
            ball.x += ball.vx * dt; ball.y += ball.vy * dt;

            // Next we'll check whether ball collided with the box or not
            // See Collision_With_Box description
            Collision_With_Box(ball);

            ball.x += ball.vx * dt;
            ball.y += ball.vy * dt;

        }
        Collision_With_Balls();

        for (auto &ball: vector_of_balls) {
            ball.x += ball.vx * dt;
            ball.y += ball.vy * dt;

        }
    }

    // Simple bouncing from ground and roof, via changing velocity to its negative value

    // But that is for now, because I want to implement continuous collision detection (google it)
    void Collision_With_Box(Ball &ball) {
        if (ball.x - ball.radius <= box[0] or ball.x + ball.radius >= box[2]) {
            ball.vx = -ball.vx;
        }
        if (ball.y - ball.radius <= box[1] or ball.y + ball.radius >= box[3]) {
            ball.vy = -ball.vy;
        }
    }

    // Not your trivial collision detection
    //
    // There I am using Sweep and Prune algorithm to find possible candidates for collision
    // For further information look into Find_Possible_Collisions
    //
    // Then we check candidates for actual collision and change their speeds accordingly
    //
    // Velocity is changed according to simple geometry combined with physics
    // You can check that out on Wikipedia
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

    // Sweep and Prune algorithm
    // Const because it does not change values
    //
    // In this algorithm we
    // 1) Sort circles by x-axis
    // 2) Maintain some active interval (circle projected on x-axis),
    // we can just start from first ball
    // 3) Check intersection of projection
    // 4) Repeat
    // 5) Update active interval (when previous active interval doesn't intercept with anything)
    // 6) Successss (yes, with 4 s-es)
    vector<pair<Ball, Ball>> Find_Possible_Collisions() const {

        // We'll sort axis_list by x-axis
        vector<Ball> axis_list = vector_of_balls;
        // Because we've already implemented comparator we can just sort our vector
        std::sort(axis_list.begin(), axis_list.end() );

        // Active list is list of all intercepting balls
        vector<Ball> active_list;
        vector<Ball> to_remove;
        vector<pair<Ball, Ball>> possible_collisions;
        for (auto &axis_ball : axis_list){
            for (auto &active_ball : active_list) {
                // Check interception:
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
};

