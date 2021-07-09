#include <iostream>
#include <memory>
#include <map>
#include <thread>
#include <mutex>

template <typename T>
using KeyBindCallBack = void(T:: *)(int, bool);

template <typename T>
struct KeyBind {
	T *ClassAdress;
	KeyBindCallBack<T> CallBack;
	bool IsPressed;
};

template <typename T>
class Binder {

public:
	Binder();
	~Binder();

public:
	void Bind(int keyCode, KeyBindCallBack<T> callBack, T *classAdress);
	void Unbind(int keyCode);
	void Listener();

private:
	std::shared_ptr<std::map<int, KeyBind<T>>> m_Keys;
	bool m_Running;
	std::thread *m_Thread;
	std::mutex m_Mutex;

};

template <typename T>
Binder<T>::Binder(): m_Running(false) {
	m_Keys = std::make_shared<std::map<int, KeyBind<T>>>();
}

template <typename T>
Binder<T>::~Binder() {
	m_Running = false;
	m_Thread->join();
	//delete m_Thread;
}

template <typename T>
void Binder<T>::Bind(int keyCode, KeyBindCallBack<T> callBack, T *classAdress) {
	KeyBind<T> keyBind;
	keyBind.CallBack = callBack;
	keyBind.ClassAdress = classAdress;
	keyBind.IsPressed = false;

	m_Keys->insert({keyCode, keyBind});

	if(m_Thread == nullptr) {
		m_Running = true;
		m_Thread = new std::thread(&Binder::Listener, this);
	}
}

template <typename T>
void Binder<T>::Unbind(int keyCode) {
	std::lock_guard<std::mutex> lock(m_Mutex);
	{
		m_Keys->erase(keyCode);
		// TODO: Delete / Stop thread if m_Keys size = 0.
	}
}

template <typename T>
void Binder<T>::Listener() {
	int anyKeyDown = 0;

	while(m_Running) {
		std::lock_guard<std::mutex> lock(m_Mutex);
		{
			for(auto it = m_Keys->begin(); it != m_Keys->end(); it++) {
				if(GetAsyncKeyState(it->first) < 0) {
					anyKeyDown = it->first;

					KeyBind<T> keyBind = it->second;

					(keyBind.ClassAdress->*keyBind.CallBack)(it->first, true);
					it->second.IsPressed = true;
				}
				else if(GetAsyncKeyState(it->first) >= 0 && it->second.IsPressed) {
					KeyBind<T> keyBind = it->second;

					(keyBind.ClassAdress->*keyBind.CallBack)(it->first, false);
					it->second.IsPressed = false;
				}
			}
			while(GetAsyncKeyState(anyKeyDown) < 0) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
