#include "WarpContext.h"


WarpContext::WarpContext(int threadIndex, Context *context)
    : threadIndex(threadIndex), context(context) {}
