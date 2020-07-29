# Rocha

Aka Off-Brand Lua, an embeddable """scripting""" language.

Made for a bit off fun.

## The Language

Rocha is a stack-based language.

This means operations work by "pushing" onto the stack, and then calling functions to operate on them.

Everything must be within a label

For example, to add two numbers together and print them out:

```
main:
    push 4      ;   Push 4 onto the stack
    push 5      ;   Push 5 onto the stack
    call add    ;   Call global add function
    call print  ;   Call global print function
```

It is also possible to create user-defined types using the C++ API (See the below section), and then call methods on them

For example, if a "Adder" object were created that allows you to call "setA" and "setB" to set some number, and then call "add" on it:

```
main:
    make Adder my_adder_object  ;   Creates an "Adder" object, called "my_adder_object"
    push 10                     ;   Push 4 onto the stack
    call my_adder_object setA   ;   Calls Adder::setA using 10
    push 6                      ;   Push 6 onto the stack
    call my_adder_object setA   ;   Calls Adder::setA using 6
    call my_adder_object add    ;   Call Adder::add, pushing 10 + 6 onto the stack
    call print                  ;   Call global print function, printing 11
```

Jump-Labels can also be defined:

```
some_function:
    push 5
    push 6

main:
    call some_function
    print
```

This example prints 11.


## The API

The API allows the creation of a Rocha Machine, which is able to load up scripts, have user-defined types added, and run Rocha functions.

The basics:

```cpp
int main() 
{
    // Createa a Rocha machine
    Rocha::Machine rocha;

    // Load up the script
    rocha.loadScript("test.roc");

    // Call the `main` function, which is defined by "test.roc"
    rocha.runFunction("main");
}
```

### Adding functions 

All functions must take in a Rocha::Machine* as a parameter and return void.

It is then possible to consume the stack and do operations.

```cpp
    void myAdder(Machine* machine)
    {
        float a = machine->getNumber();
        float b = machine->getNumber();
        machine->pushNumber(a + b);
    }
```

This uses `Rocha::Machine::getNumber()` to get  some numbers from the top of the stack, and then uses `Rocha::Machine::pushNumber(float)` to push a number back onto the stack.

The function can then be added:

```cpp
int main() 
{
    Rocha::Machine rocha;
    rocha.addFunction("myAdder", myAdder);
    rocha.runFunction("main");
}
```

This can be called directly from .roc files:

```
main:
    push 5
    push 6
    call myAdder
    print
```

### Adding User-Defined types

It is possible to add user-defined types and operations to Rocha.

For example, if the following is defined:

```cpp
struct Adder {
    int a;
    int b;
};

void Adder_makeAdder(Rocha::Machine* machine)
{
    // The "Constructor" must call Rocha::Machine::makeObject(...)
    Adder* adder = (Adder*)machine->makeObject(sizeof(Adder));
    adder->a = 0;
    adder->b = 0;
}

void Adder_setA(Rocha::Machine* machine)
{
    // On calling a function, the correct object is pushed onto the stack, which can be recieved doing Rocha::Machine::getObject()
    Adder* adder = (Adder*)machine->getObject();
    adder->a = machine->getNumber();
}

void Adder_setB(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->getObject();
    adder->b = machine->getNumber();
}

void Adder_add(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->getObject();
    machine->pushNumber(adder->a + adder->b);
}
```

These can be added to Rocha by using `Rocha::Machine::newType(string, constructor function, vector of functions)`

```cpp
int main() 
{
    Rocha::Machine rocha;

    // Adds the Adder type, defining the constructor and list of functions
    rocha.newType("Adder", Adder_makeAdder,
                  {
                      {"setA", Adder_setA},
                      {"setB", Adder_setB},
                      {"add", Adder_add},
                  });
    rocha.runFunction("main");
}
```

And then the Adder object can be used from .roc files:

```
main:
    make Adder my_adder_object  ;   Creates an "Adder" object, called "my_adder_object"
    push 10                     ;   Push 4 onto the stack
    call my_adder_object setA   ;   Calls Adder::setA using 10
    push 6                      ;   Push 5 onto the stack
    call my_adder_object setA   ;   Calls Adder::setA using 6
    call my_adder_object add    ;   Call Adder::add, pushing 10 + 6 onto the stack
    call print                  ;   Call global print function
```


## Building and Running

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

wot++ or something idk 
