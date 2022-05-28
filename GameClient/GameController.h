//
// Created by rjd
//

#ifndef SDL2GAME_CONTROLLER_H
#define SDL2GAME_CONTROLLER_H

#include <deque>
#include <vector>

#include <GameClient/GameClient.h>
#include <Message/ControlMessage.h>

using MoveDirection = ControlMessage::MoveDirection;

struct Input
{
	ROLE_ID player_id;
	uint32_t logic_frame;
	MoveDirection direction;
};
class GameController
{
public:
	GameController(const GameClientPtr& client);
	~GameController() = default;
	GameController(const GameController&) = delete;
	GameController& operator=(const GameController&) = delete;

	void PushInput(int role_id, int logic_frame, MoveDirection direction);

	void PopInput(int current_logic_frame, std::vector<Input>& result);

private:
	std::deque<Input> input_deque_;
	GameClientPtr game_client_ptr_;

	ControlMessagePtr control_message_ptr_;
};

#endif //SDL2GAME_CONTROLLER_H
