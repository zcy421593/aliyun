#include <stdio.h>
#include "ali_api_core.h"
#include "ali_string_utils.h"
#include "ali_ram.h"
#include "json/value.h"
#include "json/reader.h"
using namespace aliyun;
namespace {

void Json2Type(const Json::Value& value, std::string* item);
void Json2Type(const Json::Value& value, GetPolicyVersionPolicyVersionType* item);
void Json2Type(const Json::Value& value, GetPolicyVersionResponseType* item);

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
void Json2Type(const Json::Value& value, GetPolicyVersionPolicyVersionType* item) {
  if(value.isMember("VersionId")) {
    item->version_id = value["VersionId"].asString();
  }
  if(value.isMember("IsDefaultVersion")) {
    item->is_default_version = value["IsDefaultVersion"].asBool();
  }
  if(value.isMember("PolicyDocument")) {
    item->policy_document = value["PolicyDocument"].asString();
  }
  if(value.isMember("CreateDate")) {
    item->create_date = value["CreateDate"].asString();
  }
}
void Json2Type(const Json::Value& value, GetPolicyVersionResponseType* item) {
  if(value.isMember("PolicyVersion")) {
    Json2Type(value["PolicyVersion"], &item->policy_version);
  }
}
}

int Ram::GetPolicyVersion(const GetPolicyVersionRequestType& req,
                      GetPolicyVersionResponseType* response,
                       AliRamErrorInfo* error_info) {
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
  req_rpc->AddRequestQuery("Action","GetPolicyVersion");
  if(!req.policy_type.empty()) {
    req_rpc->AddRequestQuery("PolicyType", req.policy_type);
  }
  if(!req.policy_name.empty()) {
    req_rpc->AddRequestQuery("PolicyName", req.policy_name);
  }
  if(!req.version_id.empty()) {
    req_rpc->AddRequestQuery("VersionId", req.version_id);
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
