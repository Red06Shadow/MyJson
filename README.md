# MyJson
As the name suggests, MyJson is a library for handling `.json` files in `C++`. It leverages the power of Simple-Lexer, although I think I should look for something more lightweight if I want it to be integrated into other programs without external dependencies.

## Usage/Examples
This example demonstrates how to use the library:
```c++
#include <iostream>
#include <ajson/ajson.hpp>
#include <fstream>

using namespace std;
// Main function
int main(int args, char *argv[])
{
// Defining the JSON object
ajson::json js = ajson::json({
ajson::value(ajson::object({
{"married", ajson::value(false)},
{"name", ajson::value("I don't know, I don't remember")},
{"money", ajson::value(0.005)}
}))
}); 
// Getting the main element if only one element was added to the JSON object
ajson::object& obj = js.get<ajson::object>(); 
// Inserting elements into an object
obj.insert("direction", ajson::object(
{
{"house", ajson::value(1200)},
{"calle", ajson::value("Carolina")},
{"municipio", ajson::value("Miami")}
}
)); 
// Inserting a matrix (array)
obj.insert("family", ajson::matrix({
"mother",
"father"
})); 
// Retrieving elements
std::cout << obj.get<ajson::matrix>("family").get<string>(0) << std::endl; 
// Exporting to a file
std::ofstream out = std::ofstream("C:\\Proyectos(Red06Shadow)\\c++\\ajson\\test\\main.json"); 
out << js << std::endl; 
out.close(); 
// Reading from a file
std::ifstream in = std::ifstream("C:\\Proyectos(Red06Shadow)\\c++\\ajson\\test\\main.json"); 
ajson::json js2 = ajson::json::parser(in); 
in.close(); 
std::cout << js2 << std::endl; 
return 0;
}
```

If you run this project in your terminal, something like this should appear—naturally, you should replace the paths with your own:
```powershell
mother
{
"direction" : {
"calle" : "Carolina",
"house" : 1200,
"municipio" : "Miami"
},
"family" : [ "mother", "father" ],
"married" : false,
"money" : 0.005,
"name" : "I don't know, I don't remember"
}
```