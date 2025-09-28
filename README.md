# 🌐 Webserv - Custom HTTP Web Server

<div align="center">
  <img src="https://img.shields.io/badge/School-42-black?style=flat-square&logo=42" alt="42 School"/>
  <img src="https://img.shields.io/badge/Language-C++-blue?style=flat-square&logo=cplusplus" alt="C++"/>
  <img src="https://img.shields.io/badge/Standard-C++98-red?style=flat-square" alt="C++98"/>
  <img src="https://img.shields.io/badge/Protocol-HTTP/1.1-green?style=flat-square" alt="HTTP/1.1"/>
  <img src="https://img.shields.io/badge/CGI-Supported-orange?style=flat-square" alt="CGI"/>
</div>

## 📖 About

**Webserv** is a fully functional HTTP/1.1 web server implemented from scratch in C++98. This project is part of the 42 School curriculum and provides a deep understanding of how web servers work at a fundamental level, including socket programming, HTTP protocol implementation, and CGI support.

The server is inspired by **Nginx** and implements core web server functionality including virtual hosts, location blocks, CGI execution, file uploads, and more.

## ✨ Features

### Core HTTP Server Features
- 🌍 **HTTP/1.1 Protocol** - Full compliance with HTTP/1.1 specifications
- 🏗️ **Multi-Server Support** - Configure multiple virtual servers
- 📡 **Non-blocking I/O** - Efficient handling using `poll()` system call
- ⚙️ **Nginx-style Configuration** - Familiar configuration syntax
- 🔄 **Keep-Alive Connections** - Persistent connection support
- 📁 **Static File Serving** - Efficient static content delivery
- 📂 **Directory Listing** - Automatic index generation
- ❌ **Custom Error Pages** - Configurable error page handling

### HTTP Methods
- ✅ **GET** - Retrieve resources
- ✅ **POST** - Submit data to server
- ✅ **DELETE** - Remove resources
- ✅ **HEAD** - Retrieve headers only
- ✅ **PUT** - Upload/update resources

### Advanced Features
- 🐍 **CGI Support** - Execute PHP, Python, and other CGI scripts
- 📤 **File Uploads** - Handle multipart form data
- 🔒 **Request Size Limiting** - Configurable max body size
- 📍 **Location Blocks** - Fine-grained URL routing
- 🏠 **Virtual Hosts** - Multiple websites on one server
- 🔍 **MIME Type Detection** - Automatic content type detection
- ⏱️ **Connection Timeouts** - Configurable timeout handling

## 🛠️ Installation

### Prerequisites
- **G++** compiler with C++98 support
- **Make**
- **Unix-like system** (Linux, macOS)

### Build Instructions
```bash
# Clone the repository
git clone https://github.com/your-username/42_Webserv.git
cd 42_Webserv

# Compile the server
make

# Clean object files
make clean

# Clean everything
make fclean

# Rebuild from scratch
make re
```

## 🚀 Usage

### Starting the Server
```bash
# Run with default configuration
./webserv

# Run with custom configuration file
./webserv config/valid/default.conf

# Run with custom config
./webserv path/to/your/config.conf
```

### Testing the Server
```bash
# Test basic functionality
curl http://localhost:8080/

# Test file upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# Test CGI
curl http://localhost:8080/cgi-bin/test.php

# Test directory listing
curl http://localhost:8080/images/
```

## ⚙️ Configuration

Webserv uses an Nginx-inspired configuration syntax:

```nginx
# Server block configuration
server {
    listen 8080                          # Port to listen on
    listen 127.0.0.1:8090               # IP:Port binding
    server_name example.com www.example.com  # Server names
    root ./pages                         # Document root
    index index.html index.htm           # Index files
    error_page 404 /404.html            # Custom error pages
    client_max_body_size 1000000        # Max request body size

    # Location block for specific paths
    location / {
        allow_methods GET POST DELETE    # Allowed HTTP methods
        autoindex off                   # Directory listing
    }

    # CGI configuration
    location /cgi-bin/ {
        cgi .php /usr/bin/php-cgi       # CGI handler
        root ./test_cgi                 # CGI root directory
        allow_methods GET POST          # Allowed methods
    }

    # File upload location
    location /upload {
        root ./uploads                  # Upload directory
        allow_methods POST DELETE       # Allow uploads and deletion
        client_max_body_size 10000000   # Max upload size
    }
}

# Multiple server blocks supported
server {
    listen 8081
    server_name api.example.com
    # ... additional configuration
}
```

### Configuration Directives

| Directive | Description | Example |
|-----------|-------------|---------|
| `listen` | Port/IP to bind | `listen 8080` |
| `server_name` | Virtual host names | `server_name example.com` |
| `root` | Document root directory | `root ./pages` |
| `index` | Default index files | `index index.html` |
| `error_page` | Custom error pages | `error_page 404 /404.html` |
| `client_max_body_size` | Max request body size | `client_max_body_size 1M` |
| `allow_methods` | Allowed HTTP methods | `allow_methods GET POST` |
| `autoindex` | Directory listing | `autoindex on` |
| `cgi` | CGI script handler | `cgi .php /usr/bin/php-cgi` |

## 📁 Project Structure

