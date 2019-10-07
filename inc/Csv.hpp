#ifndef CSV_H_
#define CSV_H_

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class Csv {
private:

	int csv_fd;
public:
	Csv();
	virtual ~Csv();
	void sort_csv(int fd);
	bool create_headers_csv(int, int);
	bool csv_create(char** arr, int fd);
	bool csv_create(int c1, float c2, int c3, int fd);
	static bool sortbysec(const tuple<int, float, int>& a,
			const tuple<int, float, int>& b);
};

#endif
