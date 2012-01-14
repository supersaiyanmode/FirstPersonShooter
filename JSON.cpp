#include "JSON.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <stack>
#include <memory>

void JSONCleanUp(void);
namespace {
    std::vector<JSONValue*> cleanUpObjects;
    JSONValue* newJSONValue(JSONValue::TYPE t, void* val, int level,std::string js){
        JSONValue* jv = new JSONValue(t,val,level,js);
        cleanUpObjects.push_back(jv);
        return jv;
    }
    
    int dummy = atexit(::JSONCleanUp);

    std::string getMatchingSubString(std::string str,int rawString=0){
        char start = str[0];
        std::string acceptable = "\"[{";
        if (acceptable.find(str[0]) == std::string::npos)
            throw ("Invalid Token: " + str).c_str();
        if (start == '\"'){
            std::string t = str.substr(1);
            size_t pos = t.find('\"');
            if (pos == std::string::npos){
                throw "Invalid Literal: Unending string literal.";
            }
            return str.substr(0, pos+2);
        }
        if (rawString && std::isalpha(start)){
            std::string ret = "";
            for (int i=0; std::isalpha(str[i]); i++){
                ret += str[i];
            }
            return ret;
        }
        std::stack<char> parseStack;
        for (int i=0,length=str.length(); i<length; i++){
            if (str[i] == '[')
                parseStack.push(str[i]);
            else if (str[i] == '{')
                parseStack.push(str[i]);
            else if (str[i] == ']'){
                if (parseStack.top() != '[')
                    throw "Invalid Literal: ] character at wrong depth.";
                parseStack.pop();
                if (parseStack.empty())
                    return str.substr(0,i+1);
            }
            else if (str[i] == '}'){
                if (parseStack.top() != '{')
                    throw "Invalid Literal: } character at wrong depth.";
                parseStack.pop();
                if (parseStack.empty())
                    return str.substr(0,i+1);
            }
        }
        throw "Unending characters";
    }

    JSONValue* decodeStruct(std::string,int);
    JSONValue* _parseJSON(std::string,int);

    JSONValue* decodeString(std::string str, int level){
        //std::cout<<"Inside decode string: "<<str<<std::endl;
        if (str[0] != '\"' && str[str.length()-1] != '\"')
            throw ("Malformed String. Unmatching Quotes"+str).c_str();
        std::string str1 = str.substr(1,str.length()-2);
        JSONValue *jo = newJSONValue(JSONValue::JSON_STRING,new std::string(str1),level,str);
        return jo;
    }
    JSONValue* decodeArray(std::string str,int level){
        //std::cout<<"Inside decode Array: "<<str<<std::endl;
        int length = str.length()-2;
        str = str.substr(1,length);
        std::string cur;
        std::vector<JSONValue*>* v = new std::vector<JSONValue*>();
        JSONValue *jo = newJSONValue(JSONValue::JSON_ARRAY, v, level, str);
        
        for (int i=0; i<length; i++){ //removed the square brackets!
            cur=getMatchingSubString(str.substr(i));
            //std::cout<<"Pushing: "<<cur<<std::endl;
            v->push_back(_parseJSON(cur,level+1));
            i += cur.length();
            if (i==length)
                return jo;
            if (str[i] != ',')
                throw "Invalid Array Declaration. Expected , character.";
        }
        throw "Invalid Array Declaration.";
    }
    JSONValue* decodeStruct(std::string str,int level){
        //std::cout<<"Inside decode struct: "<<str<<std::endl;
        int length = str.length()-2;
        str = str.substr(1,length);
        std::string cur,fieldName;
        
        std::map<std::string, JSONValue*>* map = new std::map<std::string, JSONValue*>();
        JSONValue* jo = newJSONValue(JSONValue::JSON_DICT, map, level, str);
        
        for (int i=0; i<length; i++){ //removed the square brackets!
            fieldName = getMatchingSubString(str.substr(i));
            if (fieldName[0] != '\"' || fieldName[fieldName.length()-1] != '\"')
                throw "Invalid Key Name.";
            i += fieldName.length();

            fieldName = fieldName.substr(1,fieldName.length()-2);
            if (str[i] != ':')
                throw "Invalid Object Declaration. Expected : character.";
            i++;
            cur=getMatchingSubString(str.substr(i));
            i += cur.length();
            //std::cout<<"Pushing: "<<fieldName<<std::endl;
            (*map)[fieldName] = _parseJSON(cur,level+1);
            if (i==length)
                return jo;
            if (str[i] != ',')
                throw "Invalid Object Declaration. Expected , character.";
        }
        throw "Invalid Object Declaration.";
    }

    JSONValue* _parseJSON(std::string str,int level){
        switch(str[0]){
            case '[':
                if (str[str.length()-1] != ']'){
                    std::cout<<"In string: "<<str<<std::endl;
                    throw "Invalid Literal: Unmatched [ character";
                }
                return decodeArray(str,level+1);
            case '{':
                if (str[str.length()-1] != '}'){
                    std::cout<<"In string: "<<str<<std::endl;
                    std::cout<<"Damn. This: "<<str[str.length()-1]<<std::endl;
                    throw "Invalid Literal: Unmatched { character";
                }
                return decodeStruct(str,level+1);
            default:
                return decodeString(str,level+1);
        }
    }

}

