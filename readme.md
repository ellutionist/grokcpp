# grokcpp

C++ implementation of [Grok Pattern](https://www.elastic.co/guide/en/logstash/current/plugins-filters-grok.html).

## How to use

### Prerequisites

- C++17
- CMake >= 3.20
- Boost.Regex
    - For now this library employs [Boost.Regex](https://www.boost.org/doc/libs/1_79_0/libs/regex/doc/html/index.html). Planing to support more regex libraries in the future.
- Google Test
    - Gropcpp use gtest for testing purposes. This dependency can be excluded by using cmake option `-DENABLE_TESTS=OFF`.

### Build

```shell
cd grokcpp
cmake -B build .
make -C build
```

### Quick Example
```c++
#include "grokcpp/grok.h"
#include <cassert>
#include <iostream>

int main() {
  grok::Grok g {"%{EMAILADDRESS:myemail}"};
  grok::GrokMatch match;

  assert(g.match("my email is: whoami123@whereami.com", match));

  std::cout << match["myemail"] << std::endl;
  // whoami123@whereami.com

  return 0;
}
```

### Predefined Patterns

Grokcpp supports all base patterns from [here](https://github.com/hpcugent/logstash-patterns/blob/master/files/grok-patterns).

### Use Your Own Pattern

```c++
{
  grok::Grok g {"[A-Z]+"};
  g.register_self("CAPITALS");
} // life time ends, but registered as a global pattern already.

grok::Grok g { "%{CAPITALS:caps}" };
grok::GrokMatch match;
if (g.match("my name in upper case: JOHN", match)) {
  std::cout << match["caps"] << std::endl;
  // JOHN
}
```

- You can also register pattern from:
    - file, using function `grok::Grok::register_patterns_from_file(const std::string &file_name)`;
    - directory, using function `grok::Grok::register_patterns_from_dir(const std::string &file_path)`;
