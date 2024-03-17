#pragma once
#include <SDL2/SDL.h>
#include <assert.h>
#include <iostream>
#include <vector>

void createScreenAndRender(const std::vector<int>&);
void loopUntilExit();