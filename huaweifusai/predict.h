#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <cmath>
#include <ctgmath>

using namespace std;

struct Node{
    string name;
    int cpuNum;
    int memNum;
};
const int individualNum = 60;
const int iterNum =50;
const double ALPHA = 0.8;
const double BITA = 0.5;
const double SITA = 0.1;
const double WEIGHT = 0;

void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename);
void readTrainData(char * data[MAX_DATA_NUM], int data_num);
void showTainData(string dataword[][4],int data_num);
void readInputData(char * info[MAX_INFO_NUM]);
void showInputData();
time_t StringToDatetime(string str);
string DatetimeToString(time_t time);
map<string,int> predictVirtualServer(int data_num);
bool isInVirtualServer(string flavor);
map<string,int> countData(string begin_time,string end_time,int data_num);
void showPredict(const map<string,int> & predict);
vector<string> mapToVector(map<string,int> mmap);
map<string,int> vectorToMap(vector<string> svec);
void packing(map<string,int> preVServer,map<string,vector<map<string,int>>>& deployment);
Node findName(string name);
void decode(vector<int>& individual);
vector<vector<int>> initCommunity();
vector<int> inheritance(vector<vector<int>> community);

bool isMore(const vector<int> &s1,const vector<int> &s2);
double calCpuEff(vector<int> individual);
double calMemEff(vector<int> individual);
double calEff(vector<int> individual);
int findIndexPhysical(Node physical);
vector<string> vecIntTovecString(vector<int> individual);
void updateVirtualM(vector<int> individual);
void showDeploymen(map<string,vector<map<string,int>>> deployment);
string intToString(int n);
string writeAnswer(map<string,int> preVServer,map<string,vector<map<string,int>>> deployment);
void pading(map<string,int>& preVServer,map<string,vector<map<string,int>>>& deployment);
int findPhyicalServer(string name);
bool overFull(string physicalS,map<string,int> deploy);
bool compare( string s1,string s2);

#endif
