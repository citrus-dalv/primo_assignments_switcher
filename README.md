# Primo Assignments Switcher

A C++ application for managing Primo API assignments with enable/disable functionality.

## Overview

This application provides a command-line interface to manage assignments in a Primo system. It allows users to enable or disable assignments in bulk, with support for saving and restoring assignment states.

## Features

- Command-line interface with start/stop functionality
- Secure authentication
- Configuration management
- Graceful shutdown handling
- REST API integration
- JSON data handling
- Comprehensive logging
- Cross-platform support (Windows/Linux)

## Prerequisites

- C++14 compatible compiler (g++ recommended)
- libcurl and its dependencies
- Make build system
- nlohmann/json library

### Linux Dependencies

```bash
# Ubuntu/Debian
sudo apt-get install libcurl4-openssl-dev libnghttp2-dev libssl-dev libgssapi-krb5-2 libidn2-dev libldap2-dev

# CentOS/RHEL
sudo yum install libcurl-devel nghttp2-devel openssl-devel krb5-devel libidn2-devel openldap-devel
```

## Building

### Linux

1. Clone the repository
2. Navigate to the project directory
3. Run make:

```bash
make directories
make
```

The executable will be created in the `build` directory.

### Windows

1. Clone the repository
2. Ensure you have the required libraries in the `lib` directory
3. Run make:

```bash
make directories
make
```

The executable will be created in the `build` directory.

## Configuration

The application uses a configuration file to store settings. The following parameters can be configured:

- `workfilename`: Path to store assignment states
- `restapiurl`: Primo API endpoint URL
- `username`: Default username (optional)

## Usage

### Basic Commands

```bash
# Disable all assignments and save their states
./build/prog --stop

# Enable previously disabled assignments
./build/prog --start
```

### Example
![Usage](media/usage.gif)

### Authentication

If a username is not configured, the application will prompt for it. The password will always be requested securely.

## Error Handling

The application includes comprehensive error handling:
- Invalid command-line arguments
- Authentication failures
- API communication errors
- File system errors
- Signal handling for graceful shutdown

## Logging

The application logs various events and errors. Log levels include:
- ERROR: Critical errors
- WARNING: Non-critical issues
- INFO: General information
- DEBUG: Detailed debugging information

## Signal Handling

The application handles the following signals:
- SIGINT (Ctrl+C): Graceful shutdown

## Project Structure

```
.
├── src/           # Source files
├── include/       # Header files
├── lib/           # Library files
├── build/         # Build output
├── obj/           # Object files
└── Makefile       # Build configuration
```

## License

GNU GPL 3.0

## Support

For support, please contact yusovvladislav@gmail.com
