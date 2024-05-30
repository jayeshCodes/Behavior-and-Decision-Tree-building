class Cohesion : public SteeringBehavior
{
    float maxSpeed = 100.f;
    sf::Vector2f calculateSteering(const auto &character, const auto &boids[])
    {
        float neighborRadius = 100.f;
        int total = 0;
        sf::Vector2f steeringForce;

        sf::Vector2f average = sf::Vector2f();
        for (const auto &boid : boids)
        {
            if (boid != character && distanceBetween(character.position, boid.position) < neighborRadius) // make a function to calculate distanceBetween
            {
                average += boid.position;
                total++;
            }
        }
        if (total > 0)
            average /= total;
        
        steringForce = average - character.position;
        steeringForce*=maxSpeed;
        steeringForce-=character.velocity;
        steeringForce = (steeringForce/(std::sqrt((steeringForce.x*steeringForce.x)+(steeringForce.y*steeringForce.y)))) * maxAcceleration;
        return steeringForce;
    }
};