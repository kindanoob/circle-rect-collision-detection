#include <iostream>
#include <cmath>

#include <SFML/Graphics.hpp>

const double DX = 0.1;
const double DY = 0.1;
const double kCircleRadius = 40; //in pixels




double dist_squared(const sf::Vector2f& u, const sf::Vector2f& v) {
    double dx = u.x - v.x;
    double dy = u.y - v.y;
    return dx * dx + dy * dy;
}

double dist(const sf::Vector2f& u, const sf::Vector2f& v) {
    return sqrt(dist_squared(u, v));
}

double len(const sf::Vector2f& v) {
    return dist(v, sf::Vector2f(0, 0));
}

void normalize(sf::Vector2f& v) {
    double len_v = len(v);
    v.x /= len_v;
    v.y /= len_v;
}

sf::Vector2f quadratic_equation(double a, double b, double c) {
    double sqrt_d = sqrt(b * b - 4 * a * c);
    double x1 = (-b - sqrt_d) / (2 * a);
    double x2 = (-b + sqrt_d) / (2 * a);
    return sf::Vector2f(x1, x2);
}

//a, b - coefficients of the direction vector of line
double circle_line_intersection(sf::CircleShape& circle, double dx, double dy, sf::Vector2f& corner) {
    double diff_y = circle.getPosition().y - corner.y;
    double diff_x = circle.getPosition().x - corner.x;
    double a = diff_y / diff_x;
    double b = circle.getPosition().y - a * circle.getPosition().x;
    ///Now we know the line equation y = a * x + b
    sf::Vector2f intersection_points = quadratic_equation(a * a + 1, 2 * (a * b - a * circle.getPosition().y - circle.getPosition().x), circle.getPosition().x * circle.getPosition().x + (b - circle.getPosition().y) * (b - circle.getPosition().y) - circle.getRadius() * circle.getRadius());
    sf::Vector2f first_point = sf::Vector2f(intersection_points.x, a * intersection_points.x + b);
    sf::Vector2f second_point = sf::Vector2f(intersection_points.y, a * intersection_points.y + b);
    double dist1 = dist(first_point, corner);
    double dist2 = dist(second_point, corner);
    double shift = std::min(dist1 ,dist2);
    return shift;
}

bool check_collision(sf::RectangleShape& rect, sf::CircleShape& circle) {
    sf::Vector2f center = circle.getPosition() - rect.getPosition();
    double rect_w = rect.getSize().x;
    double rect_h = rect.getSize().y;
    double r = circle.getRadius();
    double dist_x = fabs(center.x) - rect_w / 2;
    double dist_y = fabs(center.y) - rect_h / 2;
    if ((dist_x > r) || (dist_y > r)) {
        return false;
    }
    if ((dist_x <= 0) || (dist_y <= 0)) {
        return true;
    }
    bool close_to_corner = (fabs(center.x) - rect_w / 2) * (fabs(center.x) - rect_w / 2) + (fabs(center.y) - rect_h / 2) * (fabs(center.y) - rect_h / 2) <= r * r;
    if (!close_to_corner) {
        return false;
    }
    return true;
}

sf::Vector2f resolve_collision(sf::RectangleShape& rect, sf::CircleShape& circle, double dx, double dy) {
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

    double shift = circle_line_intersection(circle, dx, dy, corner);
    sf::Vector2f diff = circle.getPosition() - corner;
    normalize(diff);
    diff.x *= shift;
    diff.y *= shift;
    sf::Vector2f new_pos = circle.getPosition() + diff;
    return new_pos;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Collision detection");
    window.setVerticalSyncEnabled(true);

    sf::RectangleShape rect;
    rect.setFillColor(sf::Color::Blue);
    rect.setSize(sf::Vector2f(200, 100));
    rect.setOrigin(sf::Vector2f(100, 50));
    rect.setPosition(sf::Vector2f(400, 300));

    sf::CircleShape circle;
    circle.setRadius(kCircleRadius);
    circle.setOrigin(sf::Vector2f(kCircleRadius, kCircleRadius));
    circle.setPosition(sf::Vector2f(100, 100));
    circle.setFillColor(sf::Color::Magenta);

    sf::Clock clock;
    sf::Vector2f mouse_window_prev;

    double dx = 0;
    double dy = 0;

    while (window.isOpen()) {
        double dt = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        dt /= 400;

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
            }
        }
        sf::Vector2f v = sf::Vector2f(circle.getPosition());
        double x = v.x;
        double y = v.y;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            dx = -DX;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            dx = DX;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            dy = -DY;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            dy = DY;
        }
        x += dx * dt;
        y += dy * dt;
        circle.setPosition(x, y);

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_window_curr = sf::Vector2f(static_cast<double>(mouse.x), static_cast<double>(mouse.y));
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f dist = mouse_window_curr - circle.getPosition();
            double r = circle.getRadius();
            if (len(dist) <= r) {
                sf::Vector2f mouse_diff = mouse_window_curr - mouse_window_prev;
                circle.setPosition(circle.getPosition() + mouse_diff);
                dx = mouse_diff.x;
                dy = mouse_diff.y;
            }
        }

        if (check_collision(rect, circle)) {
            sf::Vector2f new_pos = resolve_collision(rect, circle, dx, dy);
            circle.setFillColor(sf::Color::Red);
            circle.setPosition(new_pos);
        } else {
            circle.setFillColor(sf::Color::Magenta);
        }
        dx = 0;
        dy = 0;

        window.clear(sf::Color::White);
        window.draw(rect);
        window.draw(circle);
        window.display();
        mouse_window_prev = mouse_window_curr;
    }
    return 0;
}
