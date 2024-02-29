#include "../../include/webserv.hpp"


std::string Response::formatModificationTime(time_t modifTime) {
	struct tm* timeinfo = localtime(&modifTime);
	char timeStr[20];
	strftime(timeStr, sizeof(timeStr),"%d-%m-%Y %H:%M", timeinfo);
	return std::string(timeStr);
}

std::string Response::formatSize(off_t size) {
	const char * suffix[] = {"", "KB", "MB", "GB", "TB"};
	int	index = 0;
	double sizeInUnits = static_cast<double>(size);
	while (sizeInUnits >= 1024 && index < 4) {
		sizeInUnits /= 1024;
		++index;
	}
	std::ostringstream formatted;
    formatted << std::fixed << std::setprecision(0);
	if (fmod(sizeInUnits, 1.0) != 0.0)
        formatted << std::fixed << std::setprecision(1);
	formatted << sizeInUnits << suffix[index];
	return formatted.str();

}

void Response::appendHtmlHead() {
	appendStringToVector(_body, "<!DOCTYPE html>"
								"<html lang=\"en\">"
								"<head>"
								"<meta charset=\"UTF-8\">"		
								"<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
								"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
								"<title>Index of ");
	const std::string requestPath = _request.getPath();
	_body.insert(_body.end(), requestPath.begin(), requestPath.end());
	appendStringToVector(_body, "</title>"
								"<style>"
								"main {display: flex;flex-direction: column;width: 90vw;margin-top: 2em;gap: 0.5em;}"
								".rows {display: flex;flex-direction: row;width: 100%;}"
								".rows * {margin: 0;width: calc(100%/3);}"
								"hr {border: 1px solid #ccc;margin: 1em 0;}"
								"</style></head>");
					
}

void Response::appendHtmlBodyStart()
{
	appendStringToVector(_body, "<body><main>"
								"<h1>Index of ");
	const std::string requestPath = _request.getPath();
	_body.insert(_body.end(), requestPath.begin(), requestPath.end());
	appendStringToVector(_body, "</h1>"
								"<hr>");
}

void Response::appendHtmlRow(std::string& subPath, std::string& modTime, std::string& bytes) {
	appendStringToVector(_body, "<div class=\"rows\">"
								"<a href=\"/");
	appendStringToVector(_body, "..");
	const std::string requestPath = _request.getPath();
	_body.insert(_body.end(), requestPath.begin(), requestPath.end()); 
	_body.insert(_body.end(), subPath.begin(), subPath.end());
	appendStringToVector(_body, "\">");
	_body.insert(_body.end(), subPath.begin(), subPath.end());
	appendStringToVector(_body, "</a>"
								"<p>");
	_body.insert(_body.end(), modTime.begin(), modTime.end());
	appendStringToVector(_body, "</p>"
								"<p>");
	_body.insert(_body.end(), bytes.begin(), bytes.end());		
	appendStringToVector(_body, "</p></div>");
}

void Response::appendHtmlEnd() {
	appendStringToVector(_body, "</main></body></html>");
}

//generates html with the directory listing and appends it to the _body
void Response::makeDirectoryListing() {
	appendHtmlHead();
	appendHtmlBodyStart();
	DIR *dir = opendir(_path.c_str());
    if (dir == NULL)
        throw 500;
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
		if (dp->d_name[0] == '.' && dp->d_name[1] != '.')
			continue ;
		struct stat pathStat;
		std::string fullPath = _path + "/" + dp->d_name;
		if (stat(fullPath.c_str(), &pathStat) != 0)
			throw 500;
		std::string name(dp->d_name);
		std::string timeStr = formatModificationTime(pathStat.st_mtime);
		std::string bytes;
		if (S_ISDIR(pathStat.st_mode)) {
			name.append("/");
			bytes.append("-");
		}
		else
			bytes = formatSize(pathStat.st_size);
		appendHtmlRow(name, timeStr, bytes);
	}
	appendHtmlEnd();
	closedir(dir);
}
