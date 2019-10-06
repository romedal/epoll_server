/*
 * csv.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: romedal
 */
#include "../inc/csv.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <tuple>

void create_headers_csv(int fd, int sorted)
{
	// file pointer
	fstream fout;
	char name[10];
	if (sorted)
		sprintf(name, "connection%d_sorted.csv", fd);
	else
		sprintf(name, "connection%d.csv", fd);
    ifstream f(name);
    if (!access(name, F_OK ))
    {
    	return;
    }
	// opens an existing csv file or creates a new file.
//	if(!fout.is_open())
//	{
//
//	}
	fout.open(name, ios::out | ios::trunc);

//	cout<<"Enter the details of 5 students:"<<" roll name maths phy chem bio"<< endl;

//	int i, roll, phy, chem, math, bio;
//	string name;

	fout <<"C1" << ","
			<< "C2" << ","
			<< "C3"<< "\r\n";
	fout.close();
}

bool sortbysec(const tuple<int, float, int>& a,
               const tuple<int, float, int>& b)
{
    return (get<1>(a) < get<1>(b));
}


