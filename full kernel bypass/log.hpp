#pragma once

#define log(x, ...) DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, x, __VA_ARGS__)