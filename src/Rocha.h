#pragma once

#include <any>
#include <stack>
#include <unordered_map>
#include <vector>

namespace Rocha {
    class Machine;
    using RochaFunction = void (*)(Rocha::Machine*);

    enum class Instruction : uint16_t {
        Push = 0,
        Pop = 1,
        Call = 2,
        Jump = 3,
        Return = 4,
    };

    class Machine {
      public:
        Machine();
        bool loadScript(const char* fileName);

        void pushNumber(float value);
        void pushString(const char* str);

        float getNumber();
        const char* getString();

        void addFunction(const std::string& name, RochaFunction f);

        void runFunction(const std::string& name);

      private:
        void run();

        // All function byte code implemented in Rocha files are stored in here
        std::vector<uint16_t> m_bytecode;

        // Maps labels to the byte code location of Rocha-defined functions
        std::unordered_map<std::string, size_t> m_jumps;

        // Maps strings to Cpp-defined functions 
        // Accessed is via index
        std::vector<std::pair<std::string, RochaFunction>> m_calls;

        // The Rocha stack
        std::stack<std::any> m_stack;

        // Call stack for "returning" to a previous function
        std::stack<size_t> m_callStack;

        // Pointer to the currently operated index in m_bytecode 
        size_t m_instructionPtr = 0;
    };
} // namespace Rocha
