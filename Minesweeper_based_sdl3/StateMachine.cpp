#include "StateMachine.h"

std::unique_ptr<State> StateMachine::createState(StateType type){
	switch (type)
	{
	case StateType::Menu:
		return std::make_unique<MenuState>();
	case StateType::Playing:
		return std::make_unique<PlayingState>();
	case StateType::Won:
		return std::make_unique<WonState>();
	case StateType::Lost:
		return std::make_unique<LostState>();
	case StateType::Paused:
		return std::make_unique<PausedState>();
	default:
		return std::make_unique<MenuState>(); // Ä¬ÈÏ·µ»Ø²Ëµ¥×´Ì¬
	}
}
