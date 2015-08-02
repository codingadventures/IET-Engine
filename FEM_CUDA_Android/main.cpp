#include <common.h>
#include <vector>
#include <Model.h>
#include <simulation.h>


#include <cudaHelper/helper_timer.h>

using namespace Rendering;
using namespace std;

int fpsCount;        
	float fpsTotalCount; // FPS count for averaging
	int fpsLimit;        // FPS limit for sampling
	float ifps;
	float m_average;
StopWatchInterface *m_timer;


void  reset()
{
	m_average = 0;
	fpsCount = 0;
	fpsTotalCount = 0;
	fpsLimit = 1;
	sdkResetTimer(&m_timer);
}

int MeshLoad(Simulation& simulation)
{

	std::cout << "Load meshes" << std::endl;
	if (!simulation.simulation_load_fem_mesh(RAPTOR_NETGEN_MESH))
	{
		//return 1;
	}
	/*	for (unsigned int a = 0; a < render_filenames.size(); ++a)
	d_simulation->simulation_load_render_mesh(render_filenames[a].c_str());*/
	/*	if (reorder)
	d_simulation->simulation_reorder_fem_mesh();*/

	std::cout << "Init simulation" << std::endl;
	if (!simulation.simulation_init())
		return 1;
	return 0;
}
 

  
#ifndef MAX
#define MAX(a,b) ((a > b) ? a : b)
#endif

float  computeFPS()
{
	fpsCount++;

	if (fpsCount >= fpsLimit)
	{
		char fps[256];
		ifps = 1.f / (sdkGetAverageTimerValue(&m_timer) / 1000.f);

		cout << "FEM: " << ifps << "fps" << endl;

		//glutSetWindowTitle(fps);
		fpsTotalCount += fpsCount;
		m_average = m_average * ((fpsTotalCount-1)/fpsTotalCount) + ifps/fpsTotalCount;
		fpsCount = 0;

		fpsLimit = (int)MAX(1.f, ifps);
		sdkResetTimer(&m_timer);
	}
	return ifps;
}


int main(int argc, char* argv[])
{ 
	Model* model = new Model(RAPTOR_MODEL);
	Simulation* simulation  = new Simulation();
	
	sdkCreateTimer(&m_timer);
	reset();
	
	vector<Mesh>* meshes = model->GetMeshes( );

	simulation->SetMeshes(meshes);
	if (!simulation->simulation_preload())
	{
		printf("Error starting the simulation...");
	}	

	if (MeshLoad(*simulation))
	{
		printf("Error initializing the meshes...");

	}  
 	
	cout << "Simulation is about to start:" << endl;
	reset();
	while (true)
	{ 
		//main loop
		simulation->simulation_animate();

		simulation->simulation_mapping();
		 
		computeFPS();
	}

	sdkDeleteTimer(&m_timer);
	delete model;
	delete simulation;
	return 0;
}
