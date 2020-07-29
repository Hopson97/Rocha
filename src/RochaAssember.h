#pragma once

#include "Rocha.h"

namespace Rocha {
    using Tokens = std::vector<std::string>;
    class Assembler {
      public:
        Assembler(Machine& machine, std::vector<uint16_t>& bytes,
                  std::unordered_map<std::string, size_t>& jumps,
                  std::vector<std::pair<std::string, RochaFunction>>& calls);
        bool assemble(const char* filename);

      private:
        void addPush(const Tokens& tokens);
        bool addJumpOrCall(const Tokens& tokens);
        bool addMethodCall(const Tokens& tokens);
        bool addMake(const Tokens& tokens);

        void addInstruction(const std::string& instruction);
        void addInstruction(Instruction instruction);

        // Maps variables to an ID
        std::unordered_map<std::string, uint16_t> m_objects;

        // Maps variables to a type names
        std::unordered_map<uint16_t, std::string> m_objectTypes;

        std::vector<uint16_t>& m_bytes;
        std::unordered_map<std::string, size_t>& m_jumps;
        std::vector<std::pair<std::string, RochaFunction>>& m_calls;

        const Machine& m_machine;
    };
} // namespace Rocha
