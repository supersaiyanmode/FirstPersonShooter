#ifndef _JSON_h_
#define _JSON_h_
#include <string>
#include <map>
#include <vector>


class JSONValue{
public:
    enum TYPE{JSON_ARRAY, JSON_DICT, JSON_STRING};
    
    JSONValue(TYPE,void*, int, std::string);
    ~JSONValue();
    
    
    JSONValue& operator[] (size_t);
    JSONValue& operator[] (std::string);
    std::string operator* ();
    std::string& getString();
    std::map<std::string, JSONValue*>& getMap();
    void setMapValue(std::string, JSONValue*);
    std::vector<JSONValue*>& getVector();
    
    std::string stringise(bool prettyPrint = true);
    std::vector<std::string> keys();
    int length();
    friend void JSONCleanUp(void);
private:
    TYPE type;
    void *value;
    int level;
    std::string jsonString;
};
JSONValue* parseJSON(std::string);
#endif // _JSON_h_
