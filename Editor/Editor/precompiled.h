﻿#pragma once

#include <Windows.h>
#include <string>
#include <assert.h>
#include <list>
#include <map>
#include <memory>
#include <vector>
#include <fstream>


#include "CommCtrl.h"
#include "resource.h"

const int SuccessDestroyWindowValue = 0;

void checkHandle(HWND handle);
