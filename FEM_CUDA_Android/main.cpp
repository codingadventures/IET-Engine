#include <common.h>
#include <vector>
#include <Model.h>
#include <simulation.h>
#include <chrono>
#include <ctime>

#include <cudaHelper/helper_timer.h>

using namespace Rendering;
using namespace std;
 
 

int MeshLoad(Simulation& simulation)
{

	std::cout << "Load meshes" << std::endl;
	if (!simulation.simulation_load_fem_mesh(RAPTOR_NETGEN_MESH_HIGH))
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
 
  
int main(int argc, char* argv[])
{ 
	static int ifps = 0;
  	static float fpsCount = 0.0f;
	Model* model = new Model(RAPTOR_MODEL);
	Simulation* simulation  = new Simulation();
	 
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
	std::chrono::time_point<std::chrono::system_clock> start,app_start, end;


	app_start = std::chrono::system_clock::now();
	std::time_t end_time;
	std::chrono::duration<double> elapsed_seconds, total_duration;
	while (true)
	{ 
		start = std::chrono::system_clock::now();
		//main loop
		simulation->simulation_animate();

		simulation->simulation_mapping();
		 
		end = std::chrono::system_clock::now();
		end_time = std::chrono::system_clock::to_time_t(end);
		elapsed_seconds = end-start;
		total_duration += elapsed_seconds;

		if (total_duration.count() > 3.0f)
		{
			std::cout << "Avg. FPS: " << fpsCount /  (float)ifps << endl;
			total_duration = std::chrono::duration<double>::zero();
			ifps=0;  	
			fpsCount = 0;
		}
		else
		{
			ifps++;
			fpsCount += 1.0f/elapsed_seconds .count();
		}
		
	}
 
	delete model;
	delete simulation;
	return 0;
}
