#include "plotlayout.h"
#include "qwt_symbol.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_picker.h"
#include "qwt_picker_machine.h"

PlotLayout::PlotLayout( QWidget *parent, _Tp y0, _Tp y1)
    : QHBoxLayout(parent)
{
    m_plot = new QwtPlot;
    addWidget(m_plot);

    m_plot->setAxisScale(QwtPlot::xBottom, 0., 1., 0.2);
    m_plot->setAxisScale(QwtPlot::yLeft, 0., 1., 0.2);

    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->setMajorPen(Qt::darkGray, 0, Qt::DotLine);
    grid->attach(m_plot);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::Ellipse);
    symbol->setSize(7);
    symbol->setBrush(QBrush(Qt::red, Qt::SolidPattern));
    m_points = new QwtPlotCurve;
    m_points->setStyle(QwtPlotCurve::NoCurve);
    //m_points->setCurveAttribute(QwtPlotCurve::Fitted);
    m_points->setSymbol(symbol);
    m_points->attach(m_plot);

    symbol = new QwtSymbol(QwtSymbol::Ellipse);
    symbol->setSize(9);
    symbol->setBrush(QBrush(Qt::green, Qt::SolidPattern));
    m_marker = new QwtPlotCurve;
    m_marker->setSymbol(symbol);
    m_marker->setVisible(false);
    m_marker->attach(m_plot);

    m_curve = new QwtPlotCurve;
    m_curve->setPen(Qt::blue, 2);
    m_curve->attach(m_plot);

    m_picker = new QwtPlotPicker(m_plot->canvas());
    m_picker->setStateMachine(new QwtPickerDragPointMachine());
    m_picker->setTrackerMode(QwtPicker::AlwaysOff);

    connect(m_picker, SIGNAL(appended(QPointF)), this, SLOT(slotPointSelected(QPointF)));
    connect(m_picker, SIGNAL(moved(QPointF)), this, SLOT(slotPointDragged(QPointF)));

    m_show_xs.resize(m_show_pnum);
    m_show_ys.resize(m_show_pnum);
    for (int i = 0; i < m_show_pnum; i++)
    {
        m_show_xs[i] = (_Tp)i / (m_show_pnum - 1);
    }

    m_init_y0 = y0 > _eps ? y0 < 1 - _eps ? y0 : 1 : 0;
    m_init_y1 = y1 > _eps ? y1 < 1 - _eps ? y1 : 1 : 0;
    m_base_points.push_back(QPointF(0., m_init_y0));
    m_base_points.push_back(QPointF(1., m_init_y1));
    plotShow();
}

void PlotLayout::setPickerEnabled(bool enabled)
{
    m_picker->setEnabled(enabled);
}

void PlotLayout::setPickerXMovable(bool enabled)
{
    m_basex_movable = enabled;
}

void PlotLayout::setPickerInsertable(bool enabled)
{
    m_base_insertable = enabled;
}

void PlotLayout::reset()
{
    m_marker->setVisible(false);
    m_base_points.clear();
    m_base_points.push_back(QPointF(0., m_init_y0));
    m_base_points.push_back(QPointF(1., m_init_y1));
    plotShow();
}

void PlotLayout::interp(const std::vector<_Tp>& xs, std::vector<_Tp>& ys)
{
    m_sppchip.interp(xs, ys);
}

void PlotLayout::setBasePointsSamples()
{
    int pnum = m_base_points.size();
    m_base_xs.resize(pnum);
    m_base_ys.resize(pnum);
    for(int i = 0; i < pnum; i++)
    {
        m_base_xs[i] = m_base_points[i].x();
        m_base_ys[i] = m_base_points[i].y();
    }
    m_points->setSamples(m_base_xs.data(), m_base_ys.data(), pnum);
}

void PlotLayout::plotShow()
{
    setBasePointsSamples();
    m_sppchip.setup_curve(m_base_xs, m_base_ys);
    m_sppchip.interp(m_show_xs, m_show_ys);
    m_curve->setSamples(m_show_xs.data(), m_show_ys.data(), m_show_pnum);
    m_plot->replot();
    m_curveVersion += 1;
}

uint32_t PlotLayout::getCurveVersion() const
{
    return m_curveVersion;
}

