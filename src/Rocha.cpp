#include "Rocha.h"
#include "Builtin.h"
#include <iostream>

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

    void Machine::run()
    {
        while (true) {
            switch ((Instruction)m_bytecode[m_instructionPtr++]) {
                case Instruction::Push:
                    pushNumber((float)m_bytecode[m_instructionPtr++]);
                    break;

                case Instruction::Pop:
                    m_stack.pop();
                    break;

                case Instruction::Call:
                    switch ((Instruction)m_bytecode[m_instructionPtr++]) {
                        case Instruction::Jump:
                            m_callStack.push(m_instructionPtr + 1);
                            m_instructionPtr = m_bytecode[m_instructionPtr++];
                            break;

                        case Instruction::Call:
                            m_calls[m_bytecode[m_instructionPtr++]].second(this);
                            break;

                        default:
                            std::printf("Run error, unknown function type");
                            return;
                    }
                    break;

                case Instruction::Return:
                    if (!m_callStack.empty()) {
                        m_instructionPtr = m_callStack.top();
                        m_callStack.pop();
                        break;
                    }
                    return;

                default:
                    std::printf("Run error, unknown instruction");
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

    void Machine::newType(const std::string& name, RochaFunction onMake)
    {
        addFunction(name, onMake);
        m_constructors.emplace(name, m_calls.size() - 1);
    }

} // namespace Rocha