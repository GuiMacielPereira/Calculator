#include "std_lib_facilities.h"

class Variable{
	public:
	string name;
	double value;
	bool isConst = false;   // Initialize non constant variables by default
};

class AvailableVariables{
	private:
	vector<Variable> storedVars{};

	public:
	double getVar(string name);
	void setVar(string name, double value, bool isConst);
	bool checkVarExists(string n);
	void replaceVar(string name, double value);
};

// AvailableVariables function definitions 

double AvailableVariables::getVar(string n){
	for (Variable v : storedVars) if (v.name == n) return v.value;    
	error("Variable with name "+n+" not found.");
}

void AvailableVariables::setVar(string n, double v, bool isConst){    // Sets new variable if not already defined
	if (checkVarExists(n)) error("Variable is already defined. Usage: v = 5;");
	storedVars.push_back(Variable{n, v, isConst});
}

bool AvailableVariables::checkVarExists(string n){
	for (Variable v : storedVars) if (v.name == n) return true;  
	return false;
}

void AvailableVariables::replaceVar(string n, double v){     
	if (!checkVarExists(n)) error ("Tried to assign value to nonexistent variable");
	for (Variable& var : storedVars) {     // Take care to loop by reference, to store changes
		if (var.name == n) {
			if (!var.isConst) var.value = v;
			else error("Tried to assign value to constant variable!");
			return;
		}
	}
} 


