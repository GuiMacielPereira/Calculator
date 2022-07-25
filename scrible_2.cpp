#include "../std_lib_facilities.h"

struct Integer
{
    int i {0};
};


int calc (Integer& Int) {
    while (Int.i < 10) {
        if (Int.i > 5) {
            // Int.i = 11;    // Changing variable inside loop works

            // However, setting a new variable inside the if scope 
            // does not alter the variable outside the scope
            // so it does not work:
            Integer Int;   
            Int.i = 11;
        }
        Int.i++;
        cout << Int.i;
    }
}



int main () {
    Integer myInt;
    calc (myInt);
}