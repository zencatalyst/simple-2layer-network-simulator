<h1 align="center">Welcome to the README of <strong><em>Simple 2-Layer Network Simulator</em></strong> 👋</h1>

> ***Simple 2-Layer Network Simulator*** is a very basic computer network simulator that uses 2 layers (i.e. application and transport) to perform a *bidirectional multi-message simulation* between 4 processes across 2 nodes.<br />


## Build instructions

Clone the repository. Then navigate to `simple-2layer-network-simulator/`. Then run:

```shell
$ make -C src/
```

The executable will appear inside `simple-2layer-network-simulator/build/release/` or `simple-2layer-network-simulator/build/debug/` depending on the build command (release, debug, etc.) executed.

## 🚀 How to use the program:

Just run the program in the shell:

```shell
$ (cd build/release/ && ./Simple-2Layer-Network-Simulator)
```

Additionally, 8 command-line options can be used:

1. `--layers-delays=on`: adds delays to the execution of the layers by putting them to sleep for short amounts of time
2. `-d`: same as above
3. `--layers-delays=off`: no added delays in the layers
4. `--channel-faults=on`: channel will occasionally have faults when transferring bits from one node to the other causing the dependent ongoing connection to be closed
5. `-f`: same as above
6. `--channel-faults=off`: zero faults in the channel
7. `--help`: displays help info
8. `--version`: displays version info

Example:

```shell
$ (cd build/release/ && ./Simple-2Layer-Network-Simulator -f)
```

## Contributing

Contributions, issues, and feature requests are welcome.<br />

## Author

👤 **Kasra Hashemi**

- GitHub: [@zencatalyst](https://github.com/zencatalyst)

## C++ language support
- Simple 2-Layer Network Simulator requires at least **C++23**.

## Third-party

* {fmt} ([MIT License](https://github.com/fmtlib/fmt/blob/master/LICENSE.rst))
* spdlog ([MIT License](https://github.com/gabime/spdlog/blob/v1.x/LICENSE))

### License

*Simple 2-Layer Network Simulator* is licensed under the terms of the [MIT license](./LICENSE).
