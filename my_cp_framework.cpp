#include <iostream>
#include <complex>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <vector>
#include <algorithm>
#include <set>
/// Spaghetti code by DE5C3NDER(Ishak Dervisevic)

// Forward declarations wooohooo
//{
bool __are_equal(double a, double b);
double DEG_TO_RAD(double deg);
double RAD_TO_DEG(double deg);

// Geometry stuff
struct Point;
struct Line;
struct Vector2;
struct Line_Segment;
struct Polygon;

//Point related stuff
double dist_nosqrt(Point &point1, Point &point2);
double dist(Point &point1, Point &point2);
Point translate(Point point, Vector2 vec);

//Line related stuff
bool are_paralel(Line &line1, Line &line2);
double determinant(Line &line1, Line &line2);
Point intersection(Line &line1, Line &line2);

//Vector2 related stuff
double vector_dot(Vector2 &vec1, Vector2 &vec2);
double vector2_cross_product(Vector2 &vec1, Vector2 &vec2);
Vector2 vec_abs(Vector2 &vec);
Vector2 to_unit_vector(Vector2 &vec);
double determinant(Vector2 &vec1, Vector2 &vec2);

// Line segment stuff
Point intersection(Line_Segment &ls1, Line_Segment &ls2);

// Polygon stuff
vector<Polygon> split_by_intersections(Polygon poly);

// General geometry suff
double ccw(Line_Segment line, Point point);
double ccw(Line line, Point point);
double ccw(Point p1, Point p2, Point p3);
double angle(Point p1, Point p2, Point p3);
double dist_to_line_segment(Point point, Line_Segment line);
bool is_point_on_line(Point &point, Line &line);
bool is_point_on_line_segment(Point point, Line_Segment ls);

// Graph stuff
struct Adjacency_List;
struct Adjacency_Matrix;
struct Edge_List;

// End of forward declarations
//}

// Defining some constants
//{
// Max acceptable error value
const double EPS = 1e-5;

// Graph direction parameters
#define BIDIRECTIONAL true
#define UNIDIRECTIONAL false

#define NULL_POINT Point(-69420, -42069);
//}

// Non-geometry related structs
//{
    // Adjacency matrix graph
    struct Adjacency_List{
        vector<set<int>> graph;
        Adjacency_List(){}
        Adjacency_List(int vertecies){
            graph.clear();
            set<int> dummy;
            for(int i = 0; i < vertecies; i++){
                graph.push_back(dummy);
            }
            return;
        }
        void add_connection(int from, int to, bool direction){
            graph[from].insert(to);
            if(direction == BIDIRECTIONAL)
                graph[to].insert(from);
        }
        // All the stuff needed to get graph components here //{
        vector<vector<int>> get_components(){
            vector<bool> visited (graph.size()+1);
            fill(visited.begin(), visited.end(), false);

            vector<vector<int>> components;

            for(int i = 0; i < graph.size(); i++){
                if(!visited[i])
                    components.push_back(_get_components(i, visited));
            }
            return components;
        }
        vector<int> _get_components(int p, vector<bool> &visited){
            vector<int> components;
            __get_components(p, visited, components);
            return components;
        }
        void __get_components(int p, vector<bool> &visited, vector<int> &components){
            components.push_back(p);
            for(int i = 0; i < graph[p].size(); i++)
                if(!visited[i])
                    __get_components(p, visited, components);
        }
        //}
        void log(){
            for(int i = 0; i < graph.size(); i++){
                cout << "Point " << i+1 << " connects to points: ";
                std::set<int>::iterator it;
                for (it = graph[i].begin(); it != graph[i].end(); ++it){
                    cout << *it << ' ';
                }
                cout << endl;
            }
        }
    };
    struct Adjacency_Matrix{
        vector<vector<bool>> matrix;
        Adjacency_Matrix(){}
        Adjacency_Matrix(int verticies){
            matrix.clear();
            for(int i = 0; i < verticies; i++){
                matrix.push_back(vector<bool>());
                for(int j = 0; j < verticies; j++){
                    if(i == j)
                        matrix[i].push_back(true);
                    matrix[i].push_back(false);
                }
            }
        }
        Adjacency_Matrix(vector<vector<bool>> _matrix){
            matrix = _matrix;
        }
        void add_connection(int from, int to, bool direction){
            matrix[from][to] = true;
            if(direction == BIDIRECTIONAL)
                matrix[to][from] = true;
        }
        bool connection_exists(int from, int to){
            return matrix[from][to];
        }
        void clear(){
            matrix.clear();
        }
    };
