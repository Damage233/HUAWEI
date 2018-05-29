#include "predict.h"
#include <stdio.h>
#include <vector>

//你要完成的功能总入口
vector<Node> physicalServer;            //物理服务器
vector<Node> virtualServer;             //需要预测的虚拟机
string startTime;                       //预测开始时间
string endTime;                         //预测结束时间
string dataword[MAX_DATA_NUM][4];       //训练数据，一条记录的各个属性 ID、类型、日期、具体时间
vector<string> virtualM;
Node curPServer;


void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
//读取训练数据
    readTrainData(data,data_num);
////显示训练数据
//    showTainData(dataword,data_num);
//读取info数据
    readInputData(info);
////显示info数据
//    showInputData();
    map<string,int> preVServer = predictVirtualServer(data_num);
////显示预测数据
//    showPredict(preVServer);
//装箱
    map<string,vector<map<string,int>>> deployment;
    packing(preVServer,deployment);
////显示装箱结果
//    showDeploymen(deployment);
//填充
    pading(preVServer,deployment);

//保存答案
    string answer = writeAnswer(preVServer,deployment);
    cout<<endl<<"最终结果："<<endl;
    cout<<answer;

    int len = answer.length();
    char * result_file=(char *) malloc(sizeof(char) * len);
    strcpy(result_file,answer.c_str());

//	// 需要输出的内容
//	char * result_file = (char *)"17\n\n0 8 0 20";

	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
	write_result(result_file, filename);
}
void readTrainData(char * data[MAX_DATA_NUM], int data_num)  //读取训练数据，读入到
{
     for( int i=0 ;i< data_num; ++i)
    {
        int j =0 ;
        string dataline = data[i];
        istringstream stream1(dataline);
        while(stream1>>dataword[i][j++]){}
    }
}
void showTainData(string dataword[][4],int data_num)
{
    for(int i = 0; i < data_num; ++i)
    {
        cout<<dataword[i][0]<<" "<<dataword[i][1]<<" "<<dataword[i][2]<<" "<<dataword[i][3]<<endl;
    }
}
void readInputData(char * info[MAX_INFO_NUM])
{
    string startD,startT,endD,endT;         //预测的开始时间和结束时间
    string line,word;
    Node node;
    int num1,num2;
    for(int i=0; info[i]!='\0'; ++i){
        istringstream stream(info[i]);
        if(i==0){
            stream>>num1;
        }
        if(i>=1&&i<=num1){
            stream>>node.name>>node.cpuNum>>node.memNum;
            physicalServer.push_back(node);
        }
        if(i == num1+2){
            stream>>num2;
        }
        if(i>=num1+3&&i<=num1+2+num2){
            stream>>node.name>>node.cpuNum>>node.memNum;
            node.memNum = node.memNum / 1024 ;
            virtualServer.push_back(node);
        }
        if(i == num1+num2+4){
            stream>>startD;
            stream>>startT;
        }
        if(i == num1+num2+5){
            stream>>endD;
            stream>>endT;
        }
    }
    startTime = startD + " " + startT;
    endTime = endD + " " + endT;

}

void showInputData()
{
    cout<<physicalServer.size()<<endl;
    for(auto iter:physicalServer)
    {
        cout<<iter.name<<" "<<iter.cpuNum<<" "<<iter.memNum<<endl;
    }
    cout<<endl;
    cout<<virtualServer.size()<<endl;
    for(auto iter:virtualServer)
    {
         cout<<iter.name<<" "<<iter.cpuNum<<" "<<iter.memNum<<endl;
    }
    cout<<endl;
    cout<<startTime<<endl;
    cout<<endTime<<endl;
}

time_t StringToDatetime(string str)
{
    char *cha = (char*)str.data();             // 将string转换成char*。
    tm tm_;                                    // 定义tm结构体。
    int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// 将string存储的日期时间,转换为int临时变量。
    tm_.tm_year = year - 1900;                 // 年,由于tm结构体存储的是从1900年开始的时间,所以tm_year为int临时变量减去1900。
    tm_.tm_mon = month - 1;                    // 月,由于tm结构体的月份存储范围为0-11,所以tm_mon为int临时变量减去1。
    tm_.tm_mday = day;                         // 日。
    tm_.tm_hour = hour;                        // 时。
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = second;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。
    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}

