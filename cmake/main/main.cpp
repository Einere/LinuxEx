#include <iostream>
#include "fibo.h"
using namespace std;

int main(){
	int ans = 0;

	cout << "enter the fibonacci(n). (n>=1)" << endl;
	cin >> ans;

	Fibonacci fibo(ans);

	fibo.calculate();
	fibo.print_info();

	return 0;

}