//}

// Geometry related structures
//{
struct Point{
    double x, y;

    Point(){ x = y = 0.0; }

    Point(double _x, double _y){
        x = _x;
        y = _y;
        correct();
    }

    bool operator == (Point other){
        return __are_equal(x, other.x) && __are_equal(y, other.y);
    }
    void correct(){
        if(__are_equal(x, 0.0))
            x = 0;
        if(__are_equal(y, 0.0))
            y = 0;
    }
    Point* rotate_rad(double rad){
        double mem_x = x;
        x = x*cos(rad) - y*sin(rad);
        y = mem_x*sin(rad) + y*cos(rad);
        correct();
        return this;
    }

    Point* rotate_deg(double deg){
        return rotate_rad(DEG_TO_RAD(deg));
    }
    void log(){
        cout << "x: " << x << "y: " << y << endl;
    }
};

struct Line{
    double a, b, c;
    Line(){}
    Line(double _a, double _b, double _c){
        a = _a;
        b = _b;
        c = _c;
    }
    Line(Point point1, Point point2){

        if(point1.x > point2.x)
            swap(point1, point2);

        if(__are_equal(point1.x, point2.x)){
            a = 1.0;
            b = 0.0;
            c = point1.x;
        }else{
            a = (point2.y-point1.y)/fabs((point2.x-point1.x));
       /*     cout << point1.x << ' ' << point1.y << endl;
            cout << point2.x << ' ' << point2.y << endl;
            cout << (point2.y-point1.y) << endl;*/
            b = 1.0;
            c = point1.y - (a * point1.x);
        }
    }
    double f(double _x){
        return _x*a + c;
    }
    bool operator == (Line other){
        return are_paralel(*this, other) && c == other.c;
    }
    void log(){
        cout << a << "per 1 unit starting from y = " << c << endl;
    }
};

struct Vector2{
    double x, y;

    Vector2(double _x, double _y){
        x = _x;
        y = _y;
    }
    Vector2(Point point){
        x = point.x;
        y = point.y;
    }
    Vector2(Point point1, Point point2){
        x = point1.x-point2.x;
        y = point1.y-point2.y;
    }
    Vector2* scale(double scalar){
        x = x*scalar;
        y = y*scalar;
        return this;
    }
    Vector2* rotate_rad(double rad){
        Point temp = Point(x, y);
        temp.rotate_rad(rad);
        x = temp.x;
        y = temp.y;
        return this;
    }
    Vector2* rotate_deg(double deg){
        return rotate_rad(DEG_TO_RAD(deg));
    }
    Point get_point(){
        return Point(x, y);
    }
    double magnitude(){
        return sqrt(x*x + y*y);
    }
    void log(){
        cout << x << ' ' << y << endl;
    }
};

struct Line_Segment{
    Point a, b;
    double length;
    Line line;
    Line_Segment(){}
    Line_Segment(Point _a, Point _b){
        if(_a.x > _b.x)
            swap(_a, _b);
        a = _a;
        b = _b;
        line = Line(a, b);
        length = dist(a, b);
    }
    Line_Segment(double ax, double ay, double bx, double by){
        a = Point(ax, ay);
        b = Point(bx, by);
    }
    void log(){
        cout << "Parent line: ";
        line.log();
        cout << "starting point:";
        a.log();
        cout << "ending point: ";
        b.log();
        cout << "length: " << length << endl;
    }
};

