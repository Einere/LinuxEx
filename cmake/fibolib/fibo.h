class Fibonacci{
private:
	int i, n;
	int first, second, third;

public:
	Fibonacci(int n):n(n){}

	~Fibonacci(){}

	void calculate();

	void print_info();
};
