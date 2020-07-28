#include "Rocha.h"
#include <iostream>

namespace Rocha {
    void add(Machine* machine)
    {
        float a = machine->getNumber();
        float b = machine->getNumber();
        machine->pushNumber(a + b);
    }

    void print(Machine* machine)
    {
        float a = machine->getNumber();
        std::cout << a << std::endl;
    }
} // namespace Rocha
