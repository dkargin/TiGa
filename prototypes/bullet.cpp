#include "common.h"
#include "bullet.h"

Bullet::pos Bullet::getPosition() const
{
	return pose.position + pose.getDirection() * distance; 
}

Bullet::dir Bullet::getDirection() const
{
	return pose.getDirection();
}

void Bullet::update( DeltaTime dt )
{
	distance += speed * dt;
}