string DatetimeToString(time_t time)
{

    tm *tm_ = localtime(&time);                // 将time_t格式转换为tm结构体
    int year, month, day, hour, minute, second;// 定义时间的各个int临时变量。
    year = tm_->tm_year + 1900;                // 临时变量,年,由于tm结构体存储的是从1900年开始的时间,所以临时变量int为tm_year加上1900。
    month = tm_->tm_mon + 1;                   // 临时变量,月,由于tm结构体的月份存储范围为0-11,所以临时变量int为tm_mon加上1。
    day = tm_->tm_mday;                        // 临时变量,日。
    hour = tm_->tm_hour;                       // 临时变量,时。
    minute = tm_->tm_min;                      // 临时变量,分。
    second = tm_->tm_sec;                      // 临时变量,秒。
    char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];// 定义时间的各个char*变量。
    sprintf(yearStr, "%d", year);              // 年。
    sprintf(monthStr, "%d", month);            // 月。
    sprintf(dayStr, "%d", day);                // 日。
    sprintf(hourStr, "%d", hour);              // 时。
    sprintf(minuteStr, "%d", minute);          // 分。
    if (minuteStr[1] == '\0')                  // 如果分为一位,如5,则需要转换字符串为两位,如05。
    {
        minuteStr[2] = '\0';
        minuteStr[1] = minuteStr[0];
        minuteStr[0] = '0';
    }
    sprintf(secondStr, "%d", second);          // 秒。
    if (secondStr[1] == '\0')                  // 如果秒为一位,如5,则需要转换字符串为两位,如05。
    {
        secondStr[2] = '\0';
        secondStr[1] = secondStr[0];
        secondStr[0] = '0';
    }
    char s[20];                                // 定义总日期时间char*变量。
    sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);// 将年月日时分秒合并。
    string str(s);                             // 定义string变量,并将总日期时间char*变量作为构造函数的参数传入。
    return str;                                // 返回转换日期时间后的string变量。
}

