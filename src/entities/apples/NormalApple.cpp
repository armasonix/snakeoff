#include "states/PlayContext.h"
#include "core/Config.h"
#include "entities/apples/NormalApple.h"
#include "systems/Score.h"
#include "entities/Snake.h"

void NormalApple::onEaten(PlayContext& ctx)
{
    const auto params = ctx.cfg.paramsFor(ctx.cfg.difficulty);
    ctx.score.add(params.pointsPerApple);
    ctx.snake.grow(params.growthPerApple);
    if (ctx.flash) ctx.flash();
    ctx.sfxEat.play();
}