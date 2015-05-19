#ifdef UNIT_TEST
#include "btree_dir.h"
#include "utils.h"

#include <catch.hpp>
#include <format.h>

#include <vector>
#include <string>
#include <random>
#include <algorithm>

#include <unistd.h>
#include <fcntl.h>

TEST_CASE("Test BtreeDirectory")
{
    const size_t NUM_ENTRIES = 100;
    std::vector<std::string> names;
    for (size_t i = 0; i < NUM_ENTRIES; ++i)
    {
        names.emplace_back(fmt::format("file{}.abc", i));
    }
    std::mt19937 engine{std::random_device{}()};
    std::shuffle(names.begin(), names.end(), engine);
    std::vector<securefs::id_type> ids(NUM_ENTRIES);
    for (auto&& d : ids)
        securefs::generate_random(d.data(), d.size());

    securefs::key_type null_key{};
    securefs::id_type null_id{};

    char tmp1[] = "/tmp/securefs.btree1.XXXXXX";
    char tmp2[] = "/tmp/securefs.btree2.XXXXXX";

    securefs::BtreeDirectory dir(::mkstemp(tmp1), ::mkstemp(tmp2), null_key, null_id, true);
    for (size_t i = 0; i < NUM_ENTRIES; ++i)
    {
        REQUIRE(dir.add_entry(names[i], ids[i], securefs::FileBase::REGULAR_FILE));
    }
    for (size_t i = 0; i < NUM_ENTRIES / 2; ++i)
    {
        int type;
        REQUIRE(dir.remove_entry(names[i], ids[i], type));
    }
    for (size_t i = NUM_ENTRIES / 2; i < NUM_ENTRIES; ++i)
    {
        securefs::id_type id;
        int type;
        REQUIRE(dir.get_entry(names[i], id, type));
        REQUIRE(id == ids[i]);
    }
}
#endif