map<string,int> predictVirtualServer(int data_num)
{
//string dataword[MAX_DATA_NUM][4];       //训练数据，一条记录的各个属性 ID、类型、日期、具体时间
    time_t dataStartT = StringToDatetime(dataword[0][2] + " " + "00:00:00");
    time_t dataEndT = StringToDatetime(dataword[data_num-1][2] + " " + "23:59:59");
//    cout<<DatetimeToString(dataStartT)<<endl;
//    cout<<DatetimeToString(dataEndT)<<endl;
    const time_t week = StringToDatetime("2016-01-14 23:59:59")-StringToDatetime("2016-01-08 00:00:00");
    const time_t day = StringToDatetime("2015-02-20 23:59:59")-StringToDatetime("2015-02-20 00:00:00");
//map<日期，每天的统计结果>
    vector<map<string,int>> allCount;
    for(time_t time = dataStartT; time < dataEndT ; time = time + day)
    {
        map<string,int> temCount = countData(DatetimeToString(time),DatetimeToString(time+day),data_num);
        allCount.push_back(temCount);
    }

    vector<vector<int>> everyFlavorData;
    vector<vector<int>> unSortEveryFlavorData;
    for(size_t i = 0; i < virtualServer.size(); ++i)
    {
            vector<int> flavor;
            for(auto dayData : allCount)
            {
                int num = dayData[virtualServer[i].name];
                flavor.push_back(num);
            }
            unSortEveryFlavorData.push_back(flavor);
            sort(flavor.begin(),flavor.end());
            everyFlavorData.push_back(flavor);
    }
    map<string,int> abnormalHigh; //每种虚拟机的异常上界
    map<string,int> afterDeal;
    for(size_t i = 0; i < virtualServer.size(); ++i)
    {
        int indexLow = 0.25*everyFlavorData[i].size();
        int indexHigh = 0.75*everyFlavorData[i].size();
        int indexNormal = 0.5*everyFlavorData[i].size();
        int low = everyFlavorData[i][indexLow];
        int high = everyFlavorData[i][indexHigh];

        int abnormal = high + 3 * (high - low) ;
        //        int dealAbnormal = everyFlavorData[i][indexNormal];
        int dealAbnormal = abnormal;
//        int abnormal = high;
        abnormalHigh[virtualServer[i].name] = abnormal;
        afterDeal[virtualServer[i].name] = dealAbnormal;
    }
//异常处理unSortEveryFlavorData
    vector<vector<double>> doubleEveryDay;                  //log(1+x)平滑
    for(size_t i = 0; i < virtualServer.size(); ++i)
    {
        vector<double> doubleVec;
        for(vector<int>::iterator iter = unSortEveryFlavorData[i].begin(); iter != unSortEveryFlavorData[i].end();++iter)
        {
            if(*iter > abnormalHigh[virtualServer[i].name])
            {
                *iter = abnormalHigh[virtualServer[i].name];
            }
            doubleVec.push_back(log(1+*iter));
        }
        doubleEveryDay.push_back(doubleVec);
    }
//保存unSortEveryFlavorData副本
    vector<vector<int>> oldunSortEveryFlavorData(unSortEveryFlavorData);

    vector<vector<int>> weekCount;
    for(size_t i = 0; i < virtualServer.size(); ++i)
    {
        vector<int> weekC;
        int k =0,sum=0;
        for(auto iter:unSortEveryFlavorData[i])
        {
            ++k;
            sum += iter;
            if(k == 7)
            {
                weekC.push_back(sum);
                sum = 0;
                k = 0;
            }
        }
        weekCount.push_back(weekC);
    }
    map<string,int> aveWeek;
    for(int i = 0; i < virtualServer.size();++i)
    {
        int countWeek = 0;
        for(auto iter : weekCount[i])
            countWeek+=iter;
        aveWeek[virtualServer[i].name] = countWeek / weekCount[i].size();
        cout<<virtualServer[i].name<<" "<<aveWeek[virtualServer[i].name]<<endl;
    }
    map<string,vector<double>> pre_s1;
    map<string,vector<double>> pre_s2;
    map<string,vector<double>> pre_s3;
    map<string,vector<double>> bt;
    map<string,vector<double>> ct;
    vector<double> s0;
    for(size_t i = 0; i < virtualServer.size(); ++i)
    {
        double init = 0;
        for(auto iter:unSortEveryFlavorData[i])
        {
            init += iter;
        }
        s0.push_back(init / unSortEveryFlavorData[i].size());
    }
//一阶指数平滑
    for(size_t i =0; i < virtualServer.size(); ++i)
    {
        pre_s1[virtualServer[i].name].push_back(ALPHA*unSortEveryFlavorData[i][0] + (1-ALPHA)*s0[i]);
        for(size_t k = 1; k < unSortEveryFlavorData[i].size(); ++k)
        {
            double slt = ALPHA*unSortEveryFlavorData[i][k] + (1-ALPHA)*pre_s1[virtualServer[i].name][k-1];
            pre_s1[virtualServer[i].name].push_back(slt);
        }
    }
//二阶指数平滑
    for(size_t i =0; i < virtualServer.size(); ++i)
    {
        pre_s2[virtualServer[i].name].push_back(ALPHA*pre_s1[virtualServer[i].name][0] + (1-ALPHA)*s0[i]);
        for(size_t k =1; k < unSortEveryFlavorData[i].size(); ++k)
        {
            double s2t = ALPHA*pre_s1[virtualServer[i].name][k] + (1-ALPHA)*pre_s2[virtualServer[i].name][k-1];
            pre_s2[virtualServer[i].name].push_back(s2t);
        }
    }
//三阶指数平滑
    for(size_t i =0; i < virtualServer.size(); ++i)
    {
        pre_s3[virtualServer[i].name].push_back(ALPHA*pre_s2[virtualServer[i].name][0] + (1-ALPHA)*s0[i]);
        for(size_t k =1; k < unSortEveryFlavorData[i].size(); ++k)
        {
            double s3t = ALPHA*pre_s2[virtualServer[i].name][k] + (1-ALPHA)*pre_s3[virtualServer[i].name][k-1];
            pre_s3[virtualServer[i].name].push_back(s3t);
        }
    }
//    for(int m =1; m <= 7; ++m)
//    {
//
//         for(size_t i =0; i < virtualServer.size(); ++i)
//        {
//            double a = 3*pre_s1[virtualServer[i].name].back() -  3*pre_s2[virtualServer[i].name].back() + pre_s3[virtualServer[i].name].back();
//            double b = ALPHA/(2*(1-ALPHA)*(1-ALPHA))*((6-5*ALPHA)* pre_s1[virtualServer[i].name].back()
//                        -(10-8*ALPHA)*pre_s2[virtualServer[i].name].back()
//                        +(4-3*ALPHA)*pre_s3[virtualServer[i].name].back());
//            double c = (ALPHA*ALPHA)/(2*(1-ALPHA)*(1-ALPHA))
//            *(pre_s1[virtualServer[i].name].back() -  2*pre_s2[virtualServer[i].name].back() + pre_s3[virtualServer[i].name].back());
//            cout<<a+b*m+c*pow(m,2)<<endl;
//        }
//    }

////bt
//    for(size_t i =0; i < virtualServer.size(); ++i)
//    {
//        bt[virtualServer[i].name].push_back(unSortEveryFlavorData[i][1] - unSortEveryFlavorData[i][0]);
//        for(size_t k =1; k < unSortEveryFlavorData[i].size(); ++k)
//        {
//            double temT = BITA*(pre_s3[virtualServer[i].name][k]-pre_s3[virtualServer[i].name][k-1])
//                            +(1-BITA)*t[virtualServer[i].name][k-1];
//            bt[virtualServer[i].name].push_back(temT);
//        }
//    }
////ct
//    for(size_t i =0; i < virtualServer.size(); ++i)
//    {
//        ct[virtualServer[i].name].push_back(0);
//        for(size_t k =1; k < unSortEveryFlavorData[i].size(); ++k)
//        {
//            double temP;
//            if( k < 7)
//                temP = SITA*(unSortEveryFlavorData[i][k]-pre_s3[virtualServer[i].name][k-1]-t[virtualServer[i].name][k-1])
//                                +(1-SITA)*p[virtualServer[i].name][k];
//            else
//                 temP = SITA*(unSortEveryFlavorData[i][k]-pre_s3[virtualServer[i].name][k-1]-t[virtualServer[i].name][k-1])
//                                +(1-SITA)*p[virtualServer[i].name][k-7];
//            ct[virtualServer[i].name].push_back(temP);
//        }
//    }
//    for(int m = 1; m <= 7; ++m)
//    {
//        for(size_t i = 0; i < virtualServer.size(); ++i)
//        {
//            double predictNum = pre_s3[virtualServer[i].name][unSortEveryFlavorData[i].size()-1]
//                        + m*t[virtualServer[i].name][unSortEveryFlavorData[i].size()-1]
//                        + p[virtualServer[i].name][unSortEveryFlavorData[i].size() - 7 + (m-1)%7];
//            cout<<predictNum<<endl;
//        }
//    }

//    map<string ,int> predict;
//    for(size_t i = 0; i < virtualServer.size(); ++i)
//    {
//        double aT = 2 * pre_s1[virtualServer[i].name][unSortEveryFlavorData[i].size()-1] - pre_s2[virtualServer[i].name][unSortEveryFlavorData[i].size()-1];
//        double bT = ALPHA/(1-ALPHA)*(pre_s1[virtualServer[i].name][unSortEveryFlavorData[i].size()-1] - pre_s2[virtualServer[i].name][unSortEveryFlavorData[i].size()-1]);
//        predict[virtualServer[i].name] = aT + bT;
//    }






//    ofstream oFile2;
//    oFile2.open("unSortEveryFlavorData.csv",ios::out|ios::trunc);
//    for(size_t i = 0; i < virtualServer.size(); ++i)
//    {
//        int k = 0;
//        for(size_t j = 0; j < unSortEveryFlavorData[i].size(); ++j)
//        {
//            oFile2<<++k<<","<<unSortEveryFlavorData[i][j]<<","<<pre_s1[virtualServer[i].name][j]<<","<<pre_s2[virtualServer[i].name][j]<<","<<pre_s3[virtualServer[i].name][j]<<endl;
//        }
//        oFile2<<endl;
//    }
//
//    ofstream oFile;
//    oFile.open("unSortEveryFlavorData.csv",ios::out|ios::trunc);
//    for(size_t i = 0; i < virtualServer.size(); ++i)
//    {
//        int k = 0;
//        for(vector<int>::iterator iter = unSortEveryFlavorData[i].begin(); iter != unSortEveryFlavorData[i].end();++iter)
//        {
//            oFile<<++k<<","<<*iter<<endl;
//        }
//        oFile<<endl;
//    }
//
//    ofstream oFile1;
//    oFile1.open("logEveryFlavorData.csv",ios::out|ios::trunc);
//    for(size_t i = 0; i < virtualServer.size(); ++i)
//    {
//        int k = 0;
//        for(vector<double>::iterator iter = doubleEveryDay[i].begin(); iter != doubleEveryDay[i].end();++iter)
//        {
//            oFile1<<++k<<","<<*iter<<endl;
//        }
//        oFile1<<endl;
//    }



//    for(auto cou:allCount)
//    {
//        for(auto c:cou)
//        {
//            cout<<c.first<<" "<<c.second<<" ";
//        }
//        cout<<endl;
//    }
//    for(auto iter : abnormalHigh)
//    {
//        cout<<iter.first<<" "<<iter.second<<endl;
//    }
//    for(auto iter : afterDeal)
//    {
//        cout<<iter.first<<" "<<iter.second<<endl;
//    }
//    for(auto data:unSortEveryFlavorData)
//    {
//        for(auto k:data)
//            cout<<k<<" ";
//        cout<<endl;
//    }
//预测
    map<string,int> predict;
    for(int i = 1; i <= 7; ++i)
    {
        //map<string,map<string,int>> allCount;
        map<string,int> dayCount = allCount[allCount.size()-i];
        for(auto iter : dayCount)
        {
            if(iter.second > abnormalHigh[iter.first])
                predict[iter.first] += afterDeal[iter.first];
            else
                predict[iter.first] += iter.second;
        }
    }
    for(map<string,int>::iterator iter = predict.begin(); iter != predict.end();++iter)
    {
        iter -> second = WEIGHT * aveWeek[iter->first] + (1-WEIGHT)*iter->second;
    }

    time_t differ = StringToDatetime(endTime) - StringToDatetime(startTime);
    for(map<string,int>::iterator iter = predict.begin(); iter != predict.end(); ++iter)
    {
        iter->second = iter->second * static_cast<double>(differ/day) / 7.0;
    }
    return predict;
}

