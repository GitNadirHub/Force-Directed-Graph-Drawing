#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

const float Kr = 20000.f, Ka = 0.05f, r = 90.f;

float distance(sf::Vector2f A, sf::Vector2f B)
{
	return std::sqrt((A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y));
}

struct node {
	std::string label;
	unsigned int id;

	sf::Vector2f position;
	sf::Vector2f velocity;

	std::vector<int> neighbourIDs;
};


std::vector<node> nodes;

int n, m;

void graphInit()
{
	std::cin >> n >> m;
	
	nodes.resize(n + 1);

	for (int i = 1; i <= n; ++i)
	{
		nodes[i].label = std::to_string(i);
		nodes[i].id = i;

		nodes[i].position = sf::Vector2f(800 + std::sin(i) * 100, 500 + std::cos(i) * 100);
		//nodes[i].position = sf::Vector2f(400 + std::sin(i), 400 + std::cos(i));
	}

	for (int i = 1; i <= m; ++i)
	{
		int u, v;
		std::cin >> u >> v;
		nodes[u].neighbourIDs.push_back(v);
	}
}

float radius = 30.f;

void drawNode(unsigned int id, sf::RenderWindow& window)
{
	float offset = radius;
	sf::CircleShape body(radius);
	body.setOrigin({ offset, offset });
	body.setFillColor(sf::Color::Green);
	body.setPosition(nodes[id].position);
	window.draw(body);
}

void drawArc(unsigned int source, unsigned int destination, sf::RenderWindow& window)
{
	float dist = distance(nodes[source].position, nodes[destination].position) - radius - 7.f;
	sf::RectangleShape arc({ dist, 5.f });
	arc.setOrigin({ 0.f, 2.5f });
	arc.setPosition(nodes[source].position);

	double dx = (nodes[destination].position.x - nodes[source].position.x);
	double dy = (nodes[destination].position.y - nodes[source].position.y);

	float radians = std::atan2(dy, dx);
	sf::Angle angle = sf::radians(radians);
	arc.setRotation(angle);

	sf::CircleShape triangle(7.f, 3);
	triangle.setOrigin({ 7.f, 7.f });
	triangle.setFillColor(sf::Color::White);
	sf::Vector2f lineEndPoint = nodes[source].position + sf::Vector2f(
		std::cos(radians) * dist,
		std::sin(radians) * dist
	);
	triangle.setPosition(lineEndPoint);

	float arrowRadians = radians + 1.5708f;
	triangle.setRotation(sf::radians(arrowRadians));

	float targetRadius = 30.f;
	sf::Vector2f edgeOffset(
		std::cos(radians) * targetRadius,
		std::sin(radians) * targetRadius
	);

	window.draw(arc);
	window.draw(triangle);
}

void drawGraph(sf::RenderWindow &window)
{
	for (int i = 1; i <= n; ++i)
	{
		for (auto& v : nodes[i].neighbourIDs)
		{
			drawArc(i, v, window);
		}
	}
	for (int i = 1; i <= n; ++i)
	{
		drawNode(i, window);
	}
}

float damp = 0.75f;

void updatePhysics(float deltaTime)
{
	for (int i = 1; i <= n; ++i)
	{
		for (auto& v : nodes[i].neighbourIDs)
		{
			float dist = distance(nodes[i].position, nodes[v].position);
			if (dist == 0)
				continue;
			sf::Vector2f dir = (nodes[v].position - nodes[i].position) / dist;
			float Fa = Ka * (dist - r);

			sf::Vector2f appliedF = dir * Fa;

			nodes[i].velocity += appliedF;
			nodes[v].velocity -= appliedF;
		}
	}

	for (int i = 1; i <= n; ++i)
	{
		for (int j = i + 1; j <= n; ++j)
		{
			float dist = distance(nodes[i].position, nodes[j].position);
			float distSq = dist * dist;

			if (dist == 0)
				continue;

			sf::Vector2f dir = (nodes[j].position - nodes[i].position) / dist;
			float Fr = Kr / distSq;

			sf::Vector2f appliedF = dir * Fr;

			nodes[i].velocity -= appliedF;
			nodes[j].velocity += appliedF;
		}
	}

	for (int i = 1; i <= n; ++i)
	{
		nodes[i].position += nodes[i].velocity * deltaTime * 10.f;
		nodes[i].velocity *= damp;
	}
}

int main()
{
	graphInit();

	sf::RenderWindow window( sf::VideoMode( { 1600, 1000 } ), "SFML works!" );
	sf::Clock deltaClock;
	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		float deltaTime = deltaClock.restart().asSeconds();

		updatePhysics(deltaTime);

		window.clear(sf::Color::Black);
		drawGraph(window);
		window.display();
	}
}
