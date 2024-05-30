class Align : public SteeringBehavior
{
    sf::Vector2f calculateSteering(const auto &character, const auto &boids[])
    {
        float perception = 100.f;
        int total = 0;
        sf::Vector2f steeringForce;

        sf::Vector2f average = sf::Vector2f();
        for (const auto &boid : boids)
        {
            if (boid != character && distanceBetween(character.position, boid.position) < perception) // make a function to calculate distanceBetween
            {
                average += boid.velocity;
                total++;
            }
        }
        if (total > 0)
            average /= total;

        steringForce = average - character.velocity;
        steeringForce = (steeringForce / (std::sqrt((steeringForce.x * steeringForce.x) + (steeringForce.y * steeringForce.y)))) * maxAcceleration;

        return steeringForce;
    }
};