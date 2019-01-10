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
  float current() const
  {
    return begin + change*easeInOutQuad(time/duration);
  }
};

int main()
{
  sf::RenderWindow window{sf::VideoMode{500, 500}, "Snow"};
  auto const inverseFramerate = 1.f/60;
  window.setFramerateLimit(static_cast<unsigned>(1/inverseFramerate));
  auto const numberOfSnowflakes = 200;
  Tween tweens[numberOfSnowflakes];
  float yPositions[numberOfSnowflakes];
  auto generator = std::mt19937{};
  // Returns a normally distributed random number.
  auto randomChangeInX = [&generator]()
  {
    static auto distribution = std::normal_distribution<float>{0.f, 10.f};
    return distribution(generator);
  };
  // Returns a uniformly distributed random number in the range [min..max).
  auto randomBetween = [&generator](float min, float max)
  {
    return std::generate_canonical<float, 10>(generator)*(max - min) + min;
  };
  for (auto i = 0; i < numberOfSnowflakes; ++i)
  {             
    tweens[i].begin = randomBetween(0, window.getSize().x);
    tweens[i].change = randomChangeInX();
    tweens[i].duration = randomBetween(1, 5); 
    yPositions[i] = randomBetween(-20, window.getSize().y);
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
      if (tweens[i].time > tweens[i].duration)
      {
        tweens[i].begin += tweens[i].change;
        tweens[i].change = randomChangeInX();
        tweens[i].time = 0;
      }

      yPositions[i] += 1/tweens[i].duration;
      if (yPositions[i] > window.getSize().y)
      {
        yPositions[i] = -defaultRadius*2;
        tweens[i].begin = randomBetween(0, window.getSize().x);
        tweens[i].change = randomChangeInX();
        tweens[i].time = 0;
      }
    }
    window.clear();
    for (auto i = 0; i < numberOfSnowflakes; ++i)
    {
      circle.setPosition(tweens[i].current(), yPositions[i]);
      circle.setRadius(defaultRadius/tweens[i].duration);
      window.draw(circle);      
    }
    window.display();
  }
}
