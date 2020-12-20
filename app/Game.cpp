#include "Game.h"

Game::Game()
  : _window(sf::VideoMode(1280, 720), "2D BR")
  , _netClient("127.0.0.1:6000")
{}

bool Game::init()
{
  return true;
}

void Game::run()
{
  sf::Clock clock;
  while (_window.isOpen()) {
    // Check and act on input first
    sf::Event event;
    while (_window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        _window.close();
      }

      if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
        _player.handleKeyEvent(event);
      }
      if (event.type == sf::Event::MouseButtonReleased || event.type == sf::Event::MouseButtonPressed ||
          event.type == sf::Event::MouseMoved) {
        _player.handleMouseEvent(event);
      }
    }

    // Update the client
    sf::Time elapsed = clock.restart();
    _player.update(elapsed.asSeconds());
    _netClient.queueStateUpdate(_player.state());
    
    if (_player.inputStateChanged()) {
      _netClient.queueInputUpdate(_player.inputState());
    }

    _netClient.update(elapsed.asSeconds());

    // Update snapshot of world
    if (_netClient.localStateUpdated()) {
      _player.updateState(_netClient.getUpdatedLocalState());
    }

    // Render world
    _window.clear(sf::Color::Black);

    _player.draw(_window);
    _netClient.drawNetPlayers(_window);

    _window.display();
  }
}