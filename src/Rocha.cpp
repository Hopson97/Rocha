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
        m_stack.push(value);
    }

    void Machine::pushString(const char* str)
    {
        m_stack.push(str);
    }

    float Machine::getNumber()
    {
        auto value = std::any_cast<float>(m_stack.top());
        m_stack.pop();
        return value;
    }

    const char* Machine::getString()
    {
        auto value = std::any_cast<const char*>(m_stack.top());
        m_stack.pop();
        return value;
    }

    void Machine::runFunction(const std::string& name)
    {
        m_instructionPtr = m_jumps.find(name)->second;
        while (m_bytecode[m_instructionPtr] != (uint16_t)Instruction::Return) {
            switch ((Instruction)m_bytecode[m_instructionPtr++]) {
                case Instruction::Push:
                    m_stack.emplace((float)m_bytecode[m_instructionPtr++]);
                    break;
                case Instruction::Pop:
                    m_stack.pop();
                    break;
                case Instruction::Call:
                    switch ((Instruction)m_bytecode[m_instructionPtr++]) {
                        case Instruction::Jump:
                            // yeet
                            break;

                        case Instruction::Call:
                            m_calls[m_bytecode[m_instructionPtr++]].second(this);
                            break;

                        default:
                            std::printf("Run error, unknown function type");
                            return;
                    }
                    break;

                default:
                    std::printf("Run error, unknown instruction");
                    return;
            }
        }
    }

    void Machine::addFunction(const std::string& name, RochaFunction f)
    {
        m_calls.push_back(std::make_pair(name, f));
    }
} // namespace Rocha