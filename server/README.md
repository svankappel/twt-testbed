# TWT Testbed Server

The TWT Testbed Server is implemented in Java using the Californium library.

It runs two servers:
- A CoAP server on port 5683.
- A secure CoAPs server using DTLS on port 5684.

The secure server uses PSK (Pre-Shared Key) authentication with the following credentials:
- **ID**: `twttestbed`
- **PSK**: `secretkey`

This repository only contains the executable. The full project repository is available at the following link:

- [TWT-Testbed-Server Repository](https://github.com/svankappel/twt-testbed-server)


## Prerequisites

To run the server, you need to have Java installed on your system.

### Installing Java

This project requires Java 11. You can install OpenJDK 11 on Ubuntu using the following commands:

```sh
sudo apt update
sudo apt install openjdk-11-jdk
```

Verify the installation by checking the Java version:

```sh
java --version
```

You should see an output similar to:

```sh
openjdk 11.0.25 2024-10-15
OpenJDK Runtime Environment (build 11.0.25+9-post-Ubuntu-1ubuntu124.04)
OpenJDK 64-Bit Server VM (build 11.0.25+9-post-Ubuntu-1ubuntu124.04, mixed mode, sharing)
```

## Running the Pre-Built Executable

To run the server, use the following command:

```sh
java -jar twt-testbed-server.jar
```