map<string,int> countData(string begin_time,string end_time,int data_num){
    map<string,int> m_count;
    time_t beginT = StringToDatetime(begin_time);
    time_t endT = StringToDatetime(end_time);
    for(int i = 0; i < virtualServer.size(); ++i){
        m_count[virtualServer[i].name] = 0;
    }
    for(int i=0; i<data_num; i++){
        string tempstring = dataword[i][2]+" "+dataword[i][3];
        time_t temp_time = StringToDatetime(tempstring);

        if(temp_time>=beginT && temp_time<=endT){
                if(isInVirtualServer(dataword[i][1]))
                    ++m_count[dataword[i][1]];
        }
    }
    return m_count;
}

bool isInVirtualServer(string flavor)
{
    for(int i = 0; i < virtualServer.size(); ++i)
    {
        if(virtualServer[i].name == flavor)
            return true;
    }
    return false;
}
void showPredict(const map<string,int> & predict)
{
    for(auto p:predict)
    {
        cout<<p.first<<" "<<p.second<<endl;
    }
}
vector<string> mapToVector(map<string,int> mmap)
{
    vector<string> svec;
    for(map<string,int>::iterator iter = mmap.begin(); iter != mmap.end(); ++iter)
    {
        for(int i=0; i<iter->second; ++i)
            svec.push_back(iter->first);
    }
    return svec;
}
map<string,int> vectorToMap(vector<string> svec)
{
    map<string,int> simap;
    for(vector<string>::iterator iter = svec.begin(); iter != svec.end(); ++iter)
    {
        ++simap[*iter];
    }
    return simap;
}

