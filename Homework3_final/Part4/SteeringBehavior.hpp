// SteeringBehavior.hpp

#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>

class SteeringData
{
public:
    sf::Vector2f position;
    float orientation;
    sf::Vector2f velocity;
    float rotation;
    sf::Vector2f linearAcceleration;
    float angularAcceleration;

    SteeringData()
    {
        position = sf::Vector2f();
        orientation = 0.f;
        velocity = sf::Vector2f();
        rotation = 0.f;
        linearAcceleration = sf::Vector2f();
        angularAcceleration = 0.f;
    }

    SteeringData(sf::Vector2f p, float o, sf::Vector2f v, float r, sf::Vector2f la, float anga)
    {
        position = p;
        orientation = o;
        velocity = v;
        rotation = r;
        linearAcceleration = la;
        angularAcceleration = anga;
    }
};

class SteeringBehavior
{
public:
    float maxAngularAcceleration = 100.f;
    float maxAcceleration = 1.5f;
    float timeToTarget = 1.f;

    float distanceBetween(sf::Vector2f character, sf::Vector2f goal)
    {
        return std::sqrt((goal.x - character.x) * (goal.x - character.x) + (goal.y - character.y) * (goal.y - character.y));
    }

    virtual ~SteeringBehavior() = default;

    virtual sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const = 0;
};


class SeekBehavior : public SteeringBehavior
{
public:
    float maxSpeed = 200.f;
    float maxAcceleration = 500.f;

    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        // Calculate desired velocity
        sf::Vector2f desiredVelocity = goal.position - character.position;

        // Calculate distance to goal
        float distanceToGoal = std::sqrt((goal.position.x - character.position.x) * (goal.position.x - character.position.x) +
                                         (goal.position.y - character.position.y) * (goal.position.y - character.position.y));

        // Calculate desired speed
        float desiredSpeed = 0.f;
        if (distanceToGoal > 0) // Avoid division by zero
        {
            desiredSpeed = std::min(maxSpeed, distanceToGoal); // Move at max speed or less if close
        }

        // Scale desired velocity to desired speed
        desiredVelocity = (desiredVelocity / distanceToGoal) * desiredSpeed;

        // Calculate steering force
        sf::Vector2f steeringForce = desiredVelocity - character.velocity;

        // Apply limitations
        float steeringForceMagnitude = std::sqrt(steeringForce.x * steeringForce.x + steeringForce.y * steeringForce.y);
        if (steeringForceMagnitude > maxAcceleration)
        {
            steeringForce = (steeringForce / steeringForceMagnitude) * maxAcceleration; // Normalize and scale
        }

        return steeringForce;
    }
};


class PositionMatchingBehavior : public SteeringBehavior
{
public:
    float radiusOfDeceleration = 1.f;
    float radiusOfSatisfaction = 0.5f;
    float maxSpeed = 100.f;
    float maxAcceleration = 100.f;
    // float timeToTarget = 1.f;

    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        sf::Vector2f desired_velocity = goal.position - character.position;

        float distance_to_goal = std::sqrt((goal.position.x - character.position.x) * (goal.position.x - character.position.x) +
                                           (goal.position.y - character.position.y) * (goal.position.y - character.position.y));
        float desired_speed = 0.f;

        if (distance_to_goal <= radiusOfSatisfaction)
        {
            desired_speed = 0.f;
        }
        else if (distance_to_goal < radiusOfDeceleration)
        {
            desired_speed = maxSpeed * (distance_to_goal / radiusOfDeceleration);
        }
        else
        {
            desired_speed = maxSpeed;
        }

        desired_velocity = desired_velocity / std::sqrt(desired_velocity.x * desired_velocity.x + desired_velocity.y * desired_velocity.y) * desired_speed;

        sf::Vector2f acceleration = desired_velocity - character.velocity;
        if (std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y) > maxAcceleration)
        {
            acceleration = acceleration / std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y) * maxAcceleration;
        }
        return acceleration;
    }
};

