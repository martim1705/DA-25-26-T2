#include <iostream>
#include <string>
#include "CentralManager.h"
#include  "Parser.h"
#include "MaxFlow.h"
#include "Graph.h"
using namespace std; 

int main(int argc, char* argv[]) { // VOLTAR PARA COLOCAR ERROS DE USO. POR EXEMPLO SE O UTILIZADOR ESCREVER ./myprog -b DEVERIA DAR MENSAGEM DE ERRO EM BATCH MODE?

    CentralManager central_manager;
    if (argc >= 3 && string(argv[1]) == "-b") { // run batch mode
        const string inputFile = argv[2];
        string riskFile = ""; // without riskFile
        if (argc == 4) {
            riskFile = argv[3]; // with riskFile
        }
        central_manager.runBatchMode(inputFile, riskFile);
        cout << "Program finished.\n";
    }
    else {
        central_manager.runInteractiveMenu();
        cout << "Program finished.\n";
    }
    return 0; 
}