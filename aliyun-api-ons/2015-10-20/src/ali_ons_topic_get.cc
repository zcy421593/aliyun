#include <stdio.h>
#include "ali_api_core.h"
#include "ali_string_utils.h"
#include "ali_ons.h"
#include "json/value.h"
#include "json/reader.h"
using namespace aliyun;
namespace {

void Json2Type(const Json::Value& value, std::string* item);
void Json2Type(const Json::Value& value, OnsTopicGetPublishInfoDoType* item);
void Json2Type(const Json::Value& value, OnsTopicGetResponseType* item);

template<typename T>
class Json2Array {
public:
  Json2Array(const Json::Value& value, std::vector<T>* vec) {
    if(!value.isArray()) {
      return;
    }
    for(int i = 0; i < value.size(); i++) {
      T val;
      Json2Type(value[i], &val);
      vec->push_back(val);
    }
  }
};

void Json2Type(const Json::Value& value, std::string* item) {
  *item = value.asString();
}
void Json2Type(const Json::Value& value, OnsTopicGetPublishInfoDoType* item) {
  if(value.isMember("Id")) {
    item->id = value["Id"].asInt();
  }
  if(value.isMember("ChannelId")) {
    item->channel_id = value["ChannelId"].asInt();
  }
  if(value.isMember("ChannelName")) {
    item->channel_name = value["ChannelName"].asString();
  }
  if(value.isMember("RegionId")) {
    item->region_id = value["RegionId"].asString();
  }
  if(value.isMember("RegionName")) {
    item->region_name = value["RegionName"].asString();
  }
  if(value.isMember("Topic")) {
    item->topic = value["Topic"].asString();
  }
  if(value.isMember("Owner")) {
    item->owner = value["Owner"].asString();
  }
  if(value.isMember("Relation")) {
    item->relation = value["Relation"].asInt();
  }
  if(value.isMember("RelationName")) {
    item->relation_name = value["RelationName"].asString();
  }
  if(value.isMember("Status")) {
    item->status = value["Status"].asInt();
  }
  if(value.isMember("StatusName")) {
    item->status_name = value["StatusName"].asString();
  }
  if(value.isMember("Appkey")) {
    item->appkey = value["Appkey"].asInt();
  }
  if(value.isMember("CreateTime")) {
    item->create_time = value["CreateTime"].asInt();
  }
  if(value.isMember("UpdateTime")) {
    item->update_time = value["UpdateTime"].asInt();
  }
  if(value.isMember("Remark")) {
    item->remark = value["Remark"].asString();
  }
}
void Json2Type(const Json::Value& value, OnsTopicGetResponseType* item) {
  if(value.isMember("HelpUrl")) {
    item->help_url = value["HelpUrl"].asString();
  }
  if(value.isMember("Data") && value["Data"].isMember("PublishInfoDo")) {
    Json2Array<OnsTopicGetPublishInfoDoType>(value["Data"]["PublishInfoDo"], &item->data);
  }
}
}

int Ons::OnsTopicGet(const OnsTopicGetRequestType& req,
                      OnsTopicGetResponseType* response,
                       AliOnsErrorInfo* error_info) {
  std::string str_response;
  int status_code;
  int ret = 0;
  bool parse_success = false;
  AliRpcRequest* req_rpc = new AliRpcRequest(version_,
                         appid_,
                         secret_,
                         "https://" + host_);
  Json::Value val;
  Json::Reader reader;
  req_rpc->AddRequestQuery("Action","OnsTopicGet");
  if(!req.ons_region_id.empty()) {
    req_rpc->AddRequestQuery("OnsRegionId", req.ons_region_id);
  }
  if(!req.ons_platform.empty()) {
    req_rpc->AddRequestQuery("OnsPlatform", req.ons_platform);
  }
  if(!req.prevent_cache.empty()) {
    req_rpc->AddRequestQuery("PreventCache", req.prevent_cache);
  }
  if(!req.topic.empty()) {
    req_rpc->AddRequestQuery("Topic", req.topic);
  }
  if(req_rpc->CommitRequest() != 0) {
     if(error_info) {
       error_info->code = "connect to host failed";
     }
     ret = -1;
     goto out;
  }
  status_code = req_rpc->WaitResponseHeaderComplete();
  req_rpc->ReadResponseBody(str_response);
  if(status_code > 0 && !str_response.empty()){
    parse_success = reader.parse(str_response, val);
  }
  if(!parse_success) {
    if(error_info) {
      error_info->code = "parse response failed";
    }
    ret = -1;
    goto out;
  }
  if(status_code!= 200 && error_info && !parse_success) {
    error_info->request_id = val.isMember("RequestId") ? val["RequestId"].asString(): "";
    error_info->code = val.isMember("Code") ? val["Code"].asString(): "";
    error_info->host_id = val.isMember("HostId") ? val["HostId"].asString(): "";
    error_info->message = val.isMember("Message") ? val["Message"].asString(): "";
  }
  if(status_code== 200 && response) {
    Json2Type(val, response);
  }
  ret = status_code;
out:
  delete req_rpc;
  return ret;
}
