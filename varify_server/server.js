const grpc = require("@grpc/grpc-js");
const message_proto = require("./proto");
const const_moudle = require("./const");
const {v4:uuidv4} = require("uuid");
const email_moudle = require("./email")
const redis_module = require("./redis")

async function GetVarifyCode(call,callback){
    console.log("email is "+call.request.email);
    try{
        //const uniqueId = uuidv4();
        //console.log("uniqueId is "+uniqueId);
        let email_key = const_moudle.code_prefix + call.request.email;
        let query_res = await redis_module.GetRedis(email_key);
        let uniqueId = query_res;
        if(query_res == null){
            uniqueId = uuidv4();
            if(uniqueId.length > 4){
                uniqueId = uniqueId.substring(0,4);
            }
            let res = await redis_module.SetRedisExpire(email_key,uniqueId,180);
            console.log("user: " , email_key ," varify: ",uniqueId);
            if(res == null){
                callback(null,{email: call.request.email, error: const_moudle.Errors.RedisErr});
                return;
            }
            
        }

        let text_str = "您的验证码为："+uniqueId+" 请在三分钟内完成注册";
        let mailOptions = {
            from:"15577383870@163.com",
            to:call.request.email,
            subject:"azlp发来的验证码",
            text:text_str
        };
        let send_res = await email_moudle.SendMail(mailOptions);
        //console.log("send res is "+send_res);
        callback(null,{email:call.request.email,error:const_moudle.Errors.Success
        });
    }catch(error){
        console.log("error is "+error);
        callback(null,{email:call.request.email,error:const_moudle.Errors.Exception
        });
    }

}

function main(){
    var server = new grpc.Server();
    server.addService(message_proto.VarifyService.service,{GetVarifyCode:GetVarifyCode});
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(), () => {
        server.start()
        console.log('grpc server started')        
    });
}

main()
