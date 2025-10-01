// taken from openrave/src/libopenrave/utils.cpp (should be fine, both FunctionParser and OpenRAVE are LGPL)
// openrave/src/libopenrave/kinbodyjoint.cpp says: openrave joint names can hold symbols like '-' and '.' can affect the equation

#include <string>
#include <vector>
#include <algorithm>

typedef std::pair<std::string, std::string> pss;

std::string SearchAndReplace(const std::string& in, const std::vector<pss>&_pairs)
{
    std::vector<pss> pairs = _pairs;
    std::stable_sort(pairs.begin(), pairs.end(), [](const pss &p0, const pss &p1){return p0.first.size() > p1.first.size();});
    std::string out;
    size_t startindex = 0;
    while(startindex < in.size()) {
        size_t nextindex=std::string::npos;
        std::vector<pss>::const_iterator itbestp;
        for(std::vector<pss>::iterator itp = pairs.begin(); itp != pairs.end(); ++itp) {
            size_t index = in.find(itp->first,startindex);
            if((nextindex == std::string::npos)|| ((index != std::string::npos)&&(index < nextindex)) ) {
                nextindex = index;
                itbestp = itp;
            }
        }
        if( nextindex == std::string::npos ) {
            out += in.substr(startindex);
            break;
        }
        out += in.substr(startindex,nextindex-startindex);
        out += itbestp->second;
        startindex = nextindex+itbestp->first.size();
    }
    return out;
}
