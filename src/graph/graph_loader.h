#ifndef BYTEGRAPH_GRAPH_GRAPH_LOADER_H_
#define BYTEGRAPH_GRAPH_GRAPH_LOADER_H_

#include <string>
class DataSet {
public:
    void load(std::string data_path);

private:
    void load_paper2paper_edges();
    void load_author2paper_edges();
    void load_author2institutions_edges();

    void load_paper_feature();
    void load_paper_label();

    int num_papers;
    int num_papers_local;

    int num_authors;
    int num_authors_local;

    int num_institutions;
    int num_institutions_local;

    int paper_feat_dim = 768;
    int num_classes = 153;
};
#endif