## A simple command line interface for connecting via Interactive Brokers TWS API
- Cancel orders
- Close positions
- Check status

This application requires the C++ client library provided by Interactive Brokers. Add the directory via the -DIB_CLIENT_DIR parameter of cmake. See below for an example:

```
git clone https://github.com/jmjatlanta/ib_cli
cd ib_cli
git submodule update --init --recursive
mkdir build
cd build
cmake -DIB_CLIENT_DIR=/home/yourlogin/IBJts/source/cppclient/client ..
make
cd ..
```
