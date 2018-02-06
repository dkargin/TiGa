#pragma once

#include "../commandAI.h"
#include "../mover.h"

namespace sim {

namespace RVO2
{
	typedef vec2f Vector2;
	/*!
	*  @brief      Defines a directed line.
	*/
	struct Line {
		/*!
		*  @brief     A point on the directed line.
		*/
		Vector2 point;

		/*!
		*  @brief     The direction of the directed line.
		*/
		Vector2 direction;
	};

	class Obstacle
	{
	public:
		/*!
		*  @brief      Constructs a static obstacle instance.
		*/
		Obstacle();

		/*!
		*  @brief      Destroys this static obstacle instance.
		*/
		~Obstacle();

		bool isConvex_;
		Obstacle* nextObstacle;
		Vector2 point_;
		Obstacle* prevObstacle;
		Vector2 unitDir_;

		size_t id_;
	};

	class Agent
	{
	public:
		explicit Agent(): position(Pose::vec::zero()), velocity(Pose::vec::zero()), radius(0) {}
		Agent(const Agent & agent): position(agent.position), velocity(agent.velocity), radius(agent.radius){}

		Vector2 position;
		
		float radius;
		//RVOSimulator* sim_;
		
		Vector2 velocity;

		//size_t id;
		/*
		float getTimeStep() const
		{
			return 0.01f;
		}*/
	};

	class Pilot : public Agent
	{
	public:
		Pilot();

		size_t maxNeighbors;
		float maxSpeed;
		float neighborDist;
		float timeHorizon;
		float timeHorizonObst;
		float timeStep;

		// clear all saved obstacles
		void clearObstacles();

		Vector2 computeNewVelocity(const Vector2 &prefVelocity);

		/*!
		*  @brief      Inserts an agent neighbor into the set of neighbors of
		*              this agent.
		*  @param      agent           A pointer to the agent to be inserted.
		*  @param      rangeSq         The squared range around this agent.
		*/
		void insertAgentNeighbor(const RVO2::Agent* agent, float& rangeSq);

		/*!
		*  @brief      Inserts a static obstacle neighbor into the set of neighbors
		*              of this agent.
		*  @param      obstacle        The number of the static obstacle to be
		*                              inserted.
		*  @param      rangeSq         The squared range around this agent.
		*/
		void insertObstacleNeighbor(const RVO2::Obstacle* obstacle, float rangeSq);

		void computeORCA_Obstacles(float localTimeHorizon);
		void computeORCA_Agents(float localTimeHorizon);

		void render(Fx::RenderContext* rc);
	protected:
		std::vector<std::pair<float, const Agent*> > agentNeighbors;
		
		//RVO2::Vector2 newVelocity;
		std::vector<std::pair<float, const Obstacle*> > obstacleNeighbors;
		std::vector<RVO2::Line> orcaLines;
		//Vector2 prefVelocity;
	};

	/*!
	*  @brief      Solves a one-dimensional linear program on a specified line
	*              subject to linear constraints defined by lines and a circular
	*              constraint.
	*  @param      lines         Lines defining the linear constraints.
	*  @param      lineNo        The specified line constraint.
	*  @param      radius        The radius of the circular constraint.
	*  @param      optVelocity   The optimization velocity.
	*  @param      directionOpt  True if the direction should be optimized.
	*  @param      result        A reference to the result of the linear program.
	*  @returns    True if successful.
	*/
	bool linearProgram1(const std::vector<Line>& lines, size_t lineNo, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result);

	/*!
	*  @brief      Solves a two-dimensional linear program subject to linear
	*              constraints defined by lines and a circular constraint.
	*  @param      lines         Lines defining the linear constraints.
	*  @param      radius        The radius of the circular constraint.
	*  @param      optVelocity   The optimization velocity.
	*  @param      directionOpt  True if the direction should be optimized.
	*  @param      result        A reference to the result of the linear program.
	*  @returns    The number of the line it fails on, and the number of lines if successful.
	*/  
	size_t linearProgram2(const std::vector<Line>& lines, float radius, const Vector2& optVelocity, bool directionOpt, Vector2& result);

	/*!
	*  @brief      Solves a two-dimensional linear program subject to linear
	*              constraints defined by lines and a circular constraint.
	*  @param      lines         Lines defining the linear constraints.
	*  @param      numObstLines  Count of obstacle lines.
	*  @param      beginLine     The line on which the 2-d linear program failed.
	*  @param      radius        The radius of the circular constraint.
	*  @param      result        A reference to the result of the linear program.
	*/
	void linearProgram3(const std::vector<Line>& lines, size_t numObstLines, size_t beginLine, float radius, Vector2& result);
}

struct RVO2Controller: public Mover::Driver
{
public:		
	RVO2Controller(Mover *parent);
	void update(float dt);
	void render(Fx::RenderContext* rc);
protected:	
	std::list<RVO2::Agent> activeObstacles;
	RVO2::Pilot pilot;	
	void updateObstacles(float timeStep);
};

Mover::Driver * createRVO2Driver(Mover* m);

} // namespace sim
