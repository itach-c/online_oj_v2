#include "compile_run.hpp"
#include "../brpc_etcd/channel.hpp"
#include "../brpc_etcd/etcd.hpp"
#include <brpc/server.h>
#include "../comm/message.pb.h"
DEFINE_bool(mode, false, "debug or relese");
DEFINE_string(file_name, "log.txt", "logname");
DEFINE_int32(level, 0, "level 0-4");

DEFINE_string(etcd_host, "http://127.0.0.1:2379", "服务注册中心地址");
DEFINE_string(base_service, "/service", "服务目录");                     //
DEFINE_string(instance_name, "/CompileRun/instance1", "当前的实例名称"); //
DEFINE_string(compile_server_name,"judge1","编译判题的名称");
DEFINE_string(listen_port, "127.0.0.1:7070", "当前实例的监听地址");      //

class CompileRunServiceImpl : public itach_oj::CompileRunService
{
public:
    CompileRunServiceImpl()
    {
    }
    ~CompileRunServiceImpl()
    {
    }

    Language ConvertProtoLangToRequestLang(itach_oj::CompileRunRequest::Language protoLang)
    {
        switch (protoLang)
        {
        case itach_oj::CompileRunRequest::CPP:
            return Language::CPP;
        case itach_oj::CompileRunRequest::JAVA:
            return Language::JAVA;
        case itach_oj::CompileRunRequest::GO:
            return Language::GO;

        }
          return Language::CPP;
    }

    void Start(::google::protobuf::RpcController *controller,
               const ::itach_oj::CompileRunRequest *request,
               ::itach_oj::CompileRunResponse *response,
               ::google::protobuf::Closure *done) override
    {
        brpc::ClosureGuard done_guard(done);

        Request req;
        req.code = request->code();
        req.input = request->input();
        req.lang = ConvertProtoLangToRequestLang(request->language());
        req.cpu_limit = request->cpu_limit();
        req.mem_limit = request->mem_limit();
        // 需要传实例名称每个逻辑上的机器都会有一个单独的目录;
        PathContext ctx(FLAGS_compile_server_name);
        Response resp;
        CompileAndRun::Run(ctx, req, &resp);

        // 填充响应 protobuf
        response->set_status(static_cast<int>(resp.status));
        response->set_reason(resp.reason);
        response->set_proto_stdout(resp.stdout_data); // 注意这里是 proto_stdout
        response->set_proto_stderr(resp.stderr_data); // proto_stderr
        response->set_pass_num(resp.passnum);
        response->set_total_num(resp.totalnum);
        response->set_last_error_input(resp.last_error_input);
    }
};

int main(int argc, char *argv[])
{

    google::ParseCommandLineFlags(&argc, &argv, true);
    ns_log::init_log(FLAGS_mode, FLAGS_file_name, FLAGS_level);

    // 构造 echo 服务器 向 etcd服务器注册服务

    auto rcli = std::make_shared<Registry>(FLAGS_etcd_host);
    rcli->registry(FLAGS_base_service + FLAGS_instance_name, FLAGS_listen_port);
    // 运行服务

    brpc::Server server;

    // 注册服务
    CompileRunServiceImpl service_impl;
    if (server.AddService(&service_impl, brpc::SERVER_DOESNT_OWN_SERVICE) != 0)
    {
        std::cerr << "Fail to add service" << std::endl;
        return -1;
    }

    // 启动服务，绑定 8 7070 端口
    brpc::ServerOptions options;
    if (server.Start(FLAGS_listen_port.c_str(), &options) != 0)
    {
        std::cerr << "Fail to start server" << std::endl;
        return -1;
    }

    // 等待退出（ctrl-c）
    server.RunUntilAskedToQuit();
    return 0;
}