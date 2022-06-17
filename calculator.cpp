#include "std_lib_facilities.h"

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
	Statement
Print:
	;
Quit:
	exit
Help:
	H
	h
Statement:
	Expression
	# Definition
Definition:
	Word = Expression
	const Word = Expression
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
	pow( Expression, Expression )
	sqrt Expression
	Word
	Word = Expression
Number:
	floating-point-literal
Word:
	string

Input comes from cin through the TokenStream called ts.
Variables are handled through the AvailableVariables vars.
*/


const char help = 'h';
const char quit = 'q';
const char print = ';';
const char number = '8';
const char prompt = '>';
const char result = '=';
const char word = 'a';
const char let = '#';
const string quitString = "exit";
const char sq = 'sqrt';
const string sqrtString = "sqrt";
const char pwr = 'pow';
const string powString = "pow";


class Token {
public:
	char kind;
	double value;
	string name;     // Allow for tokens to store strings as well

	// Generator options
	Token(char ch): kind{ch} {};   // Input only a character and leave the remaining attributes unitialized
	Token(char ch, double v): kind{ch}, value{v} {};
	Token(char ch, string n): kind{ch}, name{n} {};   // Curly braces mean 'assign this value to variable'
	// Finish constructor with {} to signalize the end of the definition
};

class TokenStream {               // Class declarations appear first, and only then comes the definitions
public:
	void putBack(Token t);
	Token get();
	void ignore(vector<char> endingchars);

private:
	bool full{ false };
	Token tokenAvailable {word, "None"} ;    // Need to initialize Token using one of the geenrator definitions
};

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

// Create object to store and retrive user defined variables
TokenStream ts;
AvailableVariables vars;        

// Declarations allows tells the compiler to trust that this function is defined somewhere
void calculate();
double statement();
double definition();
double expression();     
double term();
double secondary();
double primary();

void printWelcome();   
void printHelp();


int main()
try {
	// Set constant variables
	vars.setVar("pi", 3.1415926535, true);
	vars.setVar("e", 2.7182818284, true);
	vars.setVar("k", 1000, true);

	printWelcome();

	calculate();

	keep_window_open("~~");
	return 0;            // Return zero to show successful completion
}
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

void calculate(){

	while (cin) 
	try {
		cout << prompt;
		Token t = ts.get();

		// Eats up the ; character, so the next input read by cin starts anew
		while (t.kind == print)	t = ts.get();    
		if (t.kind == quit) return;             
		if (t.kind == help) {printHelp(); error("\n");} // Use error to clean stream and skip to next iteration

		ts.putBack(t);
		cout << result << statement() << "\n";   
	}
	catch (exception& e){
		cerr << e.what() << '\n';
		vector<char> endingChars = { print , '\n'};
		ts.ignore(endingChars);
	}
}


double statement() {
	Token t = ts.get();
	
	if (t.kind==let) return definition();
	
	ts.putBack(t);    // If not a definition, put number back into stream
	return expression();
	
}

double definition(){
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

	double d = expression();
	vars.setVar(varName, d, isConst);     // Store variable 
	return d;     // return double to keep consistency with other functions
}


double expression() {

	double buffer = term();
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			buffer += term();
			t = ts.get();            
			break;
		case '-':
			buffer -= term();
			t = ts.get();
			break;
		default:
			ts.putBack(t);
			return buffer;
		}
	}
}


