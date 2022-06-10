#include "std_lib_facilities.h"

// This exercise was actually incredibly helpful to demonstrate tokens and grammars
// I was very impressed with it.

const char quit = 'q';
const char print = ';';
const char number = '8';
const char prompt = '>';
const char result = '=';
const char word = 'a';
const char let = 'L';
const string defString = "let";

class Token {
public:
	char kind;
	double value;
	string name;     // Allow for tokens to store strings as well

	// Generator options
	Token(char ch): kind{ch} {};   // Input only a character and leave the remaining attributes unitialized
	Token(char ch, double v): kind{ch}, value{v} {};
	Token(char ch, string n): kind{ch}, name{n} {};   // Curly braces mean 'assign this value to variable'
};

class TokenStream {               // Class declarations appear first, and only then comes the definitions
public:
	void putBack(Token t);
	Token get();
	void ignore(char c);

private:
	bool full{ false };
	Token tokenAvailable {word, "None"} ;    // Need to initialize Token using one of the geenrator definitions

};

class Variable{
	public:
	string name;
	double value;
};

class AvailableVariables{
	private:
	vector<Variable> storedVars{};
	bool checkVarExists(string n);

	public:
	double getVar(string name);
	void setVar(string name, double value);
};

TokenStream ts;
AvailableVariables vars;        // Create object to store and retrive user defined variables

void calculate();
double statement();
double definition();
double expression();     // Declarations allows tells the compiler to trust that this function is defined somewhere
double term();
double secondary();
double primary();

void printWelcome();   


int main()
try {
	vars.setVar("pi", 3.1415926535);
	vars.setVar("e", 2.7182818284);

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

void printWelcome(){
	cout << "Welcome to our simple calculator."
		<< "\nPlease enter floating point numbers."
		<< "\nExpressions available: +, -, *, /, %, !"
		<< "\nPress " << print << " to return value and " << quit << " to quit."
		<< "\nHave fun!" << "\n\n";
	return;
}


void calculate(){

	while (cin) 
	try {
		cout << prompt;
		Token t = ts.get();

		// Eats up the ; character, so the next input read by cin starts anew
		while (t.kind == print)	t = ts.get();    
		if (t.kind == quit)	return;            

		ts.putBack(t);
		cout << result << statement() << "\n";     // TODO: don't forget to replace by statement()
	}
	catch (exception& e){
		cerr << e.what() << '\n';
		ts.ignore(print);
	}
}


void TokenStream::ignore(char c){

	// Deal with characters in TokenStream
	if (full){          
		full = false;          // Refresh Token stream
		if (tokenAvailable.kind==c) return;	
	}
	
	// Deal with characters in cin stream
	char ch;
	while (cin >> ch){     // Flush remaining chars of cin stream until end of statement
		if (ch == c) break;
	}
	return;
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
	cin >> ch;   // Read one character
	switch (ch) {
	
	case print:
	case quit:
	case '+': case '-': 
	case '*': case '/': case '%': 
	case '!':
	case '(': case ')': case '{': case '}':
	case '=':
		return Token{ ch };
	
	case '0': case '1': case '2': case '3': case '4':      // Number readings
	case '5': case '6': case '7': case '8': case '9': 
	case '.':
		cin.putback(ch);
		double value;
		cin >> value;
		return Token{ number , value};

	default:
		if (isalpha(ch)){			// If letter, start reading string
		 	cout << "\nLetter recognized.";
			string name;
			name += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) {
				cout << "\n" << ch;
				name += ch;  // Keep reading if character is letter or digit
				cout << "\n" << name;
			}
			cout << "\nName of variable: " << name << "\n";
			cin.putback(ch);    				 // Character is not a digit or a letter, put it back
			if (name==defString) return Token{let};
			return Token{word, name};
		}
		error("Token not recognized.");
		return Token{'0', 0};    // Return some Token for completeness of function
	}
}


// Store available variables in vector

double AvailableVariables::getVar(string n){
	for (Variable& v : storedVars) if (v.name == n) return v.value;     // Not sure as to the reason for &
	error("get() call did not find variable with name="+n);
}

void AvailableVariables::setVar(string n, double v){
	if (checkVarExists(n)) error("Overwriting variables not supported.");
	storedVars.push_back(Variable{n, v});
}

bool AvailableVariables::checkVarExists(string n){
	for (Variable& v : storedVars) if (v.name == n) return true;
	return false;
}


// Write Grammar

double statement() {
	Token t = ts.get();
	if (t.kind==let) return definition();
	ts.putBack(t);       // If not a definition, put number back into stream
	return expression();
	
}

double definition(){

	// let was already read 
	// Read word

	Token t = ts.get();
	if (t.kind != word) error("variable name expected.");
	string varName = t.name;     // Read name of variable

	t = ts.get();
	if (t.kind != '=') error("'=' expected.");

	double d = expression();
	vars.setVar(varName, d);     // Store variable 
	return d;               // Good idea to return double, to keep consistency with other functions in grammar
}



// Second Grammar

double expression() {

	double buffer = term();
	Token t = ts.get();

	while (true) {
		switch (t.kind) {
		case '+':
			buffer += term();
			t = ts.get();            // Initially forgot to update the loop
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
	{                                  // Need curly brackets to initialize variable inside case
		double d = expression(); 
		Token t = ts.get();
		if (t.kind != ')'){
			ts.putBack(t);    		// Putting back token to cover the case for char print
			error("missing ')'");
		}          
		// Notice that char ')' gets eaten
		return d;
	}
	case '{':
	{                                  // Need curly brackets to initialize variable inside case
		double d = expression();
		Token t = ts.get();
		if (t.kind != '}'){
			ts.putBack(t);    		// Putting back token to cover the case for char print
			error("missing ')'");
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
		return vars.getVar(t.name);    // The beauty of Tokens is that string input types can just as easily be handled by the switch statement

	default:
	 	ts.putBack(t);
		error("Primary expected.");
		break;
	}
}
