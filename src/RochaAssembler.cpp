#include "Rocha.h"
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

    bool Machine::loadScript(const char* fileName)
    {
        std::ifstream infile(fileName);
        if (!infile.is_open()) {
            return false;
        }

        auto addInstruction = [&](const std::string& instruction) {
            m_bytecode.push_back(static_cast<uint16_t>(toInstruction.at(instruction)));
        };

        std::string line;
        while (std::getline(infile, line)) {
            trim(line);
            std::istringstream iss(line);
            std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                            std::istream_iterator<std::string>());

            const std::string& ins = tokens[0];
            if (ins == push) {
                addInstruction(ins);
                m_bytecode.push_back(std::stoi(tokens[1]));
            }
            else if (ins == call) {
                bool found = false;
                if (tokens.size() == 2) {
                    for (uint16_t i = 0; i < m_calls.size(); i++) {
                        if (m_calls[i].first == tokens[1]) {
                            addInstruction(ins);
                            m_bytecode.push_back(
                                static_cast<uint16_t>(Instruction::Call));
                            m_bytecode.push_back(i);
                            found = true;
                        }
                    }
                    if (!found) {
                        auto itr = m_jumps.find(tokens[1]);
                        if (itr != m_jumps.end()) {
                            addInstruction(ins);
                            m_bytecode.push_back(
                                static_cast<uint16_t>(Instruction::Jump));
                            m_bytecode.push_back(itr->second);
                            found = true;
                        }
                    }
                }
                else {
                    // Method call lol
                    auto& var = tokens[1];
                    auto& func = tokens[1];

                    // Find the variable
                    auto itr = m_objects.find(tokens[1]);
                    if (itr != m_objects.end()) {
                        auto type = m_objectTypes[itr->second];

                        // Find the function
                        auto func = m_objectFunctions[type].find(tokens[2]);
                        if (func != m_objectFunctions[type].end()) {
                            // Add the method call
                            addInstruction(ins);
                            m_bytecode.push_back(
                                static_cast<uint16_t>(Instruction::Method));
                            m_bytecode.push_back(itr->second);
                            m_bytecode.push_back(func->second);
                            found = true;
                        }
                        else {
                            std::printf("Type %s does not have function %s", type.c_str(),
                                        tokens[2].c_str());
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
                auto itr = m_constructors.find(tokens[1]);
                if (itr != m_constructors.end()) {
                    addInstruction(ins);
                    m_bytecode.push_back(itr->second);
                    auto object = m_objects.find(tokens[2]);
                    if (object == m_objects.end()) {
                        m_objects.emplace(tokens[2], m_objects.size());
                        m_objectTypes.emplace(m_objects.size() - 1, tokens[1]);
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
                m_jumps.emplace(ins.substr(0, ins.size() - 1), m_bytecode.size());
            }
            else {
                addInstruction(ins);
            }
        }

        return true;
    }
} // namespace Rocha
