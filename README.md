## Steps to build and run
* install etcd-cpp-apiv3 via vcpkg
* `mkdir debug`
* `cd debug && cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=<VCPKG_HOME>/scripts/buildsystems/vcpkg.cmake ..` 
* `make`
* `./test_etcd`
* use `top` or Activity Monitor to check the memory is increasing

