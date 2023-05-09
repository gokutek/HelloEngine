#include "model_obj.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <assert.h>

model_obj::model_obj(std::string const& filename)
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;

    while (!in.eof()) 
    {
        std::string line;
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) 
        {
            iss >> trash;
            float x, y, z;
            iss >> x >> y >> z;
            vector3 v(x, y, z);
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vn ")) 
        {
            iss >> trash >> trash;
            float x, y, z;
            iss >> x >> y >> z;
            vector3 n(x, y, z);
            norms.push_back(n.normalize());
        }
        else if (!line.compare(0, 3, "vt ")) 
        {
            iss >> trash >> trash;
            float x, y;
            iss >> x >> y;
            vector2 uv(x, y);
            tex_coord.push_back({ uv.get_x(), 1.0f - uv.get_y()});
        }
        else if (!line.compare(0, 2, "f ")) 
        {
            int f, t, n;
            iss >> trash;
            int cnt = 0;
            while (iss >> f >> trash >> t >> trash >> n) 
            {
                facet_vrt.push_back(--f);
                facet_tex.push_back(--t);
                facet_nrm.push_back(--n);
                cnt++;
            }
            assert(3 == cnt);
        }
    }

    in.close();
    
    std::cerr << "# v# " << nverts() << " f# " << nfaces() << " vt# " << tex_coord.size() << " vn# " << norms.size() << std::endl;
}

size_t model_obj::nverts() const 
{
    return verts.size();
}

size_t model_obj::nfaces() const
{
    return facet_vrt.size() / 3;
}

vector3 model_obj::vert(int i) const 
{
    return verts[i];
}

vector3 model_obj::vert(int face, int nth_vert) const 
{
    return verts[facet_vrt[face * 3 + nth_vert]];
}

vector2 model_obj::uv(int face, int nth_vert) const 
{
    return tex_coord[facet_tex[face * 3 + nth_vert]];
}

vector3 model_obj::normal(int face, int nth_vert) const 
{
    return norms[facet_nrm[face * 3 + nth_vert]];
}
