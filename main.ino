#include "src/debug/debug.h"


void setup(){
    Serial.begin(115200);
    Serial.println("============= Start =============");
	DebugHandler Debugger = DebugHandler();
    Debugger.printDebug();
    Serial.println("============= Stop =============");

}

void loop(){
	
}