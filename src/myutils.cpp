#include "myutils.h"


std::string GetCurrentTimeStamp(int time_stamp_type = 1)
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    char buffer[128];
    strftime(buffer, sizeof(buffer), "%F %T", now_tm);

    std::ostringstream ss;
    ss.fill('0');

    std::chrono::milliseconds ms;
    std::chrono::microseconds cs;
    std::chrono::nanoseconds ns;
    
    switch (time_stamp_type)
    {
    case 0:
        ss << buffer;
        break;
    case 1:
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        ss << buffer << ":" << ms.count();
        break;
    case 2:
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        cs = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
        ss << buffer << ":" << ms.count() << ":" << cs.count() % 1000;
        break;
    case 3:
        ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        cs = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;
        ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()) % 1000000000;
        ss << buffer << ":" << ms.count() << ":" << cs.count() % 1000 << ":" << ns.count() % 1000;
        break;
    default:
        ss << buffer;
        break;
    }
    return ss.str();
}
std::string GetNowTimeStamp()
{
  std::string sTimeStamps;
  auto nowTime = std::chrono::system_clock::now();
  auto microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(nowTime.time_since_epoch()).count();
  std::ostringstream osstime;
  osstime << microSeconds;
  sTimeStamps = osstime.str();
  return sTimeStamps;
}
/*
  把当前时间戳（ “2022-09-18 09:06:23:235” ）变换为 std::vector<int>类型
*/
std::vector<int> stringtime2Int(const std::string& time){
    //"2022-08-19 12:26:02:548"
    std::vector<int> time_list;
    char delim[1] = {' '};
    std::vector<std::string> time_vect = stringSplit(time,delim[0]);
    char delim_2[1] = {'-'};
    std::vector<std::string> time_year = stringSplit(time_vect[0],delim_2[0]);
    char delim_3[1] = {':'};
    std::vector<std::string> time_hour = stringSplit(time_vect[1],delim_3[0]);
    int year    = StringToint(time_year[0]);
    int month   = StringToint(time_year[1]);
    int day     = StringToint(time_year[2]);

    int hour    = StringToint(time_hour[0]);
    int min     = StringToint(time_hour[1]);
    int second  = StringToint(time_hour[2]);
    int msecond = StringToint(time_hour[3]);
    time_list.push_back(year);
    time_list.push_back(month);
    time_list.push_back(day);

    time_list.push_back(hour);
    time_list.push_back(min);
    time_list.push_back(second);
    time_list.push_back(msecond);

    // std::cout<<"year:"<<year<< " month:"<< month<<" day:"<<day <<"\n";
    // std::cout<<"hour:"<<hour<< " min:"<< min<<" second:"<<second <<"\n";
    return time_list;
}

double StringToDouble(std::string Input) 
{ 
    double Result; 
    std::stringstream Oss; 
    Oss<<Input; 
    Oss>>Result; 
    return Result; 
} 

int StringToint(std::string Input){
    int Result; 
    std::stringstream Oss; 
    Oss<<Input; 
    Oss>>Result; 
    return Result; 
}

/*
  string 的 split方法
*/
std::vector<std::string> stringSplit(const std::string& str, char delim) {
    std::string s;
    s.append(1, delim);
    std::regex reg(s);
    std::vector<std::string> elems(std::sregex_token_iterator(str.begin(), str.end(), reg, -1),
                                   std::sregex_token_iterator());
    return elems;
}

int read_files_in_dir_nyy(const char *p_dir_name, std::vector<std::string> &file_names)
 {
    DIR *p_dir = opendir(p_dir_name);
    if (p_dir == nullptr) {
        return -1;
    }
    struct dirent* p_file = nullptr;
    while ((p_file = readdir(p_dir)) != nullptr) {
        if (strcmp(p_file->d_name, ".") != 0 &&
            strcmp(p_file->d_name, "..") != 0) {
            //std::string cur_file_name(p_dir_name);
            //cur_file_name += "/";
            //cur_file_name += p_file->d_name;
            std::string cur_file_name(p_file->d_name);
            file_names.push_back(cur_file_name);
        }
    }

    closedir(p_dir);
    return 0;
 }

//IniParser
CParseIniFile::CParseIniFile()
{
}

CParseIniFile::~CParseIniFile()
{

}

bool CParseIniFile::ReadConfig(const std::string& filename, std::map<std::string, std::string>& mContent, const char* section)
{
    mContent.clear();
    std::ifstream infile(filename.c_str());
    if (!infile)
    {
        //LOG4CXX_ERROR(logger, "file open error!");
        return false;
    }
    std::string line, key, value;
    int pos = 0;
    std::string Tsection = std::string("[") + section + "]";
    bool flag = false;
    while (getline(infile, line))
    {
        if (!flag)
        {
            pos = line.find(Tsection, 0);
            if (-1 == pos)
            {
                continue;
            }
            else
            {
                flag = true;
                getline(infile, line);
            }
        }
        if (0 < line.length() && '[' == line.at(0))
        {
            break;
        }
        if (0 < line.length() && AnalyseLine(line, key, value))
        {

            if (value.length() > 0)
            {
                if (value[value.size() - 1] == '\r')
                {
                    value[value.size() - 1] = '\0';
                }
            }
            mContent[key] = value;
        }
    }
    infile.close();
    return true;
}

bool CParseIniFile::AnalyseLine(const std::string & line, std::string & key, std::string & val)
{
    if (line.empty())
    {
        return false;
    }
    int start_pos = 0, end_pos = line.size() - 1, pos = 0;
    if ((pos = line.find(COMMENT_CHAR)) != -1)
    {
        if (0 == pos)
        {//行的第一个字符就是注释字符
            return false;
        }
        end_pos = pos - 1;
    }
    std::string new_line = line.substr(start_pos, start_pos + 1 - end_pos);  // 预处理，删除注释部分

    if ((pos = new_line.find('=')) == -1)
    {
        return false;  // 没有=号
    }

    key = new_line.substr(0, pos);
    val = new_line.substr(pos + 1, end_pos + 1 - (pos + 1));

    Trim(key);
    if (key.empty())
    {
        return false;
    }
    Trim(val);
    return true;
}

void CParseIniFile::Trim(std::string & str)
{
    if (str.empty())
    {
        return;
    }
    unsigned int i, start_pos, end_pos;
    for (i = 0; i < str.size(); ++i)
    {
        if (!IsSpace(str[i]))
        {
            break;
        }
    }
    if (i == str.size())
    { //全部是空白字符串
        str = "";
        return;
    }

    start_pos = i;

    for (i = str.size() - 1; i >= 0; --i)
    {
        if (!IsSpace(str[i]))
        {
            break;
        }
    }
    end_pos = i;

    str = str.substr(start_pos, end_pos - start_pos + 1);
}

bool CParseIniFile::IsSpace(char c)
{
    if (' ' == c || '\t' == c)
    {
        return true;
    }
    return false;
}

bool CParseIniFile::IsCommentChar(char c)
{
    switch (c)
    {
    case COMMENT_CHAR:
        return true;
    default:
        return false;
    }
}

void CParseIniFile::PrintConfig(const std::map<std::string, std::string> & mContent)
{
    std::map<std::string, std::string>::const_iterator mite = mContent.begin();
    for (; mite != mContent.end(); ++mite)
    {
        std::cout << mite->first << "=" << mite->second << std::endl;
    }
}

//=================================================
