// E:\githome\TinyCAD-OnBlock\FBExample\FBexample1\FBexample1.ino composed in TinyCad-OnBlock 2021
// 2021-12-03 21:44:34 created.
// 11 FB-classes generated.
// 15 FB-instances generated.
// Unconnected FBs are ignored.

#define DSN_NAME "FBexample1"

#include <string>
#include <WiFi.h>

using namespace std;

void loopFBs();

// -----------------------------------------------------------------
// FB Class: ESP32C3_core
// -----------------------------------------------------------------
class ESP32C3_core {
private:
	uint32_t cycletime; // Var

public:
	uint32_t cyclecnt; // Output
	int cpu; // Output

public:
	ESP32C3_core(uint32_t cycletime)
		: cycletime(cycletime) {}

	uint32_t cycle = 0, idlecnt = 0, idlecnt_max = 1;
	
	void init() {
		Serial.begin(115200);
		delay(500);
		Serial.println();
		Serial.print(DSN_NAME); Serial.println(" started...");
	}
	
	TimerHandle_t htimer;
	void static fTimer(TimerHandle_t htimer) { loopFBs(); }
	
	void start() {
		htimer = xTimerCreate("fTimer", cycletime, pdTRUE, (void*)0, &fTimer);
		xTimerStart(htimer, 100);
	}
	
	void exec() {
		if (cycle == 0) idlecnt_max = idlecnt;
	
		cpu = 100 - 100 * idlecnt / idlecnt_max;
		if(cpu < 0) cpu = 0;
	
		cyclecnt = ++cycle;
		idlecnt = 0;
	}
	
	void loop() { idlecnt++; }
};

// -----------------------------------------------------------------
// FB Class: digital_in
// -----------------------------------------------------------------
class digital_in {
private:
	uint8_t pin_num; // Var

public:
	bool out; // Output

public:
	digital_in(uint8_t pin_num)
		: pin_num(pin_num) {}

	void init() { pinMode(pin_num, INPUT); }
	
	void exec() { out = digitalRead(pin_num); }
};

// -----------------------------------------------------------------
// FB Class: and2
// -----------------------------------------------------------------
class and2 {
public:
	bool a; // Input
	bool b; // Input
	bool c; // Output

public:
	and2() {}

	void init() { c = false; }
	
	void exec() { c = a && b; }
};

// -----------------------------------------------------------------
// FB Class: digital_out
// -----------------------------------------------------------------
class digital_out {
private:
	uint8_t pin_num; // Var

public:
	bool in; // Input

public:
	digital_out(uint8_t pin_num)
		: pin_num(pin_num) {}

	void init() { pinMode(pin_num, OUTPUT); }
	
	void exec() { digitalWrite(pin_num, in); }
};

// -----------------------------------------------------------------
// FB Class: ESP32_wifi_station
// -----------------------------------------------------------------
class ESP32_wifi_station {
private:
	string ssid; // Var
	string pass; // Var

public:
	bool connected; // Output
	bool connect; // Input

public:
	ESP32_wifi_station(string ssid, string pass)
		: ssid(ssid), pass(pass) {}

		void init() {
			WiFi.mode(WIFI_STA);
		}
		
		bool connect_o = false;
	
		void exec() {
			if (!connect_o && connect && !connected) WiFi.begin(ssid.c_str(), pass.c_str());
			if (connect_o && !connect && connected) WiFi.disconnect();
			connect_o = connect;
			
			if(WiFi.status() == WL_CONNECTED) connected = true;
			else connected = false;
		}
};

// -----------------------------------------------------------------
// FB Class: print_value
// -----------------------------------------------------------------
template<typename T> class print_value {
private:
	string key; // Var

public:
	T value; // Input

public:
	print_value(string key)
		: key(key) {}

	void init() {}
	
	void exec() {
		String s = key.c_str() + String(value);
		Serial.println(s);
	}
};