void JSONCleanUp(void){
    for (std::vector<JSONValue*>::iterator it = cleanUpObjects.begin();
            it != cleanUpObjects.end(); it++){
        if ((*it)->value){
            switch((*it)->type){
                case JSONValue::JSON_STRING:
                    delete (std::string*)(*it)->value;
                    break;
                case JSONValue::JSON_ARRAY:
                    delete (std::vector<JSONValue*>*)(*it)->value;
                    break;
                case JSONValue::JSON_DICT:
                    delete (std::map<std::string,JSONValue*>*)(*it)->value;
                    break;
            }
        }
        delete *it;
    }
    std::cout<<"Cleaned up "<<cleanUpObjects.size()<<" objects!\n";
}

JSONValue* parseJSON(std::string str){
    //process the string..
    std::string toParse;
    bool withinQuotes = false;
    for (int i=0,l = str.length(); i<l; i++){
        if (str[i]>32 || withinQuotes)
            toParse += str[i];
        if (str[i] == '\"')
            withinQuotes = !withinQuotes;
    }
    JSONValue* j;
    try{
        j = _parseJSON(toParse,1);
    }catch (const char* x){
        std::cout<<"Exception: "<<x<<std::endl;
        std::cout<<"Exiting."<<std::endl;
        exit(1);
    }
    return j;
}

JSONValue::JSONValue(TYPE t, void* v, int l,std::string str):type(t),value(v),level(l),jsonString(str){
    
}
JSONValue::~JSONValue(){
}


std::string JSONValue::stringise(bool prettyPrint){
    //std::cout<<"Attempting to string: "<<jsonString<<std::endl<<std::endl;
    std::string str;
    if (type == JSON_STRING){
        str += "\"" + *(std::string*)value + "\"";
    }else if (type == JSON_ARRAY){
        str += prettyPrint? "[\n":"[";
        std::vector<JSONValue*>* vector = (std::vector<JSONValue*>*)value;
        for (std::vector<JSONValue*>::iterator it=vector->begin(); it != vector->end(); it++){
            if (prettyPrint)
                str += std::string(level+1,' ');
            str += (*it)->stringise(prettyPrint);
            str += ((++it)!=vector->end()? ",":"");
            str += (prettyPrint? "\n" : "");
            --it;
        }
        if (prettyPrint)
            str += std::string(level,' ');
        str += "]";
    }else if (type == JSON_DICT){
        str += prettyPrint? "{\n": "{";
        std::map<std::string, JSONValue*>* map = (std::map<std::string, JSONValue*>*)value;
        for (std::map<std::string, JSONValue*>::iterator it=map->begin(); it!=map->end(); it++){
            if (prettyPrint)
                str += std::string(level+1,' ');
            str += "\"" + it->first + "\": ";
            str += (*(it->second)).stringise(prettyPrint);
            str += ((++it)!=map->end()? ",":"");
            str += (prettyPrint? "\n" : "");
            --it;
        }
        if (prettyPrint)
            str += std::string(level,' ');
        str += "}";
    }
    return str;
}


JSONValue& JSONValue::operator[] (size_t index){
    std::vector<JSONValue*>& vector = getVector();
    if (index < 0 || index > vector.size())
        throw "Bad index";
    return *((*(std::vector<JSONValue*>*)value)[index]);
}
JSONValue& JSONValue::operator[] (std::string key){
    std::map<std::string, JSONValue*>& map = getMap();
    if (map.find(key) == map.end())
        throw "Key not in dictionary";
    return *(map[key]);
}
std::string JSONValue::operator*(){
    return getString();
}
std::string& JSONValue::getString(){
    if (type != JSONValue::JSON_STRING)
        throw "Not a string.";
    return (*(std::string*)value);
}

void JSONValue::setMapValue(std::string key, JSONValue* j){
    getMap()[key] = j;
}

std::map<std::string, JSONValue*>& JSONValue::getMap(){
    if (type != JSONValue::JSON_DICT)
        throw "Not a dictionary.";
    return (*(std::map<std::string, JSONValue*>*)value);
}
std::vector<JSONValue*>& JSONValue::getVector(){
    if (type != JSONValue::JSON_ARRAY)
        throw "Not an array.";
    return (*(std::vector<JSONValue*>*)value);
}


std::vector<std::string> JSONValue::keys(){
    if (type != JSONValue::JSON_DICT)
        throw "Not a Dictionary";
    std::vector<std::string> ret;
    std::map<std::string, JSONValue*>* map = (std::map<std::string, JSONValue*>*)value;
    for (std::map<std::string, JSONValue*>::iterator it = map->begin(); it!= map->end(); it++){
        ret.push_back(it->first);
    }
    return ret;
}
int JSONValue::length(){
    if (type != JSONValue::JSON_ARRAY)
        throw "Not an Array";
    return ((std::vector<std::string>*)value)->size();
}
#if 0
int main(){
    std::ifstream file("world.json");
    std::stringstream ss;
    ss<<file.rdbuf();
    file.close();
    
    JSONValue& jv = *parseJSON(ss.str());
    (jv["objects"][0].getMap())["oid"] = parseJSON("\"" "hellow"  "\"");
    //(jv["textures"].getMap())["temp"] = parseJSON("\"HELLOW\"");
    std::cout<<jv.stringise(true)<<std::endl;
    std::cout<<*jv["objects"][0]["oid"]<<std::endl;
    
    //std::cout<<"Test: "<<jv["objects"][1]["transformations"][0]["type"].get()<<std::endl;
}
#endif
