bson.v8
=======

deps: 
* [mongodb-c-driver](https://github.com/mongodb/mongo-c-driver)
* [v8](https://github.com/v8/v8)

purposes:

* Convert a `v8::Handle<v8::Value>` variable to a `bson*` type.
* Convert a `bson*` or `char*` means the buffer data of a `bson*` variable to a `v8::Local<v8::Value>`.


## API:

`ParseBSON(const char* buffer)` provide u can pass a bson raw data.

`ParseBSON(const bson* buffer)` provide u can pass a bson object.

`ToBSON(Local<Value> in, bson* out)` provide u can pass 1 in v8::Value and out a bson* pointer.

### Usage:

#### create a bson from `v8::Local<v8::Value>`
```c
Handle<Value> create_bson(const Arguments& args) {
  // create a bson
  bson* b = bson_alloc();
  Utilities::ToBSON(args[0], result);

  // now u can print the bson result
  bson_print(b);
  // the end, remember destroy this bson heap
  bson_destroy(b);
}
```

#### get a `v8::Local<v8::Object>` from a bson in [mongo-c-driver](https://github.com/mongodb/mongo-c-driver)
```c
Local<Object> return_v8_value(const char* buffer) {
  // so easy, right?
  return Utilities::ParseBSON(buffer);
}
```




## LICENSE

MIT, I'm so gald for this could help u :)
