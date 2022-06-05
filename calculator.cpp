#include "std_lib_facilities.h"

// This exercise was actually incredibly helpful to demonstrate tokens and grammars
// I was very impressed with it.

const char quit = 'q';
const char print = ';';
const char number = '8';
const char prompt = '>';
const char result = '=';

class Token {
public:
	char kind;
	double value;
};

class TokenStream {               // Class declarations appear first, and only then comes the definitions
public:
	void putBack(Token t);
	Token get();

private:
	bool full{ false };
	Token tokenAvailable;
};

TokenStream ts;

void calculate();
double expression();     // Declarations allows tells the compiler to trust that this function is defined somewhere
double term();
double secondary();
double primary();


int main()
try {
	cout << "Welcome to our simple calculator."
		<< "\nPlease enter floating point numbers."
		<< "\nExpressions available: +, -, *, /, %, !"
		<< "\nPress " << print << " to return value and " << quit << " to quit."
		<< "\nHave fun!" << "\n\n";

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


void calculate(){

	while (cin) {
		cout << prompt;
		Token t = ts.get();

		 // Eats up the ; character, so the next input read by cin starts anew
		while (t.kind == print)	t = ts.get();    
		if (t.kind == quit)	return;            

		ts.putBack(t);
		cout << result << expression() << "\n";
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
	cin >> ch;   // Read one character
	switch (ch) {
	
	case print:
	case quit:
	case '+': case '-': 
	case '*': case '/': case '%': 
	case '!':
	case '(': case ')': case '{': case '}':
		return Token{ ch };
	
	case '0': case '1': case '2': case '3': case '4':      // Number readings
	case '5': case '6': case '7': case '8': case '9': 
	case '.':
		cin.putback(ch);
		double value;
		cin >> value;
		return Token{ number , value};

	default:
		error("Token not recognized.");
		return Token{'0', 0};    // Return some Token for completeness of function
	}

}

// Write Grammar

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
		if (ts.get().kind != ')') error("missing ')'");         // char ')' gets eaten here i.e. not returned to token_stream
		return d;
	}
	case '{':
	{                                  // Need curly brackets to initialize variable inside case
		double d = expression();
		if (ts.get().kind != '}') error("missing '}'");         // char ')' gets eaten here i.e. not returned to token_stream
		return d;
	}
	case number:
		return t.value;

	case '-':                  // Add the possibility for negative numbers
		return -primary();
	
	case '+':
		return primary();

	default:
		error("Primary expected.");
		return 1;
	}
}
