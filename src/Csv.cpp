/*
 * Csv.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: romedal
 */
#include "../inc/Csv.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <tuple>
#include <algorithm>
#include <stdbool.h>

Csv::Csv():csv_fd{0}
{
#ifdef DEBUG
	cout<<"creating csv tool ..."<<endl;
#endif
}

Csv::~Csv()
{
#ifdef DEBUG
	cout<<"csv tool was destroyed"<<endl;
#endif
}

bool Csv::create_headers_csv(int fd, int sorted)
{
	bool ret = true;
	fstream fout;
	char name[25];

	do{
		if (sorted){
			if(sprintf(name, "connection%d_sorted.csv", fd) < 0){
				ret = false;
				break;
			}
		}
		else{
			if(sprintf(name, "connection%d.csv", fd) < 0){
				ret = false;
				break;
			}
		}

		ifstream f(name);
		if (!access(name, F_OK ))
		{
			break;
		}
		fout.open(name, ios::out | ios::trunc);

		fout <<"C1" << ","
				<<"C2" << ","
				<<"C3" << "\n";
		fout.close();
	}while(false);

	return ret;
}

bool Csv::sortbysec(const tuple<int, float, int>& a,
               const tuple<int, float, int>& b)
{
    return (get<1>(a) < get<1>(b));
}


bool Csv::csv_create(char** arr, int fd)
{
	bool ret = true;

	do{
		fstream fout;
		char name[25];

		if (sprintf(name, "connection%d.csv", fd) < 0){
			ret = false;
			break;
		}

		fout.open(name, ios::out | ios::app);
		fout <<arr[0] << ","
			 <<arr[1] << ","
			 <<arr[2] << ","
			 <<"\n";
		fout.close();

	}while(false);

	return ret;
}

bool Csv::csv_create(int c1, float c2, int c3, int fd)
{
	bool ret = true;

	do{
		fstream fout;
		char name[25];

		if (sprintf(name, "connection%d_sorted.csv", fd) < 0){
			ret = false;
			break;
		}

		fout.open(name, ios::out | ios::app);
		fout <<c1<< ","
			 <<c2<< ","
			 <<c3<< ","
			 << "\n";
		fout.close();

	}while(false);

	return ret;
}

void Csv::sort_csv(int fd)
{
	float C2=0;
	int rowNum = 1, C3 = 0, C1 = 0;;
	char name[25] = {0}, delim =',';
	std::string orbits="";
	fstream fin;
	vector<string> row, words;
	string line, word, temp;
	vector<tuple<int, float, int>> wideVector;

	sprintf(name, "connection%d.csv", fd);
	fin.open(name, ios::in);

	while (fin >> temp) {
		row.clear();
		getline(fin, line);
		stringstream s(line);

		while (std::getline(fin, line, delim)) {
			if(line != "")
				words.push_back(line);
		}
	}

	for(auto it = words.begin(); it != words.end(); it++) {
		orbits.assign(*it);
		orbits.erase(std::remove(orbits.begin(), orbits.end(), '\n'), orbits.end());
		if(orbits != "")
		{
			if (rowNum == 3){
				rowNum=1;
				C3=stoi(orbits);
				wideVector.push_back(std::make_tuple(C1, C2, C3));
			}
			else if (rowNum == 2){
				C2=stof(orbits);
				++rowNum;
			}
			else if (rowNum == 1){
				C1=stoi(orbits);
				++rowNum;
			}
		}
	}

	fin.close();

#ifdef DEBUG
	for ( const auto& i : wideVector ) {
		cout <<"Tuple:  "<< get<0>(i)<<"  "<< get<1>(i)<<"  "<< get<2>(i) << endl;
	}
#endif

	this->create_headers_csv(fd, 1);
	sort(wideVector.begin(), wideVector.end(), Csv::sortbysec);

	for ( const auto& i : wideVector ) {
		this->csv_create(get<0>(i), get<1>(i), get<2>(i), fd);

#ifdef DEBUG
		cout <<"Tuple:  "<< get<0>(i)<<"  "<< get<1>(i)<<"  "<< get<2>(i) << endl;
#endif

	}

	wideVector.clear();
}
