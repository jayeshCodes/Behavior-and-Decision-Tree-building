// SteeringBehavior.hpp

#pragma once
#include "SteeringData.hpp"
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <iostream>


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

