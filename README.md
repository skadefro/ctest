# getting started
compile and run in debug mode

make will automaticly download clib_openiap.h and binary for current platform, if you cannot use make you need to, go to [rustapi](https://github.com/openiap/rustapi/releases) and download library for your platform and place them in the lib folder, and rename it to libopeniap_clib.so


```bash

setup default credentials
```bash
export apiurl=grpc://grpc.app.openiap.io:443
# username/password
export OPENIAP_USERNAME=username
export OPENIAP_PASSWORD=password
# or better, use a jwt token ( open https://app.openiap.io/jwtlong and copy the jwt value)
export OPENIAP_JWT=eyJhbGciOiJI....
```

```bash
make && ./client_cli
# or with gcc directly
gcc main.c -Llib -lopeniap-linux-x64 -Wl,-rpath=lib -o client_cli && ./client_cli
# to static linked, use
make STATIC=1 && ./client_cli

```
