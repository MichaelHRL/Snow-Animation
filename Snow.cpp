#include <SFML/Graphics.hpp>
#include <random>

// Maps input in the range [0..1] to an output in the range [0..1].
// Represents the piecewise function:
// y(x) = 2*x^2             when x < 0.5
//      = -2*x^2 + 4*t - 1  when x >= 0.5
float easeInOutQuad(float normalisedTime)
{
  auto & t = normalisedTime;
  return t < 0.5 ? 2*t*t : 2*t*(2 - t) - 1;
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
  return t.begin + t.change*easeInOutQuad(t.time/t.duration);
}

int main()
{
  sf::RenderWindow window{sf::VideoMode{500, 500}, "Snow"};
  auto const inverseFramerate = 1.f/60;
  window.setFramerateLimit(static_cast<unsigned>(1/inverseFramerate));
  auto distribution = std::normal_distribution<float>{0.f, 1.f};
  auto generator = std::mt19937{};
  auto const numberOfSnowflakes = 200;
  Tween tweens[numberOfSnowflakes];
  float positions[numberOfSnowflakes];
  auto const changeInX = 10.f;
  // Returns a random number in the range [min..max).
  auto randomBetween = [&generator](float min, float max)
  {
    return std::generate_canonical<float, 10>(generator)*(max - min) + min;
  };
  for (auto i = 0; i < numberOfSnowflakes; ++i)
  {             
    tweens[i].begin = randomBetween(0, window.getSize().x);
    tweens[i].change = changeInX*distribution(generator);
    tweens[i].duration = randomBetween(1, 5); 
    positions[i] = randomBetween(-20, window.getSize().y);
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
        tweens[i].change = changeInX*distribution(generator);
        tweens[i].time = 0;
      }
      // If snowflake is below screen then reset it so that it is above the screen, and reset the associated tween.
      positions[i] += 1/tweens[i].duration;
      if (positions[i] > window.getSize().y + defaultRadius*2)
      {
        positions[i] = -defaultRadius*2;
        tweens[i].begin = randomBetween(0, window.getSize().x);
        tweens[i].change = changeInX*distribution(generator);
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
