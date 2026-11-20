#include <iostream>
#include "arbreB.h"

int main(){
    ArbreB<int> index(5);
    for(int i = 1; i <= 1000; i++)
        index.inserer(i);
    std::cout << "contient 500 : " << index.contient(500) << std::endl;
    return 0;
}