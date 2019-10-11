#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
	/**
	 * @brief Constructor
	 * @param scene Reference to the scene
	 * @param color The color of the object
	 * @param ka The ambient coefficient
	 * @param kd The diffuse reflection coefficients
	 * @param ks The specular refelection coefficients
	 * @param ke The shininess exponent
	 */
	CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
		: CShaderFlat(color)
		, m_scene(scene)
		, m_ka(ka)
		, m_kd(kd)
		, m_ks(ks)
		, m_ke(ke)
	{}
	virtual ~CShaderPhong(void) = default;

	virtual Vec3f Shade(const Ray& ray) const override
	{
		// --- PUT YOUR CODE HERE ---
		Vec3f term1 = 0;
		Vec3f term2 = 0;
		Vec3f term3 = 0;
		Vec3f diffusion = 0;
		Vec3f specular = 0;

		Ray light_ray;
		light_ray.org = ray.org + ray.t * ray.dir;

		int n = 1000; /* number of rays = 1000(Task 2.6)
					     number of rays = 1 (other tasks)*/
		for(const auto& light : m_scene.m_vpLights)
		{
			for(int i = 0; i < n; i++)
			{
				std::optional<Vec3f> Li = light->Illuminate(light_ray);

				// check for occlusion to make shadows
				if(!Li || m_scene.Occluded(light_ray))
					continue;
				
				Vec3f R = light_ray.dir - 2 * light_ray.dir.dot(ray.hit->GetNormal(ray)) * ray.hit->GetNormal(ray);
				
				diffusion += *Li * std::max(0.f, light_ray.dir.dot(ray.hit->GetNormal(ray)));
				specular += *Li * std::pow(std::max(0.f, R.dot(ray.dir)), m_ke);
			}
		}

		term1 = m_ka * CShaderFlat::Shade(ray);
		term2 = m_kd * (CShaderFlat::Shade(ray)).mul(diffusion/n);
		term3 = m_ks * RGB(1,1,1).mul(specular/n);
		return term1 + term2 + term3;
	}

	
private:
	CScene& m_scene;
	float 	m_ka;    ///< ambient coefficient
	float 	m_kd;    ///< diffuse reflection coefficients
	float 	m_ks;    ///< specular refelection coefficients
	float 	m_ke;    ///< shininess exponent
};
