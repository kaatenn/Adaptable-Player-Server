#ifndef ADAPTABLE_SERVER_EP_H
#define ADAPTABLE_SERVER_EP_H

#include "api/ApplicationProtocolBase.h"
#include "fstream"
#include "vector"

using kaatenn::ApplicationProtocolBase;
class EP: public ApplicationProtocolBase {
public:
    std::string serialize() override;
    bool process_segment(char* segment, int recv_size) override;
    void error_handler() override;
    std::string get_response() override;
    std::string get_params() const;
    void set_res_data_length(int length);

private:
    // Protocol data
    int length = 0; // 4B for length
    int url_length = 0; // 1B for url length
    int param_length = 0; // 1B for data length(params but no url and file stream)
    int file_length = 0;
    int res_param_length = 0;
    std::string receive_buffer;
    std::string url;
    std::string params;
    std::string file_stream;
    std::ifstream fin;

    void generate_response();
};


#endif //ADAPTABLE_SERVER_EP_H
