//
// Created by 86137 on 2023/12/24.
//

#ifndef ADAPTABLE_SERVER_URLMAP_HPP
#define ADAPTABLE_SERVER_URLMAP_HPP

#include "map"
#include "string"

#include "Connection.hpp"
#include "Views.hpp"
#include "type_definer.hpp"

using std::map, std::string, std::vector;

using HandlerFunction = ResBuffer(*)(const Connection&);

static map<string, HandlerFunction> url_map = {
        {"music_list", get_music_list},
        {"file", send_file}
};

static vector<string> file_need_url = {
        "file"
};
#endif //ADAPTABLE_SERVER_URLMAP_HPP
