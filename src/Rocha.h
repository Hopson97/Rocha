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

        std::vector<uint16_t> m_bytecode;
        std::unordered_map<std::string, size_t> m_jumps;
        std::vector<std::pair<std::string, RochaFunction>> m_calls;

        std::stack<std::any> m_stack;
        std::stack<size_t> m_callStack;

        size_t m_instructionPtr = 0;
    };
} // namespace Rocha
