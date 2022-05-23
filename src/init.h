#pragma once

#include <SDL2/SDL.h>

namespace Init
{

int initVideo(SDL_Window** winOut, SDL_GLContext* contOut, bool* isVSyncActiveOut);

} // namespace Init
