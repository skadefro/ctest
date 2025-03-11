# getting started
compile and run in debug mode

Go to [rustapi](https://github.com/openiap/rustapi/releases) and download libraries for your platform and place them in the lib folder.
You should also update clib_openiap.h with the latest version from [here](https://github.com/openiap/rustapi/blob/main/crates/clib/clib_openiap.h).

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
```
