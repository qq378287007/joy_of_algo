#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>
#include <iterator>
using namespace std;

const double DFP = 1.0E-10;
const double INFINITE = 1.0E4;
inline bool SameFloatValue(double v1, double v2)
{
    return fabs(v1 - v2) <= DFP;
}
inline double CrossProduct(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}
inline double DotProduct(double x1, double y1, double x2, double y2)
{
    return x1 * x2 + y1 * y2;
}

struct Point
{
    double x;
    double y;
    Point(double x_ = 0.0, double y_ = 0.0)
    {
        x = x_;
        y = y_;
    };
    Point(const Point &p)
    {
        x = p.x;
        y = p.y;
    };
    Point &operator=(const Point &p)
    {
        x = p.x;
        y = p.y;
        return *this;
    }
    double Distance(const Point &p) const
    {
        return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
    }
};

struct LineSeg
{
    Point ps;
    Point pe;
    LineSeg(double x1, double y1, double x2, double y2) : ps(x1, y1), pe(x2, y2){};
    LineSeg(const Point &ps_, const Point &pe_) : ps(ps_.x, ps_.y), pe(pe_.x, pe_.y){};
    LineSeg(const LineSeg &ls) : ps(ls.ps), pe(ls.pe){};
    bool Horizontal() const
    {
        return SameFloatValue(ps.y, pe.y);
    };
    bool Vertical() const
    {
        return SameFloatValue(ps.x, pe.x);
    };

    bool Contain(const Point &p) const
    {
        double xr = (p.x - ps.x) * (p.x - pe.x);
        if (xr > 0.0)
            return false;

        double yr = (p.y - ps.y) * (p.y - pe.y);
        if (yr > 0.0)
            return false;

        double cp = CrossProduct(pe.x - ps.x, pe.y - ps.y, p.x - ps.x, p.y - ps.y); // 计算叉积
        return SameFloatValue(cp, 0.0);
    }

    bool Exclusive(const LineSeg &ls) const
    {
        double minx = min(ps.x, pe.x);
        double maxx = max(ps.x, pe.x);
        double minx2 = min(ls.ps.x, ls.pe.x);
        double maxx2 = max(ls.ps.x, ls.pe.x);
        double miny = min(ps.y, pe.y);
        double maxy = max(ps.y, pe.y);
        double miny2 = min(ls.ps.y, ls.pe.y);
        double maxy2 = max(ls.ps.y, ls.pe.y);
        return (maxx < minx2 || maxx2 < minx || maxy < miny2 || maxy2 < miny);
    }

    bool Intersect(const LineSeg &ls) const
    {
        if (Exclusive(ls)) // 排斥实验
            return false;

        //( P1 - Q1 ) × ( Q2 - Q1 )
        double p1xq = CrossProduct(ps.x - ls.ps.x, ps.y - ls.ps.y, ls.pe.x - ls.ps.x, ls.pe.y - ls.ps.y);
        //( P2 - Q1 ) × ( Q2 - Q1 )
        double p2xq = CrossProduct(pe.x - ls.ps.x, pe.y - ls.ps.y, ls.pe.x - ls.ps.x, ls.pe.y - ls.ps.y);
        if (p1xq * p2xq > 0.0)
            return false;

        //( Q1 - P1 ) × ( P2 - P1 )
        double q1xp = CrossProduct(ls.ps.x - ps.x, ls.ps.y - ps.y, pe.x - ps.x, pe.y - ps.y);
        //( Q2 - P1 ) × ( P2 - P1 )
        double q2xp = CrossProduct(ls.pe.x - ps.x, ls.pe.y - ps.y, pe.x - ps.x, pe.y - ps.y);
        return q1xp * q2xp <= 0.0;
    }
};

struct Circle
{
    Point p;
    double r;
    Circle(double x_, double y_, double r_) : p(x_, y_), r(r_){};
    Circle(const Circle &c) : p(c.p.x, c.p.y), r(c.r){};
};

struct Rect
{
    Point p1;
    Point p2;
    Rect(double x1 = 0.0, double y1 = 0.0, double x2 = 0.0, double y2 = 0.0) : p1(x1, y1), p2(x2, y2){};
    Rect(const Point &p1_, const Point &p2_) : p1(p1_.x, p1_.y), p2(p2_.x, p2_.y){};
    Rect(const Rect &rc) : p1(rc.p1), p2(rc.p2){};
    Rect(const LineSeg &ls) : p1(ls.ps), p2(ls.pe){};

    bool Contain(const Point &p) const
    {
        double xr = (p.x - p1.x) * (p.x - p2.x);
        if (xr > 0.0)
            return false;

        double yr = (p.y - p1.y) * (p.y - p2.y);
        return yr <= 0.0;
    }

    bool Intersect(const Rect &rc) const
    {
        return ((max(p1.x, p2.x) >= min(rc.p1.x, rc.p2.x)) &&
                (max(rc.p1.x, rc.p2.x) >= min(p1.x, p2.x)) &&
                (max(p1.y, p2.y) >= min(rc.p1.y, rc.p2.y)) &&
                (max(rc.p1.y, rc.p2.y) >= min(p1.y, p2.y)));
    }
};

struct Polygon
{
    vector<Point> pts;
    Polygon(Point *p, int count)
    {
        for (int i = 0; i < count; i++)
            pts.push_back(Point(p[i].x, p[i].y));
    };
    Polygon(const Polygon &py)
    {
        copy(py.pts.begin(), py.pts.end(), back_inserter(pts));
    };

