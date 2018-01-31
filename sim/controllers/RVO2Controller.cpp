#include "RVO2Controller.h"

#include "../device.h"
#include "../mover.h"
#include "../moverVehicle.h"
#include "../sim/unit.h"

namespace sim
{

void rotate90(vec2f & vec);
void rotate90n(vec2f & vec);

const float targetScale=2.0f;
const float pathErrorScale=0.1f;

namespace RVO2
{
	static const float RVO_EPSILON = 0.00001f;
	static const size_t RVO_ERROR = std::numeric_limits<size_t>::max();


	inline float det(const Vector2& vector1, const Vector2& vector2)
	{
		return vector1[0] * vector2[1] - vector1[1] * vector2[0];
	}

	inline float abs(const Vector2& vector)
	{
		return vector.length();
	}

	inline Vector2 normalize(const Vector2& vector)
	{
		return vector / abs(vector);
	}

	inline float absSq(const Vector2 & v)
	{
		return v.length_squared();
	}
	/*!
	*  @brief      Computes the squared distance from a line segment with the
	*              specified endpoints to a specified point.
	*  @param      a               The first endpoint of the line segment.
	*  @param      b               The second endpoint of the line segment.
	*  @param      c               The point to which the squared distance is to
	*                              be calculated.
	*  @returns    The squared distance from the line segment to the point.
	*/
	inline float distSqPointLineSegment(const Vector2& a, const Vector2& b,	const Vector2& c)
	{
		const float r = ((c - a) & (b - a)) / absSq(b - a);

		if (r < 0.0f)
			return absSq(c - a);
		else if (r > 1.0f)
			return absSq(c - b);
		else
			return absSq(c - (a + r * (b - a)));
	}

	/*! 
	*  @brief      Computes the signed distance from a line connecting the
	*              specified points to a specified point.
	*  @param      a               The first point on the line.
	*  @param      b               The second point on the line.
	*  @param      c               The point to which the signed distance is to
	*                              be calculated.
	*  @returns    Positive when the point c lies to the left of the line ab.
	*/
	inline float leftOf(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		return det(a - c, b - a);
	}

	/*!
	*  @brief      Computes the square of a float.
	*  @param      a               The float to be squared.
	*  @returns    The square of the float.
	*/
	inline float sqr(float a)
	{
		return a * a;
	}
	
	Pilot::Pilot() : maxNeighbors(0), maxSpeed(0.0f), neighborDist(0.0f), timeHorizon(0.0f), timeHorizonObst(0.0f), timeStep(0.f)
	{}	