struct Polygon{
    vector<Point> points;
    Polygon(){}
    Polygon(vector<Point> _points){
        points = _points;
    }
    void add_point(Point point){
        points.push_back(point);
    }
    bool is_point_in_polygon(Point point){
        sort_polar();
        double sum = 0;
        for(int i = 1; i < points.size(); i++)
            sum += angle(points[i-1], point, points[i]);
        sum += angle(points[points.size()-1], point, points[0]);
        return __are_equal(sum, 360.0) || __are_equal(sum, 0.0) || __are_equal(sum, -360.0);
    }
    void log(){
        for(int i = 0; i < points.size(); i++)
            points[i].log();
    }
    double area(){
        sort_polar();
        double area = 0;
        for(int i = 1; i < points.size(); i++){
            Vector2 a = Vector2(points[i-1]);
            Vector2 b = Vector2(points[i]);
            area += fabs(determinant(a, b))/2.0;
        }
        Vector2 a = Vector2(points[points.size()-1]);
        Vector2 b = Vector2(points[0]);
        area += fabs(determinant(a, b));
        return area;
    }
    void sort_polar(){
        sort(points.begin(), points.end(), [](Point p1, Point p2){
            return atan2(p1.y, p1.x) < atan2(p2.y, p2.x);
        });
    }
    double perimeter(){
        double perimeter = 0;
        for(int i = 1; i < points.size(); i++){
            perimeter += Line_Segment(points[i-1], points[i]).length;
        }
        perimeter += Line_Segment(points[points.size()-1], points[0]).length;
        return perimeter;
    }
};
//}

// Geometry related functions
//{
bool __are_equal(double a, double b){
    return fabs(a-b) < EPS;
}

double DEG_TO_RAD(double deg){
    return deg * (M_PI/180);
}

double RAD_TO_DEG(double deg){
    return deg * (180/M_PI);
}

double dist_nosqrt(Point &point1, Point &point2){
    return (point1.x-point2.x)*(point1.x-point2.x) + (point1.y-point2.y)*(point1.y-point2.y);
}

double dist(Point &point1, Point &point2){
    return sqrt(dist_nosqrt(point1, point2));
}

double determinant(Line &line1, Line &line2){
    if(line1.b == 0 || line2.b == 0)
        return 0;
    return (double)(line1.a * line2.b - line2.a * line1.b);
}

bool are_paralel(Line &line1, Line &line2){
    return __are_equal(determinant(line1, line2), 0.0);
}

Point intersection(Line &line1, Line &line2){
    if(are_paralel(line1, line2)){
        cerr << "LINES ARE PARALEL" << endl;
        return Point(NULL, NULL);
    }

    double x = (line2.b-line1.b+line2.c-line1.c)/(line1.a-line2.a);
    double y = x*line1.a+line1.c;

    Point intersection = Point(x, y);
    return intersection;
}

bool is_point_on_line(Point &point, Line &line){
    return ccw(line, point) == 0;
}

bool is_point_on_line_segment(Point point, Line_Segment ls){
    if(!is_point_on_line(point, ls.line))
        return false;
    return point.x >= ls.a.x && point.x <= ls.b.x;
}

Point intersection(Line_Segment &ls1, Line_Segment &ls2){
    Line l1 = ls1.line;
    Line l2 = ls2.line;
    Point intersect = intersection(l1, l2);
    if(is_point_on_line_segment(intersect, ls1) && is_point_on_line_segment(intersect, ls2))
        return intersect;
    return Point(NULL, NULL);
}

double vector_dot(Vector2 &vec1, Vector2 &vec2){
    return vec1.x*vec2.x + vec1.y*vec2.y;
}

Vector2 to_unit_vector(Vector2 &vec){
    double m = fabs(vec.magnitude());
    if(m == 0)
        return Vector2(0, 0);
  //  vec.get_point().log();
    return Vector2(vec.x/m, vec.y/m);
}

Vector2 vector_projection(Vector2 &vec1, Vector2 &vec2){
    Vector2 unit_vec2 = to_unit_vector(vec2);
    double projection_magnitude = vector_dot(vec1,unit_vec2);
    unit_vec2.scale(projection_magnitude);
    return unit_vec2;
}

double vector2_cross_product(Vector2 &vec1, Vector2 &vec2){
    //similar to determinant
    return vec1.x*vec2.y - vec1.y*vec2.x;
}

