#include "Game.h"

const int Game::passing_directions[PLAYER_COUNT] = {1, 3, 2, 0};  // left, right, across, keep

void Game::game_reset()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        total_scores[i] = 0;
    }

    winners.clear();
    passing_index = 0;
}

void Game::end_hand()
{
    // shoot the moon already handled in Game_Hand::end_hand
    for (int player = 0; player < PLAYER_COUNT; ++player)
    {
        total_scores[player] += hand.get_score(player);
        if (total_scores[player] > 99)
        {
            if (winners.empty())
            {
                winners.push_back(player);
            }
            else if (total_scores[player] > total_scores[winners[0]])
            {
                winners.clear();
                winners.push_back(player);
            }
            else if (total_scores[player] == total_scores[winners[0]])  // tie
            {
                winners.push_back(player);
            }
            // else above 99 but not highest score
        }
    }
}
