#include "../../include/Response.hpp"

void Response::processRequest() {
    checkLocation();

    if (isCGI()) {
        executeCGI();
        return ;
    }
   
    switch (_request.getMethod()) {
    case GET: 
        performGET();
        return ;
    case POST: 
        performPOST();
        return ;
    case DELETE: 
        performDELETE();
        return ;
    default:
        _status = 501;
    }
}

void Response::checkLocation() {
    _location = getLocation(_request.getPath());

    std::vector<RequestMethod> allowedMethods = _location.getAllowedMethods();
    if (std::find(allowedMethods.begin(), allowedMethods.end(), _request.getMethod()) == allowedMethods.end()) {
       throw 405; // request method is not allowed
    }

    _path = _location.getRoot() + _request.getPath();
    if (access(_path.c_str(), F_OK) != 0) {
        throw 404;
    }
    if (stat(_path.c_str(), &_pathStat) != 0) {
        throw 500;
    }

    // int returnStatus = _location.getReturn();
    // if (returnStatus != 0) {
    //     throw returnStatus;
    // }
}

void Response::fileToBody(std::string &path) {
    int fd = open(path.c_str(), O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            throw 404;
        }
        throw 500;
    }
    if (readToVector(fd, _body) == RETURN_FAILURE) {
        close(fd);
        throw 500;
    }
    close(fd);
    _status = 200;
}

//added by Triin
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
	if (fmod(sizeInUnits, 1.0) != 0.0)
        formatted << std::fixed << std::setprecision(1);
    else
        formatted << std::fixed << std::setprecision(0);
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

void Response::appendHtmlRow(std::string& subPath, std::string& modTime, std::string& bytes) {

	appendStringToVector(_body, "<div class=\"rows\">"
								"<a href=\"/..\">");
	_body.insert(_body.end(), subPath.begin(), subPath.end());
	appendStringToVector(_body, "</a>"
								"<p>");
	_body.insert(_body.end(), modTime.begin(), modTime.end());
	appendStringToVector(_body, "</p>"
								"<p>");
	_body.insert(_body.end(), bytes.begin(), bytes.end());		
	appendStringToVector(_body, "</p></div>");
}

//generates html with the directory listing and appends it to the _body
void Response::makeDirectoryListing(std::string& path) {
	appendHtmlHead();
	appendStringToVector(_body, "<body><main>"
								"<h1>Index of ");
	const std::string requestPath = _request.getPath();
	_body.insert(_body.end(), requestPath.begin(), requestPath.end());
	appendStringToVector(_body, "</h1>"
								"<hr>");
	DIR *dir = opendir(path.c_str());
    if (dir = NULL)
        throw 500;					//actual error - should be caught
    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
		if (dp->d_name[0] == '.' && dp->d_name[1] != '.')
			continue ;
		struct stat pathStat;
		std::string fullPath = path + "/" + dp->d_name;
		if (stat(fullPath.c_str(), &pathStat) == 0) {
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
		else 
       		throw 500;			//error - should be caught
	}
	appendStringToVector(_body, "</main></body></html>");
	closedir(dir);
}
// ------------

void Response::performGET() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
        fileToBody(_path);
        return ;
    }
    if (_path.back() != '/') {
        throw 301; // why???
    }
    std::string index = getIndex();
    if (index.empty() == false) {
        fileToBody(index);
        return ;
    }

    if (_server.getAutoIndexing() == true) {
        makeDirectoryListing();
    }

    throw 403;
}

void Response::performPOST() {
    if (_request.getHeaderValueForKey("Content-Length").empty()) {
        throw 411;
    }
    // if (Response::isSupportedMIMEType(_request.getHeaderValueForKey("Content-Type")) == false) {
    //     return (415);
    // }

    // what happends with directories

    // create path with type

    // if ( /*location doesn't support upload*/ ) {
    //     return (403);
    // }

    _status = 201;
}

void Response::performDELETE() {
    if (S_ISDIR(_pathStat.st_mode) == false) {
        if (std::remove(_path.c_str()) == -1) {
            throw 500;
        }
        _status = 204;
        return ;
    }
    if (_path.back() != '/') {
        throw 409; // why??
    }
    if (access(_path.c_str(), W_OK) == -1) {
        throw 403;
    }
    if (rmdir(_path.c_str()) == -1) {
        throw 500;
    }
    _status = 204;
}
