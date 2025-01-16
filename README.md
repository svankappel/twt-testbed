# TWT-Testbed

## Requirements

The testbed supports the following development kits:

| Hardware platforms | PCA      | Board target                      |
|--------------------|----------|-----------------------------------|
| nRF7002 DK         | PCA10143 | ```nrf7002dk/nrf5340/cpuapp/ns``` |


## Overview

The testbed is designed to test the Wi-Fi TWT feature. Four use cases are available and all include a PS and a TWT test and can work with a private or public server. 

The testbed implements a CoAP client and can use either directly UDP or add a DTLS security layer. Both IPv4 and IPv6 can be used. 

The testbed is entirely configurable using kconfigs. The tests can also be configured with kconfigs. Some example of configuration files are ```overlay-alltests-private.conf``` or ```overlay-alltests-public.conf```, in the app folder.

A detailed report is available in the [doc](./doc) folder. The test usecases are detailed in the Testbed Design chapter. The Testbed Client Implementation details the implementation of this code. The chapter also includes a complete description of the configuration.


## Server
The testbed can be used with two servers: a private server designed for the testbed and a public server. The private server implements more advanced tests and gives more information. The public server is hosted on the Internet, allowing the tests to be performed in a commercial application scenario.

The private server is available in the [server](./server) folder.

The testbed can also be utilized with the Eclipse Californium server, a CoAP
sandbox server available on the Internet for testing purposes. It is accessible at ```californium.eclipseprojects.io``` on port ```5683``` for CoAP/UDP and port ```5684``` for CoAP/DTLS. The server is accessible using both IPv4 and IPv6.

The DTLS credentials are provided in the README of the [eclipse-californium GitHub repository](https://github.com/eclipse/californium).

## Results

The test results are displayed in the logs at the end of each test. The [scripts](./scripts) folder contains python scripts to parse the logs and generate test reports.



## License

This project is licensed under the Nordic Semiconductor ASA 5-Clause License. For more details, see the [LICENSE](./LICENSE) file.
