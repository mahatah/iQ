/* 
 * File:   MM1.cpp
 * Author: Matthew Price
 *
 * Created on March 13, 2009, 4:12 PM
 */

#include <stdlib.h>

class node {
public:
node(element_type el);
element_type getData();
node<element_type> *getNext();
void setData(element_type el);
void setNext(node<element_type> *np);
private:
element_type data;
node<element_type> *next;
};

/*
 * 
 */
int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}