void packing(map<string,int> preVServer,map<string,vector<map<string,int>>>& deployment)
{
    virtualM = mapToVector(preVServer);
    while(!virtualM.empty())
    {
        double maxscore = -1;
        int index = -1;
        vector<int> bestIndividual;
        for(int i = 0; i < physicalServer.size(); ++i)
        {
            //初始群落
            curPServer = physicalServer[i];
            vector<vector<int> > community = initCommunity();
            //迭代求优
            vector<int> individual = inheritance(community);
            double score = calEff(individual);
            if(score > maxscore)
            {
                maxscore = score;
                index = i;
                bestIndividual = individual;
                if(maxscore == 1)
                    break;

            }
        }
        vector<map<string,int>> temDeploy = deployment[physicalServer[index].name];
        vector<string> temBox = vecIntTovecString(bestIndividual);
        map<string,int> temMBox = vectorToMap(temBox);
        temDeploy.push_back(temMBox);
        deployment[physicalServer[index].name] = temDeploy;
        cout<<"index:"<<index<<" "<<maxscore<<endl;
        updateVirtualM(bestIndividual);
//        cout<<"..."<<virtualM.size()<<endl;

    }
}
vector<int> inheritance(vector<vector<int>> community)
{
    vector<vector<int>> last_community = community;
    int length = virtualM.size();
    vector<vector<int>>new_community; //下一代群落
    int iteration = 0;           //迭代次数
//对群落排序
    sort(last_community.begin(),last_community.end(),isMore);
    double eff = calEff(last_community[0]);
    while(iteration<iterNum && eff < 1){
    //当前群落中排名前10%的直接进入下一轮
        for(int i = 0; i < 0.1*individualNum ;i++){
            new_community.push_back(last_community[i]);
        }
        while(new_community.size()<individualNum){
            int father = 0.1*individualNum+rand()%static_cast<int>(0.4*individualNum);
            int mother = 0.1*individualNum+rand()%static_cast<int>(0.4*individualNum);
            vector<int> son = last_community[father];
    //基因重组
            for(int i = 0; i < 0.6*individualNum; ++i){
                int tem = rand()%length;
                son[tem] = last_community[mother][tem];
            }
            decode(son);
            new_community.push_back(son);
        }
    //变异
        for(int i = 1;i < new_community.size(); ++i){
            for(int j = 0; j < 0.6*length;++j){
                int tem = rand()%length;
                new_community[i][tem] = 1;
            }
//            cout<<"decode前:"<<endl;
//            for(int k = 0 ;k < new_community[i].size(); ++k)
//                cout<<new_community[i][k]<<" ";
//            cout<<endl;
            decode(new_community[i]);
//            cout<<"decode后:"<<endl;
//            for(int k = 0 ;k < new_community[i].size(); ++k)
//                cout<<new_community[i][k]<<" ";
//            cout<<endl;
        }
    //排序
        sort(new_community.begin(),new_community.end(),isMore);
        last_community = new_community;
        ++iteration;
        eff = calEff(last_community[0]);
    }
//    for(auto iter:last_community[0])
//        cout<<iter<<" ";
//    cout<<endl;
//    cout<<calEff(last_community[0])<<endl;
    return last_community[0];

}
vector<vector<int>> initCommunity()
{
    int length = virtualM.size();
    vector<int> individual;         //个体
    vector<vector<int>> community; //群落
//产生初代个体
    for(int i = 0; i < individualNum; i++)
    {
        individual.clear();
        for(int j = 0; j < length; j++)
            individual.push_back(rand()%100%2);
//解码,取得合法解
        decode(individual);
        community.push_back(individual);
    }
    return community;
}
void decode(vector<int>& individual)
{
    int cpu = 0;
    int mem = 0;
    for(int i = 0; i < individual.size(); ++i){
        if(individual[i]){
            Node node = findName(virtualM[i]);
            cpu += node.cpuNum;
            mem += node.memNum;
            if(cpu>curPServer.cpuNum||mem>curPServer.memNum){
                individual[i] = 0;
                cpu -= node.cpuNum;
                mem -= node.memNum;
            }
        }
    }
}
Node findName(string name)
{

    for(int i = 0; i < virtualServer.size(); ++i)
    {
        if(name == virtualServer[i].name)
            return virtualServer[i];
    }
}
bool isMore(const vector<int> &s1,const vector<int> &s2)
{
    return calEff(s1) > calEff(s2);
}
double calCpuEff(vector<int> individual){
    int cpu = 0;
    Node node;
    for(int i = 0; i < individual.size(); ++i){
        if(individual[i]){
            node = findName(virtualM[i]);
            cpu += node.cpuNum;
        }
    }
    return static_cast<double>(cpu) / curPServer.cpuNum;
}

