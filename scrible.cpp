#include "std_lib_facilities.h"


int main(){
    char ch = 'A';
    while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) cout << "\n" << ch; 
}