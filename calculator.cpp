#include "std_lib_facilities.h"

// This exercise was actually incredibly helpful to demonstrate tokens and grammars
// I was very impressed with it.


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
	
	case '+': case '-': case '*': case '/': 
	case '=': case '(': case ')': case ';':
	case '{': case '}': case '!':
		return Token{ ch };
	
	case '0': case '1': case '2': case '3': case '4':      // Number readings
	case '5': case '6': case '7': case '8': case '9': case '.':
		cin.putback(ch);
		double value;
		cin >> value;
		return Token{ '8' , value};

	default:
		error("Token not recognized.");
		return Token{'0', 0};    // Return some Token for completeness of function
	}

}


// Write Grammar

TokenStream ts;
double expression();     // Declarations allows tells the compiler to trust that this function is defined somewhere
double term();
double secondary();
double primary();


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
	case '8':
		return t.value;

	default:
		error("Primary expected.");
		return 1;
	}
}

// const char exit = ';';


int main()
try {
	cout << "Welcome to our simple calculator."
		<< "\nPlease enter floating point numbers."
		<< "\nExpressions available: +, -, *, /"
		<< "\nPress '=' to return value and ';' to quit."
		<< "\nHave fun!" << "\n\n";

	double val = 0;
	while (cin) {
		cout << "> ";
		Token t = ts.get();

		if (t.kind == ';') break;
		if (t.kind == '=') cout << "=" << val << '\n';
		else ts.putBack(t);
		
		val = expression();    // Will be called in the next iteration
	}
	keep_window_open("~~");
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
