#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "Parser.h"
#include "TNode.h"
#include "PSubsystem.h"

using namespace std;
using namespace par;

void Parser::Parse(std::string file_name) {
    psub::PSubsystem p_subsystem;
    p_subsystem.InitDataStructures();

    // check if file exists
    ifstream source_file(file_name);
    if (source_file.is_open()) {
        // read file and process statement
        string line;
        while (std::getline(source_file, line)) {
            p_subsystem.ProcessStatement(line);
        }
        source_file.close();
    }

    p_subsystem.GetDeliverables();
}

Deliverable Parser::GetDeliverables() {
    return this->deliverable;
}
