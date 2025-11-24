#ifndef _MODEL_H_
#define _MODEL_H_

#include "board.h"
#include "Model_Define.h"

#if ModelName == MT9700_DEMO
#include "MT9700_DEMO.h"
#elif ModelName == MT9701_DEMO
#include "MT9701_DEMO.h"
#else
#warning "ModelName not valid !"
#endif

#endif