// -----------------------------------------------------------------
// FB Class: counter_en_clr
// -----------------------------------------------------------------
class counter_en_clr {
public:
	bool en; // Input
	int cnt; // Output
	bool clr; // Input

public:
	counter_en_clr() {}

	void init() { cnt = {}; }
	
	void exec() {
		if(en) cnt++;
		if(clr) cnt = {};
	}
		
};

// -----------------------------------------------------------------
// FB Class: constant_1
// -----------------------------------------------------------------
class constant_1 {
public:
	bool out; // Output

public:
	constant_1() {}

	void init() {}
	
	void exec() { out = 1; }
};

// -----------------------------------------------------------------
// FB Class: constant_0
// -----------------------------------------------------------------
class constant_0 {
public:
	bool out; // Output

public:
	constant_0() {}

	void init() {}
	
	void exec() { out = 0; }
};

// -----------------------------------------------------------------
// FB Class: print_string
// -----------------------------------------------------------------
class print_string {
public:
	string str; // Input

public:
	print_string() {}

	void init() {}
	
	void exec() {
	    Serial.print(str.c_str());
	}
};

// -----------------------------------------------------------------
// FB Class: constant_string
// -----------------------------------------------------------------
class constant_string {
private:
	string val; // Var

public:
	string out; // Output

public:
	constant_string(string val)
		: val(val) {}

	void init() {}
	
	void exec() { out = val; }
};

// -----------------------------------------------------------------
// FB Instances 
// -----------------------------------------------------------------
ESP32C3_core FB1(100);
digital_in FB7(3);
digital_in FB10(4);
and2 FB9;
digital_out FB8(19);
ESP32_wifi_station FB3("OnNet", "00330033");
print_value<bool> FB4("wifi connected = ");
digital_out FB6(18);
counter_en_clr FB13;
print_value<int> FB11("counter = ");
constant_1 FB12;
constant_0 FB14;
print_string FB15;
constant_string FB16("\n");
constant_1 FB5;

// -----------------------------------------------------------------
// Calling Sequences 
// -----------------------------------------------------------------
void initFBs() {
	FB12.init();
	FB12.exec();
	FB13.en = FB12.out;

	FB14.init();
	FB14.exec();
	FB13.clr = FB14.out;

	FB16.init();
	FB16.exec();
	FB15.str = FB16.out;

	FB5.init();
	FB5.exec();
	FB3.connect = FB5.out;

	FB1.init();

	FB7.init();
	FB7.exec();
	FB9.a = FB7.out;

	FB10.init();
	FB10.exec();
	FB9.b = FB10.out;

	FB3.init();
	FB3.exec();
	FB6.in = FB3.connected;
	FB4.value = FB3.connected;

	FB13.init();
	FB13.exec();
	FB11.value = FB13.cnt;

	FB9.init();
	FB9.exec();
	FB8.in = FB9.c;

	FB8.init();
	FB8.exec();

	FB4.init();
	FB4.exec();

	FB6.init();
	FB6.exec();

	FB11.init();
	FB11.exec();

	FB15.init();
	FB15.exec();
}

void loopFBs() {
	FB1.exec();

	FB7.exec();
	FB9.a = FB7.out;

	FB10.exec();
	FB9.b = FB10.out;

	FB3.exec();
	FB6.in = FB3.connected;
	FB4.value = FB3.connected;

	FB13.exec();
	FB11.value = FB13.cnt;

	FB9.exec();
	FB8.in = FB9.c;

	FB8.exec();

	FB4.exec();

	FB6.exec();

	FB11.exec();

	FB15.exec();
}

// -----------------------------------------------------------------
// Arduino setup() & loop() 
// -----------------------------------------------------------------
void setup() {
	initFBs();
	FB1.start();
}

void loop() {
	FB1.loop();
}

// End of E:\githome\TinyCAD-OnBlock\FBExample\FBexample1\FBexample1.ino
