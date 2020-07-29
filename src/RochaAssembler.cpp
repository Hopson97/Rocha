#include "RochaAssember.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string_view>
#include <vector>

void ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
}
void rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
}
void trim(std::string& s, const char* t = " \t\n\r\f\v")
{
    ltrim(s, t);
    rtrim(s, t);
}

const std::string push = "push";
const std::string pop = "pop";
const std::string call = "call";
const std::string ret = "ret";
const std::string make = "make";

const std::unordered_map<std::string, Rocha::Instruction> toInstruction = {
    {push, Rocha::Instruction::Push},  {pop, Rocha::Instruction::Push},
    {call, Rocha::Instruction::Call},  {make, Rocha::Instruction::Make},
    {ret, Rocha::Instruction::Return},
};

namespace Rocha {

    Assembler::Assembler(Machine& machine, std::vector<uint16_t>& bytes,
                         std::unordered_map<std::string, size_t>& jumps,
                         std::vector<std::pair<std::string, RochaFunction>>& calls)
        : m_bytes(bytes)
        , m_jumps(jumps)
        , m_calls(calls)
        , m_machine(machine)
    {
    }

    bool Assembler::assemble(const char* filename)
    {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(infile, line)) {
            trim(line);
            std::istringstream iss(line);
            std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                            std::istream_iterator<std::string>());
            if (tokens[0] == push) {
                addPush(tokens);
            }
            else if (tokens[0] == call) {
                bool found = false;
                if (tokens.size() == 2) {
                    if (!addJumpOrCall(tokens)) {
                        return false;
                    }
                }
                else {
                    if (!addMethodCall(tokens)) {
                        return false;
                    }
                }
            }
            else if (tokens[0] == make) {
                if (!addMake(tokens)) {
                    return false;
                }
            }
            else if (tokens[0].find(":") != std::string::npos) {
                m_jumps.emplace(tokens[0].substr(0, tokens[0].size() - 1),
                                m_bytes.size());
            }
            else {
                addInstruction(tokens[0]);
            }
        }
        return true;
    }

    void Assembler::addPush(const Tokens& tokens)
    {
        addInstruction(tokens[0]);
        m_bytes.push_back(std::stoi(tokens[1]));
    }

    bool Assembler::addJumpOrCall(const Tokens& tokens)
    {
        // Try add a function call
        // Produces "call call function_location"
        for (uint16_t i = 0; i < m_calls.size(); i++) {
            if (m_calls[i].first == tokens[1]) {
                addInstruction(tokens[0]);
                m_bytes.push_back(static_cast<uint16_t>(Instruction::Call));
                m_bytes.push_back(i);
                return true;
            }
        }

        // Try add a "jump" call to a script-defined label
        // Produces "call jump jump_location"
        auto itr = m_jumps.find(tokens[1]);
        if (itr != m_jumps.end()) {
            addInstruction(tokens[0]);
            m_bytes.push_back(static_cast<uint16_t>(Instruction::Jump));
            m_bytes.push_back(itr->second);
            return true;
        }
        return false;
    }

    // Produces "call method object_id function_location"
    bool Assembler::addMethodCall(const Tokens& tokens)
    {
        // Find the variable
        auto object = m_objects.find(tokens[1]);
        if (object != m_objects.end()) {
            auto type = m_objectTypes[object->second];
            auto methodLocation = m_machine.findObjectMethod(type, tokens[2]);
            if (methodLocation > -1) {
                addInstruction(tokens[0]);
                addInstruction(Instruction::Method);
                m_bytes.push_back(object->second);
                m_bytes.push_back(methodLocation);
                return true;
            }
            else {
                std::printf("Type %s does not have function %s", type.c_str(),
                            tokens[2].c_str());
                return false;
            }
        }
        std::printf("Varname %s does not exist.", tokens[1].c_str());
        return false;
    }

    // Produces "make type_id"
    bool Assembler::addMake(const Tokens& tokens)
    {
        auto constructor = m_machine.findConstructorLocation(tokens[1]);
        if (constructor > -1) {
            addInstruction(tokens[0]);
            m_bytes.push_back(constructor);
            auto object = m_objects.find(tokens[2]);
            if (object == m_objects.end()) {
                // The bytecode of make does not need to know the object id being made
                m_objects.emplace(tokens[2], m_objects.size());
                m_objectTypes.emplace(m_objects.size() - 1, tokens[1]);
                std::cout << "Added make " << tokens[1] << " " << m_bytes.back()
                          << std::endl;
                return true;
            }
            else {
                std::printf("Object with name %s already exists", tokens[2].c_str());
                return false;
            }
        }
        std::printf("Unkown type %s", tokens[1].c_str());
        return false;
    }

    void Assembler::addInstruction(const std::string& instruction)
    {
        m_bytes.push_back(static_cast<uint16_t>(toInstruction.at(instruction)));
    }

    void Assembler::addInstruction(Instruction instruction)
    {
        m_bytes.push_back(static_cast<uint16_t>(instruction));
    }
} // namespace Rocha
