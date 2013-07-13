#ifndef SIMPLE_BUFF_H
#define SIMPLE_BUFF_H

#include <node.h>
#include <v8.h>

class SimpleBuff:public node::ObjectWrap{
    public:
        static void Init(v8::Handle<v8::Object> exports);
        SimpleBuff(v8::Handle<v8::Object> wrapper, size_t length);
    private:
        ~SimpleBuff();
        static v8::Handle<v8::Value> New(const v8::Arguments& args);
        size_t length_;
        char* data_;
};
#endif
