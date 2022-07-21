#include "std_lib_facilities.h"
#include "Token.h"
#include "Variable.h"

// This exercise was actually incredibly helpful to demonstrate tokens and grammars
// I did not anticipate a seemingly simple calculator to become so intricate 

/*
Simple calculator
Taken from book C++ Programing Principles and Practices

This program implements a basic expression calculator.
Input from cin; output to cout.
The grammar for input is:

Calculate:
	Print
	Quit
	Help
	Expression
Print:
	;
	\n
Quit:
	exit
Help:
	H
	h
Expression:
	Term
	Expression + Term
	Expression – Term
Term:
	Secondary	
	Term * Secondary 
	Term / Secondary 
	Term % Secondary 
Secondary:
	Primary
	Primary !
Primary:
	Number
	( Expression )
	{ Expression }
	– Primary
	+ Primary
	Let Definition 
	Word
	Word = Expression
	pow( Expression, Expression )
	sqrt Expression
Number:
	floating-point-literal
Word:
	string
Let:
	#
Definition:
	Word = Expression
	const Word = Expression

Input comes from cin through the TokenStream called ts.
Variables are handled through the AvailableVariables vars.
*/


// Declarations allows tells the compiler to trust that this function is defined somewhere
// Passing TokenStram and Available variables by reference allows to modify them at each step
void calculate (TokenStream&, AvailableVariables&);
double definition (TokenStream&, AvailableVariables&);
double expression (TokenStream&, AvailableVariables&);     
double term (TokenStream&, AvailableVariables&);
double secondary (TokenStream&, AvailableVariables&);
double primary (TokenStream&, AvailableVariables&);
void printWelcome();   
void printHelp();


int main()
try {
	// Create object to store and retrive user defined variables
	TokenStream ts;
	AvailableVariables vars;        

	// Set constant variables
	vars.setVar("pi", 3.1415926535, true);
	vars.setVar("e", 2.7182818284, true);
	vars.setVar("k", 1000, true);

	printWelcome();

	calculate(ts, vars);

	return 0;            // Return zero to show successful completion
}
// Outer handling of errors, first layer of error handling inside calculate()
catch (exception& e) {
	cerr << e.what() << '\n';
	keep_window_open("~~");     // Keeps window open unti user types "~~"
	return 1;
}
catch (...) {
	cerr << "exception \n";
	keep_window_open("~~");
	return 2;
}


// Grammar

void calculate(TokenStream& ts, AvailableVariables& vars){   // Passed by reference because we want functions modigying only one object

	while (cin) 
	try {
		cout << prompt;
		Token t = ts.get();

		// Eats up the ; character, so the next input read by cin starts anew
		while (t.kind == print)	t = ts.get();    
		if (t.kind == quit) return;             
		if (t.kind == help) {printHelp(); error("\n");} // Use error to clean stream and skip to next iteration

		ts.putBack(t);
		cout << result << expression(ts, vars) << "\n";   
	}
	catch (exception& e){
		cerr << e.what() << '\n';
		vector<char> endingChars = { print , '\n'};
		ts.ignore(endingChars);
	}
}


double expression(TokenStream& ts, AvailableVariables& vars) {

	double buffer = term(ts, vars);
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			buffer += term(ts, vars);
			t = ts.get();            
			break;
		case '-':
			buffer -= term(ts, vars);
			t = ts.get();
			break;
		default:
			ts.putBack(t);
			return buffer;
		}
	}
}


double term(TokenStream& ts, AvailableVariables& vars) {

	double buffer = secondary(ts, vars);
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '*':
			buffer *= secondary(ts, vars);
			t = ts.get();
			break;
		case '/':
		{
			double d = secondary(ts, vars);
			if (d == 0) error("Cannot divide by zero!");
			buffer /= d;
			t = ts.get();
			break;
		}
		case '%':
		 {
			// Need to check for zero
			double d  = secondary(ts, vars);
			if (d == 0) error("Cannot perform modulo by zero!");
			buffer = fmod(buffer, d);
			t = ts.get();
			break;
		 }
		default:
			ts.putBack(t);
			return buffer;
		}
	}
}


