# video_dection

#### Description
{**When you're done, you can delete the content in this README and update the file with details for others getting started with your repository**}

#### Software Architecture
Software architecture description

#### Installation

1.  xxxx
2.  xxxx
3.  xxxx

#### Instructions

1.  xxxx
2.  xxxx
3.  xxxx

#### Contribution

1.  Fork the repository
2.  Create Feat_xxx branch
3.  Commit your code
4.  Create Pull Request


#### Gitee Feature

1.  You can use Readme\_XXX.md to support different languages, such as Readme\_en.md, Readme\_zh.md
2.  Gitee blog [blog.gitee.com](https://blog.gitee.com)
3.  Explore open source project [https://gitee.com/explore](https://gitee.com/explore)
4.  The most valuable open source project [GVP](https://gitee.com/gvp)
5.  The manual of Gitee [https://gitee.com/help](https://gitee.com/help)
6.  The most popular members  [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)

#### mysql

```
sudo apt install mysql-server     #安装MySQL服务器
sudo apt-get install mysql-client #安装MySQL客户端

sudo service mysql start          #启动MySQL数据库服务
或
sudo systemctl start mysql.service

sudo service mysql restart        #重启MySQL数据库服务
或
sudo systemctl restart mysql.service

sudo service mysql stop          #停止MySQL数据库服务
或
sudo systemctl stop mysql.service

sudo service mysql enable        #设置MySQL服务开机自启动
或
sudo systemctl enable mysql.service

sudo service mysql disable       #停止MySQL服务开机自启动
或 
sudo systemctl disable mysql.service
sudo vim /etc/mysql/mysql.conf.d/mysqld.cnf  #MySQL的配置文件

# 登录mysql并输入密码
mysql -u root -p
# 首先为用户创建一个数据库(testDB)
create database testDB;
#  授权test用户拥有testDB数据库的所有权限（某个数据库的所有权限）：
grant all privileges on testDB.* to test@"%" identified by '@Xavier123';
grant all privileges on testDB.* to test@localhost identified by '1234';
flush privileges;//刷新系统权限表
格式：grant 权限 on 数据库.* to 用户名@登录主机 identified by "密码";　
#授权test用户拥有所有数据库的某些权限：
grant select,delete,update,create,drop on *.* to test@"%" identified by "1234";
5. 列出所有数据库
　　mysql>show database;
6. 切换数据库
　　mysql>use '数据库名';
7. 列出所有表
　　mysql>show tables;
8. 显示数据表结构
　　mysql>describe 表名;
9. 删除数据库和数据表
　　mysql>drop database 数据库名;
　　mysql>drop table 数据表名;

```

