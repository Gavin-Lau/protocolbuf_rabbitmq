## WHERE WE CAN USE IT
Using google protocolbuf as serialize tool;  
Using rabbitmq as RPC channel(instead of socket)  
Based on the above two, We can do many thing in high perfermance mode.  
## INSTALL
#### Windows Platform
1. build sub module "rabbitmq" in directory "third/rabbitmq",then copy build "*.lib"s to "project-dir/../third/rabbitmq".
2. do step1 for sub modle "protobuff" too.
3. exec file "project-dir/src/proto/proto.py" to generate proto cpp source files "project-dir/src/stulist.pb.h" and "project-dir/src/stulist.pb.cc"
4. using Microsoft visual studio to build.

#### Linux Platform (not finished)
``` shell
mkdir Release/Debug
cmake .. -DBUILD_TYPE=releas/debug \
        -DPLATFORM=x32/x64
```
only two platform are supported: msvc &  unix gcc  
    mingw is not Work

## Stage
only Windows msvc supported by 2016.12.27.
