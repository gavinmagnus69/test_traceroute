# traceroute

traceroute - print the route packets trace to network host
## Dependencies
Google tests libriary

https://github.com/google/googletest/blob/main/googletest/README.md

## Build

 ```sh
 git clone https://github.com/gavinmagnus69/test_traceroute.git

 cd test_traceroute

 mkdir build && cd build

 cmake ..

 make
 ```

## Run
In build folder.
You'll need sudo.

```sh
sudo ./traceroute <host> [options] 
```

## Options

    Options:
        -m max_ttl --max-hops max_ttl      Set the max number of hops (max TTL to be reached). Default is 30
        -r rt --response-timeout rt    Set the max response timeout time (in seconds). Default is 1 second
        -n --no-dns    Do not resolve IP addresses to their domain names
        -h --help   Read this help and exit
        -q nqueries --queries nqueries    Set the number of probes per each hop. Default is 1
    
    Arguments:
        host    The host to traceroute to

## Testing
In build folder.
You'll need sudo.

```sh
sudo ./testing
```
