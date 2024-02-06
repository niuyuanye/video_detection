/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-06 09:15:55
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-07 09:42:57
 * @FilePath: /gb_detection/src/myutils.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream> 
#include <stdio.h>
#include <sys/time.h>

#include <ctime>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iostream>
#include <vector>
#include <regex>

#include <dirent.h>

/*time_stamp_type - 需要获取的时间戳的级别，0表示秒级时间戳，1表示毫秒级时间戳，2表示微秒级时间戳，3表示纳秒级时间戳
输出结果
2022-05-27 14:35:58
2022-05-27 14:35:58:879
2022-05-27 14:35:58:879:200
2022-05-27 14:35:58:879:200:100
*/

std::string GetCurrentTimeStamp(int time_stamp_type); 
std::string GetNowTimeStamp();
std::vector<int> stringtime2Int(const std::string& time);

int StringToint(std::string Input);
double StringToDouble(std::string Input);

std::vector<std::string> stringSplit(const std::string& str, char delim);

int read_files_in_dir_nyy(const char *p_dir_name, std::vector<std::string> &file_names);

// static inline int read_files_in_dir(const char *p_dir_name, std::vector<std::string> &file_names)
//  {
//     DIR *p_dir = opendir(p_dir_name);
//     if (p_dir == nullptr) {
//         return -1;
//     }
//  }


#include <fstream>
#include <iostream>
#include <string>
#include <map>
#define COMMENT_CHAR '#'

class CParseIniFile
{
public:
    CParseIniFile();
    ~CParseIniFile();
    bool ReadConfig(const std::string& filename, std::map<std::string, std::string>& mContent, const char* section);
    bool AnalyseLine(const std::string & line, std::string & key, std::string & val);
    void Trim(std::string & str);
    bool IsSpace(char c);
    bool IsCommentChar(char c);
    void PrintConfig(const std::map<std::string, std::string> & mContent);
};

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#endif //UTILS_H

