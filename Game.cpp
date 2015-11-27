#include "Game.h"

void Game::game_reset()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        total_scores[i] = 0;
    }
}

void Game::end_hand()
{
    // shoot the moon already handled in Game_Hand::end_hand
    for (int player = 0; player < PLAYER_COUNT; ++player)
    {
        total_scores[player] += hand.get_score(player);
    }
}
