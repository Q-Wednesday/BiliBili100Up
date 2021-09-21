import requests
import json
#获取用户关注的请求，填入uid和页码，页码不超过5
str1="http://api.bilibili.com/x/relation/followings?vmid={0}&pn={1}"
#获取用户投稿的请求，填入uid和页码，页码不超过5
str2="http://api.bilibili.com/x/space/arc/search?mid={0}&ps=2&pn={1}"
up_dict={37090048:{'name':''},8047632:{'name':''},326499679:{'name':''},321173469:{'name':''}}#包含所有up主的dict,键为uid,值为一个dict,储存'name'加上'follow',follow为一个列表，整型，存储关注的uid。type表示up的分区，取上传视频最多
new_list=[]#储存还没有访问的up
old_list=[]#储存已经访问的up
def get_follow(uid):
    '''通过输入的uid返回用户关注信息，注意，返回列表'''
    response=requests.get(str1.format(uid,1))
    str=response.text
    dict=json.loads(str)
    if 'data' not in dict.keys():
        return []
    num=dict['data']['total']#关注总数，最多读取250个
    list=dict['data']['list']
    i=1
    while num>50:
        try:
            num-=50
            i+=1
            response = requests.get(str1.format(uid, i))
            str = response.text
            dict = json.loads(str)
            list += dict['data']['list']
        except Exception:
            pass
    return list
def get_up_videotype(uid):
    '''通过输入uid返回用户投稿信息，返回字典，字典包含各种类型视频上传信息'''
    response=requests.get(str2.format(uid,1))
    str=response.text
    dict=json.loads(str)
    videotype=dict['data']['list']['tlist']

    return videotype


def get_up_info(uid):
    '''通过uid获取up的信息
    此时up的uid和名字都已经在up_dict储存好'''
    follow_list=get_follow(uid)
    video_type=get_up_videotype(uid)
    list=[]
    for follow_up in follow_list:
        list.append(follow_up['mid'])
        if follow_up['mid'] not in up_dict.keys():
            new_list.append(follow_up['mid'])
            up_dict[follow_up['mid']]={'name':follow_up['uname']}#把这个up加入大字典
    up_dict[uid]['follow']=list
    max=0
    type=''
    #print(video_type)
    try:
        for (key,value) in video_type.items():

            if value['count']>max:
                max=value['count']
                type=value['name']
    except Exception:
        pass
    up_dict[uid]['type']=type

def run():
    '''深搜，用关注了百大的账号开始跑，跑出所有百大信息'''
    i=0
    while(len(new_list)):
        uid=new_list.pop()
        old_list.append(uid)
        get_up_info(uid)
        i+=1
        print(i)
        if i%1000==0:
            with open("up.json", 'w') as fp:
                json.dump(up_dict, fp)

def run2():
    '''广搜，去掉官号'''
    i=0

    while(len(new_list)):
        uid=new_list.pop(0)
        old_list.append(uid)
        get_up_info(uid)
        i+=1
        print(i)
        if i%1000==0:
            with open("up_dfs.json", 'w') as fp:
                json.dump(up_dict, fp)

if __name__=="__main__":

    up_dict[562374290]={'name':"test"}
    new_list.append(562374290)
    run2()


    print(up_dict)
