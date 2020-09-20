#include <iostream>
#include <SFML/Window.hpp>

#include "net/NetClient.h"

int main()
{
  std::cout << "Hello world!" << std::endl;

  sf::Window window(sf::VideoMode(1280, 720), "2D BR");

  net::NetClient client("127.0.0.1:6000");

  sf::Clock clock;
  while (window.isOpen()) {
    // Check and act on input first
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
          client.hax();
        }
      }
    }

    // Update the client
    sf::Time elapsed = clock.restart();
    client.update(elapsed.asSeconds());

    // Update snapshot of world

    // Render world
  }

  return 0;
}