#include "common.h"
#include <vector>
#include "Model.h"
#include "simulation.h"
#include <chrono>
#include <ctime>
using namespace Rendering;
using namespace std;
int MeshLoad(Simulation &const simulation)
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
string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName( NULL, buffer, MAX_PATH );
	string::size_type pos = string( buffer ).find_last_of( "\\/" );
	return string( buffer ).substr( 0, pos);
}


int main(int argc, char* argv[])
{ 
	cout << "my directory is " << ExePath() << "\n";

	auto model = new Model(RAPTOR_MODEL);
	auto simulation  = new Simulation();
	vector<Mesh>* meshes = model->GetMeshes( );

	simulation->SetMeshes(meshes);
	if (!simulation->simulation_preload())
	{
		cerr << "Error starting the simulation..." << endl;
	}	

	if (MeshLoad(*simulation))
	{
		cerr << "Error initializing the meshes..." << endl;

	} 

	std::chrono::time_point<std::chrono::system_clock> start, end;



	std::time_t end_time;
	std::chrono::duration<double> elapsed_seconds;
	while (true)
	{
		start = std::chrono::system_clock::now();

		//main loop
		simulation->simulation_animate();

		simulation->simulation_mapping();
		end = std::chrono::system_clock::now();
		end_time = std::chrono::system_clock::to_time_t(end);
		elapsed_seconds = end-start;


		std::cout << "FPS: " << 1.0f/elapsed_seconds .count()
			<< "elapsed time: " << elapsed_seconds.count() << "s" << endl;
	}



	return 0;
}
