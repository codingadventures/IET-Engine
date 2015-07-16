#ifndef Inertia_h__
#define Inertia_h__

#include <glm/glm.hpp>
#include "Mesh.h"
using namespace Rendering;

namespace Physics
{
	namespace Tensor
	{
		static class Inertia
		{
		public:
			static void Compute_Polyhedral_Tensor(Mesh& mesh, float& mass, glm::mat3 &inertia)
			{
				//static float mult[10]={1.0f/6.0f,1.0f/24.0f,1.0f/24.0f,1.0f/24.0f,1.0f/60.0f,1.0f/60.0f,1.0f/60.0f,1.0f/120.0f,1.0f/120.0f,1.0f/120.0f};
				//float intg[10]= {0,0,0,0,0,0,0,0,0,0}; 
				//int tmax = mesh.m_vertices.size();

				//for(int t = 0; t < tmax; t = t + 3 )
				//{

				//	//get vertices of trianglet
				//	int i0 = t;
				//	int i1 = t+1;
				//	int i2 = t+2;

				//	float x0=mesh.m_vertices[i0].Position.x;
				//	float y0=mesh.m_vertices[i0].Position.y;
				//	float z0=mesh.m_vertices[i0].Position.z;

				//	float x1=mesh.m_vertices[i1].Position.x;
				//	float y1=mesh.m_vertices[i1].Position.y;
				//	float z1=mesh.m_vertices[i1].Position.z;

				//	float x2=mesh.m_vertices[i2].Position.x;
				//	float y2=mesh.m_vertices[i2].Position.y;
				//	float z2=mesh.m_vertices[i2].Position.z;

				//	float a1=x1-x0;
				//	float b1=y1-y0;
				//	float c1=z1-z0;

				//	float a2=x2 - x0;
				//	float b2=y2-y0;
				//	float c2=z2-z0;
				//	float d0=b1*c2-b2*c1;
				//	float d1=a2*c1-a1*c2;
				//	float d2=a1*b2-a2*b1;
				//	//compute integral terms
				//	float f1x,f2x,f1y,f1z,f2y,f2z,f3x,g0x,g1x,g2x,f3y,g0y,g1y,g2y,f3z,g0z,g1z,g2z;



				//	Subexpressions(x0,x1,x2,f1x,f2x,f3x,g0x,g1x,g2x);
				//	Subexpressions(y0,y1,y2,f1y,f2y,f3y,g0y,g1y,g2y);
				//	Subexpressions(z0,z1,z2,f1z,f2z,f3z,g0z,g1z,g2z);
				//	//update integrals
				//	intg[0]+=d0*f1x;
				//	intg[1]+=d0*f2x;intg[2]+=d1*f2y;intg[3]+=d2*f2z;
				//	intg[4]+=d0*f3x;intg[5]+=d1*f3y;intg[6]+=d2*f3z;
				//	intg[7]+=d0*(y0*g0x+y1*g1x+y2*g2x);
				//	intg[8]+=d1*(z0*g0y+z1*g1y+z2*g2y);
				//	intg[9]+=d2*(x0*g0z+x1*g1z+x2*g2z);
				//}

				//for(int i=0; i<10; i++)
				//	intg[i]*=mult[i];

				//mass=intg[0];

				////center of mass
				//mesh.m_polyhedral_center_of_mass.x=intg[1]/mass;
				//mesh.m_polyhedral_center_of_mass.y=intg[2]/mass;
				//mesh.m_polyhedral_center_of_mass.z=intg[3]/mass;

				////inertia tensor relative to center of mass
				//inertia[0][0]=intg[5]+intg[6]-mass*(mesh.m_polyhedral_center_of_mass.y*mesh.m_polyhedral_center_of_mass.y+mesh.m_polyhedral_center_of_mass.z*mesh.m_polyhedral_center_of_mass.z);
				//inertia[1][1]=intg[4]+intg[6]-mass*(mesh.m_polyhedral_center_of_mass.z*mesh.m_polyhedral_center_of_mass.z+mesh.m_polyhedral_center_of_mass.x*mesh.m_polyhedral_center_of_mass.x);
				//inertia[2][2]=intg[4]+intg[5]-mass*(mesh.m_polyhedral_center_of_mass.x*mesh.m_polyhedral_center_of_mass.x+mesh.m_polyhedral_center_of_mass.y*mesh.m_polyhedral_center_of_mass.y);
				//inertia[0][1]=-(intg[7]-mass*mesh.m_polyhedral_center_of_mass.x*mesh.m_polyhedral_center_of_mass.y);
				//inertia[1][2]=-(intg[8]-mass*mesh.m_polyhedral_center_of_mass.y*mesh.m_polyhedral_center_of_mass.z);
				//inertia[0][2]=-(intg[9]-mass*mesh.m_polyhedral_center_of_mass.z*mesh.m_polyhedral_center_of_mass.x);

				//inertia = glm::transpose(inertia);
			}
			static void Compute_Tensor_With_AABB(BoundingBox& bb, float mass, glm::mat3 &inertia)
			{
				float divisor = 0.0833f;
				inertia[0][0] = divisor * mass * (glm::pow2(bb.m_height) + glm::pow2(bb.m_depth));
				inertia[1][1] = divisor * mass * (glm::pow2(bb.m_width)  + glm::pow2(bb.m_depth));
				inertia[2][2] = divisor * mass * (glm::pow2(bb.m_width)  + glm::pow2(bb.m_height));

			}
		private:	
			static void Subexpressions(float& w0,float& w1,float& w2,float& f1,float& f2,float& f3,float& g0,float& g1,float& g2)
			{
				float temp0=w0+w1;
				f1=temp0+w2;
				float temp1=w0*w0;
				float temp2=temp1+w1*temp0;
				f2=temp2+w2*f1;
				f3=w0*temp1+w1*temp2+w2*f2;
				g0=f2+w0*(f1+w0);
				g1=f2+w1*(f1+w1);
				g2=f2+w2*(f1+w2);
			}
		};


	}
}

#endif // Inertia_h__
