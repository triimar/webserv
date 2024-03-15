# webserv - a non-blocking HTTP server in C++

### Overview
Webserv is a simple HTTP server that uses `poll()` for I/O multiplexing and it is able to serve a fully static website.

The following methods are implemented:
- `GET`
- `HEAD`
- `POST`
- `DELETE`

The server can handle multiple CGI scripts, including Perl, Shell, and Python.

Created in a three person team with my peers [mtravez](https://github.com/mtravez) and [Aaron Rabenstein](https://github.com/aaron-22766)

### Themes
- Unix
- Network & system administration
- Rigor
- Object-oriented programming

### Running the Program
./webserv [configuration file]
