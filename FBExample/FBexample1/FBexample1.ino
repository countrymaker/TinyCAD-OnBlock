// E:\githome\TinyCAD-OnBlock\FBExample\FBexample1\FBexample1.ino composed in TinyCad-OnBlock 2021
// 2021-12-03 00:31:46 created.
// 5 FB-classes generated.
// 6 FB-instances generated.
// Unconnected FBs are ignored.

#define DSN_NAME "FBexample1"

#include <string>

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
	uint8_t cpu; // Output

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
		cyclecnt = ++cycle;
		idlecnt = 0;
	}
	
	void loop() { idlecnt++; }
};

// -----------------------------------------------------------------
// FB Class: print_value
// -----------------------------------------------------------------
class print_value {
private:
	string key; // Var

public:
	int value; // Input

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
// FB Instances 
// -----------------------------------------------------------------
ESP32C3_core FB1(1000);
print_value FB2("cpu(%) = ");
digital_in FB3(3);
digital_in FB6(4);
and2 FB5;
digital_out FB4(18);

// -----------------------------------------------------------------
// Calling Sequences 
// -----------------------------------------------------------------
void initFBs() {
	FB1.init();
	FB2.value = FB1.cpu;

	FB2.init();
	FB2.exec();

	FB3.init();
	FB3.exec();
	FB5.a = FB3.out;

	FB6.init();
	FB6.exec();
	FB5.b = FB6.out;

	FB5.init();
	FB5.exec();
	FB4.in = FB5.c;

	FB4.init();
	FB4.exec();
}

void loopFBs() {
	FB1.exec();
	FB2.value = FB1.cpu;

	FB2.exec();

	FB3.exec();
	FB5.a = FB3.out;

	FB6.exec();
	FB5.b = FB6.out;

	FB5.exec();
	FB4.in = FB5.c;

	FB4.exec();
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
