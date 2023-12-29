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
#include "../../include/type_definer.hpp"

ResBuffer get_music_list([[maybe_unused]] const Connection& connection);
ResBuffer send_file(const Connection& connection);

#endif //ADAPTABLE_SERVER_VIEWS_HPP
