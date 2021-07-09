#include <iostream>
#include <Windows.h>
#include "KeyBinder.hpp"

using namespace std;

class Example {

public:
	Binder<Example> binder;

public:
	Example() {
		binder.Bind('A', &Example::Keys, this);
	}

	void Keys(int key, bool pressed) {
		cout << "Key " << (char)key << " " << (pressed ? "PRESSED" : "RELEASED") << "\n";
	}
};


int main() {
	Example example;

	while(true) { Sleep(1000); }
}