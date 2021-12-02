corevoid11000#include <string>
uint32_t get_cycletime() { return cycletime; }

TimerHandle_t htimer;
void static ftimer(TimerHandle_t htimer) { loopFBs(); }

void init() {
    Serial.begin(115200);
    delay(500);
    Serial.println();
    Serial.print(DSN_NAME); Serial.println(" started...");

    htimer = xTimerCreate("ftimer", get_cycletime(), pdTRUE, (void*)0, &ftimer);
    xTimerStart(htimer, 10);
}

void exec() {}

void loop() {}
55peribool519void init() { pinMode(pin_num, OUTPUT); }

void exec() { digitalWrite(pin_num, in); }55perivoid2"OnNet""00330033"#include <WiFi.h>

void init() {
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid.c_str(), pass.c_str());
	Serial.print("Connecting to ");
	Serial.println(ssid.c_str());
}

void exec() {
	if(!connected) {
		if(WiFi.status() == WL_CONNECTED) {
			connected = true;
			Serial.print("WiFi connecetd to ");
			Serial.println(WiFi.SSID());
		}
	}
	else if(WiFi.status() != WL_CONNECTED) connected = false;
}
55peribool618void init() { pinMode(pin_num, OUTPUT); }

void exec() { digitalWrite(pin_num, in); }55void init() { c = false; }

void exec() { c = a && b; }55logicbool3void init() { out = {}; }

void exec() { out = !in; }55logicbool4void init() {}

void exec() {
	string str = key + to_string(value);
	Serial.println(str.c_str());
}
55peribool8"connected = "void init() {}

void exec() { out = val; }55conststring0"ABC"void init() {}

void exec() { out = val; }55conststring0"안녕"void init() {}

void exec() { out = val; }55conststring0"ㅋㅋ"void init() {}

void exec() { out = val; }55conststring0""void init() {}

void exec() { out = val; }55conststring0""void init() {}

void exec() { out = val; }55conststring0"\n"void init() {}

void exec() {
    Serial.print(str.c_str());
}
55peristring101logicT3void init() { c = {}; }

void exec() { c = a + b; }55logicT1void init() { c = {}; }

void exec() { c = a + b; }55logicT2void init() { c = {}; }

void exec() { c = a + b; }55logicT1void init() { c = {}; }

void exec() { c = a + b; }55logicT2void init() { c = {}; }

void exec() { c = a + b; }55