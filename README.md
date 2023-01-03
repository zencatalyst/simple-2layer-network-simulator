<h1 align="center">Welcome to the README of Simple 2-Layer Network Simulator 👋</h1>

> ***Simple 2-Layer Network Simulator*** is a very basic computer network simulator that uses 2 layers (i.e. application and transport) to perform a *bidirectional multimessage simulation* between 4 processes across two nodes.<br />


## Build

Download the repository. Then go to the `/simple-2layer-network-simulator/src`:

```sh
make
```

The executable will be inside `/simple-2layer-network-simulator/build/release`.

## 🚀 How to use the program:

Just run the program in the shell:

```sh
$ ../build/release/Simple-2Layer-Network-Simulator
```

Additionally, 4 command-line arguments can be provided:

1. `-layer-delays=on`: adds delays to the layers by putting them to sleep for short amounts of time
2. `-layer-delays=off`: no delays in the layers
3. `-faulty-channel=yes`: channel will have fault when transferring bits from one node to the other
4. `-faulty-channel=no`: zero fault in the channel

## Contributing

Contributions, issues and feature requests are welcome.<br />

## Author

👤 **Kasra Hashemi**

- Github: [@zencatalyst](https://github.com/zencatalyst)

### License

*Simple 2-Layer Network Simulator* is licensed under the terms of the [MIT license](./LICENSE).
