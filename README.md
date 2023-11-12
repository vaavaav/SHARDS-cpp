# SHARDS-cpp

This is an implementation of the miss ratio curve online construction technique [SHARDS](https://www.usenix.org/conference/fast15/technical-sessions/presentation/waldspurger) in C++. It is heavily inspired by [SHARDS-C](https://github.com/jorgermurillo/SHARDS-C), a fanmade implementation in C.

## Integration through CMake 

```cmake
include_subdirectory(path/to/SHARDS-cpp)
# or find_package(Shards REQUIRED) if you instead installed Shards in your system
target_link_libraries(your_target Shards)
```

## How to use SHARDS

After including the header file, you can use SHARDS by using a building function for the desired strategy: Fixed Rate or Fixed Size.
Example code:
```cpp
#include <cstdint>
#include <Shards/Shards.h>

int main() {

    Shards * shards = Shards::fixedSize(...);
    // or Shards * shards = Shards::fixedRate(...);

    // On lookups
    std::string const key = ...;
    shards->feed(key);

    // Generate Miss Ratio Curve
    std::map<uint32_t, double> mrc = shards->mrc();

    // Reset internal state if desired
    shards->clear();

    return 0;
} 
```
