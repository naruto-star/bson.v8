
#ifndef Included_BSON2V8_H
#define Included_BSON2V8_H

#include <node.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "config.h"
#include "mongo/src/mongo.h"

using namespace v8;
using namespace std;

// Handle legacy V8 API
namespace bson2v8 {

  
  class Utilities {

  private:


  public:
    
    /*
     * convert v8string to char* type
     */
    static char* GetCStringFromV8String (Local<v8::Value> value, const char* fallback);

    /*
     * parse and convert the bson to v8 object
     */
    static Local<Object> ParseBSON (const char* buffer);
    static Local<Object> ParseBSON (const bson* buffer);


  };


}


#endif
