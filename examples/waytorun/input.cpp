#include "input.hpp"

Input globalInput{};

void Input::handleEvent(SDL_Event& event) {
  for (auto& listener : activeListeners) {
    if (event.type == listener.event &&
        event.key.keysym.sym == listener.keycode) {
      listener.f();
    }
  }
}
// void (*fptr)(void*, int, int)

void Input::addListener(int keycode, int event, std::function<void()> f) {
  activeListeners.push_front({.keycode = keycode, .event = event, .f = f });
  printf("[EventListener]: addded event total of %i\n", activeListeners.size());
}
