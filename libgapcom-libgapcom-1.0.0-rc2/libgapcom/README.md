# Libgapcom

## Author

If you have any question, please contact : geoffrey@legourrierec.epita.fr

## What's libgapcom ?

`libgapcom` is a GNU C library implementing the GAP (GISTRE ARM Project) exchange format.

`gapcli` is a GNU C executable implementing a console-based GAP client interface. 
`gapcli` uses the GAP exchange format and a serial port for communicating with a GAP server.
When invoked, it sends a single command, and expects a single response.

## Supported features

`libgapcom` offers a callback-based API for GAP exchange format.
`libgapcom` does not define the transportation medium but requires the developer to hook her own implementation.

`gapcli` integrates `libgapcom` with a serial port configured as follows :

| Property | Value |
| -------- | ----- |
| Baudrate | 9600 |
| Word length | 8 bits |
| Stop bit(s) | 1 |
| Parity | Even |
| Control flow | None |

Feature-wise, `gapcli` implements :

- Command-line selection of the serial port at program startup
- Command-line selection of the GAP request to be sent with user-friendly formatting
- Command-line display of received GAP response with user-friendly formatting

Please note that `gapcli` does not implement :

- Interactive mode (1 request is sent, and 1 response is expected)
- Other transport mediums than UART
- Other configurations of UART (hard-coded)

## Tech stack

`libgapcom` is built on top of two open-source libraries :

