#include "../include/webserv.hpp"

int main(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "Too many arguments" << std::endl;
		return (EXIT_FAILURE);
    }

	try {
		std::string file = DEFAULT_CONFIG;
		if (argc > 1) {
			file = argv[1];
        }
		Config config(file.c_str());
		config.startServers();
		config.runServers();

	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;;
	}

    return (EXIT_SUCCESS);
}