Point translate(Point point, Vector2 vec){
    return Point((double)point.x+vec.x, (double)point.y+vec.y);
}

double dist_to_line_segment(Point point, Line_Segment line){
    Vector2 vec2 = Vector2(line.a, point);
    Vector2 vec1 = Vector2(line.a, line.b);
    Point proj_tip = vector_projection(vec1, vec2).get_point();
   // proj_tip.log();
    return dist(point, proj_tip);
}

Vector2 vec_abs(Vector2 &vec){
    return Vector2(fabs(vec.x), fabs(vec.y));
}

double angle(Point p1, Point p2, Point p3){
    Vector2 vec1 = Vector2(p2, p1), vec1_abs = vec_abs(vec1);
    Vector2 vec2 = Vector2(p2, p3), vec2_abs = vec_abs(vec2);
    if(p1 == p3)
        return 0;
    double ccw_val = ccw(p2,p1,p3);
    double ang = RAD_TO_DEG(acos(vector_dot(vec1, vec2)/(vec1.magnitude()*vec2.magnitude())));
    if(__are_equal(ccw_val, 0))
        return 180;
    if(ccw_val < EPS)
        return -ang;
    return ang;
}

double ccw(Line_Segment line, Point point){
    // Counter clockwise test
    if(Line(line.a,line.b) == Line(line.a, point))
        return 0;
    Vector2 vec1  = Vector2(line.a, point), vec2 = Vector2(line.a, line.b);
    return vector2_cross_product(vec1, vec2);
}

double ccw(Point p1, Point p2, Point p3){
    // Counter clockwise test
  //  if(Line(p1,p2) == Line(p1,p3))
   //     return 0;
    Vector2 vec1  = Vector2(p1, p3), vec2 = Vector2(p1, p2);
    return vector2_cross_product(vec1, vec2);
}

double ccw(Line line, Point point){
    // HELLA BROKEN
    Point a = Point(0, line.f(0));
    Point b = Point(1, line.f(1));
    Vector2(a, point).log();
    Vector2(a, b).log();
    if(Line(a, b) == Line(a, point))
        return 0;
    Vector2 vec1  = Vector2(a, point), vec2 = Vector2(a, b);
    return vector2_cross_product(vec1, vec2);
}

double determinant(Vector2 &vec1, Vector2 &vec2){
    return (double)(vec1.x * vec2.y - vec2.x * vec1.y);
}
//}

// Niche stuff
//{
    // Split polygon if there are intersections between lines;
    vector<Polygon> split_by_intersections(Polygon poly){

        // Make a graph from polygon with added verticies at intersections
        int s = poly.points.size();
        Adjacency_List graph = Adjacency_List(2*s);
        poly.create_graph();
        for(int i = 0; i < s; i++){
            for(int j = i; j < s; j++){
                if(poly.connection_exists(i, j)){
                    for(int k = i+1; k < s; k++){
                        for(int l = k; l < s; l++){
                            Line_Segment ls1 = Line_Segment(poly.points[i], poly.points[j]);
                            Line_Segment ls2 = Line_Segment(poly.points[k], poly.points[l]);
                            Point intersect = intersection(ls1, ls2);
                            if(!intersect.is_null()){
                                intersect.log();
                             //   cout << i << ' ' << j << ' ' << k << ' ' << l << ' ' << poly.points.size() << endl;
                                poly.add_point(intersect);
                                graph.add_connection(i, poly.points.size()-1, BIDIRECTIONAL);
                                graph.add_connection(poly.points.size()-1, j,  BIDIRECTIONAL);
                                graph.add_connection(k, poly.points.size()-1, BIDIRECTIONAL);
                                graph.add_connection(poly.points.size()-1, l,  BIDIRECTIONAL);
                            }else{
                                graph.add_connection(i, j, BIDIRECTIONAL);
                            }
                        }
                    }

                    }
            }
        }
        // Use the graph to create a vector of seperate polygons
        graph.log();
        return vector<Polygon>();
    }

//}

int main(){

    return 0;
}