	void Pilot::computeORCA_Obstacles(float localTimeHorizon)
	{
		const float invTimeHorizonObst = 1.0f / localTimeHorizon;

		/* Create obstacle ORCA lines. */
		for (size_t i = 0; i < obstacleNeighbors.size(); ++i) 
		{

			const Obstacle* obstacle1 = obstacleNeighbors[i].second;
			const Obstacle* obstacle2 = obstacle1->nextObstacle;

			const Vector2 relativePosition1 = obstacle1->point_ - position;
			const Vector2 relativePosition2 = obstacle2->point_ - position;

			/* 
			* Check if velocity obstacle of obstacle is already taken care of by
			* previously constructed obstacle ORCA lines.
			*/
			bool alreadyCovered = false;

			for (size_t j = 0; j < orcaLines.size(); ++j) 
			{
				if (det(invTimeHorizonObst * relativePosition1 - orcaLines[j].point, orcaLines[j].direction) - invTimeHorizonObst * radius >= -RVO_EPSILON && 
					det(invTimeHorizonObst * relativePosition2 - orcaLines[j].point, orcaLines[j].direction) - invTimeHorizonObst * radius >=  -RVO_EPSILON)
				{
					alreadyCovered = true;
					break;
				}
			}

			if (alreadyCovered)
				continue;

			/* Not yet covered. Check for collisions. */

			const float distSq1 = absSq(relativePosition1);
			const float distSq2 = absSq(relativePosition2);

			const float radiusSq = sqr(radius);

			const Vector2 obstacleVector = obstacle2->point_ - obstacle1->point_;
			const float s = (-relativePosition1 & obstacleVector) / absSq(obstacleVector);
			const float distSqLine = absSq(-relativePosition1 - s * obstacleVector);

			Line line;

			if (s < 0 && distSq1 <= radiusSq) 
			{
				/* Collision with left vertex. Ignore if non-convex. */
				if (obstacle1->isConvex_) { 
					line.point = Vector2(0,0);
					line.direction = normalize(Vector2(-relativePosition1[1], relativePosition1[0]));
					orcaLines.push_back(line);
				}
				continue;
			} 
			else if (s > 1 && distSq2 <= radiusSq) 
			{
				/* Collision with right vertex. Ignore if non-convex 
				* or if it will be taken care of by neighoring obstace */
				if (obstacle2->isConvex_ && det(relativePosition2, obstacle2->unitDir_) >= 0) 
				{ 
					line.point = Vector2(0,0);
					line.direction = normalize(Vector2(-relativePosition2[1], relativePosition2[0]));
					orcaLines.push_back(line);
				}
				continue;
			} else if (s >= 0 && s < 1 && distSqLine <= radiusSq) 
			{
				/* Collision with obstacle segment. */
				line.point = Vector2(0,0);
				line.direction = -obstacle1->unitDir_;
				orcaLines.push_back(line);
				continue;
			} 

			/* 
			* No collision.  
			* Compute legs. When obliquely viewed, both legs can come from a single
			* vertex. Legs extend cut-off line when nonconvex vertex.
			*/

			Vector2 leftLegDirection, rightLegDirection;

			if (s < 0 && distSqLine <= radiusSq) {
				/*
				* Obstacle viewed obliquely so that left vertex
				* defines velocity obstacle.
				*/
				if (!obstacle1->isConvex_) {
					/* Ignore obstacle. */
					continue;
				}

				obstacle2 = obstacle1;

				const float leg1 = std::sqrt(distSq1 - radiusSq);
				leftLegDirection = Vector2(relativePosition1[0] * leg1 - relativePosition1[1] * radius, relativePosition1[0] * radius + relativePosition1[1] * leg1) / distSq1;
				rightLegDirection = Vector2(relativePosition1[0] * leg1 + relativePosition1[1] * radius, -relativePosition1[0] * radius + relativePosition1[1] * leg1) / distSq1;
			} else if (s > 1 && distSqLine <= radiusSq) {
				/*
				* Obstacle viewed obliquely so that
				* right vertex defines velocity obstacle.
				*/
				if (!obstacle2->isConvex_) {
					/* Ignore obstacle. */
					continue;
				}

				obstacle1 = obstacle2;

				const float leg2 = std::sqrt(distSq2 - radiusSq);
				leftLegDirection = Vector2(relativePosition2[0] * leg2 - relativePosition2[1] * radius, relativePosition2[0] * radius + relativePosition2[1] * leg2) / distSq2;
				rightLegDirection = Vector2(relativePosition2[0] * leg2 + relativePosition2[1] * radius, -relativePosition2[0] * radius + relativePosition2[1] * leg2) / distSq2;
			} 
			else 
			{
				/* Usual situation. */
				if (obstacle1->isConvex_) 
				{
					const float leg1 = std::sqrt(distSq1 - radiusSq);
					leftLegDirection = Vector2(relativePosition1[0] * leg1 - relativePosition1[1] * radius, relativePosition1[0] * radius + relativePosition1[1] * leg1) / distSq1;
				} 
				else 
				{
					/* Left vertex non-convex; left leg extends cut-off line. */
					leftLegDirection = -obstacle1->unitDir_;
				}

				if (obstacle2->isConvex_) 
				{
					const float leg2 = std::sqrt(distSq2 - radiusSq);
					rightLegDirection = Vector2(relativePosition2[0] * leg2 + relativePosition2[1] * radius, -relativePosition2[0] * radius + relativePosition2[1] * leg2) / distSq2;
				} 
				else 
				{
					/* Right vertex non-convex; right leg extends cut-off line. */
					rightLegDirection = obstacle1->unitDir_;
				}
			}

			/* 
			* Legs can never point into neighboring edge when convex vertex,
			* take cutoff-line of neighboring edge instead. If velocity projected on
			* "foreign" leg, no constraint is added. 
			*/

			const Obstacle* const leftNeighbor = obstacle1->prevObstacle;

			bool isLeftLegForeign = false;
			bool isRightLegForeign = false;

			if (obstacle1->isConvex_ && det(leftLegDirection, -leftNeighbor->unitDir_) >= 0.0f) 
			{
				/* Left leg points into obstacle. */
				leftLegDirection = -leftNeighbor->unitDir_;
				isLeftLegForeign = true;
			}

			if (obstacle2->isConvex_ && det(rightLegDirection, obstacle2->unitDir_) <= 0.0f) 
			{
				/* Right leg points into obstacle. */
				rightLegDirection = obstacle2->unitDir_;
				isRightLegForeign = true;
			}

			/* Compute cut-off centers. */
			const Vector2 leftCutoff = invTimeHorizonObst * (obstacle1->point_ - position);
			const Vector2 rightCutoff = invTimeHorizonObst * (obstacle2->point_ - position);
			const Vector2 cutoffVec = rightCutoff - leftCutoff;

			/* Project current velocity on velocity obstacle. */

			/* Check if current velocity is projected on cutoff circles. */
			const float t = (obstacle1 == obstacle2 ? 0.5f : ((velocity - leftCutoff) & cutoffVec) / absSq(cutoffVec));
			const float tLeft = ((velocity - leftCutoff) & leftLegDirection);
			const float tRight = ((velocity - rightCutoff) & rightLegDirection);

			if ((t < 0.0f && tLeft < 0.0f) || (obstacle1 == obstacle2 && tLeft < 0.0f && tRight < 0.0f)) 
			{
				/* Project on left cut-off circle. */
				const Vector2 unitW = normalize(velocity - leftCutoff);

				line.direction = Vector2(unitW[1], -unitW[0]);
				line.point = leftCutoff + radius * invTimeHorizonObst * unitW;
				orcaLines.push_back(line);
				continue;
			} 
			else if (t > 1.0f && tRight < 0.0f) 
			{
				/* Project on right cut-off circle. */
				const Vector2 unitW = normalize(velocity - rightCutoff);

				line.direction = Vector2(unitW[1], -unitW[0]);
				line.point = rightCutoff + radius * invTimeHorizonObst * unitW;
				orcaLines.push_back(line);
				continue;
			} 

			/* 
			* Project on left leg, right leg, or cut-off line, whichever is closest
			* to velocity.
			*/
			const float distSqCutoff = ((t < 0.0f || t > 1.0f || obstacle1 == obstacle2) ? std::numeric_limits<float>::infinity() : absSq(velocity - (leftCutoff + t * cutoffVec)));
			const float distSqLeft = ((tLeft < 0.0f) ? std::numeric_limits<float>::infinity() : absSq(velocity - (leftCutoff + tLeft * leftLegDirection)));
			const float distSqRight = ((tRight < 0.0f) ? std::numeric_limits<float>::infinity() : absSq(velocity - (rightCutoff + tRight * rightLegDirection)));

			if (distSqCutoff <= distSqLeft && distSqCutoff <= distSqRight) 
			{
				/* Project on cut-off line. */
				line.direction = -obstacle1->unitDir_;
				line.point = leftCutoff + radius * invTimeHorizonObst * Vector2(-line.direction[1], line.direction[0]);
				orcaLines.push_back(line);
				continue;
			} 
			else if (distSqLeft <= distSqRight) 
			{
				/* Project on left leg. */
				if (isLeftLegForeign)
					continue;

				line.direction = leftLegDirection;
				line.point = leftCutoff + radius * invTimeHorizonObst * Vector2(-line.direction[1], line.direction[0]);
				orcaLines.push_back(line);
				continue;
			} 
			else 
			{
				/* Project on right leg. */
				if (isRightLegForeign)
					continue;

				line.direction = -rightLegDirection;
				line.point = rightCutoff + radius * invTimeHorizonObst * Vector2(-line.direction[1], line.direction[0]);
				orcaLines.push_back(line);
				continue;
			}
		}
	}
	void Pilot::computeORCA_Agents(float localTimeHorizon)
	{
		const float invTimeHorizon = 1.0f / localTimeHorizon;

		/* Create agent ORCA lines. */
		for (size_t i = 0; i < agentNeighbors.size(); ++i) 
		{
			const Agent* const other = agentNeighbors[i].second;

			const Vector2 relativePosition = other->position - position;
			const Vector2 relativeVelocity = velocity - other->velocity;
			const float distSq = absSq(relativePosition);
			const float combinedRadius = radius + other->radius;
			const float combinedRadiusSq = sqr(combinedRadius);

			Line line;
			Vector2 u;

			if (distSq > combinedRadiusSq)
			{
				/* No collision. */
				const Vector2 w = relativeVelocity - invTimeHorizon * relativePosition; 
				/* Vector from cutoff center to relative velocity. */
				const float wLengthSq = absSq(w);

				const float dotProduct1 = w & relativePosition;

				if (dotProduct1 < 0.0f && sqr(dotProduct1) > combinedRadiusSq * wLengthSq)
				{
					/* Project on cut-off circle. */
					const float wLength = std::sqrt(wLengthSq);
					const Vector2 unitW = w / wLength;

					line.direction = Vector2(unitW[1], -unitW[0]);
					u = (combinedRadius * invTimeHorizon - wLength) * unitW;
				}
				else
				{
					/* Project on legs. */
					const float leg = std::sqrt(distSq - combinedRadiusSq);

					if (det(relativePosition, w) > 0.0f)
					{
						/* Project on left leg. */
						line.direction = Vector2(relativePosition[0] * leg - relativePosition[1] * combinedRadius, relativePosition[0] * combinedRadius + relativePosition[1] * leg) / distSq;
					}
					else
					{
						/* Project on right leg. */
						line.direction = -Vector2(relativePosition[0] * leg + relativePosition[1] * combinedRadius, -relativePosition[0] * combinedRadius + relativePosition[1] * leg) / distSq;
					}

					const float dotProduct2 = relativeVelocity & line.direction;

					u = dotProduct2 * line.direction - relativeVelocity;
				}
			} 
			else 
			{
				/* Collision. Project on cut-off circle of time timeStep. */
				const float invTimeStep = 1.0f / timeStep;

				/* Vector from cutoff center to relative velocity. */
				const Vector2 w = relativeVelocity - invTimeStep * relativePosition; 

				const float wLength = abs(w);
				const Vector2 unitW = w / wLength;

				line.direction = Vector2(unitW[1], -unitW[0]);
				u = (combinedRadius * invTimeStep - wLength) * unitW;
			}

			line.point = velocity + 0.5f * u;
			orcaLines.push_back(line);
		}
	}
	/* Search for the best new velocity. */
	Vector2 Pilot::computeNewVelocity(const Vector2 &prefVelocity)
	{
		orcaLines.clear();

		computeORCA_Obstacles(timeHorizonObst);

		const size_t numObstLines = orcaLines.size();

		computeORCA_Agents(timeHorizon);

		Vector2 newVelocity;
		size_t lineFail = linearProgram2(orcaLines, maxSpeed, prefVelocity, false, newVelocity);

		if (lineFail < orcaLines.size()) 
		{
			linearProgram3(orcaLines, numObstLines, lineFail, maxSpeed, newVelocity);
		}

		return newVelocity;
	}

