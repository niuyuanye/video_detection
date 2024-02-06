#/bin/sh
###
 # @Author: nyy imniuyuanye@sina.com
 # @Date: 2023-02-06 09:48:08
 # @LastEditors: nyy imniuyuanye@sina.com
 # @LastEditTime: 2023-02-22 14:33:18
 # @FilePath: /gb_detection/start.sh
 # @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
### 
#export LD_LIBRARY_PATH=/home/sw/workspace/c++/ftpC++/ftp/libx64/log4cplus/lib$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/usr/lib/aarch64-linux-gnu:$LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=/home/xavier/workspace/nyy/ftp/bin:$LD_LIBRARY_PATH


# export LD_LIBRARY_PATH=/usr/local/opencv34/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/dev/shm/opencv/opencv3420/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/dev/shm//vpf1/video_detection/bin:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/dev/shm/vpf1/video_detection/HK/libx64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/dev/shm/vpf1/video_detection/log4cplus/libx64/log4cplus/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/ffmpeg4.4/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/dev/shm/video_detection/HK/libx64/:$LD_LIBRARY_PATH

cd ./build
make clean
make -j40
cd ../bin
./demo


