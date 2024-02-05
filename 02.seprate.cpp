#include <vector>
#include <cmath>
#include <cfloat>
using namespace std;

struct Point
{
    double x, y;
};

struct Vector
{
    double x, y;

    Vector(double m_x, double m_y) : x(m_x), y(m_y) {}
    Vector(const Point &p) : x(p.x), y(p.y) {}

    Vector GetNormal()
    {
        Vector v = GetPerpendicular();
        v.Normalize();
        return v;
    }
    double DotProduct(const Vector &v) const
    {
        return x * v.x + y * v.y;
    }

    static Vector MakeEdge(const Point &p1, const Point &p2)
    {
        Vector u(p1);
        Vector v(p2);
        return u.Subtarct(v);
        // return Vector(p1.x - p2.x, p1.y - p2.y);
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

struct Projection
{
    double min, max;
    Projection(double m_min, double m_max) : min(m_min), max(m_max) {}
    bool IsOverlap(const Projection &p) const
    {
        return min < p.max && max > p.min;
    }
};

struct Polygon
{
    vector<Point> vertex;

public:
    void GetAxes(vector<Vector> &axs) const
    {
        const size_t n = vertex.size();
        for (size_t i = 0; i < n; i++)
        {
            Point p1 = vertex[i];
            Point p2 = vertex[(i + 1) % n];
            Vector edge = Vector::MakeEdge(p1, p2);
            axs.push_back(edge.GetNormal());
        }
    }
    Projection GetProject(const Vector &axs) const
    {
        double min = DBL_MAX;
        double max = -DBL_MAX;
        for (const Point &i : vertex)
        {
            Vector p(i);
            double prj = p.DotProduct(axs);
            if (min > prj)
                min = prj;
            if (max < prj)
                max = prj;
        }
        return Projection(min, max);
    }
};

bool CollisionTest(const Polygon &pa, const Polygon &pb)
{
    vector<Vector> axs;
    pa.GetAxes(axs);
    pb.GetAxes(axs);
    for (const Vector &ax : axs)
    {
        Projection pa_prj = pa.GetProject(ax);
        Projection pb_prj = pb.GetProject(ax);
        if (!pa_prj.IsOverlap(pb_prj))
            return false;
    }
    return true;
}

int main()
{
    return 0;
}
