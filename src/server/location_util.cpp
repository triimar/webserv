//
//
//// checks location config for `loc`, if there is one and has allow_methods checks if `method` is allowed - otherwise tries (global) server config for allow_methods
//bool Server::checkLocationMethod(std::string &path, RequestMethod method) const {
//
//}
//
//// takes request path looks in location config, if there is root combine the paths and return - otherwise use (global) server config for root
//std::string Server::getLocationPath(std::string &path) const {
//    // combinePaths(correct_root, request);
//}
//
//// checks if location has a return and sets status and body accordingly
//bool Server::hasLocationReturn(std::string &path, uint16_t &status, std::vector<char> &body) const {
//
//}
//
//// checks if location allows the request method
//bool Server::isLocationMethodAllowed(std::string &path, RequestMethod method) const {
//
//}
//
//// returns path to error page of that status if it was defined in location
//std::string Server::getLocationErrorPage(std::string &path, uint16_t status) const {
//
//}
