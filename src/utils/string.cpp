
#include "../../include/utils.hpp"

void strToLower(std::string& str) {
    for (std::size_t i = 0; i < str.size(); ++i) {
        str[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(str[i])));
    }
}

std::string& trimString(std::string& str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start != std::string::npos)
		str.erase(0, start);
	else {
		str.clear();
		return str;
	}
	size_t end = str.find_last_not_of(" \t\r\n");
	if (end != std::string::npos)
		str.erase(end + 1);
	return str;
}

bool containsControlChar(std::string& str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (std::iscntrl(static_cast<unsigned char>(*it))) {
			return true;
		}
	}
	return false;
}