    bool IsValid() const
    {
        return pts.size() >= 3;
    };

    int GetPolyCount() const
    {
        return static_cast<int>(pts.size());
    };

    Rect GetPolygonEnvelopRect() const
    {
        assert(IsValid()); // 只考虑正常的多边形，边数>=3

        double minx = pts[0].x;
        double maxx = -pts[0].x;
        double miny = pts[0].y;
        double maxy = -pts[0].y;
        for (int i = 1; i < GetPolyCount(); i++)
        {
            if (pts[i].x < minx)
                minx = pts[i].x;
            if (pts[i].x > maxx)
                maxx = pts[i].x;
            if (pts[i].y < miny)
                miny = pts[i].y;
            if (pts[i].y > maxy)
                maxy = pts[i].y;
        }

        return Rect(minx, miny, maxx, maxy);
    }

    bool Contain(const Point &p) const
    {
        assert(IsValid()); // 只考虑正常的多边形，边数>=3

        LineSeg ll = LineSeg(p, Point(-INFINITE, p.y)); // 射线L
        const int n = GetPolyCount();
        int count = 0;
        for (int i = 0; i < n; i++)
        {
            // 当前点和下一个点组成线段P1P2
            LineSeg pp = LineSeg(pts[i], pts[(i + 1) % n]);
            if (pp.Contain(p))
                return true;

            if (!pp.Horizontal())
            {
                if ((SameFloatValue(pp.ps.y, p.y)) && (pp.ps.y > pp.pe.y))
                {
                    count++;
                }
                else if ((SameFloatValue(pp.pe.y, p.y)) && (pp.pe.y > pp.ps.y))
                {
                    count++;
                }
                else
                {
                    if (ll.Intersect(pp))
                    {
                        count++;
                    }
                }
            }
        }

        return (count % 2) == 1;
    }
};

int main()
{
    Point p1 = Point(1, 1);
    Point p2 = Point(2, 2);
    double dist = p1.Distance(p2);
    assert(fabs(dist - 1.4142135623) < DFP);

    p1 = Point(0, 0);
    dist = p1.Distance(p2);
    assert(fabs(dist - 2.8284271247) < DFP);

    p1 = Point(-1, -2);
    dist = p1.Distance(p2);
    assert(fabs(dist - 5.0) < DFP);

    assert(Rect(-1, -1, 1, 1).Contain(Point(0, 0)));
    assert(Rect(0, 0, 5, 8).Contain(Point(0, 0)));
    assert(Rect(0, 0, 5, 8).Contain(Point(1, 1)));
    assert(!Rect(0, 0, 5, 8).Contain(Point(-1, 1)));

    LineSeg ls(0, 0, 3, 5);
    assert(ls.Contain(Point(0, 0)));
    assert(ls.Contain(Point(3, 5)));

    ls = LineSeg(-1, -1, 9, 9);
    assert(ls.Contain(Point(-1, -1)));
    assert(ls.Contain(Point(4, 4)));
    assert(ls.Contain(Point(9, 9)));

    LineSeg le(0, 0, 3, 5);
    assert(!ls.Exclusive(le));
    assert(ls.Intersect(le));

    le = LineSeg(-1, 10, 6, 8);
    assert(!ls.Exclusive(le));

    le = LineSeg(-1, 10, 6, 12);
    assert(ls.Exclusive(le));
    assert(!ls.Intersect(le));

    le = LineSeg(-1, 10, 9, 5);
    assert(ls.Intersect(le));

    ls = LineSeg(1, 1, 9, 5);
    le = LineSeg(-1, 1, 9, -5);
    assert(!ls.Intersect(le));

    ls = LineSeg(1, 1, 9, 5);
    le = LineSeg(2, 2, 5, -1);
    assert(ls.Intersect(le));

    ls = LineSeg(1, 1, 9, 9);
    le = LineSeg(3, 2, 5, -1);
    assert(!ls.Intersect(le));

    Point pts[] = {{0, 0}, {0, 5}, {5, 0}};
    Polygon py = Polygon(pts, sizeof(pts) / sizeof(pts[0]));
    assert(py.Contain(Point(0, 0)));
    assert(py.Contain(Point(0, 5)));
    assert(py.Contain(Point(5, 0)));
    assert(py.Contain(Point(4, 1)));
    assert(py.Contain(Point(1, 4)));
    assert(!py.Contain(Point(5, 0.1)));

    Point pts2[] = {{1, 6}, {-1, 1}, {5, 3}, {7, 2}, {12, 2}, {9, 8}, {4, 4}};
    Polygon py2 = Polygon(pts2, sizeof(pts2) / sizeof(pts2[0]));
    assert(py2.Contain(Point(5, 4)));
    assert(py2.Contain(Point(7, 3)));
    assert(py2.Contain(Point(9, 6)));
    assert(py2.Contain(Point(9, 2)));
    assert(py2.Contain(Point(7, 2)));
    assert(py2.Contain(Point(0, 2)));
    assert(py2.Contain(Point(4, 4)));

    assert(!py2.Contain(Point(12, 3)));
    assert(!py2.Contain(Point(4, 1)));
    assert(!py2.Contain(Point(5, 2)));
    assert(!py2.Contain(Point(4, 5)));
    assert(!py2.Contain(Point(-1, 3)));

    return 0;
}
