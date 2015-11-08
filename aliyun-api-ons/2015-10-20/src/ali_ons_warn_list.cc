#include <stdio.h>
#include "ali_api_core.h"
#include "ali_string_utils.h"
#include "ali_ons.h"
#include "json/value.h"
#include "json/reader.h"
using namespace aliyun;
namespace {

void Json2Type(const Json::Value& value, std::string* item);
void Json2Type(const Json::Value& value, OnsWarnListYunContactType* item);
void Json2Type(const Json::Value& value, OnsWarnListWarnViewDOType* item);
void Json2Type(const Json::Value& value, OnsWarnListResponseType* item);

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
void Json2Type(const Json::Value& value, OnsWarnListYunContactType* item) {
  if(value.isMember("Name")) {
    item->name = value["Name"].asString();
  }
  if(value.isMember("Value")) {
    item->value = value["Value"].asString();
  }
  if(value.isMember("Type")) {
    item->type = value["Type"].asString();
  }
  if(value.isMember("Params")) {
    item->params = value["Params"].asString();
  }
  if(value.isMember("Ari")) {
    item->ari = value["Ari"].asString();
  }
}
void Json2Type(const Json::Value& value, OnsWarnListWarnViewDOType* item) {
  if(value.isMember("AliyunPK")) {
    item->aliyun_pk = value["AliyunPK"].asString();
  }
  if(value.isMember("ConsumerID")) {
    item->consumer_id = value["ConsumerID"].asString();
  }
  if(value.isMember("Topic")) {
    item->topic = value["Topic"].asString();
  }
  if(value.isMember("Threshold")) {
    item->threshold = value["Threshold"].asString();
  }
  if(value.isMember("Status")) {
    item->status = value["Status"].asBool();
  }
  if(value.isMember("contacts") && value["contacts"].isMember("YunContact")) {
    Json2Array<OnsWarnListYunContactType>(value["contacts"]["YunContact"], &item->contacts);
  }
}
void Json2Type(const Json::Value& value, OnsWarnListResponseType* item) {
  if(value.isMember("HelpUrl")) {
    item->help_url = value["HelpUrl"].asString();
  }
  if(value.isMember("Data") && value["Data"].isMember("WarnViewDO")) {
    Json2Array<OnsWarnListWarnViewDOType>(value["Data"]["WarnViewDO"], &item->data);
  }
}
}

int Ons::OnsWarnList(const OnsWarnListRequestType& req,
                      OnsWarnListResponseType* response,
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
  req_rpc->AddRequestQuery("Action","OnsWarnList");
  if(!req.ons_region_id.empty()) {
    req_rpc->AddRequestQuery("OnsRegionId", req.ons_region_id);
  }
  if(!req.ons_platform.empty()) {
    req_rpc->AddRequestQuery("OnsPlatform", req.ons_platform);
  }
  if(!req.prevent_cache.empty()) {
    req_rpc->AddRequestQuery("PreventCache", req.prevent_cache);
  }
  if(!req.consumer_id.empty()) {
    req_rpc->AddRequestQuery("ConsumerId", req.consumer_id);
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