double calMemEff(vector<int> individual){
    int mem = 0;
    Node node;
    for(int i = 0; i < individual.size(); ++i){
        if(individual[i]){
            node = findName(virtualM[i]);
            mem += node.memNum;
        }
    }
    return static_cast<double>(mem) / curPServer.memNum;
}
double calEff(vector<int> individual)
{
    return (calCpuEff(individual)+calMemEff(individual))/2;
}
vector<string> vecIntTovecString(vector<int> individual)
{
    vector<string> vec;
    for(int i = 0; i < individual.size(); ++i)
    {
        if(individual[i])
            vec.push_back(virtualM[i]);
    }
    return vec;
}

void updateVirtualM(vector<int> individual)
{
    vector<string> vec;
    for(int i = 0; i < individual.size(); ++i)
    {
        if(individual[i] == 0)
            vec.push_back(virtualM[i]);

    }
    virtualM = vec;
}
void showDeploymen(map<string,vector<map<string,int>>> deployment)
{
    for(auto deploy:deployment)
    {
        cout<<deploy.first<<" "<<(deploy.second).size()<<endl;
        int i = 0;
        for(auto box : (deploy.second))
        {
            cout<<deploy.first + "-"<<++i;
            for(auto iter : box)
            {
                cout<<" "<<iter.first<<" "<<iter.second;
            }
            cout<<endl;
        }
        cout<<endl;
    }

}

