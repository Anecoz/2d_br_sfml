#include <iostream>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "net/NetClient.h"
#include "Player.h"

int main()
{
  sf::RenderWindow window(sf::VideoMode(1280, 720), "2D BR");

  net::NetClient client("127.0.0.1:6000");
  Player player;

  sf::Clock clock;
  while (window.isOpen()) {
    // Check and act on input first
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }

      if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        player.handleKeyEvent(event);
      }
    }

    // Update the client
    sf::Time elapsed = clock.restart();
    player.update(elapsed.asSeconds());
    client.queuePositionUpdate(player.pos());
    client.update(elapsed.asSeconds());

    // Update snapshot of world

    // Render world
    window.clear(sf::Color::Black);

    player.draw(window);
    client.drawNetPlayers(window);

    window.display();
  }

  return 0;
}