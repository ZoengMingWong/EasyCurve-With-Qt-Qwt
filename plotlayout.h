#ifndef PLOTLAYOUT_H
#define PLOTLAYOUT_H

#include <QHBoxLayout>
#include <QWidget>
#include "sppchip.h"
#include "qwt_plot.h"
#include "qwt_plot_curve.h"
#include "qwt_plot_picker.h"

class PlotLayout : public QHBoxLayout
{
    Q_OBJECT
public:
    explicit PlotLayout(QWidget* = nullptr, _Tp y0 = 0, _Tp y1 = 1);

    void setPickerEnabled(bool);
    void setPickerXMovable(bool);
    void setPickerInsertable(bool);
    void deleteSelectedPoint();
    void reset();
    void interp(const std::vector<_Tp>& xs, std::vector<_Tp>& ys);
    uint32_t getCurveVersion() const;

protected slots:
    void slotPointSelected(const QPointF& mousePos);
    void slotPointDragged(const QPointF& mousePos);

protected:
    void plotShow();
    void setBasePointsSamples();

protected:
    QwtPlot *m_plot = NULL;
    QwtPlotCurve *m_points = NULL;
    QwtPlotCurve *m_curve = NULL;
    QwtPlotCurve *m_marker = NULL;
    QwtPlotPicker *m_picker = NULL;

    SPPCHIP m_sppchip;
    QList<QPointF> m_base_points;
    _Tp m_init_y0 = 0.;
    _Tp m_init_y1 = 1.;

    const int m_max_base_pnum = 16;
    std::vector<_Tp> m_base_xs;
    std::vector<_Tp> m_base_ys;
    bool m_basex_movable = 1;           // if the base point can be moved horizontally
    bool m_base_insertable = 1;         // if a new base point can be inserted

    const int m_show_pnum = 51;         // points number in linspace(0., 1.)
    std::vector<_Tp> m_show_xs;
    std::vector<_Tp> m_show_ys;

    _Tp m_mark_x = 0;
    _Tp m_mark_y = 0;
    bool m_mark_selected = 0;
    int  m_mark_base_idx = 0;

    uint32_t m_curveVersion = 0;
};


#endif // PLOTLAYOUT_H
