#include "userinfo.h"

SearchInfo::SearchInfo(int uid, int sex, QString name, QString nick, QString desc, QString icon)
    :uid(uid),sex(sex),name(name),nick(nick),desc(desc),icon(icon)
{

}

void FriendInfo::appendmsg(std::vector<std::shared_ptr<TextChatData>> msgs)
{
    for(auto& msg:msgs){

        _chat_msgs.push_back(msg);

    }
}
