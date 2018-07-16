#include <algorithm>

#include "fxmanager.h"
#include "fxobjects.h"

namespace Fx
{

/////////////////////////////////////////////////////////////////////////////////
void Pyro::runEffect(EntityPtr effect)
{
	TimedEffect desc;
	desc.left = effect->duration();
	desc.effect = effect;
	effects.push_back(desc);		
}

void Pyro::update(FxManager * manager, float dt)
{		
	for(size_t i = 0; i < effects.size();)
	{
		TimedEffect &e = effects[i];
		e.left -= dt;
		if(e.left < 0 && e.effect != NULL)
		{
			delete e.effect.get();
			e.effect = 0;
			e = effects.back();
			effects.resize(effects.size()-1);
		}
		else 
		{
			e.effect->update(dt);
			i++;
		}
	}
}	

void Pyro::render(RenderQueue* queue, const Pose &pose,float scale)
{
	for(size_t i = 0; i < effects.size(); i++)
	{
		TimedEffect &e = effects[i];
		if(e.effect)
		{
			queue->enqueue(pose, e.effect);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
void RenderQueue::flush()
{
	objects.resize(0);
	heap.resize(0);
}

void RenderQueue::render(RenderContext* rc, const Pose & base)
{
	if(heap.empty())
		return;
	int * start = &heap.front();
	int * end = &heap.back() + 1;	
	HeapEntry * array = &objects.front();
	std::sort(array,array + heap.size(),[=](const HeapEntry & a,const HeapEntry & b)->bool
	{
		float za = a.pose.position[2] + a.effect->getPose().position[2];
		float zb = b.pose.position[2] + b.effect->getPose().position[2];
		return za < zb;
	});
	for(size_t i = 0; i < heap.size(); i++)
	{
		array[i].effect->render(rc, base * array[i].pose);
	}
	flush();
}

void RenderQueue::enqueue(const Pose & pose, EntityPtr effect)
{
	int index = objects.size();
	HeapEntry entry = {pose,effect};
	
	objects.push_back(entry);
	HeapEntry * array = &objects.front();
	heap.push_back(index);
	/*
	std::push_heap(&heap.front(),&heap.back()+1,[=](int a,int b)->bool
	{
		float za = array[a].pose.z + array[a].effect->z();
		float zb = array[b].pose.z + array[b].effect->z(); 
		return za > zb;
	});*/
}

} // namespace Fx
