#include "build_info.h"
#include <string>

static const std::string wl_bid = "unofficial-git-82b4c66424";
static const std::string wl_bt  = "Debug";

const std::string & build_id()
{
	return wl_bid;
}

const std::string & build_type()
{
	return wl_bt;
}