```
webserv/
├── 📁 src/                     # Source files
│   ├── 📁 servers/            # Server core classes
│   │   ├── Server.cpp         # Individual server instance
│   │   └── WebServer.cpp      # Main server manager
│   ├── 📁 client/             # Client connection handling
│   │   ├── Client.cpp         # Individual client
│   │   └── ClientManager.cpp  # Client connection manager
│   ├── 📁 config/             # Configuration parsing
│   │   ├── ConfigParser.cpp   # Main config parser
│   │   ├── ServerConfig.cpp   # Server block parsing
│   │   └── LocationConfig.cpp # Location block parsing
│   ├── 📁 http/               # HTTP protocol implementation
│   │   ├── Request.cpp        # HTTP request parsing
│   │   ├── Response.cpp       # HTTP response generation
│   │   └── RequestHandler.cpp # Request routing and handling
│   ├── 📁 cgi/                # CGI execution
│   └── 📁 utils/              # Utility functions
├── 📁 inc/                    # Header files
├── 📁 config/                 # Configuration files
│   └── 📁 valid/             # Example configurations
├── 📁 pages/                  # Web content
├── 📁 test_cgi/              # CGI test scripts
└── 📄 Makefile               # Build configuration
```

## 🔧 HTTP Implementation Details

### Request Processing Pipeline
1. **🔌 Connection Acceptance** - Accept incoming connections
2. **📨 Request Parsing** - Parse HTTP request headers and body
3. **🎯 Route Resolution** - Match request to appropriate location block
4. **⚡ Handler Execution** - Process request (static file, CGI, etc.)
5. **📤 Response Generation** - Generate HTTP response
6. **🔄 Connection Management** - Handle keep-alive or close

### Supported HTTP Status Codes
- `200 OK` - Successful requests
- `201 Created` - Successful resource creation
- `204 No Content` - Successful DELETE requests
- `301 Moved Permanently` - Permanent redirects
- `400 Bad Request` - Malformed requests
- `403 Forbidden` - Access denied
- `404 Not Found` - Resource not found
- `405 Method Not Allowed` - Method not supported
- `413 Payload Too Large` - Request body too large
- `500 Internal Server Error` - Server errors
- `501 Not Implemented` - Method not implemented

### CGI Implementation
- **Environment Variables** - Full CGI environment setup
- **Process Management** - Fork/exec for script execution
- **Timeout Handling** - Prevents hanging CGI scripts
- **Input/Output Pipes** - Proper stdin/stdout handling
- **Multiple Languages** - PHP, Python, Bash, etc.

## 🧪 Testing

### Unit Testing
```bash
# Test configuration parsing
./webserv config/valid/default.conf

# Test invalid configurations
./webserv config/invalid/test.conf
```

### Load Testing
```bash
# Using Apache Bench
ab -n 1000 -c 10 http://localhost:8080/

# Using curl for concurrent requests
for i in {1..100}; do curl http://localhost:8080/ & done
```

### CGI Testing
```bash
# Test PHP CGI
curl http://localhost:8080/cgi-bin/info.php

# Test Python CGI
curl http://localhost:8080/cgi-bin/test.py

# Test POST to CGI
curl -X POST -d "name=test" http://localhost:8080/cgi-bin/form.php
```

## 🐛 Error Handling

The server handles various error scenarios:
- ❌ Invalid configuration files
- ❌ Port binding failures
- ❌ File permission errors
- ❌ CGI script timeouts
- ❌ Client disconnections
- ❌ Memory allocation failures
- ❌ Malformed HTTP requests

## 📊 Performance Features

- **⚡ Non-blocking I/O** - Efficient connection handling
- **🔄 Connection Pooling** - Reuse connections when possible
- **📦 Static File Caching** - Efficient static content delivery
- **⏱️ Request Timeouts** - Prevent resource exhaustion
- **🎯 Efficient Parsing** - Optimized HTTP parsing
- **💾 Memory Management** - Careful memory usage

## 📚 Learning Objectives

This project teaches:
- **Network Programming**: Socket programming, TCP/IP
- **HTTP Protocol**: Deep understanding of HTTP/1.1
- **System Programming**: Process management, file I/O
- **C++ Design**: Object-oriented design, RAII principles
- **Web Technologies**: CGI, MIME types, web standards
- **Performance**: Non-blocking I/O, efficient parsing

## 🔍 Key Algorithms

### Request Parsing
- **State Machine** - Efficient HTTP header parsing
- **Chunked Encoding** - Handle transfer-encoding
- **Multipart Forms** - File upload processing

### Connection Management
- **Poll-based I/O** - Handle multiple connections
- **State Tracking** - Manage connection states
- **Timeout Management** - Cleanup stale connections

## 🎯 42 School Evaluation

### Mandatory Requirements
- ✅ HTTP/1.1 compliance
- ✅ Non-blocking I/O with poll()
- ✅ Configuration file support
- ✅ GET, POST, DELETE methods
- ✅ Static file serving
- ✅ Error page handling
- ✅ CGI support

### Bonus Features
- ✅ Multiple CGI support
- ✅ Session management
- ✅ Advanced configuration options
- ✅ Performance optimizations

## 🔗 Testing Resources

- **[HTTP/1.1 RFC](https://tools.ietf.org/html/rfc7230)** - Official HTTP specification
- **[CGI Specification](https://tools.ietf.org/html/rfc3875)** - CGI standard
- **[Nginx Documentation](https://nginx.org/en/docs/)** - Configuration reference

## 👥 Contributors

- **[Your Name](https://github.com/your-username)** - Server Architecture & HTTP Implementation
- **[Partner Name](https://github.com/partner-username)** - Configuration Parser & CGI Support

## 📄 License

This project is part of the 42 School curriculum. Educational use only.

## 🙏 Acknowledgments

- **42 School** for the comprehensive web server curriculum
- **Nginx** for configuration syntax inspiration
- **HTTP/1.1 RFC** authors for the protocol specification
- The web development community for best practices

---

<div align="center">
  <strong>🌐 Ready to serve the web? Compile and deploy! 🚀</strong>
</div>
