/*
 * csv.cpp
 *
 *  Created on: Oct 6, 2019
 *      Author: romedal
 */
#include "../inc/Csv.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <tuple>

Csv::Csv()
{

}

Csv::~Csv()
{
//	cout<<"server destroying ..."<<endl;
//	if(close(epoll_fd))
//	{
//		fprintf(stderr, "Failed to close epoll file descriptor\n");
//	}
//	cout<<"server was destroyed"<<endl;
}
void Csv::create_headers_csv(int fd, int sorted)
{
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
	fout.open(name, ios::out | ios::trunc);

	fout <<"C1" << ","
		 <<"C2" << ","
		 <<"C3" << "\r\n";
	fout.close();
}

bool Csv::sortbysec(const tuple<int, float, int>& a,
               const tuple<int, float, int>& b)
{
    return (get<1>(a) < get<1>(b));
}


bool Csv::csv_create(char** arr, int fd)
{
	fstream fout;
	char name[10];
	sprintf(name, "connection%d.csv", fd);

	fout.open(name, ios::out | ios::app);
	fout <<arr[0] << ","
		 <<arr[1] << ","
		 <<arr[2] << ","
		 <<"\r\n";

	fout.close();
	return 0;
}

bool Csv::csv_create(int c1, float c2, int c3, int fd)
{
	fstream fout;
	char name[10];
	sprintf(name, "connection%d_sorted.csv", fd);
	fout.open(name, ios::out | ios::app);

	fout <<c1<< ","
		 <<c2<< ","
		 <<c3<< ","
		 << "\r\n";
	fout.close();
	return 0;

}
