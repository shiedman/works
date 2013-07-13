#include <node.h>
#include <v8.h>
#include "simplebuff.h"

using namespace v8;

static Persistent<String> length_symbol;

Handle<Value> SimpleBuff::New(const Arguments &args) {
    HandleScope scope;
    size_t length = args[0]->Uint32Value();
    new SimpleBuff(args.This(),length);
    return args.This();
}
SimpleBuff::SimpleBuff(Handle<Object> wrapper, size_t length):ObjectWrap(){
    Wrap(wrapper);
    length_ = length;
    data_ = new char[length_];
    //set indexed properties 
    handle_->SetIndexedPropertiesToExternalArrayData(data_,
            kExternalUnsignedByteArray,
            length_);
    //set length property
    handle_->Set(length_symbol, Integer::NewFromUnsigned(length_));
    //notify v8 that external memory allocated
    V8::AdjustAmountOfExternalAllocatedMemory(sizeof(SimpleBuff) + length_);
    printf("[simplebuff]allocate %lu bytes\n",length_);
}
SimpleBuff::~SimpleBuff(){
    delete [] data_;
    //notify v8 that external memory freed
    V8::AdjustAmountOfExternalAllocatedMemory(
             -static_cast<intptr_t>(sizeof(SimpleBuff) + length_));
    printf("[simplebuff]free %lu bytes\n",length_);
}

void SimpleBuff::Init(Handle<Object> exports){
    HandleScope scope;
    length_symbol = NODE_PSYMBOL("length");
    Local<FunctionTemplate> tpl = FunctionTemplate::New(SimpleBuff::New);
    tpl->SetClassName(String::NewSymbol("SimpleBuff"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("SimpleBuff"), constructor);
}
NODE_MODULE(simplebuff,SimpleBuff::Init)
