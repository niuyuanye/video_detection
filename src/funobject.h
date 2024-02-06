/*
 * @Author: nyy imniuyuanye@sina.com
 * @Date: 2023-02-08 08:49:33
 * @LastEditors: nyy imniuyuanye@sina.com
 * @LastEditTime: 2023-02-08 09:26:41
 * @FilePath: /gb_detection/src/object.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef FUNOBJECT_H
#define FUNOBJECT_H
#include <iomanip>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <string>
#include <string.h>

#include <pthread.h>
#include <functional>

class Object
{
public:
    Object* self()
    {
        return this;
    }
    std::function<Object* (void)>  m_sender;
};

#endif //FUNOBJECT_H