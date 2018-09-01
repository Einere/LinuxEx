#include <iostream>
#include "fibo.h"
using namespace std;

void Fibonacci::calculate(){
	first = 0, second = 1, third = 0;
	
	if(n <= 2){
		third = n-1;
	}
	else{
		for(i=3; i<=n; i++){
			third = first + second;
			first = second;
			second = third;
		}
	}
}

void Fibonacci::print_info(){
	cout << "fibonacci(" << n << ") is " << third << endl;
}

