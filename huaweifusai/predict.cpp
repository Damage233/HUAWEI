#include "predict.h"
#include <stdio.h>
#include <vector>

//��Ҫ��ɵĹ��������
vector<Node> physicalServer;            //���������
vector<Node> virtualServer;             //��ҪԤ��������
string startTime;                       //Ԥ�⿪ʼʱ��
string endTime;                         //Ԥ�����ʱ��
string dataword[MAX_DATA_NUM][4];       //ѵ�����ݣ�һ����¼�ĸ������� ID�����͡����ڡ�����ʱ��
vector<string> virtualM;
Node curPServer;


void predict_server(char * info[MAX_INFO_NUM], char * data[MAX_DATA_NUM], int data_num, char * filename)
{
//��ȡѵ������
    readTrainData(data,data_num);
////��ʾѵ������
//    showTainData(dataword,data_num);
//��ȡinfo����
    readInputData(info);
////��ʾinfo����
//    showInputData();
    map<string,int> preVServer = predictVirtualServer(data_num);
////��ʾԤ������
//    showPredict(preVServer);
//װ��
    map<string,vector<map<string,int>>> deployment;
    packing(preVServer,deployment);
////��ʾװ����
//    showDeploymen(deployment);
//���
    pading(preVServer,deployment);

//�����
    string answer = writeAnswer(preVServer,deployment);
    cout<<endl<<"���ս����"<<endl;
    cout<<answer;

    int len = answer.length();
    char * result_file=(char *) malloc(sizeof(char) * len);
    strcpy(result_file,answer.c_str());

//	// ��Ҫ���������
//	char * result_file = (char *)"17\n\n0 8 0 20";

	// ֱ�ӵ�������ļ��ķ��������ָ���ļ���(ps��ע���ʽ����ȷ�ԣ�����н⣬��һ��ֻ��һ�����ݣ��ڶ���Ϊ�գ������п�ʼ���Ǿ�������ݣ�����֮����һ���ո�ָ���)
	write_result(result_file, filename);
}
void readTrainData(char * data[MAX_DATA_NUM], int data_num)  //��ȡѵ�����ݣ����뵽
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
    string startD,startT,endD,endT;         //Ԥ��Ŀ�ʼʱ��ͽ���ʱ��
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
    char *cha = (char*)str.data();             // ��stringת����char*��
    tm tm_;                                    // ����tm�ṹ�塣
    int year, month, day, hour, minute, second;// ����ʱ��ĸ���int��ʱ������
    sscanf(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);// ��string�洢������ʱ��,ת��Ϊint��ʱ������
    tm_.tm_year = year - 1900;                 // ��,����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ��,����tm_yearΪint��ʱ������ȥ1900��
    tm_.tm_mon = month - 1;                    // ��,����tm�ṹ����·ݴ洢��ΧΪ0-11,����tm_monΪint��ʱ������ȥ1��
    tm_.tm_mday = day;                         // �ա�
    tm_.tm_hour = hour;                        // ʱ��
    tm_.tm_min = minute;                       // �֡�
    tm_.tm_sec = second;                       // �롣
    tm_.tm_isdst = 0;                          // ������ʱ��
    time_t t_ = mktime(&tm_);                  // ��tm�ṹ��ת����time_t��ʽ��
    return t_;                                 // ����ֵ��
}

string DatetimeToString(time_t time)
{

    tm *tm_ = localtime(&time);                // ��time_t��ʽת��Ϊtm�ṹ��
    int year, month, day, hour, minute, second;// ����ʱ��ĸ���int��ʱ������
    year = tm_->tm_year + 1900;                // ��ʱ����,��,����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ��,������ʱ����intΪtm_year����1900��
    month = tm_->tm_mon + 1;                   // ��ʱ����,��,����tm�ṹ����·ݴ洢��ΧΪ0-11,������ʱ����intΪtm_mon����1��
    day = tm_->tm_mday;                        // ��ʱ����,�ա�
    hour = tm_->tm_hour;                       // ��ʱ����,ʱ��
    minute = tm_->tm_min;                      // ��ʱ����,�֡�
    second = tm_->tm_sec;                      // ��ʱ����,�롣
    char yearStr[5], monthStr[3], dayStr[3], hourStr[3], minuteStr[3], secondStr[3];// ����ʱ��ĸ���char*������
    sprintf(yearStr, "%d", year);              // �ꡣ
    sprintf(monthStr, "%d", month);            // �¡�
    sprintf(dayStr, "%d", day);                // �ա�
    sprintf(hourStr, "%d", hour);              // ʱ��
    sprintf(minuteStr, "%d", minute);          // �֡�
    if (minuteStr[1] == '\0')                  // �����Ϊһλ,��5,����Ҫת���ַ���Ϊ��λ,��05��
    {
        minuteStr[2] = '\0';
        minuteStr[1] = minuteStr[0];
        minuteStr[0] = '0';
    }
    sprintf(secondStr, "%d", second);          // �롣
    if (secondStr[1] == '\0')                  // �����Ϊһλ,��5,����Ҫת���ַ���Ϊ��λ,��05��
    {
        secondStr[2] = '\0';
        secondStr[1] = secondStr[0];
        secondStr[0] = '0';
    }
    char s[20];                                // ����������ʱ��char*������
    sprintf(s, "%s-%s-%s %s:%s:%s", yearStr, monthStr, dayStr, hourStr, minuteStr, secondStr);// ��������ʱ����ϲ���
    string str(s);                             // ����string����,����������ʱ��char*������Ϊ���캯���Ĳ������롣
    return str;                                // ����ת������ʱ����string������
}

