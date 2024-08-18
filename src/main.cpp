#include <iostream>
#include <cmath>
#include <string>
#include <utility>
#include <SFML/Graphics.hpp>

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
const std::string kWindowTitle = "Collision detection";
const sf::Color kBackgroundColor = sf::Color(255, 255, 255);

constexpr double kCircleRadiusInPixels = 40;
const sf::Color kCircleDefaultColor = sf::Color(255, 0, 255); // Magenta
const sf::Color kCircleCollisionColor = sf::Color(255, 0, 0); // Red
constexpr double kCircleInitialPositionX = 100.0;
constexpr double kCircleInitialPositionY = 100.0;

constexpr int kRectangleWidth = 200;
constexpr int kRectangleHeight = 100;
const sf::Color kRectangleDefaultColor = sf::Color(0, 0, 255); // Blue
constexpr double kRectangleInitialPositionX = 400.0;
constexpr double kRectangleInitialPositionY = 300.0;

constexpr double kDx = 0.1;
constexpr double kDy = 0.1;

constexpr double kEpsilon = 1e-3;
constexpr double kTimeAdjustmentCoeff = 400.0;




double DistSquared(const sf::Vector2f& u, const sf::Vector2f& v) {
    double dx = u.x - v.x;
    double dy = u.y - v.y;
    return dx * dx + dy * dy;
}

double Dist(const sf::Vector2f& u, const sf::Vector2f& v) {
    return sqrt(DistSquared(u, v));
}

double Len(const sf::Vector2f& v) {
    return Dist(v, sf::Vector2f(0, 0));
}

void Normalize(sf::Vector2f& v) {
    double len_v = Len(v);
    if (len_v < kEpsilon) {
        return;
    }
    v.x /= len_v;
    v.y /= len_v;
}

// This function returns true if there is a collision between circle and rectangle
// and false otherwise.
bool CheckCollision(sf::RectangleShape& rect, sf::CircleShape& circle) {
    // Calculate distance from circle's center to rectangle's center
    sf::Vector2f center_vector = circle.getPosition() - rect.getPosition();
    double rect_w = rect.getSize().x;
    double rect_h = rect.getSize().y;
    double r = circle.getRadius();
    double Dist_x = fabs(center_vector.x) - rect_w / 2;
    double Dist_y = fabs(center_vector.y) - rect_h / 2;
    // First perform two simple tests for collision
    // If distance between centers is too large, there cannot be a collision.
    if ((Dist_x > r) || (Dist_y > r)) {
        return false;
    }
    // If distance is too small, there is definitely a collision.
    if ((Dist_x < kEpsilon) || (Dist_y < kEpsilon)) {
        return true;
    }
    // If the above two tests were inconclusive, do one more test for collision.
    // Compare distance from rectangle's corner closest to the circle to circle's center
    double dx = fabs(center_vector.x) - rect_w / 2;
    double dy = fabs(center_vector.y) - rect_h / 2;
    bool close_to_corner =  dx * dx + dy * dy < (r * r + kEpsilon);
    if (!close_to_corner) {
        return false;
    }
    return true;
}


std::pair<bool, sf::Vector2f> QuadraticEquation(double a, double b, double c) {
    double d = b * b - 4 * a * c;
    if (d < 0) {
        std::cerr << "Discriminant is negative." << std::endl;
        return std::make_pair(false, sf::Vector2f());
    }
    double sqrt_d = sqrt(d);
    double x1 = (-b - sqrt_d) / (2 * a);
    double x2 = (-b + sqrt_d) / (2 * a);
    return std::make_pair(true, sf::Vector2f(x1, x2));
}


double CircleLineIntersection(sf::CircleShape& circle, double dx, double dy, sf::Vector2f& corner) {
    double diff_y = circle.getPosition().y - corner.y;
    double diff_x = circle.getPosition().x - corner.x;
    //a, b are the coefficients of the direction vector of line
    double a = diff_y / diff_x;
    double b = circle.getPosition().y - a * circle.getPosition().x;
    ///Now we know the line equation y = a * x + b
    auto intersection_points = QuadraticEquation(a * a + 1, 2 * (a * b - a * circle.getPosition().y - circle.getPosition().x), circle.getPosition().x * circle.getPosition().x + (b - circle.getPosition().y) * (b - circle.getPosition().y) - circle.getRadius() * circle.getRadius());
    // If there is no intersection, return zero
    if (!intersection_points.first) {
        return 0.0;
    }
    sf::Vector2f first_point = sf::Vector2f(intersection_points.second.x, a * intersection_points.second.x + b);
    sf::Vector2f second_point = sf::Vector2f(intersection_points.second.y, a * intersection_points.second.y + b);
    double Dist1 = Dist(first_point, corner);
    double Dist2 = Dist(second_point, corner);
    double shift = std::min(Dist1 ,Dist2);
    return shift;
}


