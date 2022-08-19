#ifndef SPPCHIP_H
#define SPPCHIP_H

#include <vector>

#ifndef _Tp
#define _Tp double
#endif

#define _eps	1e-5		// 1./65536=1.526e-5

class SPPCHIP
{
public:
    SPPCHIP();
    SPPCHIP(const std::vector<_Tp>& _xs, const std::vector<_Tp>& _ys);
    void setup_curve(const std::vector<_Tp>& _xs, const std::vector<_Tp>& _ys);

    void interp(const std::vector<_Tp>& new_xs, std::vector<_Tp>& new_ys);
    std::vector<_Tp> interp(const std::vector<_Tp>& new_xs);
    _Tp interp(_Tp new_x);

private:
    void get_slope();
    void interp_process(const std::vector<_Tp>& new_xs, std::vector<_Tp>& new_ys);

private:
    std::vector<_Tp> xs;
    std::vector<_Tp> ys;
    std::vector<_Tp> bs;
    std::vector<_Tp> cs;
    std::vector<_Tp> ds;
};

#endif // SPPCHIP_H
