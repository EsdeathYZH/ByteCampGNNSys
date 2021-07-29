#include "client_base.h"

#include <glog/logging.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

using std::string;
using namespace ::ByteGraph;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

::std::shared_ptr<att::TSocket> socket_;
::std::shared_ptr<att::TTransport> transport_;
::std::shared_ptr<atp::TProtocol> protocol_;
::std::shared_ptr<ByteGraph::GraphServicesClient> client_;

int main(int argc, char **argv) {
  // Initialize Google’s logging library.
  google::InitGoogleLogging(argv[0]);
  google::SetLogDestination(google::INFO, "/tmp/log/INFO_");
  // google::SetStderrLogging(google::INFO);
  FLAGS_logtostderr = 1;
  string ret;

  // ...
  LOG(INFO) << "Found " << 1 << " cookies";
  FLAGS_logtostderr = 0;
  LOG(INFO) << "Found " << 2 << " cookies";
  FLAGS_logtostderr = 1;
  LOG(INFO) << "Found " << 3 << " cookies";

  string peerIP = "127.0.0.1";
  int port = 9090;
  socket_ = std::make_shared<att::TSocket>(peerIP, port);
  transport_ = ::std::shared_ptr<att::TTransport>(new att::TFramedTransport(socket_));
  protocol_ = ::std::shared_ptr<atp::TProtocol>(new atp::TBinaryProtocol(transport_));
  client_ = std::make_shared<ByteGraph::GraphServicesClient>(protocol_);
  transport_->open();
  LOG(INFO) << "connected to server " << peerIP;

  client_->sayHello(ret, 1234, "thrift_test");
  LOG(INFO) << ret;
  NodeFeature r;
  client_->GetNodeFeature(r, 14, {22});
  LOG(INFO) << "GetNodeFeature";
  for (auto i : r) {
    LOG(INFO) << i;
  }

  return 0;
}
