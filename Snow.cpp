#include <SFML/Graphics.hpp>
#include <random>

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
  auto const defaultRadius = 5.f;
  auto circle = sf::CircleShape{};
  auto distribution = std::normal_distribution<float>{0.f, 1.f};
  auto generator = std::mt19937{};
  auto const numberOfSnowflakes = 200;
  float depths[numberOfSnowflakes];
  Tween tweens[numberOfSnowflakes];
  auto const changeInX = 10.f;
  sf::Vector2f positions[numberOfSnowflakes];
  for (auto i = 0; i < numberOfSnowflakes; ++i)
  {             
    depths[i] = std::generate_canonical<float, 10>(generator); // [0..1)
    auto & tween = tweens[i];
    tween.begin = std::generate_canonical<float, 10>(generator)*window.getSize().x; // [0..window.getSize().x)
    tween.change = changeInX*distribution(generator);
    tween.duration = 4*depths[i] + 1; // [1..5)
    positions[i].y = std::generate_canonical<float, 10>(generator)*(window.getSize().y + 20) - 20; // [-20..window.getSize().y)
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
    window.clear();
    for (auto i = 0; i < numberOfSnowflakes; ++i)
    {
      auto & tween = tweens[i];
      if (positions[i].y > window.getSize().y + defaultRadius*2)
      {
        positions[i].y = -defaultRadius*2;
        tween.begin = std::generate_canonical<float, 10>(generator)*window.getSize().x;
        tween.change = changeInX*distribution(generator);
        tween.time = 0;
      }
      tween.time += inverseFramerate;
      if (tween.time > tween.duration)
      {
        tween.begin += tween.change;
        tween.change = changeInX*distribution(generator);
        tween.time = 0;
      }
      auto & position = positions[i];
      position.x = current(tween);
      position.y += 1/tween.duration;
      circle.setPosition(position);
      circle.setRadius(defaultRadius/tween.duration);
      window.draw(circle);
    }
    window.display();
  }
}
