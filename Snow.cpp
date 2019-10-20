#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

namespace random
{
	auto generator = std::mt19937{};
	auto uniform(float const min, float const max)
	{
		return std::uniform_real_distribution<float>{min, max}(generator);
	}
	auto normal(float const mean = 0, float const standard_deviation = 1)
	{
		return std::normal_distribution<float>{mean, standard_deviation}(generator);
	}
};

struct Snowflake
{
	float start;
	float end;
	float elapsed_time;
	float duration;
	sf::CircleShape shape;
};

auto current(Snowflake const & s)
{
	static auto f = [](float x){return x < 0.5 ? 2*x*x : 2*x*(2 - x) - 1;};
	return s.start + (s.end - s.start)*f(s.elapsed_time/s.duration);
}

auto create_snowflake(sf::RenderWindow const & window, float const max_radius, float const change_in_x)
{
	auto const a = random::uniform(0, window.getSize().x);
	auto const b = a + change_in_x*random::normal();
	auto const d = random::uniform(1, 5);
	auto const t = random::uniform(0, d); 
	auto c = sf::CircleShape{max_radius/d};
	c.setPosition(a, random::uniform(-20, window.getSize().y));
	return Snowflake{a, b, t, d, c}; 
}

auto update_snowflake(Snowflake & s, sf::RenderWindow const & window, float const framerate, float const max_radius, float const change_in_x)
{
	s.elapsed_time += 1/framerate;
	if (s.elapsed_time > s.duration)
	{
		s.start = s.end;
		s.end += change_in_x*random::normal();
		s.elapsed_time = 0;
	}
	s.shape.setPosition(s.shape.getPosition().x, s.shape.getPosition().y + 1/s.duration);
	if (s.shape.getPosition().y > window.getSize().y)
	{
		s.shape.setPosition(s.shape.getPosition().x, -2*max_radius);
		s.start = random::uniform(0, window.getSize().x);
		s.end = s.start + change_in_x*random::normal();
		s.elapsed_time = 0;
	}
	s.shape.setPosition(current(s), s.shape.getPosition().y);
}

auto draw_snowflake(Snowflake const & s, sf::RenderWindow & w)
{
	w.draw(s.shape);
}

int main()
{
	sf::RenderWindow window{sf::VideoMode{500, 500}, "Snow"};
	auto constexpr framerate = 60.f;
	window.setFramerateLimit(framerate);
	auto constexpr numberOfSnowflakes = 200;
	auto constexpr change_in_x = 10.f;
	auto constexpr max_radius = 5.f;
	auto snowflakes = std::vector<Snowflake>{};
	for (auto i = 0; i < numberOfSnowflakes; ++i)
	{
		snowflakes.push_back(create_snowflake(window, max_radius, change_in_x));
	}
	while (window.isOpen())
	{
		auto event = sf::Event{};
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		for (auto i = 0; i < numberOfSnowflakes; ++i)
		{
			update_snowflake(snowflakes[i], window, framerate, max_radius, change_in_x);
		}
		window.clear();
		for (auto i = 0; i < numberOfSnowflakes; ++i)
		{
			draw_snowflake(snowflakes[i], window);
		}
		window.display();
	}
}
