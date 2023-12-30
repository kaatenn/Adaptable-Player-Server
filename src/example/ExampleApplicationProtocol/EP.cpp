//
// Created by 86137 on 2023/12/30.
//

#include "EP.h"
#include "UrlMap.hpp"
#include "json.hpp"

bool EP::process_segment(char *segment, int recv_size) {
    receive_buffer.append(segment, recv_size);
    if (length == 0) {
        if (receive_buffer.size() >= 4) {
            length = *(int *) receive_buffer.data();
            receive_buffer.erase(0, 4);
        } else {
            return false;
        }
    }
    if (url_length == 0) {
        if (receive_buffer.size() >= 1) {
            url_length = *(int *) receive_buffer.data();
            receive_buffer.erase(0, 1);
        } else {
            return false;
        }
    }
    if (param_length == 0) {
        if (receive_buffer.size() >= 1) {
            param_length = *(int *) receive_buffer.data();
            receive_buffer.erase(0, 1);
        } else {
            return false;
        }
    }
    if (url.empty()) {
        if (receive_buffer.size() >= url_length) {
            url = receive_buffer.substr(0, url_length);
            receive_buffer.erase(0, url_length);
        } else {
            return false;
        }
    }
    if (params.empty()) {
        if (receive_buffer.size() >= param_length) {
            params = receive_buffer.substr(0, param_length);
            receive_buffer.erase(0, param_length);
        } else {
            return false;
        }
    }
    file_length = length - url_length - param_length - 6;
    if (file_stream.empty()) {
        if (receive_buffer.size() >= file_length) {
            file_stream = receive_buffer.substr(0, file_length);
            receive_buffer.erase(0, file_length);
            nlohmann::json json;
            json = nlohmann::json::parse(params);
            std::string file_name = json["file_name"];
            std::ofstream fout("music/" + file_name, std::ios::binary);
            fout.write(file_stream.data(), file_stream.size());
            fout.close();
        } else {
            return false;
        }
    }
    return true;
}

void EP::set_res_data_length(int length) {
    this->res_param_length = length;
}

std::string EP::get_params() const {
    return params;
}

void EP::generate_response() {
    std::string res_data = url_map[url](*this);
    int res_file_len = res_data.size() - res_param_length;
    int res_len = 6 + url_length + param_length + res_file_len;
    std::string res;
    res.append((char *) &res_len, 4);
    res.append((char *) &url_length, 1);
    res.append((char *) &param_length, 1);
    res.append(url);
    res.append(res_data); // res_data = res_params + res_file_stream
    this->response = res;
}

std::string EP::get_response() {
    generate_response();
    return response;
}

std::string EP::serialize() {
    std::string result;
    result.append((char *) &length, 4);
    result.append((char *) &url_length, 1);
    result.append((char *) &param_length, 1);
    result.append(url);
    result.append(params);
    result.append(file_stream);
    return result;
}

void EP::error_handler() {
    // This is just a demo, so we do not need to handle errors.
}
