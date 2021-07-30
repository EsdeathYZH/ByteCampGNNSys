#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>

#include "gen-cpp/GraphServices.h"
#include "engine/graph_engine.h"

using std::string;

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

using namespace ::ByteGraph;

class GraphServicesHandler : virtual public GraphServicesIf {
   public:
    GraphServicesHandler(std::shared_ptr<Byte::GraphEngine> engine);

    void sayHello(std::string& _return, const int32_t workerId, const std::string& content);

    void getFullGraphInfo(GraphInfo& _return);

    void SampleBatchNodes(BatchNodes& _return, const NodeType type, const int32_t batch_size,
                          const SampleStrategy::type strategy);

    void GetNodeFeature(NodeFeature& _return, const NodeId node_id, const std::vector<FeatureType>& feature_id);

    void GetNodeNeighbors(Neighbor& _return, const NodeId node_id, const EdgeType edge_type);

    void GetNeighborsWithFeature(std::vector<IDFeaturePair>& _return, const NodeId node_id,
                                 const EdgeType neighbor_type, const std::vector<FeatureType>& feature_types);

    void SampleNeighbor(std::vector<IDNeighborPair>& _return, const int32_t batch_size, const NodeType node_type,
                        const NodeType neighbor_type, const int32_t sample_num);

    void RandomWalk(std::vector<NodeId>& _return, const int32_t batch_size, const int32_t walk_len);

   private:
    std::shared_ptr<Byte::GraphEngine> engine_;
};
