#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TSocket.h>

#include <iostream>

#include "gen-cpp/GraphServices.h"
#include "glog/logging.h"

using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

using namespace ::ByteGraph;

::std::shared_ptr<att::TSocket> socket_;
::std::shared_ptr<att::TTransport> transport_;
::std::shared_ptr<atp::TProtocol> protocol_;
::std::shared_ptr<ByteGraph::GraphServicesClient> client_;