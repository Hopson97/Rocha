# Rocha

Aka Off-Brand Lua, an embeddable """scripting""" language.

Made for a bit off fun.

Example:

If a file test.roc contains:
```
rochaFunctionExample:
    push 4
    push 3
    ret
test:
    call rochaFunctionExample
    call add
    call print
    push 6
    push 6
    call multiplyTest
    call print
    ret
```

Then the function `test` can be ran using:

```cpp
int main() 
{
    // Createa a Rocha machine
    Rocha::Machine rocha;

    // Add a function
    rocha.addFunction("multiplyTest", [](Rocha::Machine* machine){
        float a = machine->getNumber();
        float b = machine->getNumber();
        machine->pushNumber(a * b);
    });

    // Load up the script
    rocha.loadScript("test.roc");

    // Call the `test` function directly
    rocha.runFunction("test");
}
```

This will output 

```
7
36
```


## Bytecode

Table of bytes codes and operands etc

| Name | Opcode | Operands | Code              | What Does It Do                                                                                     |
|------|--------|----------|-------------------|-----------------------------------------------------------------------------------------------------|
| Push | 0      | n        | push 5            | Pushes "n" onto the stack                                                                           |
| Pop  | 1      |          | pop               | Pops a value from the stack                                                                         |
| Call | 2      | 2 I      | call func         | Calls a C++ Function with ID I                                                                      |
| Call | 2      | 3 R      | call func         | Calls a Rocha function at location R (Sets ip to R)                                                 |
| Call | 2      | 4 V I    | call              | Call a C++ function that is a method, with object ID V with C++ function ID I                       |
| Make | 5      | I        | make type varname | Creates an object of type "type", creating variable varname, calling C++ function I to construct it |


## Building and Running

### Libraries

SFML is required for the sandbox example application.

These can be installed from your project manager. For example, on Debian/ Ubuntu:

```sh
sudo apt install libsfml-dev
```

If this is not possible (eg windows), you can install these manually from their respective websites:

https://www.sfml-dev.org/download.php

### Linux

To build, at the root of the project:

```sh
sh scripts/build.sh
```

To run, at the root of the project:

```sh
sh scripts/run.sh
```

To build and run in release mode, simply add the `release` suffix:

```sh
sh scripts/build.sh release
sh scripts/run.sh release
```

You can also create a deployable build (that can be sent) by doing:

```sh
sh scripts/deploy.sh
```
