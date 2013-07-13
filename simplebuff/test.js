var SimpleBuff = require('./index');

console.log('rss: %dM',(process.memoryUsage().rss/1024/1024)); // < 10MB
var h = {};
for(var i=0;i<10000;i++){
      h['key_' + i] = new SimpleBuff(1024*16);//16k
}
console.log('rss: %dM',(process.memoryUsage().rss/1024/1024)); // > 100MB
console.log('SimpleBuff:',SimpleBuff.info());

h = {};
for(var i=0;i<10000;i++){
      h['key' + i] = new SimpleBuff(1024*16);//16k
}
console.log('rss: %dM',(process.memoryUsage().rss/1024/1024)); // > 200MB
h = null;
gc();
console.log('------after gc------');
console.log('SimpleBuff:',SimpleBuff.info());

setInterval(function(){
        console.log('rss: %dM',(process.memoryUsage().rss/1024/1024)); // > 200MB
}, 2000);
