#include <sys/mman.h> //for mmap
#include <string.h>
#include <node.h>
#include <v8.h>
#include "simplebuff.h"

using namespace v8;

static Persistent<String> length_symbol;
static Persistent<String> created_symbol;
static Persistent<String> destroyed_symbol;
size_t SimpleBuff::created = 0;
size_t SimpleBuff::destroyed = 0;

Handle<Value> SimpleBuff::New(const Arguments &args) {
    HandleScope scope;
    size_t length = args[0]->Uint32Value();
    new SimpleBuff(args.This(),length);
    return args.This();
}
SimpleBuff::SimpleBuff(Handle<Object> wrapper, size_t length):ObjectWrap(){
    Wrap(wrapper);
    length_ = length;
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_ANON | MAP_PRIVATE;
    data_ = static_cast<char*>(mmap(NULL,length_,prot,flags,-1,0));
    //set data_ to 0, force memory allocated
    memset(data_,0,length_);
    //set indexed properties 
    handle_->SetIndexedPropertiesToExternalArrayData(data_,
            kExternalUnsignedByteArray,
            length_);
    //set length property
    handle_->Set(length_symbol, Integer::NewFromUnsigned(length_));
    //notify v8 that external memory allocated
    V8::AdjustAmountOfExternalAllocatedMemory(sizeof(SimpleBuff) + length_);
    created++;
}
SimpleBuff::~SimpleBuff(){
    munmap(data_,length_);
    //notify v8 that external memory freed
    V8::AdjustAmountOfExternalAllocatedMemory(
             -static_cast<intptr_t>(sizeof(SimpleBuff) + length_));
    destroyed++;
}

Handle<Value> SimpleBuff::Info(const Arguments &args) {
  HandleScope scope;
  Local<Object> info = Object::New();
  info->Set(created_symbol, Number::New(SimpleBuff::created));
  info->Set(destroyed_symbol, Number::New(SimpleBuff::destroyed));
  return scope.Close(info);
}
void SimpleBuff::Init(Handle<Object> exports){
    HandleScope scope;
    length_symbol = NODE_PSYMBOL("length");
    created_symbol = NODE_PSYMBOL("created");
    destroyed_symbol = NODE_PSYMBOL("destroyed");
    Local<FunctionTemplate> tpl = FunctionTemplate::New(SimpleBuff::New);
    tpl->SetClassName(String::NewSymbol("SimpleBuff"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
    NODE_SET_METHOD(constructor, "info", SimpleBuff::Info);
    exports->Set(String::NewSymbol("SimpleBuff"), constructor);
}
NODE_MODULE(simplebuff, SimpleBuff::Init)
