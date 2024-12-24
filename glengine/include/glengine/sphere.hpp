#ifndef GLENGINE_SPHERE_HPP
#define GLENGINE_SPHERE_HPP

namespace GLEngine {
    class Sphere {
    public:
        Sphere(float radius = 0.1f, int sectorCount = 36, int stackCount = 18);
        ~Sphere();
        
        void draw() const;
        void cleanup();
        
    private:
        unsigned int VAO, VBO, EBO;
        int indexCount;
        
        void setupSphere(float radius, int sectorCount, int stackCount);
    };
}

#endif // GLENGINE_SPHERE_HPP