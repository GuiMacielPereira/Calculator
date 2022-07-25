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
	var
	var = Expression
	pow( Expression, Expression )
	sqrt Expression
Number:
	floating-point-literal
var:
	string
Let:
	#
Definition:
	var = Expression
	const var = Expression

Input comes from cin through the TokenStream called ts.
Variables are handled through the AvailableVariables vt.
*/


// Declarations allows tells the compiler to trust that this function is defined somewhere
// Passing TokenStram and Available variables by reference allows to modify them at each step
void calculate (TokenStream&, AvailableVariables&);
void inputFile (TokenStream&, AvailableVariables&);
void outputFile (TokenStream&, AvailableVariables&);
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
	TokenStream ts;        // ts for token stream
	AvailableVariables vt; // vt for variable table       

	// Set constant variables
	vt.setVar("pi", 3.1415926535, true);
	vt.setVar("e", 2.7182818284, true);
	vt.setVar("k", 1000, true);

	printWelcome();

	calculate(ts, vt);

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

void calculate(TokenStream& ts, AvailableVariables& vt){   // Passed by reference because we want functions modigying only one object

	while (ts.ist) 
	try {
		ts.ost << prompt;
		Token t = ts.get();

		// Eats up the ; character, so the next input read by cin starts anew
		while (t.kind==print) t = ts.get();    
		if (t.kind==quit || t.kind==eof) return;    // Take into acount end of input stream to quit as well       
		if (t.kind==help) {printHelp(); cout<<'\n'; continue;} // Use error to clean stream and skip to next iteration
		// Currently this output stream stays open indefinitelyy, need to think of a way to close it
		if (t.kind==to) {outputFile(ts, vt); cout<<'\n'; continue;}
		if (t.kind==from) {inputFile(ts, vt); cout<<'\n'; continue;}  // Creates inner loop to calculate from file

		ts.putBack(t);
		ts.ost << result << expression(ts, vt) << "\n";   
	}
	catch (exception& e){
		cerr << e.what() << '\n';
		ts.clean();
	}
}


double expression(TokenStream& ts, AvailableVariables& vt) {

	double buffer = term(ts, vt);
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			buffer += term(ts, vt);
			t = ts.get();            
			break;
		case '-':
			buffer -= term(ts, vt);
			t = ts.get();
			break;
		default:
			ts.putBack(t);
			return buffer;
		}
	}
}


double term(TokenStream& ts, AvailableVariables& vt) {

	double buffer = secondary(ts, vt);
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '*':
			buffer *= secondary(ts, vt);
			t = ts.get();
			break;
		case '/':
		{
			double d = secondary(ts, vt);
			if (d == 0) error("Cannot divide by zero!");
			buffer /= d;
			t = ts.get();
			break;
		}
		case '%':
		 {
			// Need to check for zero
			double d  = secondary(ts, vt);
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
double secondary(TokenStream& ts, AvailableVariables& vt) {  
	double buffer = primary(ts, vt);

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


double primary(TokenStream& ts, AvailableVariables& vt) {

	Token t = ts.get();

	switch (t.kind) {
	case '(':
	{                                  
		double d = expression(ts, vt); 
		Token t = ts.get();
		if (t.kind != ')'){       // Notice that char ')' gets eaten
			error("missing ')'");
		}          
		return d;
	}
	case '{':
	{                                  
		double d = expression(ts, vt);
		Token t = ts.get();
		if (t.kind != '}'){
			error("missing '}'");
		}          
		return d;
	}
	case number:
		return t.value;

	case '-':                  // Add the possibility for negative numbers
		return -primary(ts, vt);
	
	case '+':                 // This will mean that repeated +'s are skiped: eg. 1++++2; works
		return primary(ts, vt);

	case let:
		return definition(ts, vt);

	case var:
	{
		string name = t.name;    // Extract name of variable before modifying Token t
		double value = vt.getVar(name);    // In case var is detected, read double from stored variables

		// Assignment of existing variable
		Token t = ts.get();
		if (t.kind=='='){
			double d = expression(ts, vt);
			vt.replaceVar(name, d);
			return d;
		}
		ts.putBack(t);
		return value;    // The beauty of Tokens is that string input types can just as easily be handled by the switch statement
	}

	// Square root function, works with or without brakets
	case sq: 
	{  
		double d = expression(ts, vt);
		if (d<0) error("Square root of negative number not allowed.");
		return sqrt(d);
	}
	
	// Power function, requires brackets as in pow(double base, int i)
	case pwr:
	{   
		t = ts.get();
		if (t.kind != '(') error ("'(' expected for calling function pow(double n, int i).");
		double base = expression(ts, vt);
		t = ts.get();
		if (t.kind != ',') error ("Missing ',' when calling pow(double n, int i).");
		double exponent = expression(ts, vt);
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


double definition(TokenStream& ts, AvailableVariables& vt){
	// 'let' was already read 

	Token t = ts.get();

	bool isConst = false;
	if (t.name == "const") {
		isConst = true;
		t = ts.get();  // Get var following "const"
	}
	if (t.kind != var) error("Variable name expected.");
	string varName = t.name;     // Read name of variable

	t = ts.get();
	if (t.kind != '=') error("Equal sign '=' expected after variable '"+varName+"'.");

	double d = expression(ts, vt);
	vt.setVar(varName, d, isConst);     // Store variable 
	return d;     // return double to keep consistency with other functions
}

void inputFile (TokenStream& ts, AvailableVariables& vt)
{	// Token from was already read

	Token t = ts.get();

	if (t.kind != path) error ("Unable to find path specified, make sure to include '.' and '/' in path name");
	ifstream ifile {t.name};
	if (!ifile) error ("Error ocurred for opening of file.");

	// Change private input stream to file by initializing token stream 
	TokenStream tsf (ifile, ts.ost);
	calculate (tsf, vt);    // Start reading from file
	return;
}


void outputFile (TokenStream& ts, AvailableVariables& vt)
{
	// Token to was already read

	Token t = ts.get();

	if (t.kind != path) error ("Unable to find path specified, make sure to include '.' and '/' in path name");
	ofstream ofile {t.name};
	if (!ofile) error ("Error ocurred for opening of file.");

	// Initialize a new calculate loop with the new output stream
	TokenStream tsf (ts.ist, ofile);
	calculate (tsf, vt);
	return;

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

