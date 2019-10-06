#ifndef CSV_H_
#define CSV_H_

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

void create_headers_csv(int, int);
bool sortbysec(const tuple<int, float, int>& a,
               const tuple<int, float, int>& b);

#endif
