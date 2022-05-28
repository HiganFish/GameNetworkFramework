//
// Created by rjd
//
#include <iostream>

#include "GameController.h"

GameController::GameController(const GameClientPtr& client):
	game_client_ptr_(client),
	control_message_ptr_(SpawnNewMessage<ControlMessage>())
{
	game_client_ptr_->SetMsgCallback(MessageType::CONTROL,
			[this](ROLE_ID role_id, const BaseMessagePtr& msg)
			{
				ControlMessagePtr ptr = CastBaseMsgTo<ControlMessage>(msg);
				input_deque_.push_back({role_id,
										ptr->tick, ptr->move_direction});
			});
}

void GameController::PushInput(int role_id, int logic_frame, MoveDirection direction)
{
	// input_deque_.push_back({ player_id, logic_frame, direction});
	control_message_ptr_->tick = logic_frame;
	control_message_ptr_->role_id = role_id;
	control_message_ptr_->move_direction = direction;
	game_client_ptr_->SendMsgAsync(role_id, control_message_ptr_);
}

void GameController::PopInput(int current_logic_frame, std::vector<Input>& result)
{
	while (!input_deque_.empty())
	{
		auto& input = input_deque_.front();
		if (current_logic_frame >= input_deque_.front().logic_frame)
		{
			result.push_back(input);
			input_deque_.pop_front();
		}
		else
		{
			break;
			// std::cout << "input some error" << std::endl;
			// result.push_back(input);
		}
		// input_deque_.pop_front();
	}
}