class OrientationMatchingBehavior : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        // Calculate the difference in orientation between character and goal
        float rotation = (goal.orientation - character.orientation);

        // Ensure rotation is within [-π, π] range
        while (rotation > M_PI)
        {
            rotation -= 2 * M_PI;
        }
        while (rotation < -M_PI)
        {
            rotation += 2 * M_PI;
        }

        // Calculate angular acceleration to match orientation
        float angularAcceleration = rotation / timeToTarget;

        // Clamp angular acceleration to maximum value
        if (std::abs(angularAcceleration) > maxAngularAcceleration)
        {
            angularAcceleration = std::copysign(maxAngularAcceleration, angularAcceleration);
        }

        // Return the desired change in orientation
        return sf::Vector2f(0.f, angularAcceleration);
    }
};

class VelocityMatchingBehavior : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        sf::Vector2f acceleration = (goal.velocity - character.velocity) / timeToTarget;

        // Clamp acceleration to maximum value
        float accelerationMagnitude = std::sqrt(acceleration.x * acceleration.x + acceleration.y * acceleration.y);
        if (accelerationMagnitude > maxAcceleration)
        {
            acceleration *= maxAcceleration / accelerationMagnitude;
        }
        return acceleration;
    }
};

class RotationMatchingBehavior : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        return sf::Vector2f(goal.rotation - character.rotation, goal.rotation - character.rotation);
    }
};


// The following code has been referenced from the YouTube channel - The Coding Train
class Align : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        return sf::Vector2f();
    }

    sf::Vector2f alignForce(const SteeringData &character, SteeringData boids[])
    {
        float perception = 20.f;
        int total = 0;
        sf::Vector2f steeringForce;

        sf::Vector2f average = sf::Vector2f();
        for (int i = 0; i < 59; i++)
        {
            if (boids[i].position != character.position && distanceBetween(character.position, boids[i].position) < perception) // make a function to calculate distanceBetween
            {
                average += boids[i].velocity;
                total++;
            }
        }
        if (total > 0)
        {
            average /= static_cast<float>(total); // Calculate average velocity
            steeringForce = average - character.velocity;
            steeringForce *= maxAcceleration; // Apply maximum acceleration
        }

        return steeringForce;
    }
};

class Cohesion : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        return sf::Vector2f();
    }

    float maxSpeed = 100.f;
    sf::Vector2f cohesionForce(const SteeringData &character, SteeringData boids[])
    {
        float neighborRadius = 30.f;
        int total = 0;
        sf::Vector2f steeringForce;

        sf::Vector2f average = sf::Vector2f();
        for (int i = 0; i < 59; i++)
        {
            if (boids[i].position != character.position && distanceBetween(character.position, boids[i].position) < neighborRadius) // make a function to calculate distanceBetween
            {
                average += boids[i].position;
                total++;
            }
        }
        if (total > 0)
        {
            average /= static_cast<float>(total); // Calculate average position
            steeringForce = average - character.position;
            steeringForce = (steeringForce / std::sqrt(steeringForce.x * steeringForce.x + steeringForce.y * steeringForce.y)) * maxAcceleration;
        }
        return steeringForce;
    }
};

class Separation : public SteeringBehavior
{
public:
    sf::Vector2f calculateSteering(const SteeringData &character, const SteeringData &goal) const override
    {
        return sf::Vector2f();
    }

    float maxSpeed = 100.f;
    sf::Vector2f separationForce(const SteeringData &character, SteeringData boids[])
    {
        float neighborRadius = 10.f;
        int total = 0;
        sf::Vector2f steeringForce;

        sf::Vector2f average = sf::Vector2f();
        sf::Vector2f diff;
        for (int i = 0; i < 59; i++)
        {
            if (boids[i].position != character.position && distanceBetween(character.position, boids[i].position) <= neighborRadius) // make a function to calculate distanceBetween
            {
                diff = character.position - boids[i].position;
                diff /= distanceBetween(character.position, boids[i].position);
                average += diff;
                total++;
            }
        }
        if (total > 0)
        {
            average /= static_cast<float>(total); // Calculate average direction away from other boids
            steeringForce = average - character.velocity;
            steeringForce = (steeringForce / std::sqrt(steeringForce.x * steeringForce.x + steeringForce.y * steeringForce.y)) * maxAcceleration;
        }
        return steeringForce;
    }
};
