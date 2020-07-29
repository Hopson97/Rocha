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
        Method = 4,
        Make = 5,
        Return = 6,
    };

    enum class Type { Object, Number };

    struct StackObject {
        Type type;
        union Data {
            float number;
            void* object;
        } data;
    };

    class Machine {
      public:
        Machine();
        bool loadScript(const char* fileName);

        void pushNumber(float value);
        void pushObject(void* object);

        float getNumber();
        void* getObject();

        void addFunction(const std::string& name, RochaFunction f);

        void runFunction(const std::string& name);

        void newType(const std::string& name, RochaFunction onMake,
                     const std::vector<std::pair<std::string, RochaFunction>>& methods);

        void* makeObject(size_t size);

        int findConstructorLocation(const std::string& name) const;
        int findObjectMethod(const std::string& type, const std::string& name) const;

      private:
        void run();

        // All function byte code implemented in Rocha files are stored in here
        std::vector<uint16_t> m_bytecode;

        // Maps labels to the byte code location of Rocha-defined functions
        std::unordered_map<std::string, size_t> m_jumps;

        // Maps strings to Cpp-defined functions
        // Accessed is via index
        std::vector<std::pair<std::string, RochaFunction>> m_calls;

        // Maps types to constructor calls, and types to functions
        std::unordered_map<std::string, size_t> m_constructors;
        std::unordered_map<std::string, std::unordered_map<std::string, size_t>>
            m_objectFunctions;

        std::vector<void*> m_objectAlloc;

        // The Rocha stack
        std::stack<StackObject> m_stack;

        // Call stack for "returning" to a previous function
        std::stack<size_t> m_callStack;

        // Pointer to the currently operated index in m_bytecode
        size_t m_instructionPtr = 0;
    };
} // namespace Rocha
