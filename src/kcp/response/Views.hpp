//
// Created by 86137 on 2023/12/24.
//

#ifndef ADAPTABLE_SERVER_VIEWS_HPP
#define ADAPTABLE_SERVER_VIEWS_HPP

#include "Connection.hpp"
#include "string"
#include "vector"
#include "sstream"
#include "fstream"
#include "type_definer.hpp"

ResBuffer get_music_list([[maybe_unused]] const Connection& connection) {
    vector<string> music_list;

    for (const auto& entry : std::filesystem::directory_iterator("music")) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            music_list.push_back(entry.path().filename().string());
        }
    }

    Connection response("music_list", music_list);
    vector<char> buffer = response.to_buffer();
    ResBuffer result{buffer};
    return result;
}

ResBuffer send_file(const Connection& connection) {
    vector<string> url = connection.get_params();
    assert(url.size() == 1);
    // we should only send one file at one time
    string file_path = "./music/" + url[0]/*"test.hpp"*/;
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "file not found" << std::endl;
        Connection error_response(connection.get_url(), {"failed"});
        file.close();
        ResBuffer error_result{error_response.to_buffer()};
        return error_result;
    }
    auto file_length_i = std::filesystem::file_size(file_path);
    string file_length_str = std::to_string(file_length_i);
    Connection response(connection.get_url(), {"file", url[0]/*"test.hpp"*/, file_length_str});
    vector<char> file_buffer(std::istreambuf_iterator<char>(file), {}); // read the whole file
    file.close();

    ResBuffer result{response.to_buffer(), file_buffer};

    return result;
}

#endif //ADAPTABLE_SERVER_VIEWS_HPP
