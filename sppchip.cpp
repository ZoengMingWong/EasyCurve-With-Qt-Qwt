#include <algorithm>
#include "sppchip.h"

/* If the input series is sorted, the sorting operation would be skipped
 * for a lower computation complexity.
*/
#define _IS_SORTED_INPUT     1

SPPCHIP::SPPCHIP()
{
    xs.resize(0);
    ys.resize(0);
}

SPPCHIP::SPPCHIP(const std::vector<_Tp>& _xs, const std::vector<_Tp>& _ys)
{
    setup_curve(_xs, _ys);
}

void SPPCHIP::setup_curve(const std::vector<_Tp>& _xs, const std::vector<_Tp>& _ys)
{
    int len_xs = _xs.size(), len_ys = _ys.size();
    if (len_xs != len_ys)
    {
        //throw runtime_error("Sizes of xs and ys must be equal!");
        len_xs = 0;
    }
    if (len_xs)
    {
        xs.resize(len_xs);
        ys.resize(len_xs);

        if (len_xs >= 2)
        {
            std::vector<int> ord(len_xs);
            for (int i = 0; i < len_xs; i++)
            {
                ord[i] = i;
            }
#if !_IS_SORTED_INPUT
            std::sort(ord.begin(), ord.end(), [&_xs](int i1, int i2) { return _xs[i2] - _xs[i1] > _eps; });
#endif
            for (int i = 0; i < len_xs; i++)
            {
                xs[i] = _xs[ord[i]];
                ys[i] = _ys[ord[i]];
            }
            if (len_xs > 2)
            {
                get_slope();
            }
        }
        else
        {
            xs[0] = _xs[0];
            ys[0] = _ys[0];
        }
    }
    else
    {
        xs.resize(0);
    }
}

void SPPCHIP::interp(const std::vector<_Tp>& new_xs, std::vector<_Tp>& new_ys)
{
    interp_process(new_xs, new_ys);
}

std::vector<_Tp> SPPCHIP::interp(const std::vector<_Tp>& new_xs)
{
    std::vector<_Tp> new_ys;
    interp_process(new_xs, new_ys);
    return new_ys;
}

_Tp SPPCHIP::interp(_Tp new_x)
{
    int len_xs = xs.size();
    if (len_xs == 0)
    {
        return 0;
    }
    else if (len_xs == 1)
    {
        return ys[0];
    }
    else if (len_xs == 2)
    {
        _Tp h = xs[1] - xs[0];
        _Tp delta = ys[1] - ys[0];
        _Tp slope = h > _eps ? delta / h : 0;
        return (new_x - xs[0]) * slope + ys[0];
    }

    if (xs[0] - new_x > _eps)
    {
        return ys[0];
    }
    for (int idx_xs = 0; idx_xs < len_xs - 1; idx_xs++)
    {
        if (new_x - xs[idx_xs] > _eps)
        {
            if (xs[idx_xs + 1] - new_x > _eps)
            {
                _Tp dif = new_x - xs[idx_xs];
                return ys[idx_xs] + dif * (ds[idx_xs] + dif * (cs[idx_xs] + dif * bs[idx_xs]));
            }
        }
        else
        {
            return ys[idx_xs];
        }
    }
    return ys[len_xs - 1];
}

