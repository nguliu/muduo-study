#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <stdio.h>

typedef boost::function<void(int)> Callback;

class Test {
public:
	void setCallback(Callback cb) { _callback = cb; }

	void run(int n) {
		_callback(n);
	}

	void callback1(int n) {
		printf("I am callback1\n");
	}

	void callback2() {
		printf("I am callback2\n");
	}

private:
	Callback _callback;
};


int main() {
	Test test;

	test.setCallback(boost::bind(&Test::callback1, &test, _1));
	test.run(1);

	test.setCallback(boost::bind(&Test::callback2, &test));
	test.run(2);

	return 0;
}