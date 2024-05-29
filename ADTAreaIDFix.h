#pragma once
#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

typedef uint32_t uint32;

uint32 FindFirstMCNKOffset(FILE* filename);
uint32 ReadUInt32(FILE* filename);
