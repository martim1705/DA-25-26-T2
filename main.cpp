#include <iostream>
#include <string>
#include "CentralManager.h"

// Se houver -b, corre em modo batch; caso contrário abre o menu interativo.
int main(int argc, char* argv[]) {
    CentralManager central_manager;

    if (argc >= 3 && std::string(argv[1]) == "-b") {
        const std::string inputFile = argv[2];
        std::string riskFile = "";
        if (argc == 4) {
            riskFile = argv[3];
        }
        // O modo batch usa o ficheiro de input e, opcionalmente, um ficheiro para risco.
        central_manager.runBatchMode(inputFile, riskFile);
        std::cout << "Program finished.\n";
    }
    else {
        central_manager.runInteractiveMenu();
        std::cout << "Program finished.\n";
    }

    return 0;
}