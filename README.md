# One DayGlow to Rule Them All

Monorepo for the Metropolitan Aerospace Combustion Hub software team, focusing on combining code and readable documentation inline for future members.

# Engine Computer
<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/ISO_C%2B%2B_Logo.svg/306px-ISO_C%2B%2B_Logo.svg.png" alt="c++ logo" height="50px">

[Engine computer](engine-computer/src) is the software component written in [C++](https://en.cppreference.com/w/) that controls the LabJack devices used on our ground support equipment (GSE).

Currently, the rocket config in folder [config](config/) is parsed and executed by engine-computer (see [config_parser](engine-computer/src/config_parser.cpp)), making it the main control software in the system (at least until Dayglo).

# Go Server
<img src="https://go.dev/blog/go-brand/Go-Logo/PNG/Go-Logo_Aqua.png" alt="golang logo" height="50px">

The [go server](go/src) is our software component written in [Golang](https://go.dev/doc/) that currently acts as an interface between the GUI and engine-computer. It's really easy to make cross-platform programs in Go (unlike C++), so the purpose of this component is to be able to move some complexity away from the C++ code.

Additionally, one of the requirements for our new system design is the ability to control and aquire sensor data from both the GSE and rocket at the same time. Therefore the go server will also interface between the GUI and the avionics software.

Another cool feature of the go server is that it allows multiple clients to be connected at the same time (by broadcasting sensor data to all clients). This helps keep our engine-computer simple as it only needs to accept one connection (the go server).

# GUI
<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/c3/Python-logo-notext.svg/1869px-Python-logo-notext.svg.png" alt="python logo" height="50px">

The [gui](/gui/main.py) is the frontend interface that connects to the go server and allows easy control of the rocket. 

Using the [tkinter framework](https://docs.python.org/3/library/tkinter.html), it consists of a single file with buttons to open or close any valve configured on the system, along with bars to display sensor readings like pressure transmitter or thermocouple readings.


# File Structure
```
monorepo/
├── CMakeLists.txt
├── engine-computer/
│   ├── CMakeLists.txt
│   ├── readme
│   └── src/
│       └── ...
├── go/
│   ├── CMakeLists.txt
│   ├── readme
│   └── src/
│       └── ...
├── proto/
│   ├── CMakeLists.txt
│   ├── readme
│   └── src/
│       └── ...
├── ...
└── docs/
    ├── readme
    ├── pdfs/
    │   └── ...
    ├── installation/
    │   └── ...
    └── code/
        └── ...
```
