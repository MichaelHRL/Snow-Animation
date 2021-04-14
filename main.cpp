#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

namespace random {
    std::mt19937 generator;
    float uniform(float const min, float const max) {
        return std::uniform_real_distribution<float>(min, max)(generator);
    }
    float normal(float const mean = 0.0f, float const standard_deviation = 1.0f) {
        return std::normal_distribution<float>(mean, standard_deviation)(generator);
    }
};

struct Snowflake {
    float start;
    float end;
    float elapsed_time;
    float duration;
    sf::CircleShape shape{1};
};

float current(Snowflake const & s) {
    static auto f = [](float x){return x < 0.5 ? 2 * x * x : 2 * x * (2 - x) - 1;};
    return s.start + (s.end - s.start) * f(s.elapsed_time / s.duration);
}

Snowflake create_snowflake(sf::RenderWindow const & window, float const max_radius, float const change_in_x) {
    Snowflake s;
    s.start = random::uniform(0, window.getSize().x);
    s.end = s.start + change_in_x * random::normal();
    s.duration = random::uniform(1, 5);
    s.elapsed_time = random::uniform(0, s.duration); 

    s.shape.setRadius(max_radius / s.duration);
    s.shape.setPosition(s.start, random::uniform(-2 * max_radius, window.getSize().y));
    return s;
}

void update_snowflake(Snowflake & s, sf::RenderWindow const & window, float const framerate, float const max_radius, float const change_in_x) {
    s.elapsed_time += 1 / framerate;
    if (s.elapsed_time > s.duration) {
        s.start = s.end;
        s.end += change_in_x * random::normal();
        s.elapsed_time = 0;
    }
    s.shape.setPosition(s.shape.getPosition().x, s.shape.getPosition().y + 1 / s.duration);
    if (s.shape.getPosition().y > window.getSize().y) {
        s.shape.setPosition(s.shape.getPosition().x, -2 * max_radius);
        s.start = random::uniform(0, window.getSize().x);
        s.end = s.start + change_in_x * random::normal();
        s.elapsed_time = 0;
    }
    s.shape.setPosition(current(s), s.shape.getPosition().y);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(500, 500), "Snow");
    float constexpr framerate = 260.0f;
    window.setFramerateLimit(framerate);

    float constexpr change_in_x = 10.0f;
    float constexpr max_radius = 5.0f;
    std::vector<Snowflake> snowflakes;
    for (int i = 0; i < 200; ++i) {
        snowflakes.push_back(create_snowflake(window, max_radius, change_in_x));
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        for (int i = 0; i < snowflakes.size(); ++i) {
            update_snowflake(snowflakes[i], window, framerate, max_radius, change_in_x);
        }

        window.clear();
        for (int i = 0; i < snowflakes.size(); ++i) {
            window.draw(snowflakes[i].shape);
        }
        window.display();
    }
}
