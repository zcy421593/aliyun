#include <stdio.h>
#include "ali_api_core.h"
#include "ali_string_utils.h"
#include "ali_ubsms.h"
#include "json/value.h"
#include "json/reader.h"
using namespace aliyun;
namespace {

void Json2Type(const Json::Value& value, std::string* item);
void Json2Type(const Json::Value& value, NotifyUserBusinessCommandResponseType* item);

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
void Json2Type(const Json::Value& value, NotifyUserBusinessCommandResponseType* item) {
  if(value.isMember("Success")) {
    item->success = value["Success"].asBool();
  }
}
}

int Ubsms::NotifyUserBusinessCommand(const NotifyUserBusinessCommandRequestType& req,
                      NotifyUserBusinessCommandResponseType* response,
                       AliUbsmsErrorInfo* error_info) {
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
  req_rpc->AddRequestQuery("Action","NotifyUserBusinessCommand");
  if(!req.uid.empty()) {
    req_rpc->AddRequestQuery("Uid", req.uid);
  }
  if(!req.service_code.empty()) {
    req_rpc->AddRequestQuery("ServiceCode", req.service_code);
  }
  if(!req.cmd.empty()) {
    req_rpc->AddRequestQuery("Cmd", req.cmd);
  }
  if(!req.region.empty()) {
    req_rpc->AddRequestQuery("Region", req.region);
  }
  if(!req.instance_id.empty()) {
    req_rpc->AddRequestQuery("InstanceId", req.instance_id);
  }
  if(!req.client_token.empty()) {
    req_rpc->AddRequestQuery("ClientToken", req.client_token);
  }
  if(!req.password.empty()) {
    req_rpc->AddRequestQuery("Password", req.password);
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
  if(status_code!= 200 && error_info) {
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
