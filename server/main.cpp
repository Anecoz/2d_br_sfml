#include <iostream>

#include "GameServer.h"

int main()
{
  std::cout << "Starting server..." << std::endl;
  server::GameServer server("127.0.0.1:6000");
  if (!server.start()) {
    std::cerr << "Server could not be started" << std::endl;
    return 1;
  }

  server.run();
}