	void Pilot::insertAgentNeighbor(const Agent* agent, float& rangeSq)
	{
		if (this != agent) 
		{
			const float distSq = absSq(position - agent->position);

			if (distSq < rangeSq) 
			{
				if (agentNeighbors.size() < maxNeighbors) 
				{
					agentNeighbors.push_back(std::make_pair(distSq,agent));
				}
				size_t i = agentNeighbors.size() - 1;
				while (i != 0 && distSq < agentNeighbors[i-1].first) 
				{
					agentNeighbors[i] = agentNeighbors[i-1];
					--i;
				}
				agentNeighbors[i] = std::make_pair(distSq, agent);

				if (agentNeighbors.size() == maxNeighbors)
				{
					rangeSq = agentNeighbors.back().first;
				}
			}
		}
	}

	void Pilot::insertObstacleNeighbor(const Obstacle* obstacle, float rangeSq)
	{
		const Obstacle* const nextObstacle = obstacle->nextObstacle;

		const float distSq = distSqPointLineSegment(obstacle->point_, nextObstacle->point_, position);

		if (distSq < rangeSq) 
		{
			obstacleNeighbors.push_back(std::make_pair(distSq,obstacle));

			size_t i = obstacleNeighbors.size() - 1;
			while (i != 0 && distSq < obstacleNeighbors[i-1].first) {
				obstacleNeighbors[i] = obstacleNeighbors[i-1];
				--i;
			}
			obstacleNeighbors[i] = std::make_pair(distSq, obstacle);
		}
	}