double term() {

	double buffer = secondary();
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '*':
			buffer *= secondary();
			t = ts.get();
			break;
		case '/':
		{
			double d = secondary();
			if (d == 0) error("Cannot divide by zero!");
			buffer /= d;
			t = ts.get();
			break;
		}
		case '%':
		 {
			// Need to check for zero
			double d  = secondary();
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
double secondary() {  
	double buffer = primary();
	Token t = ts.get();

	if (t.kind == '!') {
		int x = int(buffer);
		int fact = 1;
		for (int i = 1; i <= x; i++) fact *= i;
		return fact;
	}
	else
		ts.putBack(t);
	return buffer;
}


double primary() {

	Token t = ts.get();

	switch (t.kind) {
	case '(':
	{                                  
		double d = expression(); 
		Token t = ts.get();
		if (t.kind != ')'){       // Notice that char ')' gets eaten
			error("missing ')'");
		}          
		return d;
	}
	case '{':
	{                                  
		double d = expression();
		Token t = ts.get();
		if (t.kind != '}'){
			error("missing '}'");
		}          
		return d;
	}
	case number:
		return t.value;

	case '-':                  // Add the possibility for negative numbers
		return -primary();
	
	case '+':                 // This will mean that repeated +'s are skiped: eg. 1++++2; works
		return primary();

	case word:
	{
		string name = t.name;    // Extract name of variable before modifying Token t
		double value = vars.getVar(name);    // In case word is detected, read double from stored variables

		// Assignment of existing variable
		Token t = ts.get();
		if (t.kind=='='){
			double d = expression();
			vars.replaceVar(name, d);
			return d;
		}
		ts.putBack(t);
		return value;    // The beauty of Tokens is that string input types can just as easily be handled by the switch statement
	}

	case let:   // Added this option for inline definition, experimental
		return definition();

	case sq: 
	{  
		 // Square root function, works with or without brakets
		double d = expression();
		if (d<0) error("Square root of negative number not allowed.");
		return sqrt(d);
	}

	case pwr:
	{   
		// Power function, requires brackets as in pow(double base, int i)
		t = ts.get();
		if (t.kind != '(') error ("'(' expected for calling function pow(double n, int i).");
		double base = expression();
		t = ts.get();
		if (t.kind != ',') error ("Missing ',' when calling pow(double n, int i).");
		double exponent = expression();
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


// TokenStream Functions

void TokenStream::ignore(vector<char> endingChars){

	// Deal with characters in TokenStream
	if (full){          
		full = false;          // Refresh Token stream
		for (char c : endingChars) if (tokenAvailable.kind==c) return;	
	}
	
	// Deal with characters in cin stream
	char ch;
	while (cin.get(ch)){     // Flush remaining chars of cin stream until end of statement
		for (char c : endingChars) if (ch == c) return;
	}
}


void TokenStream::putBack(Token t) {
	full = true;
	tokenAvailable = t;
}

Token TokenStream::get() {

	if (full) {             // If token already available, do not read from cin
		full = false;
		return tokenAvailable;
	}

	char ch;
	while ((cin.get(ch)) && (ch==' '));   // Skip all whitespace characters, '/n' included
	switch (ch) {
	
	case print:
	case let:
	case '+': case '-': 
	case '*': case '/': case '%': 
	case '!':
	case '(': case ')': case '{': case '}':
	case '=': case ',':
		return Token{ ch };
	
	case '0': case '1': case '2': case '3': case '4':      // Number readings
	case '5': case '6': case '7': case '8': case '9': 
	case '.':
		cin.putback(ch);
		double value;
		cin >> value;
		return Token{ number , value};

	case '\n': 
		return Token{ print };

	case 'h': case 'H':
		return Token{ help };

	default:
		if (isalpha(ch)){			// If letter, start reading string
			string name;
			name += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch=='_')) name += ch;
			cin.putback(ch);    				 // Character not part of variable name, put it back
			if (name==quitString) return Token{quit};
			if (name==sqrtString) return Token{sq};
			if (name==powString) return Token{pwr};
			return Token{word, name};
		}
		error("Token not recognized.");
		return Token{word, "error"};    // Return some Token for completeness of function
	}
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
        << "\nBut aa a general guidance for the calculator:"
        << "\n\nType numbers and +, -, *, /, % for usual calculations."
        << "\nPress ';' or enter to output result."
        << "\nIf you get stuck, press ';' to clean cin stream"
        << "\nType exit to exit."
        << "\nThis calculator accepts defiining veriables!"
        << "\nUse '# var = 1' to define variable"
        << "\nCan assign different value to defined variable ie 'var = 2'";
}

