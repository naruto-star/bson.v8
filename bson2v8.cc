#include "bson2v8.h"

using namespace v8;
using namespace std;

// Handle legacy V8 API
namespace bson2v8 {


  char* 
  Utilities::GetCStringFromV8String (Local<Value> value, const char* fallback) {
    if (value->IsString()) {
      v8::String::AsciiValue string(value);
      char *str = (char *) malloc(string.length() + 1);
      strcpy(str, *string);
      return str;
    };
    char *str = (char *) malloc(strlen(fallback) + 1);
    strcpy(str, fallback);
    return str;
  }

  Local<Object>
  Utilities::ParseBSON (const char* buffer) {

    Local<Object> object = Object::New();
    bson_iterator iterator[1];
    bson_iterator_from_buffer(iterator, buffer);

    while (bson_iterator_next(iterator)) {
      const int type = bson_iterator_type(iterator);
      const char* name = bson_iterator_key(iterator);
      char oid_hex[25];

      if (type == 0) break;
      switch (type) {
      case BSON_DOUBLE:
        object->Set(String::NewSymbol(name), 
          Number::New(bson_iterator_double(iterator)));
        break;
      case BSON_INT:
        object->Set(String::NewSymbol(name), 
          Integer::New(bson_iterator_int(iterator)));
        break;
      case BSON_LONG:
        object->Set(String::NewSymbol(name), 
          Integer::NewFromUnsigned(bson_iterator_long(iterator)));
        break;
      case BSON_STRING:
      case BSON_SYMBOL:
        object->Set(String::NewSymbol(name), 
          String::New(bson_iterator_string(iterator)));
        break;
      case BSON_OBJECT:
      case BSON_ARRAY:
        object->Set(String::NewSymbol(name), ParseBSON(bson_iterator_value(iterator)));
        break;
      case BSON_BINDATA:
        /* TODO */
        object->Set(String::NewSymbol(name), String::New("null"));
        break;
      case BSON_OID:
        oid_hex[0] = '\0';
        bson_oid_to_string(bson_iterator_oid(iterator), oid_hex);
        object->Set(String::NewSymbol(name), String::New(oid_hex));
        break;
      case BSON_BOOL:
        object->Set(String::NewSymbol(name), 
          Boolean::New(bson_iterator_bool(iterator)));
        break;
      case BSON_DATE:
        object->Set(String::NewSymbol(name), 
          Date::New(bson_iterator_date(iterator)));
        break;
      };

    };

    return object;
  }

  Local<Object> 
  Utilities::ParseBSON (const bson* b) {
    return ParseBSON(b->data);
  }


}





