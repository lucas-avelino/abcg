#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <functional>
#include <list>

#include "abcg.hpp"

struct Listener {
  int keycode;
  std::function<void()> f;
};

class Input {
 public:
  void handleEvent(SDL_Event &event);
  void addListener(int keycode, std::function<void()> f);

 private:
  std::list<Listener> activeListeners{};
};

#endif