# CompileTimeJSON
A C++26 proof-of-concept library for converting structs/classes to JSON directly at compile-time.

## Usage example
```cpp
#include "json/json.h"
#include <iostream>

class User
{
public:
    std::string Username;
    size_t FriendCount;

    struct
    {
        std::string Gender;
        std::string Email;
        unsigned int LikedPosts;
    } ExtraData;
};

int main()
{
    User Bob = User("Bobby Person", 4, {"Male", "bobbyson@example.com", 0});

    std::cout << CompileTimeJSON::StructToJSON(Bob) << "\n";
}
```

```json
{
    "Username": "Bobby Person",
    "FriendCount": 4,
    "ExtraData": {
        "Gender": "Male",
        "Email": "bobbyson@example.com",
        "LikedPosts": 0
    }
}
```