string writeAnswer(map<string,int> preVServer,map<string,vector<map<string,int>>> deployment)
{
    string answer;
    int preNum = 0;
    for(auto iter:preVServer)
    {
        preNum += iter.second;
    }
    answer += intToString(preNum) + "\n";
    for(auto iter:preVServer)
    {
        answer += iter.first + " " + intToString(iter.second)+"\n";
    }

    for(int i = 0; i < physicalServer.size(); ++i)
    {
        vector<map<string,int>> deploy = deployment[physicalServer[i].name];
        if(deploy.size())
        {
            answer += "\n";
            answer += physicalServer[i].name + " " + intToString(deploy.size()) + "\n";
            int k = 0;
            for(auto box : deploy)
            {
                answer += physicalServer[i].name + "-" + intToString(++k);
                for(auto iter : box)
                {
                    answer += " " + iter.first + " " + intToString(iter.second);
                }
                answer += "\n";
            }
        }
    }

//    for(auto deploy:deployment)
//    {
//        answer += "\n";
//        if((deploy.second).size())
//        {
//            answer += deploy.first + " " + intToString((deploy.second).size()) + "\n";
//            int i = 0;
//            for(auto box : (deploy.second))
//            {
//                answer += deploy.first + "-" + intToString(++i);
//                for(auto iter : box)
//                {
//                    answer += " " + iter.first + " " + intToString(iter.second);
//                }
//                answer += "\n";
//            }
//        }
//    }
    return answer;
}
string intToString(int n)
{
    ostringstream stream;
    stream<<n;  //n为int类型
    return stream.str();
}

void pading(map<string,int>& preVServer,map<string,vector<map<string,int>>>& deployment)
{
    typedef map<string,vector<map<string,int>>>::iterator Miterator;
    typedef vector<map<string,int>>::iterator Viterator;
    vector<string> sortedVec = mapToVector(preVServer);
    sort(sortedVec.begin(),sortedVec.end(),compare);
    for(Miterator miter = deployment.begin();miter != deployment.end(); ++miter)
    {
        for(Viterator viter = (miter->second).begin() ; viter != (miter->second).end(); ++viter)
        {
             while(!overFull(miter->first,*viter))
            {
                int ran = rand()%(sortedVec.size());
                ++(*viter)[sortedVec[ran]];
                ++preVServer[sortedVec[ran]];
                if(overFull(miter->first,*viter))
                {
                    --(*viter)[sortedVec[ran]];
                    --preVServer[sortedVec[ran]];
                    break;
                }

            }
            while(!overFull(miter->first,*viter))
            {
                ++(*viter)[sortedVec[0]];
                ++preVServer[sortedVec[0]];
                if(overFull(miter->first,*viter))
                {
                    --(*viter)[sortedVec[0]];
                    --preVServer[sortedVec[0]];
                    break;

                }

            }
        }

    }

}
bool compare( string s1,string s2)
{
    int a = findName(s1).cpuNum + findName(s1).memNum;
    int b = findName(s2).cpuNum + findName(s2).memNum;
    return a < b;
}
bool overFull(string physicalS,map<string,int> deploy)
{
    int index = findPhyicalServer(physicalS);
    int cpu = 0;
    int mem = 0;
    for(auto iter:deploy)
    {
        cpu += findName(iter.first).cpuNum * iter.second;
        mem += findName(iter.first).memNum * iter.second;
    }
    if(cpu >physicalServer[index].cpuNum || mem > physicalServer[index].memNum)
        return true;
    else
        return false;

}
int findPhyicalServer(string name)
{
    for(int i = 0; i < physicalServer.size(); ++i)
    {
        if(physicalServer[i].name == name)
            return i;
    }
    return -1;
}











