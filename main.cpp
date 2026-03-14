#include <iostream>
using namespace std; 

int main() {
     
    int op; 
    bool running = true;
    while (running) {
        
        cout << "------------------------------------\n";

        cout << "\t1 - Load input file\t\n";
        cout << "\t2 - Show submissions\t\n";
        cout << "\t3 - Show reviewers\t\n";
        cout << "\t4 - Load input file\t\n";
        cout << "\t5 - Show submissions\t\n";
        cout << "\t6 - Show reviewers\t\n";
        cout << "\t7 - Load input file\t\n";
        cout << "\t8 - Show submissions\t\n";
        cout << "\t0 - Exit\t";
        
        cout << "\n------------------------------------\n";
        cout << "Operation: ";
        cin >> op;

        switch(op) {

            case 0:
                running = false;
                break;
            
            case 1:
                break;
            
            case 2:
                break;
            
            case 3:
                break;
            
            case 4:
                break;
            
            case 5:
                break;
            
            case 6:
                break;
            
            case 7:
                break;
            
            case 8:
                break;

            default:

                cout << "Operation unavailable\n";
        }
    }

    return 0; 
}