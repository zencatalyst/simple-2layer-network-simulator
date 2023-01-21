<h1 align="center">Welcome to the README of <strong><em>Simple 2-Layer Network Simulator</em></strong> 👋</h1>

> ***Simple 2-Layer Network Simulator*** is a very basic computer network simulator that uses 2 layers (i.e. application and transport) to perform a *bidirectional multi-message simulation* between 4 processes across 2 nodes.<br />


## Build

Clone the repository. Then navigate to the `/simple-2layer-network-simulator/src`. Then run:

```sh
$ make
```

The executable will appear inside `/simple-2layer-network-simulator/build/release`.

## 🚀 How to use the program:

Just run the program in the shell:

```sh
$ ./build/release/Simple-2Layer-Network-Simulator
```

Additionally, 4 command-line arguments can be provided:

1. `-layer-delays=on`: adds delays to the execution of the layers by putting them to sleep for short amounts of time
2. `-layer-delays=off`: no delays in the layers
3. `-faulty-channel=yes`: channel will occasionally have faults when transferring bits from one node to the other causing the dependent ongoing connection to be closed 
4. `-faulty-channel=no`: zero faults in the channel

## Contributing

Contributions, issues, and feature requests are welcome.<br />

## Author

👤 **Kasra Hashemi**

- Github: [@zencatalyst](https://github.com/zencatalyst)

### License

*Simple 2-Layer Network Simulator* is licensed under the terms of the [MIT license](./LICENSE).
