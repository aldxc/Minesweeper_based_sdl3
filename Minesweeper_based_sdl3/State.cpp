#include "State.h"
// State.cpp

void MenuState::renderer() const noexcept{

}

void MenuState::handleInput() noexcept{
}

void MenuState::update() noexcept{
	MenuState::renderer();
	MenuState::handleInput();
}

void PlayingState::renderer() const noexcept{
}

void PlayingState::handleInput() noexcept{
}

void PlayingState::update() noexcept{
	PlayingState::renderer();
	PlayingState::handleInput();
}

void WonState::renderer() const noexcept{
}

void WonState::handleInput() noexcept{
}

void WonState::update() noexcept{
	WonState::renderer();
	WonState::handleInput();
}

void LostState::renderer() const noexcept{
}

void LostState::handleInput() noexcept{
}

void LostState::update() noexcept{
	LostState::renderer();
	LostState::handleInput();
}

void PausedState::renderer() const noexcept{
}

void PausedState::handleInput() noexcept{
}

void PausedState::update() noexcept{
	PausedState::renderer();
	PausedState::handleInput();
}
