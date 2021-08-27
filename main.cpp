#include "GUI/GUI.h"
#include <iostream>

// using std::cout, std::endl;

int main(int argc, char *argv[])
{
  App* app= new App();
  app->create("org.BAA.Task2");
  return app->run(*app->window);
}
