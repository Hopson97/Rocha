#include "Rocha.h"
#include "Builtin.h"
#include <iostream>

#define ROCHA_DEBUG
#ifdef ROCHA_DEBUG
void printout(const char* c)
{
    printf("%s\n", c);
}
#else
void printout(const char*)
{
}
#endif

namespace Rocha {
    Machine::Machine()
    {
        addFunction("add", Rocha::add);
        addFunction("print", Rocha::print);
    }

    void Machine::pushNumber(float value)
    {
        StackObject stackObject;
        stackObject.type = Type::Number;
        stackObject.data.number = value;
        m_stack.push(stackObject);
    }

    void Machine::pushObject(void* object)
    {
        StackObject stackObject;
        stackObject.type = Type::Object;
        stackObject.data.object = object;
        m_stack.push(stackObject);
    }

    float Machine::getNumber()
    {
        if (m_stack.top().type == Type::Number) {
            float f = m_stack.top().data.number;
            m_stack.pop();
            return f;
        }
        else {
            return 0.0f;
        }
    }

    void* Machine::getObject()
    {
        if (m_stack.top().type == Type::Object) {
            void* ptr = m_stack.top().data.object;
            m_stack.pop();
            return ptr;
        }
        else {
            return nullptr;
        }
    }

    int Machine::findConstructorLocation(const std::string& name) const
    {
        auto itr = m_constructors.find(name);
        if (itr != m_constructors.end()) {
            return itr->second;
        }
        else {
            return -1;
        }
    }

    int Machine::findObjectMethod(const std::string& type, const std::string& name) const
    {
        auto func = m_objectFunctions.at(type).find(name);
        if (func != m_objectFunctions.at(type).end()) {
            return func->second;
        }
        return -1;
    }

    void Machine::run()
    {
        while (true) {
            Instruction ins = (Instruction)m_bytecode[m_instructionPtr++];
            switch (ins) {
                case Instruction::Push:
                    printout("PUSH");
                    pushNumber((float)m_bytecode[m_instructionPtr++]);
                    break;

                case Instruction::Pop:
                    printout("POP");
                    m_stack.pop();
                    break;

                case Instruction::Call:
                    switch ((Instruction)m_bytecode[m_instructionPtr++]) {
                        case Instruction::Jump:
                            printout("JUMP");
                            m_callStack.push(m_instructionPtr + 1);
                            m_instructionPtr = m_bytecode[m_instructionPtr++];
                            break;

                        case Instruction::Call:
                            printout("CALL");
                            m_calls[m_bytecode[m_instructionPtr++]].second(this);
                            break;

                        case Instruction::Method: {
                            printout("METHOD");
                            auto object = m_bytecode[m_instructionPtr++];
                            pushObject(m_objectAlloc[object]);
                            m_calls[m_bytecode[m_instructionPtr++]].second(this);
                            break;
                        }

                        default:
                            std::printf("Run error, unknown function type");
                            return;
                    }
                    break;

                case Instruction::Make: {
                    printout("MAKE");
                    int sizeBefore = m_objectAlloc.size();
                    m_calls[m_bytecode[m_instructionPtr++]].second(this);
                    int sizeAfter = m_objectAlloc.size();
                    if (sizeBefore == sizeAfter) {
                        std::printf("Run error, make must construct an object! (call");
                        return;
                    }
                } break;

                case Instruction::Return:
                    printout("RETURN");
                    if (!m_callStack.empty()) {
                        m_instructionPtr = m_callStack.top();
                        m_callStack.pop();
                        break;
                    }
                    return;

                default:
                    std::printf("Run error, unknown instruction %d", (int)ins);
                    return;
            }
        }
    }

    void Machine::runFunction(const std::string& name)
    {
        m_instructionPtr = m_jumps.find(name)->second;
        run();
    }

    void Machine::addFunction(const std::string& name, RochaFunction f)
    {
        m_calls.push_back(std::make_pair(name, f));
    }

    void
    Machine::newType(const std::string& name, RochaFunction onMake,
                     const std::vector<std::pair<std::string, RochaFunction>>& methods)
    {
        addFunction(name, onMake);
        m_constructors.emplace(name, m_calls.size() - 1);
        std::cout << "Added constructor to location " << m_calls.size() - 1 << std::endl;
        std::unordered_map<std::string, size_t> methodMap;
        for (auto& method : methods) {
            int index = m_calls.size();
            addFunction(name + method.first, method.second);
            methodMap.emplace(method.first, index);
        }
        m_objectFunctions.emplace(name, methodMap);
    }

    void* Machine::makeObject(size_t size)
    {
        return m_objectAlloc.emplace_back(std::calloc(0, size));
    }

} // namespace Rocha