	bool linearProgram1(const std::vector<Line>& lines, size_t lineNo, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result)
	{
		const float dotProduct = lines[lineNo].point & lines[lineNo].direction;
		const float discriminant = sqr(dotProduct) + sqr(radius) - absSq(lines[lineNo].point);

		if (discriminant < 0.0f) {
			/* Max speed circle fully invalidates line lineNo. */
			return false;
		}

		const float sqrtDiscriminant = std::sqrt(discriminant);
		float tLeft = -dotProduct - sqrtDiscriminant;
		float tRight = -dotProduct + sqrtDiscriminant;

		for (size_t i = 0; i < lineNo; ++i) 
		{
			const float denominator = det(lines[lineNo].direction, lines[i].direction);
			const float numerator = det(lines[i].direction, lines[lineNo].point - lines[i].point);

			if (std::fabs(denominator) <= RVO_EPSILON) 
			{
				/* Lines lineNo and i are (almost) parallel. */
				if (numerator < 0.0f)
					return false;
				else
					continue;
			}

			const float t = numerator / denominator;

			if (denominator >= 0.0f) 
			{
				/* Line i bounds line lineNo on the right. */
				tRight = std::min(tRight, t);
			} 
			else 
			{
				/* Line i bounds line lineNo on the left. */
				tLeft = std::max(tLeft, t);
			}

			if (tLeft > tRight)
				return false;
		}

		if (directionOpt) 
		{
			/* Optimize direction. */
			if ((optVelocity & lines[lineNo].direction) > 0.0f) 
			{
				/* Take right extreme. */
				result = lines[lineNo].point + tRight * lines[lineNo].direction;
			} 
			else 
			{
				/* Take left extreme. */
				result = lines[lineNo].point + tLeft * lines[lineNo].direction;
			}
		} 
		else 
		{
			/* Optimize closest point. */
			const float t = lines[lineNo].direction & (optVelocity - lines[lineNo].point);

			if (t < tLeft) 
				result = lines[lineNo].point + tLeft * lines[lineNo].direction;
			else if (t > tRight) 
				result = lines[lineNo].point + tRight * lines[lineNo].direction;
			else
				result = lines[lineNo].point + t * lines[lineNo].direction;
		}

		return true;
	}