static inline _Tp calDist(const QPointF &p1, const QPointF &p2)
{
    _Tp difx = p1.x() - p2.x();
    _Tp dify = p1.y() - p2.y();
    return std::sqrt(difx * difx + dify * dify);
}

static int closestPoint(QwtPlotCurve *curve, const QPointF &point, _Tp *dist)
{
    if (NULL == curve || curve->dataSize() <= 0)
    {
        return -1;
    }

    int idx = -1;
    _Tp min_dist = 0.;

    for (int i = 0; i < curve->dataSize(); i++)
    {
        _Tp di = calDist(curve->sample(i), point);
        if (i == 0)
        {
            min_dist = di;
            idx = i;
        }
        else if(min_dist - di > 1e-3)
        {
            min_dist = di;
            idx = i;
        }
    }
    if (NULL != dist)
    {
        *dist = min_dist;
    }
    return idx;
}

void PlotLayout::slotPointSelected(const QPointF& mousePos)
{
    if (!this->isEnabled())
    {
        return;
    }

    double dist = 1000.;
    int idx = closestPoint(m_points, mousePos, &dist);
    m_mark_selected = 0;

    if (dist > 0.02)
    {
        if (m_base_insertable && m_base_points.size() <= m_max_base_pnum)
        {
            _Tp mouse_x = mousePos.x();
            _Tp interp_y = m_sppchip.interp(mouse_x);
            if (fabs(interp_y - mousePos.y()) > 0.02)
            {
                m_mark_selected = 0;
            }
            else
            {
                for(int i = 0; i < m_base_points.size() - 1; i++)
                {
                    if (mouse_x - m_base_points[i].x() > 0.03 && m_base_points[i + 1].x() - mouse_x > 0.03)
                    {
                        m_base_points.insert(i + 1, QPointF(mouse_x, interp_y));
                        setBasePointsSamples();
                        m_mark_selected = 1;
                        m_mark_base_idx = i + 1;
                        m_mark_x = mouse_x;
                        m_mark_y = interp_y;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        m_mark_selected = 1;
        m_mark_base_idx = idx;
        m_mark_x = m_base_points[idx].x();
        m_mark_y = m_base_points[idx].y();
    }

    if (m_mark_selected)
    {
        m_marker->setSamples(&m_mark_x, &m_mark_y, 1);
        m_marker->setVisible(true);
    }
    else
    {
        m_marker->setVisible(false);
    }
    m_plot->replot();
}

void PlotLayout::slotPointDragged(const QPointF& mousePos)
{
    if (!m_mark_selected)
    {
        return;
    }
    if (calDist(mousePos, m_base_points[m_mark_base_idx]) < 0.01)
    {
        return;
    }

    _Tp mouse_x = mousePos.x();
    _Tp mouse_y = mousePos.y();

    if (m_basex_movable)
    {
        if (m_mark_base_idx > 0 && m_mark_base_idx < m_base_points.size() - 1)
        {
            if (mouse_x - m_base_points[m_mark_base_idx - 1].x() > 0.015)
            {
                if (m_base_points[m_mark_base_idx + 1].x() - mouse_x > 0.015)
                {
                    m_mark_x = mouse_x;
                }
                else
                {
                    m_mark_x =  m_base_points[m_mark_base_idx + 1].x() - 0.015;
                }
            }
            else
            {
                m_mark_x = m_base_points[m_mark_base_idx - 1].x() + 0.015;
            }
        }
    }

    if (mouse_y > _eps)
    {
        if (mouse_y < 1 - _eps)
        {
            m_mark_y = mouse_y;
        }
        else
        {
            m_mark_y = 1.;
        }
    }
    else
    {
        m_mark_y = 0.;
    }
    m_base_points[m_mark_base_idx] = QPointF(m_mark_x, m_mark_y);
    m_marker->setSamples(&m_mark_x, &m_mark_y, 1);
    plotShow();
}

void PlotLayout::deleteSelectedPoint()
{
    if (!m_mark_selected)
    {
        return;
    }
    if (m_mark_base_idx == 0 || m_mark_base_idx == m_base_points.size() - 1)
    {
        return;
    }
    m_base_points.takeAt(m_mark_base_idx);
    m_marker->setVisible(false);
    plotShow();
}