sf::Vector2f ResolveCollision(sf::RectangleShape& rect, sf::CircleShape& circle, double dx, double dy) {
    sf::Vector2f center = circle.getPosition() - rect.getPosition();
    double rect_w = rect.getSize().x;
    double rect_h = rect.getSize().y;
    double r = circle.getRadius();
    if (fabs(center.x) <= rect_w / 2) {
        if (center.y >= rect_h / 2) {
            return sf::Vector2f(circle.getPosition().x, rect.getPosition().y + rect_h / 2 + r);
        }
        return sf::Vector2f(circle.getPosition().x, rect.getPosition().y - rect_h / 2 - r);
    }
    if (fabs(center.y) <= rect_h / 2) {
        if (center.x >= rect_w / 2) {
            return sf::Vector2f(rect.getPosition().x + rect_w / 2 + r, circle.getPosition().y);
        }
        return sf::Vector2f(rect.getPosition().x - rect_w / 2 - r, circle.getPosition().y);
    }
    sf::Vector2f corner;
    if (center.x > 0) {
        if (center.y > 0) {
            corner = sf::Vector2f(rect.getPosition() + sf::Vector2f(rect_w / 2, rect_h / 2));
        } else {
            corner = sf::Vector2f(rect.getPosition() + sf::Vector2f(rect_w / 2, -rect_h / 2));
        }
    } else {
        if (center.y > 0) {
           corner = sf::Vector2f(rect.getPosition() + sf::Vector2f(-rect_w / 2, rect_h / 2));
        } else {
            corner = sf::Vector2f(rect.getPosition() + sf::Vector2f(-rect_w / 2, -rect_h / 2));
        }
    }

    double shift = CircleLineIntersection(circle, dx, dy, corner);
    sf::Vector2f diff = circle.getPosition() - corner;
    Normalize(diff);
    diff.x *= shift;
    diff.y *= shift;
    sf::Vector2f new_pos = circle.getPosition() + diff;
    return new_pos;
}


int main() {
    sf::RenderWindow window(sf::VideoMode(kWindowWidth, kWindowHeight), kWindowTitle);
    window.setVerticalSyncEnabled(true);

    sf::RectangleShape rect;
    rect.setFillColor(kRectangleDefaultColor);
    rect.setSize(sf::Vector2f(kRectangleWidth, kRectangleHeight));
    rect.setOrigin(sf::Vector2f(kRectangleWidth / 2, kRectangleHeight / 2));
    rect.setPosition(sf::Vector2f(kRectangleInitialPositionX, kRectangleInitialPositionY));

    sf::CircleShape circle;
    circle.setRadius(kCircleRadiusInPixels);
    circle.setOrigin(sf::Vector2f(kCircleRadiusInPixels, kCircleRadiusInPixels));
    circle.setPosition(sf::Vector2f(kCircleInitialPositionX, kCircleInitialPositionY));
    circle.setFillColor(kCircleDefaultColor);

    sf::Clock clock;
    sf::Vector2f mouse_window_prev;

    double dx = 0.0;
    double dy = 0.0;

    while (window.isOpen()) {
        double dt = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        dt /= kTimeAdjustmentCoeff;

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            dx = -kDx;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            dx = kDx;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            dy = -kDy;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            dy = kDy;
        }
        sf::Vector2f circle_position = sf::Vector2f(circle.getPosition());        
        circle.setPosition(circle_position + sf::Vector2f(dx * dt, dy * dt));

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_window_curr = sf::Vector2f(static_cast<double>(mouse.x), static_cast<double>(mouse.y));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f Dist = mouse_window_curr - circle.getPosition();
            if (Len(Dist) <= circle.getRadius()) {
                sf::Vector2f mouse_diff = mouse_window_curr - mouse_window_prev;
                circle.setPosition(circle.getPosition() + mouse_diff);
                dx = mouse_diff.x;
                dy = mouse_diff.y;
            }
        }

        if (CheckCollision(rect, circle)) {
            sf::Vector2f new_pos = ResolveCollision(rect, circle, dx, dy);
            circle.setFillColor(kCircleCollisionColor);
            circle.setPosition(new_pos);
        } else {
            circle.setFillColor(kCircleDefaultColor);
        }
        dx = 0.0;
        dy = 0.0;

        window.clear(kBackgroundColor);
        window.draw(rect);
        window.draw(circle);
        window.display();
        mouse_window_prev = mouse_window_curr;
    }
    return 0;
}