	size_t linearProgram2(const std::vector<Line>& lines, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result)
	{
		if (directionOpt) 
		{
			/* 
			* Optimize direction. Note that the optimization velocity is of unit
			* length in this case.
			*/
			result = optVelocity * radius;
		} 
		else if (absSq(optVelocity) > sqr(radius)) 
		{
			/* Optimize closest point and outside circle. */
			result = normalize(optVelocity) * radius;
		} 
		else 
		{
			/* Optimize closest point and inside circle. */
			result = optVelocity;
		}

		for (size_t i = 0; i < lines.size(); ++i) 
		{
			if (det(lines[i].direction, lines[i].point - result) > 0.0f)
			{
				/* Result does not satisfy constraint i. Compute new optimal result. */
				const Vector2 tempResult = result;
				if (!linearProgram1(lines, i, radius, optVelocity, directionOpt, result))
				{
					result = tempResult;
					return i;
				}
			}
		}

		return lines.size();
	}

	void linearProgram3(const std::vector<Line>& lines, size_t numObstLines, size_t beginLine, float radius, Vector2& result)
	{
		float distance = 0.0f;

		for (size_t i = beginLine; i < lines.size(); ++i)
		{
			if (det(lines[i].direction, lines[i].point - result) > distance)
			{
				/* Result does not satisfy constraint of line i. */
				std::vector<Line> projLines(lines.begin(), lines.begin() + numObstLines);

				for (size_t j = numObstLines; j < i; ++j)
				{
					Line line;

					float determinant = det(lines[i].direction, lines[j].direction);

					if (std::fabs(determinant) <= RVO_EPSILON)
					{
						/* Line i and line j are parallel. */
						if ((lines[i].direction & lines[j].direction) > 0.0f)
						{
							/* Line i and line j point in the same direction. */
							continue;
						}
						else
						{
							/* Line i and line j point in opposite direction. */
							line.point = 0.5f * (lines[i].point + lines[j].point);
						}
					} 
					else 
					{
						line.point = lines[i].point + (det(lines[j].direction, lines[i].point - lines[j].point) / determinant) * lines[i].direction;
					}

					line.direction = normalize(lines[j].direction - lines[i].direction);
					projLines.push_back(line);
				}

				const Vector2 tempResult = result;
				if (linearProgram2(projLines, radius, Vector2(-lines[i].direction[1], lines[i].direction[0]), true, result) < projLines.size())
				{
					/* This should in principle not happen.  The result is by definition
					* already in the feasible region of this linear program. If it fails,
					* it is due to small floating point error, and the current result is
					* kept.
					*/
					result = tempResult;
				}

				distance = det(lines[i].direction, lines[i].point - result);
			}
		}
	}
}

vec2f randVec(float dist)
{
	auto frand=[&]()
	{
		return dist*((float)rand()/RAND_MAX - 0.5); 
	};
	return vec2f(frand(),frand());
}

void FillAgent(RVO2::Agent & agent, GameObject * object)
{
	Sphere2 bounds = object->getBoundingSphere();

	agent.position = object->getPosition();
	agent.radius = bounds.radius;
	agent.velocity = object->getVelocityLinear();
}

void RVO2::Pilot::clearObstacles()
{
	agentNeighbors.clear();
	obstacleNeighbors.clear();
}

// ��������� ������ � VelocityObstacles
void RVO2Controller::updateObstacles(float timeStep)
{
	FillAgent(pilot, mover->getMaster());
	pilot.timeStep = timeStep;
	pilot.clearObstacles();
	activeObstacles.clear();

	auto checkObstacle = [&](const Obstacle & obstacle)
	{		
		float range = 100;//vecDistance(obstacle.first.getPosition(0), pilot.position);
		activeObstacles.push_back(RVO2::Agent());

		RVO2::Agent & agent = activeObstacles.back();

		agent.position = obstacle.first.start;
		agent.velocity = obstacle.first.velocity;
		agent.radius = obstacle.second;

		pilot.insertAgentNeighbor(&agent,range);
	};

	std::for_each(obstacles.begin(), obstacles.end(), checkObstacle);
}

void RVO2Controller::update(float dt)
{
	updatePath(dt);
	updateObstacles(dt);
	
	float maxVelocity = mover->getMaxVelocity(0);
	// ����������, ��� ������ �������� �� ���������� ��������� � 
	// �������� ��������������� ��������� ��������
	float turningSpeed = mover->getMaxVelocity(1);
	float turningRadius = maxVelocity / turningSpeed;
	Pose moverPose = mover->getGlobalPose();
	Pose::vec preferedDirection = Pose::vec::zero();

	float preferedAngle = 0;
	float preferedSpeed = 0;
	bool findMax = true;
	bool turnOnly = false;
	if( pathCurrent != -1 )
	{
		Pose::vec pathDir = pathDirection();
		Pose::vec targ = targetScale * pathDir;
		Pose::vec path = pathErrorScale * pathError();

		if( fabs( pathDir[0] ) < 2 * turningRadius && fabs( pathDir[1] ) < 2*turningRadius)
		{
			float x = pathDir & moverPose.axisX();
			float y = fabs(pathDir & moverPose.axisY()) - turningRadius;
			turnOnly = ( x*x  + y*y < turningRadius*turningRadius );
		}

		preferedDirection = (path + targ).normalize();
		preferedAngle = atan2(preferedDirection[1], preferedDirection[0]);	// � �� ����� �� ��� ��� ����
		preferedSpeed = maxVelocity;										// � ���������
		MoverVehicle * mv = (MoverVehicle*)mover;
		if(mv->kinematic)
		{
			Pose::vec bodyVelocityLinear = conv(mover->getBody()->GetLinearVelocity());
			float bodyVelocityAngular = mover->getBody()->GetAngularVelocity();
			/*
			velProj = (vel & pathDir) / |pathDir|
			time = |pathDir| / |velProj| = |pathDir| * |pathDir| / (vel & pathDir)
			*/
			//float distanceLeft = bodyVelocityLinear & pathDir.normalise();
			/*
			float arriveTime = pathDir.length_squared() / (bodyVelocityLinear & pathDir);
			float brakeTime = bodyVelocityLinear.length() / mv->definition->acceleration[0];
			if(arriveTime > 0 && arriveTime < brakeTime)
			{
			preferedSpeed = arriveTime * mv->definition->acceleration[0];
			if(preferedSpeed > maxVelocity) 
			preferedSpeed = maxVelocity;
			}*/
		}
	}
	else
	{
		findMax = false;
		preferedSpeed = 0;													// �������� �� ������ �� ����� � �� ���������
		preferedAngle = mover->getGlobalPose().orientation;					// � �������� � �� �� �������		
	}

	Pose::vec bestVelocity = pilot.computeNewVelocity(preferedDirection * preferedSpeed);		// ������ ��������
	// choose best segment	
	if(fabs(bestVelocity[1]) > 0.001)
		int w =0;
	mover->directionControl(bestVelocity, turnOnly ? 0.f : bestVelocity.length());
}

RVO2Controller::RVO2Controller(Mover *parent)
	: Mover::Driver(parent)
{
	pilot.timeHorizon = 1.0;
	pilot.timeHorizonObst = 1.0;
	pilot.maxNeighbors = 10;
	pilot.maxSpeed = mover->getMaxVelocity(0);
	pilot.neighborDist = 100;
}

void RVO2::Pilot::render(Fx::RenderContext* rc)
{
	for(auto it = orcaLines.begin(); it != orcaLines.end(); ++it)
	{
		Line & line = *it;
		drawLine(rc, position + line.point, position + line.point + line.direction * radius * 10, Fx::MakeRGB(255,0,127));
	}
}

void RVO2Controller::render(Fx::RenderContext* rc)
{
	Mover::Driver::render(rc);
	pilot.render(rc);
}

Mover::Driver * createRVO2Driver(Mover* m)
{
	return new RVO2Controller(m);
}

}
