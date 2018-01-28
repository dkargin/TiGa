#pragma once

#error "fuck this predictor"

class PredictorBase
{
public:
	struct State
	{
		vec3 position;		
		vec3 velocity[2];		// {linear,angular}
		vec3 acseleration[2];	// {linear,angular}
		enum Flags
		{
			knowNothing=0,
			knowPosition=1,
			knowVelocityL=2,
			knowVelocityA=4,
			knowAcselerationL=8,
			knowAcselerationA=16,
			knowAll=31,
		};
		int flags;		
		State():flags(State::knowAll){}
		State(const State &state)
		{
			position=state.position;
			velocity[0]=state.velocity[0];
			velocity[1]=state.velocity[1];
			acseleration[0]=state.acseleration[0];
			acseleration[1]=state.acseleration[1];
			flags=state.flags;
		}
	};
	virtual State getState(float dt)=0;
	virtual void update(const vec3 &position,float dt)=0;
};

//typedef TraectoryLinear TraectoryL;
class Tracked: public PredictorBase
{
public:	
	Tracked()
	{
		state.flags=State::knowNothing;
	}
	Tracked(const Tracked &mover)
	:state(mover.state),stateLast(mover.stateLast)
	{
	}
	~Tracked(void)
	{
	}

	State getState(float time)
	{
		State target=state;
		target.flags=State::knowPosition & State::knowVelocityL;
		target.position+=(state.velocity[0]*time);
		return state;
	}
	void update(const vec3 &pos,float dt)
	{
		stateLast=state;
		state.position=pos;
		state.velocity[0]=(pos-stateLast.position)/dt;
		state.flags|=state.knowVelocityL;
	}

	pathProject::TraectoryLinear getTraectoryLinear()
	{
		pathProject::TraectoryLinear res(state.position,state.velocity[0]);
		return res;
	}


public:	
	State state;
	State stateLast;
};
