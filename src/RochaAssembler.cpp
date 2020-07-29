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

    Assembler::Assembler(std::vector<uint16_t>& bytes,
                         std::unordered_map<std::string, size_t>& jumps,
                         std::vector<std::pair<std::string, RochaFunction>>& calls)
        : m_bytes(bytes)
        , m_jumps(jumps)
        , m_calls(calls)
    {
    }

    bool Assembler::assemble(const Machine& machine, const char* filename)
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
            const std::string& ins = tokens[0];
            if (ins == push) {
                addInstruction(ins);
                m_bytes.push_back(std::stoi(tokens[1]));
            }
            else if (ins == call) {
                bool found = false;
                if (tokens.size() == 2) {
                    for (uint16_t i = 0; i < m_calls.size(); i++) {
                        if (m_calls[i].first == tokens[1]) {
                            addInstruction(ins);
                            m_bytes.push_back(static_cast<uint16_t>(Instruction::Call));
                            m_bytes.push_back(i);
                            found = true;
                        }
                    }
                    if (!found) {
                        auto itr = m_jumps.find(tokens[1]);
                        if (itr != m_jumps.end()) {
                            addInstruction(ins);
                            m_bytes.push_back(static_cast<uint16_t>(Instruction::Jump));
                            m_bytes.push_back(itr->second);
                            found = true;
                        }
                    }
                }
                else {
                    // Find the variable
                    auto itr = m_objects.find(tokens[1]);
                    if (itr != m_objects.end()) {
                        auto type = m_objectTypes[itr->second];
                        auto method = machine.findObjectMethod(type, tokens[2]);
                        if (method > -1) {
                            addInstruction(ins);
                            addInstruction(Instruction::Method);
                            m_bytes.push_back(method);
                            found = true;
                        }
                        else {
                            std::printf("Type %s does not have function %s", type.c_str(),
                                        tokens[2].c_str());
                            return false;
                        }
                    }
                    else {
                        std::printf("Varname %s does not exist where %s",
                                    tokens[1].c_str(), line.c_str());
                        return false;
                    }
                }
                if (!found) {
                    std::printf("Unknown call to '%s' where '%s'", tokens[1].c_str(),
                                line.c_str());
                    return false;
                }
            }
            else if (ins == make) {
                auto constructor = machine.findConstructorLocation(tokens[1]);
                if (constructor > -1) {
                    addInstruction(ins);
                    m_bytes.push_back(constructor);
                    auto object = m_objects.find(tokens[2]);
                    if (object == m_objects.end()) {
                        m_objects.emplace(tokens[2], m_objects.size());
                        m_objectTypes.emplace(m_objects.size() - 1, tokens[1]);
                        std::cout << "Added make " << tokens[1] << " " << m_bytes.back()
                                  << std::endl;
                    }
                    else {
                        std::printf("Object with name %s already exists",
                                    tokens[2].c_str());
                        return false;
                    }
                }
                else {
                    std::printf("Unkown type %s", tokens[1].c_str());
                    return false;
                }
            }
            else if (ins.find(":") != std::string::npos) {
                m_jumps.emplace(ins.substr(0, ins.size() - 1), m_bytes.size());
            }
            else {
                addInstruction(ins);
            }
        }
        return true;
    }

    void Assembler::addPush(const Tokens& tokens)
    {
    }

    void Assembler::addCall(const Tokens& tokens)
    {
    }

    void Assembler::addJump(const Tokens& tokens)
    {
    }

    void Assembler::addMethod(const Tokens& tokens)
    {
    }

    void Assembler::addMake(const Tokens& tokens)
    {
    }

    void Assembler::addReturn(const Tokens& tokens)
    {
    }

    void Assembler::addInstruction(const std::string& instruction)
    {
        m_bytes.push_back(static_cast<uint16_t>(toInstruction.at(instruction)));
    }

    void Assembler::addInstruction(Instruction instruction)
    {
        m_bytes.push_back(static_cast<uint16_t>(instruction));
    }

    bool Machine::loadScript(const char* fileName)
    {
        Assembler asmler(m_bytecode, m_jumps, m_calls);
        return asmler.assemble(*this, fileName);
    }
} // namespace Rocha
