#include "src/debug/debug.h"


class Mod2 : public CrashableModule {
    public:
        Mod2(UnCrashable &uncrashableParent, bool addSelfToParent = true) 
            : CrashableModule(uncrashableParent, addSelfToParent) {};

        void minorFailure(const char* func, const char* file, u16 failLine);

};

void Mod2::minorFailure(const char* func, const char* file, u16 failLine){
    DBG_PRINTLN("Minor Failure in Mod2");
}




void setup(){
    Serial.begin(115200);
    Serial.println("============= Start =============");
	Serial.println(BUILD_DATE); 
	Serial.println(BUILD_VERSION); 
    Serial.println(_PRINTF_BUFFER_LENGTH_);

    UnCrashable DemoRoot;
    CrashableModule Mod1(DemoRoot, false);
    Mod2 Mod2(DemoRoot);
    CrashableModule Mod3(DemoRoot, false);

    DemoRoot.addModule(Mod3);
    
    DemoRoot.minorFailure(__FUNCTION__, __FILE__, __LINE__);
    
    Serial.println("============= Stop =============");

}

void loop(){
	
}