#include "Debugger.h"
#include "Windows.h"

void Debugger::ErrorCheck(long result) {
	if (FAILED(result)) {
		throw result;
	}
}