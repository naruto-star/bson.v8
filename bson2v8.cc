#include "bson2v8.h"

#define V8Symbol v8::String::NewSymbol

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
  
  void
  Utilities::ToBSON (Local<Value> in, bson* out) {
    
    Local<Object> global = Context::GetCurrent()->Global();
    Local<Object> JSObject = Local<Object>::Cast(global->Get(V8Symbol("Object")));
    Local<Function> ObjKeysFunc = Local<Function>::Cast(JSObject->Get(V8Symbol("keys")));

    int ownsData = out->ownsData;
    uint32_t keys_len = 0;
    Local<Value> argv[1] = {in};
    Local<Array> keys = Local<Array>::Cast(ObjKeysFunc->Call(global, 1, argv));

    if (1 != ownsData) bson_init(out);    
    if (in->IsArray()) {
      keys_len = Local<Array>::Cast(in)->Length();
    } else if (in->IsObject()) {
      Local<Value> argv[1] = { in };
      keys_len = keys->Length();
    }

    for (uint32_t i=0; i<keys_len; i++) {
      char* key; Local<Value> value;
      if (in->IsArray()) {
        std::ostringstream oss; oss << i;
        key = const_cast<char*>(oss.str().c_str());
        value = Local<Object>::Cast(in)->Get(i);
      }
      else if (in->IsObject()) {
        key = GetCStringFromV8String(keys->Get(i), "");
        value = Local<Object>::Cast(in)->Get(V8Symbol(key));
      };
      if (value->IsString() || value->IsStringObject()) {
        bson_append_string(out, key, GetCStringFromV8String(value, ""));
      }
      else if (value->IsNumber() || value->IsNumberObject()) {
        bson_append_int(out, key, value->Uint32Value());
      }
      else if (value->IsTrue()) {
        bson_append_bool(out, key, true);
      }
      else if (value->IsFalse()) {
        bson_append_bool(out, key, false);
      }
      else if (value->IsDate()) {
        bson_append_date(out, key, Local<Date>::Cast(value)->NumberValue());
      }
      else if (value->IsArray()) {
        bson_append_start_array(out, key);
        ToBSON(value, out);
        bson_append_finish_array(out);
      }
      else if (value->IsObject()) {
        bson_append_start_object(out, key);
        ToBSON(value, out);
        bson_append_finish_object(out);
      };
    };
    // finish this bson
    if (1 != ownsData) bson_finish(out);
  }


}





