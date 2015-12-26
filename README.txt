////////* for MacOSX FreeBSD and Linux*/

checkout code to qing:
svn co svn://192.168.32.6/qing/trunk/ qing
cd qing/

make qing and samples:
cmake -DCMAKE_BUILD_TYPE=DEBUG .
make

run sample TCPServer:
./bin/TCPServer

///////* for Android */

checkout code to qing:
svn co svn://192.168.32.6/qing/trunk/ qing
cd qing/
ndk-build.sh