void SPPCHIP::interp_process(const std::vector<_Tp>& new_xs, std::vector<_Tp>& new_ys)
{
    int len_xs = xs.size();
    int len_out = new_xs.size();

    if (len_out == 0)
    {
        new_ys.resize(0);
        return;
    }
    if (len_xs == 0)
    {
        new_ys.resize(len_out, 0.);
        return;
    }
    else if (len_xs == 1)
    {
        new_ys.resize(len_out, ys[0]);
        return;
    }

    if (len_out != new_ys.size())
    {
        new_ys.resize(len_out);
    }

    if (len_xs == 2)
    {
        _Tp h = xs[1] - xs[0];
        _Tp delta = ys[1] - ys[0];
        _Tp slope = h > _eps ? delta / h : 0;
        for (int i = 0; i < len_out; i++)
        {
            new_ys[i] = (new_xs[i] - xs[0]) * slope + ys[0];
        }
        return;
    }

    std::vector<int> ord(len_out);
    for (int i = 0; i < len_out; i++)
    {
        ord[i] = i;
    }
#if !_IS_SORTED_INPUT
    std::sort(ord.begin(), ord.end(), [&new_xs](int i1, int i2) { return new_xs[i2] - new_xs[i1] > _eps; });
#endif

    int idx_ord = 0;
    while (xs[0] - new_xs[ord[idx_ord]] > _eps)
    {
        new_ys[ord[idx_ord]] = ys[0];
        idx_ord++;
        if (idx_ord >= len_out)
        {
            break;
        }
    }
    for (int idx_xs = 0; idx_xs < len_xs - 1; idx_xs++)
    {
        while (idx_ord < len_out)
        {
            _Tp tmp_x = new_xs[ord[idx_ord]];
            if (tmp_x - xs[idx_xs] > _eps)
            {
                if (xs[idx_xs + 1] - tmp_x > _eps)
                {
                    _Tp dif = tmp_x - xs[idx_xs];
                    new_ys[ord[idx_ord]] = ys[idx_xs] + dif * (ds[idx_xs] + dif * (cs[idx_xs] + dif * bs[idx_xs]));
                }
                else
                {
                    break;
                }
            }
            else
            {
                new_ys[ord[idx_ord]] = ys[idx_xs];
            }
            idx_ord++;
        }
    }
    for (; idx_ord < len_out; idx_ord++)
    {
        new_ys[ord[idx_ord]] = ys[len_xs - 1];
    }
}

static inline int sign(_Tp x)
{
    if (x > _eps)
        return 1;
    else if (fabs(x) > _eps)
        return -1;
    else
        return 0;
}

static _Tp get_slope_end(_Tp h1, _Tp h2, _Tp delta1, _Tp delta2)
{
    if (h1 > _eps && h2 > _eps)
    {
        _Tp d = ((2 * h1 + h2) * delta1 - h1 * delta2) / (h1 + h2);
        int sign_d = sign(d);
        int sign_delta1 = sign(delta1);
        int sign_delta2 = sign(delta2);
        if (!sign_delta1 || sign_d != sign_delta1)
        {
            d = 0;
        }
        else if (sign_delta1 != sign_delta2 && fabs(d) - fabs(3 * delta1) > _eps)
        {
            d = 3 * delta1;
        }
        return d;
    }
    else
    {
        return 0;
    }
}

void SPPCHIP::get_slope()
{
    if (xs.size() < 3)
    {
        return;
    }

    int len = xs.size();
    std::vector<_Tp> delta(len - 1);
    std::vector<_Tp> h(len - 1);

    ds.resize(len);
    cs.resize(len);
    bs.resize(len);

    for (int i = 0; i < len - 1; i++)
    {
        h[i] = xs[i + 1] - xs[i];
        if (h[i] > _eps)
            delta[i] = (ys[i + 1] - ys[i]) / h[i];
        else
            delta[i] = 0;
    }

    ds[0] = get_slope_end(h[0], h[1], delta[0], delta[1]);
    for (int i = 1; i < len - 1; i++)
    {
        if (h[i] > _eps && h[i - 1] > _eps)
        {
            int sign_pre = sign(delta[i - 1]);
            int sign_cur = sign(delta[i]);
            if (sign_pre * sign_cur <= 0)
            {
                ds[i] = 0;
            }
            else
            {
                _Tp w1 = 2 * h[i] + h[i - 1];
                _Tp w2 = h[i] + 2 * h[i - 1];
                ds[i] = (w1 + w2) / (w1 / delta[i - 1] + w2 / delta[i]);
            }
        }
        else
        {
            ds[i] = 0;
        }
    }
    ds[len - 1] = get_slope_end(h[len - 2], h[len - 3], delta[len - 2], delta[len - 3]);

    for (int i = 0; i < len - 1; i++)
    {
        if (h[i] > _eps)
        {
            cs[i] = (3 * delta[i] - 2 * ds[i] - ds[i + 1]) / h[i];
            bs[i] = (ds[i] - 2 * delta[i] + ds[i + 1]) / (h[i] * h[i]);
        }
        else
        {
            cs[i] = 0;
            bs[i] = 0;
        }
    }
}