// Introduce new layer just for factorial (stronger binding than *, /, %)
double secondary(TokenStream& ts, AvailableVariables& vars) {  
	double buffer = primary(ts, vars);

	while (true) {

		Token t = ts.get();

		switch(t.kind){
		case '!': 
		{
			int x = narrow_cast<int>(buffer);
			int fact = 1;
			for (int i = 1; i <= x; i++) {
				fact *= i;      // Should see over flow over 12! but seeing it at 17!
				if (fact < 0) error("Overflow of factorial!");   // Using double means this doesn't catch alll wrong factorials
			}
			buffer = fact;
			break;
		}
		case k:
			buffer *= 1000;
			break;

		default:	
			ts.putBack(t);
			return buffer;
		}
	}
}


double primary(TokenStream& ts, AvailableVariables& vars) {

	Token t = ts.get();

	switch (t.kind) {
	case '(':
	{                                  
		double d = expression(ts, vars); 
		Token t = ts.get();
		if (t.kind != ')'){       // Notice that char ')' gets eaten
			error("missing ')'");
		}          
		return d;
	}
	case '{':
	{                                  
		double d = expression(ts, vars);
		Token t = ts.get();
		if (t.kind != '}'){
			error("missing '}'");
		}          
		return d;
	}
	case number:
		return t.value;

	case '-':                  // Add the possibility for negative numbers
		return -primary(ts, vars);
	
	case '+':                 // This will mean that repeated +'s are skiped: eg. 1++++2; works
		return primary(ts, vars);

	case let:
		return definition(ts, vars);

	case word:
	{
		string name = t.name;    // Extract name of variable before modifying Token t
		double value = vars.getVar(name);    // In case word is detected, read double from stored variables

		// Assignment of existing variable
		Token t = ts.get();
		if (t.kind=='='){
			double d = expression(ts, vars);
			vars.replaceVar(name, d);
			return d;
		}
		ts.putBack(t);
		return value;    // The beauty of Tokens is that string input types can just as easily be handled by the switch statement
	}

	// Square root function, works with or without brakets
	case sq: 
	{  
		double d = expression(ts, vars);
		if (d<0) error("Square root of negative number not allowed.");
		return sqrt(d);
	}
	
	// Power function, requires brackets as in pow(double base, int i)
	case pwr:
	{   
		t = ts.get();
		if (t.kind != '(') error ("'(' expected for calling function pow(double n, int i).");
		double base = expression(ts, vars);
		t = ts.get();
		if (t.kind != ',') error ("Missing ',' when calling pow(double n, int i).");
		double exponent = expression(ts, vars);
		int i = narrow_cast<int>(exponent);
		t = ts.get();
		if (t.kind != ')') error ("Missing ')' when calling pow(double n, int i).");
		return pow(base, i);
	}

	default:
	 	ts.putBack(t);
		error("Primary expected.");
		break;
	}
}


double definition(TokenStream& ts, AvailableVariables& vars){
	// 'let' was already read 

	Token t = ts.get();

	bool isConst = false;
	if (t.name == "const") {
		isConst = true;
		t = ts.get();  // Get word following "const"
	}
	if (t.kind != word) error("Variable name expected.");
	string varName = t.name;     // Read name of variable

	t = ts.get();
	if (t.kind != '=') error("Equal sign '=' expected after variable '"+varName+"'.");

	double d = expression(ts, vars);
	vars.setVar(varName, d, isConst);     // Store variable 
	return d;     // return double to keep consistency with other functions
}


// Printing functions

void printWelcome(){
    cout << "Welcome to our simple calculator."
        << "\nPlease enter floating point numbers."
        << "\nExpressions available: +, -, *, /, %, !"
        << "\nPress " << print << " to return value and " << quit << " to quit."
        << "\nHave fun!" << "\n\n";
    return;
}

void printHelp(){
    cout << "You have reached the Help page."
         << "\nUnfortunately this section is under development."
        << "\nBut as a general guidance for the calculator:"
        << "\n\nType numbers and +, -, *, /, %, ! for usual calculations."
		<< "\nIncludes pow() and sqrt()."
        << "\nPress ';' or enter to output result."
        << "\nType exit to exit."
        << "\nThis calculator accepts defiining variables!"
        << "\nUse '# var = 1' to define variable"
        << "\nCan assign different value to defined variable ie 'var = 2'";
}

