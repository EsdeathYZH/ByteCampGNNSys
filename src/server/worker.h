#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>

#include "gen-cpp/GraphServices.h"
#include "engine/graph_engine.h"

namespace at = ::apache::thrift;
namespace atp = ::apache::thrift::protocol;
namespace att = ::apache::thrift::transport;

class GraphServicesHandler : virtual public ByteGraph::GraphServicesIf {
   public:
    GraphServicesHandler(std::shared_ptr<Byte::GraphEngine> engine);

    void sayHello(std::string& _return, const int32_t workerId, const std::string& content);

    void getFullGraphInfo(ByteGraph::GraphInfo& _return);

    void SampleBatchNodes(ByteGraph::BatchNodes& _return, 
                          const ByteGraph::NodeType type, 
                          const int32_t batch_size,
                          const ByteGraph::SampleStrategy::type strategy);

    void GetNodeFeature(std::vector<ByteGraph::NodeFeature>& _return, 
                        const std::vector<ByteGraph::NodeId>& nodes, 
                        const ByteGraph::FeatureType feat_type);

    void GetNodeNeighbors(ByteGraph::Neighbor& _return, 
                          const ByteGraph::NodeId node_id, 
                          const ByteGraph::EdgeType edge_type);

    void GetNeighborsWithFeature(std::vector<ByteGraph::IDFeaturePair>& _return, 
                                 const ByteGraph::NodeId node_id,
                                 const ByteGraph::EdgeType edge_type, 
                                 const ByteGraph::FeatureType feat_type);

    void SampleNeighbor(std::vector<ByteGraph::IDNeighborPair>& _return, 
                        const int32_t batch_size, 
                        const ByteGraph::NodeType node_type,
                        const ByteGraph::EdgeType edge_type, 
                        const int32_t sample_num);

    void RandomWalk(std::vector<ByteGraph::NodeId>& _return,
                    const int32_t batch_size,
                    const int32_t walk_len);

   private:
    std::shared_ptr<Byte::GraphEngine> engine_;
};
