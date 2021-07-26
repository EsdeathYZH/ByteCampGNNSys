#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>

#include "../gen-cpp/GraphServices.h"
using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

using namespace ::ByteGraph;

class GraphServicesHandler : virtual public GraphServicesIf {
   public:
    GraphServicesHandler();

    void sayHello(std::string& _return, const int32_t workerId, const std::string& content);
};