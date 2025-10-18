#include "entities/apples/NormalApple.h"
#include "states/PlayState.h"
#include "systems/Score.h"
#include "entities/Snake.h"

void NormalApple::onEaten(PlayContext& ctx) 
{
    const auto params = ctx.config.paramsFor(ctx.config.difficulty);
    ctx.score.add(params.pointsPerApple);
    ctx.snake.grow(params.growthPerApple);
    ctx.flashSnake();
    ctx.sfxEat.play();
}