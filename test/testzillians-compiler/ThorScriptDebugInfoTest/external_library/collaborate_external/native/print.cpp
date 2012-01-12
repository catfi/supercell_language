#include <iostream>

void print()
{
	std::cout << "Hello World!" << std::endl;
}

void print_value(short int x)
{
	std::cout << x << std::endl;
}

extern void callback();

void please_callback()
{
	callback();
}