- [TinyFrame](https://github.com/MightyPork/TinyFrame) is a lightweight, callback-based communication API.
`TinyFrame` is decoupled from the actual transport medium and assumes no underlying transport logic.
It defines the role of the Sender and the Receiver, and its state machine implements frame parsing and callback invocation.
- [nanopb](https://github.com/nanopb/nanopb) is a C back-end for the [Protocol Buffers ("protobuf")](https://protobuf.dev/) serialization library.
In short, `protobuf` allows defining a language-agnostic data format.
A selected language transpiler (here for C) generates the actual serialization library.
`protobuf` is also decoupled from the actual transport medium.

The added value of `libgapcom` lies in defining the actual GAP exchange format in the form of GAP requests and GAP responses.
Is is important to note that `libgapcom` _DOES NOT_ modify `TinyFrame` and `nanopb` : it only configures them.

`gapcli` is a very simple client program with no extra dependencies.

## Supported platforms

### Build

Please see "Building the project" for more information on the build environment.

| Component | Linux AMD64 | Windows AMD64 |
| --------- | ----------- | ------------- |
| libgapcom | GCC + Cmake | MinGW GCC + Cmake |
| gapcli | GCC + Cmake | MinGW GCC + Cmake |

### Run

| Component | Linux AMD64 | Windows AMD64 | STM32F429 |
| --------- | ----------- | ------------- | --------- |
| libgapcom | OK | OK | OK |
| gapcli | OK | OK | NOK |

## Developer guide

### Building the project

**WARNING** : Unless you _really_ need to rebuild `gapcli`, please use the precompiled binaries offered to you in `prebuilt/`.
There is one for each supported platform.

If you're using a Mac : good luck. :-)

#### Building on Windows

##### Using CMake

This codebase uses the CMake meta build system.
It is adapted for rebuilding `gapcli` for your host machine.
The `gapcli` build system will detect the Windows platform and apply DOS-style line endings `\r\n` to logs emitted by the program.

First, install system dependencies.
We recommend using the [chocolatey](https://chocolatey.org/) package manager and provide a configuration file to that effect.
If you don't want to use `chocolatey`, just look at dependencies in the configuration file :

```powershell
# In Powershell console with admin rights :
cd host-tools\
choco install .
```

Second, install Python dependencies.
We recommend using a virtual environment :

```powershell
# In Powershell console :
mkdir .venv
.\venv\Scripts\Activate.ps1
python.exe -m pip install -r host-tools\requirements.txt
```

Should you open another Powershell console, you will need to enter this _venv_ before building.
Now let's build :

```powershell
# In Powershell console :
mkdir build
cd build
cmake -G "Unix Makefiles" ..
make
```

Please find `gap-agents\gapcli.exe`.

##### Using STM32CubeIDE

The goal here is to build `libgapcom` in a STM32CubeIDE project.

Although configuring STM32CubeIDE for using a CMake-based codebase is possible, it has not been done for this project.
We recommend creating a STM32CubeIDE library type project, and copy-pasting sources from `gen/libgapcom-stm32`.
The STM32CubeIDE project configuration is not provided here, because it would be constrainted to a given IDE version and would take up too much space.

Please take care of the following project settings :

- Language standard : GNU17
- Include paths : ../Inc/gapcom ../Inc/nanopb ../Inc/TinyFrame

You should then be able to build the library.
It requires no further modification on your side.

#### Building on Linux

##### Using CMake

This codebase uses the CMake meta build system.
It is adapted for rebuilding `gapcli` for your host machine.
The `gapcli` build system will detect the Linux platform and apply UNIX-style line endings `\n` to logs emitted by the program.

First, install system dependencies.
Depending on your distribution, package names will vary, but you need :

```
gcc
```

Second, install Python dependencies.
We recommend using a virtual environment :

```bash
# In Bash console :
mkdir .venv
. ./venv/bin/activate
python3 -m pip install -r host-tools/requirements.txt
```

Should you open another shell, you will need to enter this _venv_ before building.
Now let's build :

```bash
mkdir build
cd build
cmake ..
make
```

Please find `gap-agents/gapcli`.

##### Using STM32CubeIDE

This has not been tested on Linux, but should behave similarly as on Windows.
See dedicated paragraph above.

### Testing the project

This was tested using :

- A Linux or a Windows PC,
- An STM32F429 devkit "DISCO-1",
- An USB cable allowing powering and programming the STM32F429 devkit.

A test program on the board has been configured with STM32CubeIDE to fit the UART configuration described in the "Supported features" section above.
It uses an UART running on top of the USB line.

#### Testing on Windows

Testing with [Termite](https://www.compuphase.com/software_termite.htm) as console-over-serial program.
Assuming that COM port "COM3" is the one matching our serial-over-USB port :

```powershell
gapcli.exe -com COM3 ping
```

#### Testing on Linux

Testing with [cutecom](https://cutecom.sourceforge.net/) as console-over-serial program.
Assuming that pseudo-TTY /dev/ttyUSB0 is the one matching our serial-over-USB port :

```powershell
gapcli.exe -com /dev/ttyUSB0 ping
```

### Integrating libgapcom in my application

Libgapcom is based on TinyFrame and Protocol Buffers, but you only need to be aware of the latter to use the library.
The definition of Protocol Buffers messages is available in _gap-com/proto/gap.proto_.

Libgapcom is an asynchronous communication library, which means sending and receiving are not tied together.
The _client_ and the _server_ exchange requests and responses.
It is always the client that sends requests, and always the server that sends responses.
The client always takes the initiative.

To process responses, the client installs callbacks at program initialization that will be automatically called by the libgapcom state machine when given the received data.

You should take a look at the _gapcli.c_ C file to have an example of client code.
No server implementation is available, so we'll let you figure that out. ;-)

The key steps are :

1. Create a session handle with `gapcom_create()`
2. Install callbacks for response type messages using `gapcom_install_callback()`. For example, install a callback to be automatically invoked when `GAPCOM_MSG_PING_RESP` is received (Ping response)
4. Explain to libgapcom how to send bytes with `gapcom_set_sender_impl()`. This is your responsibility: should bytes be sent though a serial port ? A socket ? A pidgeon ?
5. Send requests
6. Read bytes (depends again on the communication medium you implemented) and call `gapcom_accept()` on the buffer you just filled.
The libgapcom state machine will parse the frame, and if it finds a callback for that message type, invoke it with the Protocol Buffer payload as a argument
