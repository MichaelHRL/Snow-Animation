#include <SFML/Graphics.hpp>
#include <random>

// Maps input in the range [0..1] to an output in the range [0..1].
// Represents the piecewise function:
// y(x) = 2*x^2             when x < 0.5
//      = -2*x^2 + 4*t - 1  when x >= 0.5
auto ease_in_out_quad(float const x)
{
	return x < 0.5 ? 2*x*x : 2*x*(2 - x) - 1;
}

struct Tween
{
  float begin;
  float change;
  float time{0};
  float duration;
};

float current(Tween const & t)
{
  return t.begin + t.change*ease_in_out_quad(t.time/t.duration);
}

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

int main()
{
  sf::RenderWindow window{sf::VideoMode{500, 500}, "Snow"};
  auto const inverseFramerate = 1.f/60;
  window.setFramerateLimit(static_cast<unsigned>(1/inverseFramerate));
  auto const numberOfSnowflakes = 200;
  Tween tweens[numberOfSnowflakes];
  float positions[numberOfSnowflakes];
  auto const changeInX = 10.f;
  // Returns a random number in the range [min..max).
  for (auto i = 0; i < numberOfSnowflakes; ++i)
  {             
    tweens[i].begin = random::uniform(0, window.getSize().x);
    tweens[i].change = changeInX*random::normal();
    tweens[i].duration = random::uniform(1, 5);
    positions[i] = random::uniform(-20, window.getSize().y);
  }
  auto const defaultRadius = 5.f;
  auto circle = sf::CircleShape{};
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
      tweens[i].time += inverseFramerate;
      // If tween finished then reset it so that it continues to another end-point.
      if (tweens[i].time > tweens[i].duration)
      {
        tweens[i].begin += tweens[i].change;
        tweens[i].change = changeInX*random::normal();
        tweens[i].time = 0;
      }
      // If snowflake is below screen then reset it so that it is above the screen, and reset the associated tween.
      positions[i] += 1/tweens[i].duration;
      if (positions[i] > window.getSize().y)
      {
        positions[i] = -defaultRadius*2;
        tweens[i].begin = random::uniform(0, window.getSize().x);
        tweens[i].change = changeInX*random::normal();
        tweens[i].time = 0;
      }
    }
    window.clear();
    for (auto i = 0; i < numberOfSnowflakes; ++i)
    {
      circle.setPosition(current(tweens[i]), positions[i]);
      circle.setRadius(defaultRadius/tweens[i].duration);
      window.draw(circle);      
    }
    window.display();
  }
}
