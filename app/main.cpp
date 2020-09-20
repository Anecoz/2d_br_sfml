#include <iostream>
#include <SFML/Window.hpp>

int main()
{
  std::cout << "Hello world!" << std::endl;



  sf::Window window(sf::VideoMode(1280, 720), "2D BR");

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
  }

  return 0;
}