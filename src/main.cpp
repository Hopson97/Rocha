#include "Rocha.h"

struct Adder {
    int a;
    int b;
};

void makeAdder(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->makeObject(sizeof(Adder));
    adder->a = 0;
    adder->b = 0;
}

void setA(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->getObject();
    adder->a = machine->getNumber();
}

void setB(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->getObject();
    adder->b = machine->getNumber();
}

void add(Rocha::Machine* machine)
{
    Adder* adder = (Adder*)machine->getObject();
    machine->pushNumber(adder->a + adder->b);
}

int main()
{
    Rocha::Machine rocha;
    // Add a function
    rocha.addFunction("multiplyTest", [](Rocha::Machine* machine) {
        float a = machine->getNumber();
        float b = machine->getNumber();
        machine->pushNumber(a * b);
    });

    rocha.newType("Adder", makeAdder,
                  {
                      {"setA", setA},
                      {"setB", setB},
                      {"add", add},
                  });

    if (rocha.loadScript("test.roc")) {
        rocha.runFunction("test");
    }
}
