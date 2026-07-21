#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "message.pb.h"
#include "data.h"
class ChatServiceImpl final : public message::ChatService::Service
{
public:
    ChatServiceImpl();
    ~ChatServiceImpl();

    bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
    virtual ::grpc::Status NotifyAddFriend(::grpc::ServerContext* context, const ::message::AddFriendReq* request, ::message::AddFriendRsp* response) override;
    virtual ::grpc::Status ReplyAddFriend(::grpc::ServerContext* context, const ::message::ReplyFriendReq* request, ::message::ReplyFriendRsp* response) override;
    virtual ::grpc::Status SendChatMsg(::grpc::ServerContext* context, const ::message::SendChatMsgReq* request, ::message::SendChatMsgRsp* response) override;
    virtual ::grpc::Status NotifyAuthFriend(::grpc::ServerContext* context, const ::message::AuthFriendReq* request, ::message::AuthFriendRsp* response) override;
    virtual ::grpc::Status TextChatMsg(::grpc::ServerContext* context, const ::message::TextChatMsgReq* request, ::message::TextChatMsgRsp* response) override;

private:


};

