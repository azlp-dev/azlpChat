const config_module = require("./config")
const redis_module = require("ioredis")

const redis_cli = new redis_module({
    host: config_module.redis_host,
    port: config_module.redis_port,
    password: config_module.redis_passwd
});

redis_cli.on("error",function(err){
    console.log("redis connect error");
    redis_cli.quit();
})

async function GetRedis(key){
    try{
        const result = await redis_cli.get(key);
        if(result === null){
            console.log("result <" ,result ,"> ,this key can not be found ");
            return null;
        }
        console.log("result <" ,result ,"> ,get key success ");
        return result;
    }catch(error){
        console.log("Get redis key error: ",error);
        return null;
    }
}

async function QueryRedis(key){
    try{
        const result = await redis_cli.exists(key);
        if(result === 0){
            console.log("result <" ,result ,"> ,this key can not be found ");
            return null;
        }
        console.log("result <" ,result ,"> ,query key success ");
        return result;
    }catch(error){
        console.log("Query redis key error: ",error);
        return null;
    }
}

async function SetRedisExpire(key,value,exptime){
    try{
        await redis_cli.set(key,value);
        await redis_cli.expire(key,exptime);
        return true;
    }catch(error){
        console.log("Set expire key error");
        return true;
    }

}

function Quit(){
    redis_cli.quit();
    //return true;
}

module.exports = {GetRedis,QueryRedis,SetRedisExpire,Quit};