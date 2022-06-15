#include "std_lib_facilities.h"

class Variable{
    public:
    string name;
    double value;
};

int main(){
    vector<Variable> vars = {Variable{"n", 0}, Variable{"p", 0}};

    cout << "\n\nInitial value of variable " << vars[0].name << ": " << vars[0].value;

    // Loop with temporary variable that gets discarded at the end
    for (Variable v : vars) if (v.name == "n") v.value = 1;  // Variable stored in vector remains unchanged       
    cout << "\nLoop 1: Value of variable " << vars[0].name << ": " << vars[0].value;

    // Loop by reference, change is stored in variable inside vector
    for (Variable& v : vars) if (v.name == "n") v.value = 2;       
    cout << "\nLoop 2: Value of variable " << vars[0].name << ": " << vars[0].value;

    // Loop by idx, same effect as above, since directly accessing variable
    for (int i = 0; i < vars.size(); i++) if (vars[i].name == "n") vars[i].value = 3;
    cout << "\nLoop 3: Value of variable " << vars[0].name << ": " << vars[0].value;
    
    cout << "\n\n";
}