map<string,int> predictVirtualServer(int data_num)
{
//string dataword[MAX_DATA_NUM][4];       //ѵ�����ݣ�һ����¼�ĸ������� ID�����͡����ڡ�����ʱ��
    time_t dataStartT = StringToDatetime(dataword[0][2] + " " + "00:00:00");
    time_t dataEndT = StringToDatetime(dataword[data_num-1][2] + " " + "23:59:59");
//    cout<<DatetimeToString(dataStartT)<<endl;
//    cout<<DatetimeToString(dataEndT)<<endl;
    const time_t week = StringToDatetime("2016-01-14 23:59:59")-StringToDatetime("2016-01-08 00:00:00");
    const time_t day = StringToDatetime("2015-02-20 23:59:59")-StringToDatetime("2015-02-20 00:00:00");
//map<���ڣ�ÿ���ͳ�ƽ��>
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
    map<string,int> abnormalHigh; //ÿ����������쳣�Ͻ�
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
//�쳣����unSortEveryFlavorData
    vector<vector<double>> doubleEveryDay;                  //log(1+x)ƽ��
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
//����unSortEveryFlavorData����
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
//һ��ָ��ƽ��
    for(size_t i =0; i < virtualServer.size(); ++i)
    {
        pre_s1[virtualServer[i].name].push_back(ALPHA*unSortEveryFlavorData[i][0] + (1-ALPHA)*s0[i]);
        for(size_t k = 1; k < unSortEveryFlavorData[i].size(); ++k)
        {
            double slt = ALPHA*unSortEveryFlavorData[i][k] + (1-ALPHA)*pre_s1[virtualServer[i].name][k-1];
            pre_s1[virtualServer[i].name].push_back(slt);
        }
    }
//����ָ��ƽ��
    for(size_t i =0; i < virtualServer.size(); ++i)
    {
        pre_s2[virtualServer[i].name].push_back(ALPHA*pre_s1[virtualServer[i].name][0] + (1-ALPHA)*s0[i]);
        for(size_t k =1; k < unSortEveryFlavorData[i].size(); ++k)
        {
            double s2t = ALPHA*pre_s1[virtualServer[i].name][k] + (1-ALPHA)*pre_s2[virtualServer[i].name][k-1];
            pre_s2[virtualServer[i].name].push_back(s2t);
        }
    }
//����ָ��ƽ��
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
//Ԥ��
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
            //��ʼȺ��
            curPServer = physicalServer[i];
            vector<vector<int> > community = initCommunity();
            //��������
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
    vector<vector<int>>new_community; //��һ��Ⱥ��
    int iteration = 0;           //��������
//��Ⱥ������
    sort(last_community.begin(),last_community.end(),isMore);
    double eff = calEff(last_community[0]);
    while(iteration<iterNum && eff < 1){
    //��ǰȺ��������ǰ10%��ֱ�ӽ�����һ��
        for(int i = 0; i < 0.1*individualNum ;i++){
            new_community.push_back(last_community[i]);
        }
        while(new_community.size()<individualNum){
            int father = 0.1*individualNum+rand()%static_cast<int>(0.4*individualNum);
            int mother = 0.1*individualNum+rand()%static_cast<int>(0.4*individualNum);
            vector<int> son = last_community[father];
    //��������
            for(int i = 0; i < 0.6*individualNum; ++i){
                int tem = rand()%length;
                son[tem] = last_community[mother][tem];
            }
            decode(son);
            new_community.push_back(son);
        }
    //����
        for(int i = 1;i < new_community.size(); ++i){
            for(int j = 0; j < 0.6*length;++j){
                int tem = rand()%length;
                new_community[i][tem] = 1;
            }
//            cout<<"decodeǰ:"<<endl;
//            for(int k = 0 ;k < new_community[i].size(); ++k)
//                cout<<new_community[i][k]<<" ";
//            cout<<endl;
            decode(new_community[i]);
//            cout<<"decode��:"<<endl;
//            for(int k = 0 ;k < new_community[i].size(); ++k)
//                cout<<new_community[i][k]<<" ";
//            cout<<endl;
        }
    //����
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
    vector<int> individual;         //����
    vector<vector<int>> community; //Ⱥ��
//������������
    for(int i = 0; i < individualNum; i++)
    {
        individual.clear();
        for(int j = 0; j < length; j++)
            individual.push_back(rand()%100%2);
//����,ȡ�úϷ���
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
    stream<<n;  //nΪint����
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











