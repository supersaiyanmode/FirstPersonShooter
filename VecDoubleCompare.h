#ifndef VECDOUBLECOMPARE_H
#define VECDOUBLECOMPARE_H
#include <vector>
#include <cmath>

inline bool vecDoubleEqual(const std::vector<double>& v1, const std::vector<double>& v2){
    if (v1.size() != v2.size() || !v1.size())
        return false;
    
    for (int i=0,len=v1.size(); i<len; i++)
        if (std::fabs(v1[i]-v2[i]) >  1e-4)
            return false;
    return true;
}

#endif