// Invoke necessary header files
#include "Driver.hpp"

// Simple program main() function
int main() {
	// Prompt user to hit a key to run
	std::cout << "Press Enter to Run";
	std::cin.ignore();

	// Instantiate the Driver class
	Driver* driver = new Driver("ConwayLowThrust");

	// Run the driver
	Integer exitFlag = driver->Run();

	// Prompt user to hit a key to exit
	std::cout << "\nPress Enter to Exit";
	std::cin.ignore();

	// Delete driver instantiation
	delete driver;

	// return exit flag to caller
	return exitFlag;
}