#pragma once

#include <iostream>
#include <string>
#include <regex>

void pretty_print(const char* str) {

	// delete all instances of struct and any whitespace in the string
	auto clean = std::regex_replace(str, std::regex(R"((struct )|( ))"), "");

    std::string indent_ = std::string("\n");
    size_t token = 0;
    std::string indent = "  ";
    while ((token = clean.find_first_of("<>,", token)) != std::string::npos) {
        switch (clean[token]) {
        case '<': indent_.append(indent);
        case ',': clean.insert(token + 1, indent_);
            break;
        case '>': indent_.erase(indent_.size() - indent.size());
            clean.insert(token, indent_);
        }
        token += indent_.size() + 1;
        const size_t nw = clean.find_first_not_of(" ", token);
        if (nw != std::string::npos) {
            clean.erase(token, nw - token);
        }
    }

    std::cout << clean << '\n';
}
