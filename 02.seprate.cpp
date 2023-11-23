#include <vector>
#include <cmath>
using namespace std;

class Point
{
public:
    double x;
    double y;
};

class Vector
{
public:
    double x;
    double y;

public:
    Vector(double m_x, double m_y) : x(m_x), y(m_y) {}
    Vector(const Point &p) : x(p.x), y(p.y) {}

public:
    Vector GetNormal()
    {
        Vector v = GetPerpendicular();
        v.Normalize();
        return v;
    }
    static Vector MakeEdge(const Point &p1, const Point &p2)
    {
        Vector u(p1);
        Vector v(p2);
        return u.Subtarct(v);
        // return Vector(p1.x-p2.x,p1.y-p2.y);
    }
    double DotProduct(const Vector &v) const
    {
        return x * v.x + y * v.y;
    }

protected:
    Vector Subtarct(const Vector &v) const
    {
        return Vector(x - v.x, y - v.y);
    }
    Vector GetPerpendicular() const
    {
        return Vector(-y, x);
    }
    void Normalize()
    {
        double dist = sqrt(x * x + y * y);
        if (dist != 0.0)
        {
            x /= dist;
            y /= dist;
        }
    }
};

class Projection
{
    double min;
    double max;

public:
    Projection(double m_min, double m_max) : min(m_min), max(m_max) {}
    bool IsOverlap(const Projection &p) const
    {
        return min < p.max && max > p.min;
    }
};

class Polygon
{
    vector<Point> vertex;

public:
    void GetAxes(vector<Vector> &axes) const
    {
        const size_t n = vertex.size();
        for (size_t i = 0; i < n; i++)
        {
            Point p1 = vertex[i];
            Point p2 = vertex[(i + 1) % n];
            Vector edge = Vector::MakeEdge(p1, p2);
            axes.push_back(edge.GetNormal());
        }
    }
    Projection GetProject(const Vector &axes) const
    {
        // double min = RANGE_MAX;
        // double max = RANGE_MIN;
        double min;
        double max;
        for (const Point &i : vertex)
        {
            Vector p(i);
            double prj = p.DotProduct(axes);
            if (prj < min)
                min = prj;
            else if (prj > max)
                max = prj;
        }
        return Projection(min, max);
    }
};

bool CollisionTest(const Polygon &pa, const Polygon &pb)
{
    vector<Vector> axess;
    pa.GetAxes(axess);
    pb.GetAxes(axess);
    for (const Vector &axes : axess)
    {
        Projection pa_prj = pa.GetProject(axes);
        Projection pb_prj = pb.GetProject(axes);
        if (!pa_prj.IsOverlap(pb_prj))
            return false;
    }
    return true;
}

int main()
{
    return 0;
}