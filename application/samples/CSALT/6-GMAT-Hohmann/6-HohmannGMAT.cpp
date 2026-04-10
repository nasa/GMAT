// Invoke necessary header files
#include "Driver.hpp"
#include "Moderator.hpp"

static Moderator* mod = NULL;

// Simple program main() function
int main() {
	// Prompt user to hit a key to run
	std::cout << "Press Enter to Run";
	std::cin.ignore();

	std::string startUpFile = "gmat_startup_file.txt";

	mod = Moderator::Instance();
	if (mod == NULL || !(mod->Initialize(startUpFile))) {
		std::cout << "Moderator failed to initialize! Unable to run CSALT with GMAT." << std::endl;
		exit(EXIT_FAILURE);
	}
	mod->CreateDefaultParameters();

	// Instantiate the Driver class
	Driver* driver = new Driver("HohmannGMAT");

	// Run the driver
	Integer exitFlag = driver->Run();

	// Prompt user to hit a key to exit
	std::cout << "\nPress Enter to Exit";
	std::cin.ignore();

	// Delete driver instantiation
	delete driver;

	Moderator::Instance()->Finalize();

	// return exit flag to caller
	return exitFlag;
}