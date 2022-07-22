#include "std_lib_facilities.h"

const char help = 'h';
const char quit = 'q';
const char print = ';';
const char number = '8';
const char prompt = '>';
const char result = '=';
const char var = 'a';
const char let = '#';
const char k = 'k';
const string quitString = "exit";
const char sq = 's';
const string sqrtString = "sqrt";
const char pwr = 'p';
const string powString = "pow";
const char from = 'f';
const string fromString = "from";
const char path = '/';

class Token {
public:
	char kind;
	double value;
	string name;     // Allow for tokens to store strings as well

	// Generator options
	Token(char ch): kind{ch} {};   // Input only a character and leave the remaining attributes unitialized
	Token(char ch, double v): kind{ch}, value{v} {};
	Token(char ch, string n): kind{ch}, name{n} {};   
};

class TokenStream {    // Class declarations appear first, and only then comes the definitions
public:
	void putBack(Token t);
	Token get();
	void ignore(vector<char> endingchars);
	// void setIStream (istream& is) {ist = is;}   // How to set an input stream to another?? Maybe use constructor

	// Define input stream for Tokens
    TokenStream (istream& is): ist {is} {};   
	TokenStream (): ist {cin} {};
	TokenStream (Token t, istream& is): full {true}, tokenAvailable {t}, ist {is} {};
private:
	bool full{ false };
	Token tokenAvailable {var, "None"} ;    // Need to initialize Token using one of the geenrator definitions
    istream& ist;    // Have not tried this yet

};



// TokenStream Functions

void TokenStream::ignore(vector<char> endChars){

	// Deal with characters in TokenStream
	if (full){          
		full = false;          // Refresh Token stream
		for (char c : endChars) if (tokenAvailable.kind==c) return;	
	}
	
	// Deal with characters in cin stream
	char ch;
	while (ist.get(ch)){     // Flush remaining chars of cin stream until end of statement
		for (char c : endChars) if (ch == c) return;
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
	while ((ist.get(ch)) && (ch==' '));   // Skip all whitespace characters, '/n' included
	switch (ch) {
	
	case print:
	case let:
	case k:
	case '+': case '-': 
	case '*': case '/': case '%': 
	case '!':
	case '(': case ')': case '{': case '}':
	case '=': case ',':
		return Token{ ch };
	
	case '0': case '1': case '2': case '3': case '4':      // Number readings
	case '5': case '6': case '7': case '8': case '9': 
	case '.':
		ist.putback(ch);
		double value;
		ist >> value;
		return Token{ number , value};

	case '\n': 
		return Token{ print };

	case 'h': case 'H':
		return Token{ help };

	default:
		if (isalpha(ch)){			// If letter, start reading string
			string name;
			name += ch;

			// Accept names that start with letter, and include numbers or underscores
			// Allow to read paths with '/' and '.'
			bool isPath = false;
			while (ist.get(ch) && (isalpha(ch) || isdigit(ch) || ch=='_' || ch=='/' || ch=='.'))
			{
				name += ch;
				if (ch=='/' || ch=='.') isPath = true;     // Flag to indicate that we have read a path instead of variable
			}
			ist.putback(ch);  // Last character not part of variable name, put it back

			if (name==quitString) return Token{quit};
			if (name==sqrtString) return Token{sq};
			if (name==powString) return Token{pwr};
			if (name==fromString) return Token{from};
			if (isPath) return Token{path, name};
			return Token{var, name};
		}
		error("Token not recognized:", ch);
		return Token{var, "error"};    // Return some Token for completeness of function
	}
}